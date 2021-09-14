#include "fileassistant.h"
#include <QFileDialog>
#include <QStandardItemModel>
#include <QFileIconProvider>
#include <QDataStream>
#include <QDebug>

FileAssistant::FileAssistant(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    Initial();
}

void FileAssistant::Initial()
{
    ui.cboFileType->addItem(QStringLiteral("所有"));
    ui.cboFileType->addItem(QStringLiteral("视频"));
    ui.cboFileType->addItem(QStringLiteral("音乐"));
    ui.cboFileType->addItem(QStringLiteral("文档"));
    ui.cboFileType->addItem(QStringLiteral("压缩文件"));

    ui.spinFrom->setSingleStep(0.5);
    ui.spinFrom->setValue(0.0);
	ui.spinTo->setSingleStep(0.5);
    ui.spinTo->setRange(0.0, 10240.0);
	ui.spinTo->setValue(10240.0);

	ui.cboDoType->addItem(QStringLiteral("复制"));
	ui.cboDoType->addItem(QStringLiteral("移动"));
    ui.cboDoType->addItem(QStringLiteral("删除"));
	ui.cboDoType->addItem(QStringLiteral("压缩"));
	ui.cboDoType->addItem(QStringLiteral("解压"));

    m_pModel.reset(new QStandardItemModel());

    m_pModel->setColumnCount(6);
    m_pModel->setHeaderData(0, Qt::Horizontal, QStringLiteral("X"));
    m_pModel->setHeaderData(1, Qt::Horizontal, QStringLiteral("文件名"));
    m_pModel->setHeaderData(2, Qt::Horizontal, QStringLiteral("位置"));
    m_pModel->setHeaderData(3, Qt::Horizontal, QStringLiteral("大小"));
    m_pModel->setHeaderData(4, Qt::Horizontal, QStringLiteral("类型"));
    m_pModel->setHeaderData(5, Qt::Horizontal, QStringLiteral("状态"));

	ui.tableView->setModel(m_pModel.get());

	//表头信息显示居左
	ui.tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui.tableView->verticalHeader()->setHidden(true);
    // 设置列宽
    ui.tableView->setColumnWidth(0, 10);
	ui.tableView->setColumnWidth(1, 150);
    ui.tableView->setColumnWidth(2, 280);
    ui.tableView->setColumnWidth(3, 70);
    ui.tableView->setColumnWidth(4, 60);
    ui.tableView->setColumnWidth(5, 100);

    ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);    //禁止编辑
    ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);   //选择整行

    // 隐藏进度条
    ui.progressBar->hide();

    ui.editKeyword->setEnabled(false);

    // 槽函数绑定
    connect(ui.btnBowers1, &QPushButton::clicked, this, &FileAssistant::OnBtnBowers);
    connect(ui.btnBowers2, &QPushButton::clicked, this, &FileAssistant::OnBtnBowers);
    connect(ui.btnFind, &QPushButton::clicked, this, &FileAssistant::OnSearch);
    connect(ui.cboFileType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FileAssistant::UpdateListItems);
    connect(ui.btnDoing, &QPushButton::clicked, this, &FileAssistant::OnDoing);
    connect(ui.checkBox, &QPushButton::clicked, this, &FileAssistant::OnShowDoneFile);
    connect(ui.checkKeyword, &QPushButton::clicked, this, &FileAssistant::OnKeyword);

    connect(&m_searchThread, &SearchThread::started, this, &FileAssistant::OnSearchStarted);
    connect(&m_searchThread, &SearchThread::finished, this, &FileAssistant::OnSearchFinished);
    connect(&m_searchThread, &SearchThread::PathChanged, this, &FileAssistant::OnPathChanged);

	connect(&m_dealThread, &DealFileThread::started, this, &FileAssistant::OnDealStarted);
	connect(&m_dealThread, &DealFileThread::finished, this, &FileAssistant::OnDealFinished);
    connect(&m_dealThread, &DealFileThread::ValueChanged, this, &FileAssistant::OnValueChanged);
}

void FileAssistant::OnBtnBowers()
{
	QString filePath;
    QFileDialog filedlg(this);
    filedlg.setFileMode(QFileDialog::DirectoryOnly);
     if (filedlg.exec())
     {
        QStringList strlist = filedlg.selectedFiles();
        if (!strlist.empty())
        {
            filePath = strlist[0];
        }
     }

     QPushButton* pButton = qobject_cast<QPushButton*>(sender());
     if (pButton == ui.btnBowers1)
     {
         ui.editLocation1->setText(filePath);
     } 
     else // ui.btnBowers2
     {
         ui.editLocation2->setText(filePath);
     }
}

