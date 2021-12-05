#include "pch.h"
#include "util.h"

namespace fileassistant {

	QMap<QString, QString> ReadFileData()
	{
		QMap<QString, QString> keyValue;
		QFile file(SETTING_FILE_PATH);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream stream(&file);
			stream.setCodec(QTextCodec::codecForName("UTF-8"));
			while (!stream.atEnd())
			{
				QString data = stream.readLine();
				int nPos = data.indexOf('=');
				if (nPos != -1)
				{
					auto skey = data.left(nPos);
					auto sValue = data.mid(nPos + 1);
					keyValue.insert(skey, sValue);
				}
			}
			file.close();
		}
		else
		{
			qDebug() << L"Open File Failed£¡";
		}

		return std::move(keyValue);
	}

	void WriteFileData(const QMap<QString, QString>& keyValue)
	{
		QFile file(SETTING_FILE_PATH);
		if (file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			auto i = keyValue.constBegin();
			for (; i != keyValue.constEnd(); i++)
			{
				QString sItem = i.key() + "=" + i.value() + "\n";
				file.write(sItem.toUtf8());
			}

			file.close();
		}
		else
		{
			qDebug() << L"Open File Failed£¡";
		}
	}

	QString GetSettingValue(const QString& key)
	{
		auto keyValue = ReadFileData();
		return keyValue[key];
	}

	void SetSettingValue(const QString& key, const QString& value)
	{
		auto keyValue = ReadFileData();
		keyValue[key] = value;
		if (!keyValue.isEmpty())
		{
			WriteFileData(keyValue);
		}
	}

}