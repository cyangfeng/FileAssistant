#pragma once

#define SETTING_FILE_PATH "../fsetting.fa"
#define SETTING_SOURCE_PATH "Source Path"
#define SETTING_TARGET_PATH "Target Path"
#define SETTING_DEFAULT_FONT "Default Font"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QFontDatabase>
#include <qthread.h>
#include <QStandardItem>
#include <QThread>
#include <QFileInfoList>
#include <QStandardItemModel>
#include <QDesktopServices>
#include <QFileIconProvider>
#include <QMenu>
#include <QPalette>

#include "include/JlCompress.h"
#include "util.h"