void FileAssistant::OnSearch()
{
	QString filePath = ui.editLocation1->text();
	if (filePath.isEmpty())
		return;
    qint64 nMin = ui.spinFrom->value() * 1024 * 1024;
    qint64 nMax = ui.spinTo->value() * 1024 * 1024;
    m_searchThread.SetRange(nMin, nMax);
    m_searchThread.SetFilePath(filePath);

    m_searchThread.start();
}

void FileAssistant::OnDoing()
{
    QList<int> listRows;
	QStringList fromFiles;
	int nSize = m_pModel->rowCount();
	int nIndex = ui.cboDoType->currentIndex();
    m_dealThread.SetDealType(nIndex);
    switch (nIndex)
    {
    case 0: // copy
    case 1: // move
    {
        for (int i = 0; i < nSize; i++)
        {
            Qt::CheckState state = m_pModel->item(i)->checkState();
            if (state == Qt::Checked)
            {
                auto pItem = m_pModel->item(i);
                auto itemData = pItem->data().value<XItemData>();
                fromFiles.push_back(itemData.m_fileInfo.filePath());
                listRows.push_back(i);
            }
        }

        if (nIndex == 0)
        {
			ui.labelInfo->setText(QStringLiteral("复制中："));
        } 
        else
        {
			ui.labelInfo->setText(QStringLiteral("移动中："));
        }
        ui.progressBar->setRange(0, fromFiles.size());
		ui.progressBar->show();
		QString toPath = ui.editLocation2->text();
		bool bAutoCreate = ui.checkAuto->checkState() == Qt::Checked ? true : false;
		m_dealThread.SetCopyOptions(fromFiles, toPath, listRows, bAutoCreate);
		m_dealThread.start();
        break;
    }
    case 2: // delete
    {
        for (int i = 0; i < nSize; i++)
        {
            Qt::CheckState state = m_pModel->item(i)->checkState();
            if (state == Qt::Checked)
            {
				if (state == Qt::Checked)
				{
					auto itemData = m_pModel->item(i)->data().value<XItemData>();
					fromFiles.push_back(itemData.m_fileInfo.filePath());
					listRows.push_back(i);
				}
            }
        }

		ui.labelInfo->setText(QStringLiteral("删除中："));
		ui.progressBar->setRange(0, fromFiles.size());
		ui.progressBar->show();
		m_dealThread.SetDeleteOptions(fromFiles, listRows);
		m_dealThread.start();
        break;
    }
    case 3: // Compress
    {
        for (int i = 0; i < nSize; i++)
        {
            Qt::CheckState state = m_pModel->item(i)->checkState();
            if (state == Qt::Checked)
            {
                auto itemData = m_pModel->item(i)->data().value<XItemData>();
                fromFiles.push_back(itemData.m_fileInfo.filePath());
                listRows.push_back(i);
            }
        }

        ui.labelInfo->setText(QStringLiteral("压缩中："));
		ui.progressBar->setRange(0, 10);
        ui.progressBar->setValue(3);
		ui.progressBar->show();
		QString toPath = ui.editLocation2->text() + "/" + ui.lineEdit->text() + ".zip";
		m_dealThread.SetCompressOptions(fromFiles, toPath, listRows);
		m_dealThread.start();
        break;
    }
    case 4:
    {
		for (int i = 0; i < nSize; i++)
		{
			Qt::CheckState state = m_pModel->item(i)->checkState();
			if (state == Qt::Checked)
			{
				auto itemData = m_pModel->item(i)->data().value<XItemData>();
				fromFiles.push_back(itemData.m_fileInfo.filePath());
				listRows.push_back(i);
			}
		}

		ui.labelInfo->setText(QStringLiteral("解压中："));
		ui.progressBar->setRange(0, fromFiles.size());
        ui.progressBar->show();
		QString toPath = ui.editLocation2->text();
		m_dealThread.SetCompressOptions(fromFiles, toPath, listRows);
		m_dealThread.start();
        break;
    }
    default:
        break;
    }
}

void FileAssistant::OnShowDoneFile()
{
    UpdateListItems(ui.cboFileType->currentIndex());
}

