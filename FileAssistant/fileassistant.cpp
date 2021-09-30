#include "fileassistant.h"
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QFileIconProvider>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QMenu>

FileAssistant::FileAssistant(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    Initial();
}

void FileAssistant::Initial()
{
    // 添加最大最小化按钮
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

    ui.cboFileType->addItem(QStringLiteral("所有"));
    ui.cboFileType->addItem(QStringLiteral("视频"));
    ui.cboFileType->addItem(QStringLiteral("音乐"));
    ui.cboFileType->addItem(QStringLiteral("文档"));
    ui.cboFileType->addItem(QStringLiteral("压缩文件"));

    ui.spinFrom->setSingleStep(0.5);
    ui.spinFrom->setValue(0.0);
	ui.spinTo->setSingleStep(5);
    ui.spinTo->setRange(0.0, 10240.0);
	ui.spinTo->setValue(10240.0);

	ui.cboDoType->addItem(QStringLiteral("复制"));
	ui.cboDoType->addItem(QStringLiteral("移动"));
    ui.cboDoType->addItem(QStringLiteral("删除"));
	ui.cboDoType->addItem(QStringLiteral("压缩"));
	ui.cboDoType->addItem(QStringLiteral("解压"));

    m_upModel.reset(new QStandardItemModel());

    m_upModel->setColumnCount(6);
    m_upModel->setHeaderData(0, Qt::Horizontal, QStringLiteral("X"));
    m_upModel->setHeaderData(1, Qt::Horizontal, QStringLiteral("文件名"));
    m_upModel->setHeaderData(2, Qt::Horizontal, QStringLiteral("位置"));
    m_upModel->setHeaderData(3, Qt::Horizontal, QStringLiteral("大小"));
    m_upModel->setHeaderData(4, Qt::Horizontal, QStringLiteral("类型"));
    m_upModel->setHeaderData(5, Qt::Horizontal, QStringLiteral("状态"));

	ui.tableView->setModel(m_upModel.get());

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
    ui.tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    // 隐藏进度条
    ui.progressBar->hide();
    ui.editKeyword->setEnabled(false);

    // 鼠标右键绑定
    m_pMenu = new QMenu(this);
    m_pMenu->addAction(QStringLiteral("反选"), this, &FileAssistant::OnReCheck);
    m_pMenu->addAction(QStringLiteral("打开"), this, &FileAssistant::OnOpen);
    m_pMenu->addAction(QStringLiteral("删除"), this, &FileAssistant::OnDelete);

    OnDealTypeChanged(ui.cboDoType->currentIndex());

    // 槽函数绑定
    connect(ui.btnBowers1, &QPushButton::clicked, this, &FileAssistant::OnBtnBowers);
    connect(ui.btnBowers2, &QPushButton::clicked, this, &FileAssistant::OnBtnBowers);
    connect(ui.btnFind, &QPushButton::clicked, this, &FileAssistant::OnSearch);
    connect(ui.btnDoing, &QPushButton::clicked, this, &FileAssistant::OnDoing);
    connect(ui.checkBox, &QPushButton::clicked, this, &FileAssistant::OnShowDoneFile);
    connect(ui.checkKeyword, &QPushButton::clicked, this, &FileAssistant::OnKeyword);
	connect(ui.tableView, &QTableView::customContextMenuRequested, this, &FileAssistant::OnTableRightClicked);
    connect(ui.cboFileType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FileAssistant::UpdateListItems);
    connect(ui.cboDoType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FileAssistant::OnDealTypeChanged);

    connect(&m_searchThread, &SearchThread::started, this, &FileAssistant::OnSearchStarted);
    connect(&m_searchThread, &SearchThread::finished, this, &FileAssistant::OnSearchFinished);
    connect(&m_searchThread, &SearchThread::PathChanged, this, &FileAssistant::OnPathChanged);

	connect(&m_dealThread, &DealFileThread::started, this, &FileAssistant::OnDealStarted);
	connect(&m_dealThread, &DealFileThread::finished, this, &FileAssistant::OnDealFinished);
    connect(&m_dealThread, &DealFileThread::ValueChanged, this, &FileAssistant::OnValueChanged);
}

void FileAssistant::resizeEvent(QResizeEvent* event)
{
	//列宽随窗口大小改变而改变，每列平均分配，充满整个表，但是此时列宽不能拖动进行改变
	ui.tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	//设置一下列适应内容长短分配大小（从0开始计数）
	ui.tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
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
	if (filePath.isEmpty()) return;

    m_searchThread.SetFilePath(filePath);
    m_searchThread.start();
}

