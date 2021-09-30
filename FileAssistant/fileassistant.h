#pragma once

#include <QtWidgets/QDialog>
#include <QFileInfoList>
#include <QStandardItemModel>
#include "ui_fileassistant.h"
#include "SearchThread.h"
#include "DealFileThread.h"

enum FileState {
	STATE_COPY,
	STATE_COMPRESS = 3,
	STATE_DECOMPRESS,
    STATE_NONE
};

class XItemData
{
public:
	XItemData() {};
	XItemData(const QFileInfo& fileInfo, const FileState& fileState)
	{
		m_fileState = fileState;
		m_fileInfo = fileInfo;
	};
	~XItemData() {};

	QFileInfo m_fileInfo;
	FileState m_fileState;
};
Q_DECLARE_METATYPE(XItemData)  //注册，必不可少

class FileAssistant : public QDialog
{
    Q_OBJECT

public:
    FileAssistant(QWidget *parent = Q_NULLPTR);

protected:
    void resizeEvent(QResizeEvent* event);

private:
    void Initial();

    void OnBtnBowers();
    void OnSearch();
    void OnDoing();
    void OnShowDoneFile();
    void OnKeyword();
    void OnDealTypeChanged(int nIndex);
    void OnTableRightClicked(const QPoint&);

    // 列表右键事件
    void OnReCheck();
    void OnOpen();
    void OnDelete();

    void OnSearchStarted();
    void OnSearchFinished();
    void OnPathChanged(const QString& sPath);

	void OnDealStarted();
	void OnDealFinished();
    void OnValueChanged(int nValue);

    QString ConvertFielSize(qint64 nSize);
    void UpdateListItems(int nIndex);

    bool IsMoiveFile(const QFileInfo& fileInfo);
    bool IsMusicFIle(const QFileInfo& fileInfo);
    bool IsDocumentFile(const QFileInfo& fileInfo);
    bool IsCompressFile(const QFileInfo& fileInfo);

    FileState GetFileState(const QFileInfo& fileInfo);
    QString GetFileStateName(const QFileInfo& fileInfo);
    bool ContainKeyword(const QString& fileName) const;

    void RemoveInvalidFileInfo(const QFileInfo& fileInfo);
    void EnableControls(bool bEable = true);

private:
    Ui::FileAssistantClass ui;

    SearchThread m_searchThread;
    DealFileThread m_dealThread;
    QFileInfoList m_fileInfoList;
    QMap<QString, FileState> m_mapDoneFile;

    QMenu *m_pMenu;
    std::unique_ptr<QStandardItemModel> m_upModel;
};
