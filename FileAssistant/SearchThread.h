#pragma once
#include <QThread>
#include <QFileInfoList>

class SearchThread : public QThread
{
	Q_OBJECT

public:
	SearchThread();

	void SetFilePath(const QString& filePath);
	void SetRange(qint64 nMin, qint64 nMax);
	QFileInfoList GetFileInfoList() const;

protected:

	void run() override;

signals:
	void PathChanged(const QString& sPath);

private:

	bool FindFile(const QString& filePath);

	qint64 m_nMaxSize = 0;
	qint64 m_nMinSize = 0;
	QString m_sFilePath;
	QFileInfoList m_fileInfoList;
};

