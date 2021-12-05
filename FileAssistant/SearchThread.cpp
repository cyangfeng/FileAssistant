
#include "SearchThread.h"

namespace fileassistant {

	SearchThread::SearchThread()
	{

	}

	void SearchThread::SetFilePath(const QString& filePath)
	{
		m_sFilePath = filePath;
	}

	QFileInfoList SearchThread::GetFileInfoList() const
	{
		return m_fileInfoList;
	}

	void SearchThread::run()
	{
		if (!m_fileInfoList.isEmpty())
		{
			m_fileInfoList.clear();
		}

		if (!m_sFilePath.isEmpty())
		{
			FindFile(m_sFilePath);
		}
	}

	bool SearchThread::FindFile(const QString& filePath)
	{
		QDir dir(filePath);   //QDir��·��һ��Ҫ��ȫ·�������·�����д���
		if (!dir.exists())
			return false;

		//ȡ�����е��ļ����ļ�����ȥ��.��..�ļ���
		dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
		dir.setSorting(QDir::DirsFirst);
		//����ת��Ϊһ��list
		QFileInfoList list = dir.entryInfoList();
		if (list.size() < 1)
		{
			return false;
		}

		int i = 0;
		//���õݹ��㷨
		do {
			QFileInfo fileInfo = list[i];
			if (fileInfo.isDir())
			{
				emit PathChanged(fileInfo.filePath());
				FindFile(fileInfo.filePath());
			}
			else
			{
				m_fileInfoList.append(fileInfo);
				qDebug() << fileInfo.filePath() << ":" << fileInfo.fileName();
			}

			++i;
		} while (i < list.size());

		return true;
	}
}