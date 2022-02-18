#ifndef DOWNLOADS_STATUS_H
#define DOWNLOADS_STATUS_H

#include <thread>
#include <mutex>

#include <QWidget>
#include <QPointer>
#include <QVBoxLayout>

#include "download_data.h"
#include "download_widget.h"
#include "downloader_pool.h"

class CStatusWidget : public QWidget
{
	using DownloadsPool = std::unique_ptr<Downloaders::Concurrency::CConcurrentDownloader>;
public:
	explicit CStatusWidget(QWidget *parrent = nullptr);
	~CStatusWidget();

public slots:
	void AddDownload(QString sUriToDownload, const QFileInfo &pathToSave) noexcept;

private:

private:
	// Thread that will be responsible for updating progress of downloads
	std::thread m_updaterThread;
	std::atomic<bool> m_bShouldStopUpdates{false};

	DownloadsPool m_pool;

	std::mutex m_entriesMutex;
	QList<DownloadsData::DownloadEntry> m_downloadEntries;

	std::mutex m_widgetsMutex;
	QList<CDownloadWidget *> m_pendingDownloads;

	QWidget *m_pDownloadsHolder;

	static constexpr const size_t SLEEP_TIME_MILISECONS{10};

	Q_OBJECT
};



#endif // DOWNLOADS_STATUS_H
