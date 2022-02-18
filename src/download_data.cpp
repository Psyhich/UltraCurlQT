#include "download_data.h"

QString DownloadsData::SDownloadEntry::GetStatusString() const noexcept
{
	switch(m_status) {
		case EDownloadStatus::Downloading:
		{
			return QString{"Currently downloading"};
		}
		case EDownloadStatus::Done:
		{
			return QString{"Done"};
		}
		case EDownloadStatus::Failed:
		{
			return QString{"Failed to download"};
		}
		default:
		{
			return QString{"No status"};
		}

	}
}
