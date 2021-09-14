#pragma once

#include <QtWidgets/QDialog>
#include <QFileInfoList>
#include <QStandardItemModel>
#include "ui_fileassistant.h"
#include "SearchThread.h"
#include "DealFileThread.h"

enum DoneType {
    DONE_COPY,
    DONE_MOVE,
    DONE_COMPRESS,

};

class XItemData
{
public:
    XItemData() {};
    XItemData(const QFileInfo& fileInfo, const QString& fileState)
    {
        m_fileState = fileState;
        m_fileInfo = fileInfo;
    };
    ~XItemData() {};

    QFileInfo m_fileInfo;
    QString m_fileState;
};
Q_DECLARE_METATYPE(XItemData)  //注册，必不可少

class FileAssistant : public QDialog
{
    Q_OBJECT

public:
    FileAssistant(QWidget *parent = Q_NULLPTR);

private:
    void Initial();

    void OnBtnBowers();
    void OnSearch();
    void OnDoing();
    void OnShowDoneFile();

    void OnSearchStarted();
    void OnSearchFinished();
    void OnPathChanged(const QString& sPath);

	void OnDealStarted();
	void OnDealFinished();
	void OnProcessChanged(const QString& sRatio);

    QString ConvertFielSize(qint64 nSize);
    void UpdateListItems(int nIndex);

    bool IsMoiveFile(const QFileInfo& fileInfo);
    bool IsMusicFIle(const QFileInfo& fileInfo);
    bool IsDocumentFile(const QFileInfo& fileInfo);
    bool IsCompressFile(const QFileInfo& fileInfo);

    QString GetFileState(const QFileInfo& fileInfo);

private:
    Ui::FileAssistantClass ui;

    SearchThread m_searchThread;
    DealFileThread m_dealThread;
    QFileInfoList m_fileInfoList;
    QMap<QString, QString> m_setDoneFile;

    std::unique_ptr<QStandardItemModel> m_pModel;
};
