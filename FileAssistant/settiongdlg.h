#pragma once

#include <QDialog>
#include "ui_settiongdlg.h"

namespace fileassistant {

	class FileAssistant;
	class Settiongdlg : public QDialog
	{
		Q_OBJECT

	public:
		Settiongdlg(QWidget* parent = Q_NULLPTR);
		~Settiongdlg();

	private:

		void Init();

		void OnBtnOk();
		void OnBtnCancel();
		void OnBtnCustom();
		void OnBtnBowers();

	private:
		Ui::Settiongdlg ui;
	};
}