void FileAssistant::OnDoing()
{
    QList<int> listRows;
	QStringList fromFiles;
	int nSize = m_upModel->rowCount();
	int nIndex = ui.cboDoType->currentIndex();
    m_dealThread.SetDealType(nIndex);
    switch (nIndex)
    {
    case 0: // copy
    case 1: // move
    {
        for (int i = 0; i < nSize; i++)
        {
            Qt::CheckState state = m_upModel->item(i)->checkState();
            if (state == Qt::Checked)
            {
                auto pItem = m_upModel->item(i);
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
            Qt::CheckState state = m_upModel->item(i)->checkState();
            if (state == Qt::Checked)
            {
				if (state == Qt::Checked)
				{
					auto itemData = m_upModel->item(i)->data().value<XItemData>();
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
            Qt::CheckState state = m_upModel->item(i)->checkState();
            if (state == Qt::Checked)
            {
                auto itemData = m_upModel->item(i)->data().value<XItemData>();
                fromFiles.push_back(itemData.m_fileInfo.filePath());
                listRows.push_back(i);
            }
        }

        ui.labelInfo->setText(QStringLiteral("压缩中："));
		ui.progressBar->setRange(0, 10);
        ui.progressBar->setValue(3);
		ui.progressBar->show();
		QString toPath = ui.editLocation2->text() + "/" + ui.editName->text() + ".zip";
		m_dealThread.SetCompressOptions(fromFiles, toPath, listRows);
		m_dealThread.start();
        break;
    }
    case 4:
    {
		for (int i = 0; i < nSize; i++)
		{
			Qt::CheckState state = m_upModel->item(i)->checkState();
			if (state == Qt::Checked)
			{
				auto itemData = m_upModel->item(i)->data().value<XItemData>();
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

void FileAssistant::OnDealTypeChanged(int nIndex)
{
    switch (nIndex)
    {
    case 0:
	case 1:
	case 2:
	case 4:
        ui.editName->setEnabled(false);
        break;
    case 3:
        ui.editName->setEnabled(true);
        break;
    default:
        break;
    }
}

void FileAssistant::OnTableRightClicked(const QPoint&)
{
	if (ui.tableView->currentIndex().isValid())
        m_pMenu->exec(cursor().pos());
}

void FileAssistant::OnReCheck()
{
    QModelIndexList listIndex = ui.tableView->selectionModel()->selectedRows();
    for (const auto& index : listIndex)
    {
        auto pItem = m_upModel->item(index.row());
        if (pItem && pItem->checkState() == Qt::Checked)
        {
            pItem->setCheckState(Qt::Unchecked);
            continue;
        }
        pItem->setCheckState(Qt::Checked);
    }
}

void FileAssistant::OnOpen()
{
    auto index = ui.tableView->currentIndex();
    auto pItem = m_upModel->item(index.row());
    auto itemData = pItem->data().value<XItemData>();
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(itemData.m_fileInfo.filePath())))
    {
        QMessageBox::information(this, QStringLiteral("警告"), QStringLiteral("文件打开失败！"), QMessageBox::StandardButton::Ok);
    }
}

void FileAssistant::OnDelete()
{
    QList<int> listRows;
    QStringList fromFiles;
	QModelIndexList listIndex = ui.tableView->selectionModel()->selectedRows();
	for (const auto& index : listIndex)
	{
        int nRow = index.row();
        listRows.push_back(nRow);
        auto itemData = m_upModel->item(nRow)->data().value<XItemData>();
		fromFiles.push_back(itemData.m_fileInfo.filePath());
	}

    m_dealThread.SetDealType(2);
	ui.labelInfo->setText(QStringLiteral("删除中："));
	ui.progressBar->setRange(0, fromFiles.size());
	ui.progressBar->show();
	m_dealThread.SetDeleteOptions(fromFiles, listRows);
	m_dealThread.start();
}

void FileAssistant::OnSearchStarted()
{
    m_fileInfoList.clear();
    m_upModel->removeRows(0, m_upModel->rowCount()); //删除所有行

    ui.labelInfo->setText(QStringLiteral("开始搜索") + ui.editLocation1->text() + "...");
    EnableControls(false);
}

void FileAssistant::OnSearchFinished()
{
	m_fileInfoList = m_searchThread.GetFileInfoList();
    int nIndex = ui.cboFileType->currentIndex();
    UpdateListItems(nIndex);
    EnableControls(true);

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
    EnableControls(false);
}

void FileAssistant::OnDealFinished()
{
	if (m_dealThread.GetSucessed())
	{
		auto listRows = m_dealThread.GetItemRows();
		for (const auto nRow : listRows)
		{
			auto pItem = m_upModel->item(nRow);
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
                    RemoveInvalidFileInfo(itemData.m_fileInfo);
                    break;
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
    else
    {
		int nType = m_dealThread.GetDealType();
		switch (nType)
		{
		case 0:
			QMessageBox::information(this, QStringLiteral("警告"), QStringLiteral("文件复制失败！"), QMessageBox::StandardButton::Ok);
			break;
		case 1:
			QMessageBox::information(this, QStringLiteral("警告"), QStringLiteral("文件移动失败！"), QMessageBox::StandardButton::Ok);
            break;
		case 2:
			QMessageBox::information(this, QStringLiteral("警告"), QStringLiteral("文件删除失败！"), QMessageBox::StandardButton::Ok);
			break;
		case 3:
			QMessageBox::information(this, QStringLiteral("警告"), QStringLiteral("文件压缩失败！"), QMessageBox::StandardButton::Ok);
			break;
		case 4:
			QMessageBox::information(this, QStringLiteral("警告"), QStringLiteral("文件解压失败！"), QMessageBox::StandardButton::Ok);
			break;
		default:
			break;
		}
    }

    ui.progressBar->hide();
    ui.labelInfo->setText("");
    EnableControls(true);
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
    m_upModel->removeRows(0, m_upModel->rowCount()); //删除所有行
    auto InsertItem = [this](int nRow, const QFileInfo& fileInfo){

		QIcon icon = QFileIconProvider().icon(fileInfo);
		auto pItem = new QStandardItem();
		pItem->setCheckable(true);
        FileState fileState = GetFileState(fileInfo);
        XItemData data(fileInfo, fileState);
        QVariant variant;
        variant.setValue(data);
        pItem->setData(variant);
        m_upModel->setItem(nRow, 0, pItem);

        pItem = new QStandardItem(icon, fileInfo.baseName());
        pItem->setData(fileInfo.baseName(), Qt::ToolTipRole);
        m_upModel->setItem(nRow, 1, pItem);

        pItem = new QStandardItem(fileInfo.path());
		pItem->setData(fileInfo.path(), Qt::ToolTipRole);
        m_upModel->setItem(nRow, 2, pItem);

		pItem = new QStandardItem(ConvertFielSize(fileInfo.size()));
		pItem->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
        pItem->setData(ConvertFielSize(fileInfo.size()), Qt::ToolTipRole);
        m_upModel->setItem(nRow, 3, pItem);

        pItem = new QStandardItem(fileInfo.suffix());
        pItem->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
        pItem->setData(fileInfo.suffix(), Qt::ToolTipRole);
        m_upModel->setItem(nRow, 4, pItem);

        pItem = new QStandardItem(GetFileStateName(fileInfo.fileName()));
        pItem->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
        pItem->setData(GetFileStateName(fileInfo.fileName()), Qt::ToolTipRole);
        m_upModel->setItem(nRow, 5, pItem);
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

        qint64 nMin = ui.spinFrom->value() * 1024 * 1024;
        qint64 nMax = ui.spinTo->value() * 1024 * 1024;
        if (fileInfo.size() < nMin || fileInfo.size() > nMax)
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
    QStringList strList{"mp3", "wma", "wav", "mod", "ra", "cd", "asf", "acc", "mp3pro", \
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

void FileAssistant::EnableControls(bool bEable /*= true*/)
{
    ui.btnFind->setEnabled(bEable);
    ui.btnDoing->setEnabled(bEable);
    ui.btnBowers1->setEnabled(bEable);
    ui.btnBowers2->setEnabled(bEable);
    ui.cboDoType->setEnabled(bEable);
    ui.cboFileType->setEnabled(bEable);
    ui.checkBox->setEnabled(bEable);
    ui.checkAuto->setEnabled(bEable);
    ui.checkKeyword->setEnabled(bEable);
	ui.editLocation1->setEnabled(bEable);
	ui.editLocation2->setEnabled(bEable);
    if (bEable && ui.cboDoType->currentIndex() == 3)
    {
		ui.editName->setEnabled(bEable);
    }
}
