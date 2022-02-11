#ifndef DOWNLOAD_WIDGET_H
#define DOWNLOAD_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

#include "download_data.h"

class CDownloadWidget : public QWidget
{
public:
	explicit CDownloadWidget(DownloadsData::DownloadEntry pEntryToUse, QWidget *pParrent = nullptr);
	void UpdateData() noexcept;
signals:
	// void CancelClicked(DownloadsData::DownloadEntry entryToCancell);
	
private:

private:
	const QSharedPointer<const DownloadsData::SDownloadEntry> m_pCurrentEntry;

	QLabel *m_nameLabel;
	QLabel *m_pathLabel;

	QProgressBar *m_currentProgress;

	// QPushButton *m_cancelButton; // For future use

	Q_OBJECT
};

#endif // DOWNLOAD_WIDGET_H
