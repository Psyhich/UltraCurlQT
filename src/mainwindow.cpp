#include <QGridLayout>
#include <QFileDialog>
#include <QDebug>

#include "mainwindow.h"

CMainWindow::CMainWindow(QWidget *parent)
  : QMainWindow(parent)
{
	QWidget *pCentralWidget = new QWidget(this);
	setCentralWidget(pCentralWidget);

	QGridLayout *layout = new QGridLayout(pCentralWidget);
	pCentralWidget->setLayout(layout);

	m_downloadInput = new QLineEdit(pCentralWidget);
	layout->addWidget(m_downloadInput, 0, 0, 1, 3);

	m_downloadButton = new QPushButton(pCentralWidget);
	m_downloadButton->setText("Download");
	layout->addWidget(m_downloadButton, 0, 3, 1, 1);

	m_downloadsHolder = new CStatusWidget();
	layout->addWidget(m_downloadsHolder, 1, 0, 2, 4);

	// Connecting button clicks and start of download
	QObject::connect(m_downloadButton, &QPushButton::clicked, this, &CMainWindow::CreateDownload);
}

void CMainWindow::CreateDownload() noexcept
{
	QString sURIString = m_downloadInput->text();
	if(sURIString.isEmpty())
	{
		return;
	}
	// Firstly asking user for download destination
	QString path = QFileDialog::getSaveFileName(this, "Choose where to save");
	if(!path.isEmpty())
	{
		m_downloadsHolder->AddDownload(sURIString, QFileInfo(path));
	}
}

CMainWindow::~CMainWindow()
{
}