void FileAssistant::OnKeyword()
{
    if (ui.checkKeyword->checkState() == Qt::Checked)
    {
        ui.editKeyword->setEnabled(true);
    } 
    else
    {
        ui.editKeyword->setEnabled(false);
    }
}

void FileAssistant::OnSearchStarted()
{
    m_fileInfoList.clear();
    m_pModel->removeRows(0, m_pModel->rowCount()); //删除所有行

    ui.labelInfo->setText(QStringLiteral("开始搜索") + ui.editLocation1->text() + "...");
    ui.btnBowers1->setEnabled(false);
    ui.btnBowers2->setEnabled(false);
	ui.btnFind->setEnabled(false);
	ui.btnDoing->setEnabled(false);
}

void FileAssistant::OnSearchFinished()
{
	m_fileInfoList = m_searchThread.GetFileInfoList();

    int nIndex = ui.cboFileType->currentIndex();
    UpdateListItems(nIndex);

	ui.btnBowers1->setEnabled(true);
	ui.btnBowers2->setEnabled(true);
	ui.btnFind->setEnabled(true);
	ui.btnDoing->setEnabled(true);

    ui.labelInfo->setText("");
}

void FileAssistant::OnPathChanged(const QString& sPath)
{
    QString sFilePath = QStringLiteral(" 正在搜索:  ") + sPath;
    if (sFilePath.length() > 80)
    {
        sFilePath = sFilePath.left(80) + "...";
    }

    ui.labelInfo->setText(sFilePath);
}

void FileAssistant::OnDealStarted()
{
//     ui.groupBox->setEnabled(false);
//     ui.groupBox_2->setEnabled(false);
}

void FileAssistant::OnDealFinished()
{
	if (m_dealThread.GetSucessed())
	{
		auto listRows = m_dealThread.GetItemRows();
		for (const auto nRow : listRows)
		{
			auto pItem = m_pModel->item(nRow);
			auto itemData = pItem->data().value<XItemData>();
			if (itemData.m_fileState == STATE_NONE)
			{
                int nType = m_dealThread.GetDealType();
				switch (nType)
				{
				case 0:
                    m_mapDoneFile.insert(itemData.m_fileInfo.fileName(), STATE_COPY);
                    break;
				case 1:
                case 2:
                    RemoveInvalidFileInfo(itemData.m_fileInfo);
					break;
				case 3:
                    m_mapDoneFile.insert(itemData.m_fileInfo.fileName(), STATE_COMPRESS);
                    ui.progressBar->setValue(10);
                    break;
				case 4:
                    m_mapDoneFile.insert(itemData.m_fileInfo.fileName(), STATE_DECOMPRESS);
					break;
				default:
					break;
				}
			}
		}
		int nIndex = ui.cboFileType->currentIndex();
		UpdateListItems(nIndex);
	}

    ui.progressBar->hide();
    ui.labelInfo->setText("");
	ui.groupBox->setEnabled(true);
	ui.groupBox_2->setEnabled(true);
}

void FileAssistant::OnValueChanged(int nValue)
{
    ui.progressBar->setValue(nValue);
}

QString FileAssistant::ConvertFielSize(qint64 nSize)
{
    QString sSize;
    if (nSize > 1024 * 1024)
    {
        return QString().sprintf("%0.2fMB", (double)nSize / (1024 * 1024));
    }
    else if(nSize > 1024)
    {
        return QString().sprintf("%0.2fKB", (double)nSize / 1024);
    }
    else
    {
        return QString().sprintf("% 4dB", nSize);
    }
}

