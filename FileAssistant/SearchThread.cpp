
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
		QDir dir(filePath);   //QDir的路径一定要是全路径，相对路径会有错误
		if (!dir.exists())
			return false;

		//取到所有的文件和文件名，去掉.和..文件夹
		dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
		dir.setSorting(QDir::DirsFirst);
		//将其转化为一个list
		QFileInfoList list = dir.entryInfoList();
		if (list.size() < 1)
		{
			return false;
		}

		int i = 0;
		//采用递归算法
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