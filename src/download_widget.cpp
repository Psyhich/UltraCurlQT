#include <QGridLayout>
#include <QSizePolicy>

#include "download_widget.h"

CDownloadWidget::CDownloadWidget(DownloadsData::DownloadEntry pEntryToUse, QWidget *pParrent) : 
	QWidget(pParrent), m_pCurrentEntry{pEntryToUse}
{
	QGridLayout *pCurrentLayout = new QGridLayout(this);
	pCurrentLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);

	setLayout(pCurrentLayout);

	m_nameLabel = 
		new QLabel(m_pCurrentEntry->GetName(), this);
	pCurrentLayout->addWidget(m_nameLabel, 0, 0, 1, 1);


	m_pathLabel = 
		new QLabel(m_pCurrentEntry->GetPath(), this);
	pCurrentLayout->addWidget(m_pathLabel, 0, 1, 1, 2);

	m_currentProgress = new QProgressBar(this);
		m_currentProgress->setRange(0, 100);
		m_currentProgress->setFormat("%p " + m_pCurrentEntry->GetStatusString());
		m_currentProgress->setValue(0);
	pCurrentLayout->addWidget(m_currentProgress, 1, 0, 1, 3);
}

void CDownloadWidget::UpdateData() noexcept
{
	m_nameLabel->setText(m_pCurrentEntry->GetName());

	m_pathLabel->setText(m_pCurrentEntry->GetPath());

	m_currentProgress->setFormat("%p " + m_pCurrentEntry->GetStatusString());
	m_currentProgress->setValue(m_pCurrentEntry->m_iDownloadProgress);
}

