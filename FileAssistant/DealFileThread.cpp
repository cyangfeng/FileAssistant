#include "DealFileThread.h"
#include <QFileDialog>
#include <QDebug>

#include "include/JlCompress.h"


DealFileThread::DealFileThread()
{

}

void DealFileThread::SetDeleteOptions(const QStringList& fromFiles, const QList<int>& listRows)
{
	m_FromFiles = fromFiles;
	m_listRows = listRows;
}

void DealFileThread::SetCopyOptions(const QStringList& fromFiles, const QString& toPath, const QList<int>& listRow, bool bCreateDir /*= false*/)
{
	m_toPath = toPath;
	m_FromFiles = fromFiles;
	m_listRows = listRow;
	m_bCreateDir = false;
}

void DealFileThread::SetCompressOptions(const QStringList& fromFiles, const QString& toPath, const QList<int>& listRows)
{
	m_FromFiles = fromFiles;
	m_toPath = toPath;
	m_listRows = listRows;
}

void DealFileThread::run()
{
	switch (m_nType)
	{
	case 0:
	{
		m_bSucessed = CopyFiles();
		break;
	}
	case 1:
	{
		m_bSucessed = MoveFiles();
		break;
	}
	case 2:
	{
		m_bSucessed = DeleteFiles();
		break;
	}
	case 3:
	{
		m_bSucessed = CompressFiles();
		break;
	}
	case 4:
	{
		m_bSucessed = DeCompressFile();
		break;
	}
	default:
		break;
	}

}

bool DealFileThread::CopyFiles()
{
	int nCount = 0;
	for (const auto& filePath : m_FromFiles)
	{
		QString fileName = filePath.right(filePath.length() - filePath.lastIndexOf('/') - 1);
		if (!QFile::copy(filePath, m_toPath + "/" + fileName))
		{
			qDebug() << fileName << QStringLiteral(" ¸´ÖÆÊ§°Ü£¡");
			return false;
		}
		emit ValueChanged(++nCount);
	}
	return true;
}

bool DealFileThread::MoveFiles()
{
	int nCount = 0;
	for (const auto& filePath : m_FromFiles)
	{
		QString fileName = filePath.right(filePath.length() - filePath.lastIndexOf('/') - 1);
		if (!QFile::rename(filePath, m_toPath + "/" + fileName))
		{
			qDebug() << fileName << QStringLiteral("ÒÆ¶¯Ê§°Ü£¡");
			return false;
		}
		emit ValueChanged(++nCount);
	}
	return true;
}

bool DealFileThread::DeleteFiles()
{
	int nCount = 0;
	for (const auto& fileName : m_FromFiles)
	{
		if (!QFile::remove(fileName))
		{
			return false;
		}
		emit ValueChanged(++nCount);
	}
	return true;
}

bool DealFileThread::CompressFiles()
{
	return JlCompress::compressFiles(m_toPath, m_FromFiles);
}

bool DealFileThread::DeCompressFile()
{
	int nCount = 0;
	for (const auto filePath : m_FromFiles)
	{
		QStringList fileList = JlCompress::extractDir(filePath, m_toPath);
		if (fileList.empty())
		{
			return false;
		}
		emit ValueChanged(++nCount);
	}

	return true;
}

