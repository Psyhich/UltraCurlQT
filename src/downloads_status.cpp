#include <chrono>

#include <QErrorMessage>
#include <QTextStream>
#include <QScrollArea>

#include "download_data.h"
#include "downloads_status.h"
#include "download_widget.h"
#include "http_downloader.h"

CStatusWidget::CStatusWidget(QWidget *pParrent) : QWidget(pParrent)
{
	QVBoxLayout *pOuterLayout = new QVBoxLayout(this);
	setLayout(pOuterLayout);

	QScrollArea *pScrollArea = new QScrollArea(this);
	pScrollArea->setWidgetResizable(true);

	pOuterLayout->addWidget(pScrollArea);

	m_pDownloadsHolder = new QWidget();
	QVBoxLayout *pInnerLayout = new QVBoxLayout();
	m_pDownloadsHolder->setLayout(pInnerLayout);

	pInnerLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinAndMaxSize);

	pScrollArea->setWidget(m_pDownloadsHolder);
	// pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	// pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);




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
							if(std::get<1>(progress->second) <= 1e-14)
							{
								entry->m_iDownloadProgress = 0;
							}
							else
							{
								entry->m_iDownloadProgress = 
									std::get<0>(progress->second) / std::get<1>(progress->second) * 100;
							}
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
	m_updaterThread.join();
}

void CStatusWidget::AddDownload(QString sUriToDownload, const QFileInfo &pathToSave) noexcept
{
	// Creating entry that will contain data about download
	std::string sStdURIString = sUriToDownload.toStdString();
	DownloadsData::DownloadEntry pEntry
		{new DownloadsData::SDownloadEntry(CURI{sStdURIString})};
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

	m_pool->AddNewTask(CURI{sStdURIString},
		[&, pathToSave, pEntry](std::optional<HTTP::CHTTPResponse> &&response) -> bool
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
