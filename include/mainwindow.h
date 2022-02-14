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

private slots:
	void CreateDownload() noexcept;

private:
	QLineEdit *m_downloadInput;
	QPushButton *m_downloadButton;

	CStatusWidget *m_downloadsHolder;

	Q_OBJECT
};
#endif // MAINWINDOW_H
