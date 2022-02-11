#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>

#include "downloads_status.h"

class CMainWindow : public QMainWindow
{
public:
	CMainWindow(QWidget *parent = nullptr);
	~CMainWindow();

private:

private:
	QLineEdit *m_downloadInput;
	QPushButton *m_downloadButton;
	DownloadsData::EDownloadStatus *m_downloadsStatus;

	Q_OBJECT
};
#endif // MAINWINDOW_H
