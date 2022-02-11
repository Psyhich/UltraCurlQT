#include <chrono>

#include <QErrorMessage>
#include <QTextStream>
#include <QScrollArea>
#include <QHBoxLayout>

#include "download_data.h"
#include "downloads_status.h"
#include "download_widget.h"
#include "http_downloader.h"

CStatusWidget::CStatusWidget(QWidget *pParrent) : QWidget(pParrent)
{
	QScrollArea *pScrollArea = new QScrollArea(this);
	pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	m_pDownloadsHolder = new QWidget(pScrollArea);
	pScrollArea->setWidget(m_pDownloadsHolder);

	QHBoxLayout *pInnerLayout = new QHBoxLayout(m_pDownloadsHolder);
	pInnerLayout->setContentsMargins(0, 0, 0, 0);
	pInnerLayout->setSpacing(0);
	m_pDownloadsHolder->setLayout(pInnerLayout);


	m_pool = 
		Downloaders::Concurrency::CConcurrentDownloader::AllocatePool(
			Downloaders::CHTTPDownloader::ValidSocketFactory);

	m_updaterThread = std::thread(
		[this]
		{
			do
			{
				// I decided not to do updates simulatenously, but to lock resources in series
				{
					// Acquiring lock to update all entries
					const auto downloadProgress = m_pool->GetDownloadProgres();
					std::scoped_lock<std::mutex> dataLock{m_entriesMutex};
					for(auto &entry : m_downloadEntries)
					{
						if(const auto progress = downloadProgress.find(entry->m_uriDownloading);
							progress != downloadProgress.end())
						{
							// Calculating progress of download in percents
							entry->m_iDownloadProgress = 
								std::get<0>(progress->second) / std::get<1>(progress->second) * 100;
						}
					}
				}

				std::this_thread::sleep_for(
					std::chrono::milliseconds(SLEEP_TIME_MILISECONS));

				// After we updated all entries, updating widgets
				std::scoped_lock<std::mutex> widgetLock{m_widgetsMutex};
				for(auto &widget : m_pendingDownloads)
				{
					widget->UpdateData();
				}
			}
			while(!m_bShouldStopUpdates.load());
		});
}

CStatusWidget::~CStatusWidget()
{
	m_bShouldStopUpdates.store(true);
}

void CStatusWidget::AddDownload(std::string sUriToDownload, const QFileInfo &pathToSave) noexcept
{
	// Creating entry that will contain data about download
	DownloadsData::DownloadEntry pEntry;
	pEntry->m_savedFile = pathToSave;
	pEntry->m_status = DownloadsData::EDownloadStatus::Downloading;

	{
		std::scoped_lock<std::mutex> entriesLock{m_entriesMutex};
		m_downloadEntries.push_back(pEntry);
	}

	// Creating widget that represent this download
	CDownloadWidget *pStatus = new CDownloadWidget(pEntry, m_pDownloadsHolder);
	m_pDownloadsHolder->layout()->addWidget(pStatus);

	{
		std::scoped_lock<std::mutex> widgetsLock{m_widgetsMutex};
		m_pendingDownloads.push_back(pStatus);
	}

	m_pool->AddNewTask(std::move(CURI{sUriToDownload}),
		[&](std::optional<HTTP::CHTTPResponse> &&response) -> bool
		{
			DownloadsData::EDownloadStatus statusToSet{
				DownloadsData::EDownloadStatus::Failed};
			do
			{
				if(response)
				{
					if(response->GetData().empty())
					{
						break;
					}
					
					if(const auto cEncodingHeader = response->GetHeaders().find("content-encoding");
						cEncodingHeader != response->GetHeaders().end() &&
						cEncodingHeader->second.find("zstd") != std::string::npos)
					{
						if(!response->DecompressBody())
						{
							QErrorMessage errorDialog;
							errorDialog.showMessage("Failed to decompress file, writing compressed data");
						}
					}
					// Assuming user already know that frontend 
					// alrerady checked if we should overwrite file
					QFile fileToWrite{pathToSave.absoluteFilePath()};
					if(fileToWrite.open(QIODevice::WriteOnly))
					{
						// Constructing non-owning QByteArray
						QByteArray bytesToWrite = 
							QByteArray::fromRawData(response->GetData().data(), response->GetData().size());

						QTextStream writeStream{&fileToWrite};
						writeStream << bytesToWrite;
						statusToSet = DownloadsData::EDownloadStatus::Done;
					}
				}
			}
			while(false);

			std::scoped_lock<std::mutex> dataLock{m_entriesMutex};
			pEntry->m_status = statusToSet;

			return false;
		});
}
