#pragma once
#include <QThread>
#include <QFileInfoList>

class SearchThread : public QThread
{
	Q_OBJECT

public:
	SearchThread();

	void SetFilePath(const QString& filePath);
	QFileInfoList GetFileInfoList() const;

protected:

	void run() override;

signals:
	void PathChanged(const QString& sPath);

private:

	bool FindFile(const QString& filePath);

	QString m_sFilePath;
	QFileInfoList m_fileInfoList;
};

