#pragma once
#include <qthread.h>
#include <QStandardItem>

class DealFileThread : public QThread
{
	Q_OBJECT
public:
	DealFileThread();
	
	int GetDealType() const { return m_nType; }
	QList<int> GetItemRows() const { return m_listRows; }
	bool GetSucessed() const { return m_bSucessed; }
	void SetDealType(int nType) { m_nType = nType; }
	void SetDeleteOptions(const QStringList& fromFiles, const QList<int>& listRows);
	void SetCopyOptions(const QStringList& fromFiles, const QString& toPath, const QList<int>& listRow, bool bCreateDir = false);
	void SetCompressOptions(const QStringList& fromFiles, const QString& toPath, const QList<int>& listRows);

protected:
	void run() override;

private:

	bool CopyFiles();
	bool MoveFiles();
	bool DeleteFiles();
	bool CompressFiles();
	bool DeCompressFile();

signals:
	void ValueChanged(int nValue);

private:

	int m_nType;

	bool m_bSucessed = false;
	bool m_bCreateDir = false;
	QString	m_toPath;

	QStringList m_FromFiles;
	QList<int> m_listRows;
};