void FileAssistant::UpdateListItems(int nIndex)
{
    m_pModel->removeRows(0, m_pModel->rowCount()); //删除所有行
    auto InsertItem = [this](int nRow, const QFileInfo& fileInfo){

		QIcon icon = QFileIconProvider().icon(fileInfo);
		auto pItem = new QStandardItem();
		pItem->setCheckable(true);
        FileState fileState = GetFileState(fileInfo);
        XItemData data(fileInfo, fileState);
        QVariant variant;
        variant.setValue(data);
        pItem->setData(variant);
		m_pModel->setItem(nRow, 0, pItem);
		m_pModel->setItem(nRow, 1, new QStandardItem(icon, fileInfo.baseName()));
		m_pModel->setItem(nRow, 2, new QStandardItem(fileInfo.path()));
		m_pModel->setItem(nRow, 3, new QStandardItem(ConvertFielSize(fileInfo.size())));
		m_pModel->setItem(nRow, 4, new QStandardItem(fileInfo.suffix()));
		m_pModel->setItem(nRow, 5, new QStandardItem(GetFileStateName(fileInfo.fileName())));
    };

    int nRow = 0;
	for (const auto fileInfo : m_fileInfoList)
	{
		if (ui.checkBox->checkState() != Qt::Checked && \
            GetFileState(fileInfo) != STATE_NONE)
		{
			continue;
		}

		if (ui.checkKeyword->checkState() == Qt::Checked && \
            !ContainKeyword(fileInfo.fileName()))
		{
			continue;
		}

        switch (nIndex)
        {
        case 0: // All
            InsertItem(nRow++, fileInfo);
            break;
        case 1: // Moive
            if (IsMoiveFile(fileInfo))
            {
                InsertItem(nRow++, fileInfo);
            }
            break;
        case 2:
            if (IsMusicFIle(fileInfo))
            {
                InsertItem(nRow++, fileInfo);
            }
            break;
        case 3:
            if (IsDocumentFile(fileInfo))
            {
                InsertItem(nRow++, fileInfo);
            }
            break;
        case 4:
            if (IsCompressFile(fileInfo))
            {
                InsertItem(nRow++, fileInfo);
            }
            break;
        default:
            break;
        }
	}
}

bool FileAssistant::IsMoiveFile(const QFileInfo& fileInfo)
{
    QStringList strList{ "avi", "rmvb", "rm", "asf", "divx", "mpg" ,"mpeg",\
                         "mpe", "wmv" ,"mp4" ,"mkv" ,"vob", "flv" };
    for (const auto x : strList)
    {
        if (x == fileInfo.suffix())
        {
            return true;
        }
    }
    return false;
}

bool FileAssistant::IsMusicFIle(const QFileInfo& fileInfo)
{
    QStringList strList{"mp3", "wma", "wav", "mod", "ra", "cd", "md", "asf", "acc", "mp3pro", \
                        "vqf", "flac", "ape", "mid", "ogg", "m4a", "acc+", "fiff", "au", "vqf" };
	for (const auto x : strList)
	{
		if (x == fileInfo.suffix())
		{
			return true;
		}
	}
	return false;
}

bool FileAssistant::IsDocumentFile(const QFileInfo& fileInfo)
{
	QStringList strList{ "doc", "docx", "xls", "xlsx", "pdf", "txt" ,"pdf", "ofd" };
	for (const auto x : strList)
	{
		if (x == fileInfo.suffix())
		{
			return true;
		}
	}
	return false;
}

bool FileAssistant::IsCompressFile(const QFileInfo& fileInfo)
{
	QStringList strList{ "rar", "tar", "zip", "gzip", "cab", "uue" ,"arj",\
                        "bz2", "lzh", "jar", "ace", "ios", "7-zip", "z" };
	for (const auto x : strList)
	{
		if (x == fileInfo.suffix())
		{
			return true;
		}
	}
	return false;
}

FileState FileAssistant::GetFileState(const QFileInfo& fileInfo)
{
    auto it = m_mapDoneFile.find(fileInfo.fileName());
    if (it == m_mapDoneFile.end())
    {
        return STATE_NONE;
    }
    else
    {
        return m_mapDoneFile[fileInfo.fileName()];
    }
}

QString FileAssistant::GetFileStateName(const QFileInfo& fileInfo)
{
    FileState fileState = GetFileState(fileInfo);
    switch (fileState)
    {
    case STATE_NONE:
        return QStringLiteral("未处理");
    case STATE_COPY:
        return QStringLiteral("已复制");
    case STATE_COMPRESS:
        return QStringLiteral("已压缩");
    case STATE_DECOMPRESS:
        return QStringLiteral("已解压");
    default:
        return QStringLiteral("未处理");
    }
}

bool FileAssistant::ContainKeyword(const QString& fileName) const
{
    QString sKeyword = ui.editKeyword->text();
    if (fileName.indexOf(sKeyword) == -1)
    {
        return false;
    }
    return true;
}

void FileAssistant::RemoveInvalidFileInfo(const QFileInfo& fileInfo)
{
	auto itr = m_fileInfoList.begin();
	for (; itr != m_fileInfoList.end(); itr++)
	{
		if (*itr == fileInfo)
		{
			m_fileInfoList.erase(itr);
			break;
		}
	}
}
