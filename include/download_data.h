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
		CURI m_uriDownloading;

		QFileInfo m_savedFile;

		EDownloadStatus m_status;

		int m_iDownloadProgress;

		QString GetStatusString() const noexcept;
		inline QString GetPath() const noexcept
		{
			return m_savedFile.absolutePath();
		}
		inline QString GetName() const noexcept
		{
			return m_savedFile.fileName();
		}
	};

} // DownloadsData 

#endif // DOWNLOAD_DATA_H
