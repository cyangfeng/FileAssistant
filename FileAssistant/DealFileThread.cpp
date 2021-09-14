#include "DealFileThread.h"
#include <QFileDialog>
#include <QDataStream>
#include <QDebug>

#include "include/JlCompress.h"


DealFileThread::DealFileThread()
{

}

void DealFileThread::SetCopyOptions(const QString& fromPath, const QString& toPath, int nRow, bool bRemove /*= false*/, bool bCreateDir /*= false*/)
{
	m_toPath = toPath;
	m_fromPath = fromPath;
	m_bRemove = bRemove;
	m_bCreateDir = false;
	m_nRow = nRow;
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
	case 1:
	{
		if (CopyFile())
		{
			if (m_bRemove)
			{
				if (QFile::remove(m_fromPath))
				{
					m_bSucessed = true;
				}
				else
				{
					QFile::remove(m_toPath);
					qDebug() << m_fromPath << ": �ƶ�ʧ�ܣ�";
				}
			}
			else
			{
				m_bSucessed = true;
			}
		}
		else
		{
			qDebug() << m_fromPath << ": ����ʧ�ܣ�";
		}
		break;
	}
	case 3:
	{
		m_bSucessed = CompressFiles();
	}
	default:
		break;
	}

}

bool DealFileThread::CopyFile()
{
	//���·�������ڣ��򴴽�
	QDir toDir;
	if (!toDir.exists(m_toPath))
	{
		if (m_bCreateDir)
		{
			toDir.mkpath(m_toPath);
		}
		else
		{
			qDebug() << m_toPath << ": ·�������ڣ�";
			return false;
		}
	}

	int nPos = m_fromPath.lastIndexOf("/");
	QString fileName = m_fromPath.right(m_fromPath.length() - nPos - 1);
	m_toPath = m_toPath + "/" + fileName;
	char byteTemp[4096] = {0};//�ֽ�����
	QFile tofile;
	tofile.setFileName(m_toPath);
	tofile.open(QIODevice::WriteOnly);
	QDataStream out(&tofile);
	out.setVersion(QDataStream::Qt_4_8);

	QFile fromfile;
	fromfile.setFileName(m_fromPath);
	if (!fromfile.open(QIODevice::ReadOnly)) {
		qDebug() << "open fromfile failed������";
		return false;
	}
	int fileSize = fromfile.size();
	QDataStream in(&fromfile);
	in.setVersion(QDataStream::Qt_4_8);

	int totalCopySize = 0;
	while (!in.atEnd())
	{
		int readSize = 0;
		//�����ֽ�����,���ض�ȡ���ֽ����������С��4096�������ļ�β
		readSize = in.readRawData(byteTemp, 4096);
		out.writeRawData(byteTemp, readSize);
		totalCopySize += readSize;

		int nRatio = (double)totalCopySize / fileSize * 100;
		QString strRatio;
		if (m_bRemove)
		{
			strRatio = QStringLiteral("�ƶ���: ") + std::to_string(nRatio).c_str();
		} 
		else
		{
			strRatio = QStringLiteral("������: ") + std::to_string(nRatio).c_str();
		}
		emit ProgressChanged(strRatio);
	}
	if (totalCopySize == fileSize) {
		tofile.setPermissions(QFile::ExeUser);
		return true;
	}
	else
		return false;
}

bool DealFileThread::CompressFiles()
{
	return JlCompress::compressFiles(m_toPath, m_FromFiles);
}

