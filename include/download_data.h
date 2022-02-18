#ifndef DOWNLOAD_DATA_H
#define DOWNLOAD_DATA_H

#include <QFileInfo>

#include "uri.h"
#include <filesystem>

namespace DownloadsData {
	struct SDownloadEntry;

	using DownloadEntry = QSharedPointer<SDownloadEntry>;

	enum class EDownloadStatus
	{
		None,
		Downloading,
		Done,
		Failed
	};

	struct SDownloadEntry
	{
	public:
		explicit SDownloadEntry(CURI uriToDownload) : 
			m_uriDownloading{uriToDownload}
		{

		}

		QString GetStatusString() const noexcept;
		inline QString GetPath() const noexcept
		{
			return m_savedFile.absoluteFilePath();
		}
		inline QString GetName() const noexcept
		{
			return m_savedFile.fileName();
		}

		CURI m_uriDownloading;

		QFileInfo m_savedFile;

		EDownloadStatus m_status{EDownloadStatus::None};

		int m_iDownloadProgress{0};
	};

} // DownloadsData 

#endif // DOWNLOAD_DATA_H
