#pragma once
#include <qthread.h>
#include <QStandardItem>

class DealFileThread : public QThread
{
	Q_OBJECT
public:
	DealFileThread();
	
	int GetDealType() const { return m_nType; }
	int GetItemRow() const { return m_nRow; }
	QList<int> GetItemRows() const { return m_listRows; }
	bool GetSucessed() const { return m_bSucessed; }
	void SetDealType(int nType) { m_nType = nType; }
	void SetCopyOptions(const QString& fromPath, const QString& toPath, int nRow, bool bRemove = false, bool bCreateDir = false);
	void SetCompressOptions(const QStringList& fromFiles, const QString& toPath, const QList<int>& listRows);

protected:
	void run() override;

private:

	bool CopyFile();
	bool CompressFiles();

signals:
	void ProgressChanged(const QString& sRatio);

private:

	int m_nType;

	int	 m_nRow = 0;
	bool m_bSucessed = false;
	bool m_bCreateDir = false;
	bool m_bRemove = false;
	QString m_fromPath;
	QString	m_toPath;

	QStringList m_FromFiles;
	QList<int> m_listRows;
};

