#include "pch.h"
#include "settiongdlg.h"

namespace fileassistant {

	Settiongdlg::Settiongdlg(QWidget* parent)
		: QDialog(parent)
	{
		ui.setupUi(this);

		Init();
	}

	Settiongdlg::~Settiongdlg()
	{
	}

	void Settiongdlg::Init()
	{
		setWindowTitle(QStringLiteral("设置"));

		QPalette paletteL;
		paletteL.setBrush(QPalette::Background, QBrush(Qt::white));
		setPalette(paletteL);//设置背景

		int nIndex = 0;
		QString defFont = GetSettingValue(SETTING_DEFAULT_FONT);
		if (defFont.isEmpty())
		{
			defFont = this->font().defaultFamily();
		}
			
		QFontDatabase fontDatabase;
		auto listFont = fontDatabase.families();
		for (int i = 0; i < listFont.size(); i++)
		{
			ui.cmboxFont->addItem(listFont[i]);
			if (listFont[i] == defFont)
			{
				nIndex = i;
			}
		}
		ui.cmboxFont->setCurrentIndex(nIndex);
		this->setFont(defFont);

		ui.cmboxSkin->addItem(QStringLiteral("月光白"));
		ui.cmboxSkin->addItem(QStringLiteral("中国红"));
		
		ui.cmboxNight->addItem(QStringLiteral("关闭"));
		ui.cmboxNight->addItem(QStringLiteral("打开"));

		ui.editFindPath->setText(GetSettingValue(SETTING_SOURCE_PATH));
		ui.editDealPath->setText(GetSettingValue(SETTING_TARGET_PATH));

		// 信号槽绑定
		connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &Settiongdlg::OnBtnOk);
		connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &Settiongdlg::OnBtnCancel);
		connect(ui.btnCostum, &QPushButton::clicked, this, &Settiongdlg::OnBtnCustom);
		connect(ui.btnBower1, &QPushButton::clicked, this, &Settiongdlg::OnBtnBowers);
		connect(ui.btnBower2, &QPushButton::clicked, this, &Settiongdlg::OnBtnBowers);
	}

	void Settiongdlg::OnBtnOk()
	{
		// 默认路径
		QString strSorPath = ui.editFindPath->text();
		QDir dir;
		if (dir.exists(strSorPath))
		{
			SetSettingValue(SETTING_SOURCE_PATH, strSorPath);
		}
		else
		{
			QMessageBox::information(this, QStringLiteral("警告"), QStringLiteral("资源路径不存在！"), QMessageBox::Ok);
			return;
		}

		QString strTarPath = ui.editDealPath->text();
		if (dir.exists(strSorPath))
		{
			SetSettingValue(SETTING_TARGET_PATH, strTarPath);
		}
		else
		{
			QMessageBox::information(this, QStringLiteral("警告"), QStringLiteral("目标路径不存在！"), QMessageBox::Ok);
			return;
		}

		// 字体
		QString strFont = ui.cmboxFont->currentText();
		SetSettingValue(SETTING_DEFAULT_FONT, strFont);

		QDialog::accept();
	}

	void Settiongdlg::OnBtnCancel()
	{
		QDialog::reject();
	}

	void Settiongdlg::OnBtnCustom()
	{
		// todo
	}

	void Settiongdlg::OnBtnBowers()
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
		if (pButton == ui.btnBower1)
		{
			ui.editFindPath->setText(filePath);
		}
		else // ui.btnBower1
		{
			ui.editDealPath->setText(filePath);
		}
	}
}