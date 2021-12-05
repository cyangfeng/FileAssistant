#pragma once

namespace fileassistant {

	QMap<QString, QString> ReadFileData();
	void WriteFileData(const QMap<QString, QString>& keyValue);

	QString GetSettingValue(const QString& key);
	void SetSettingValue(const QString& key, const QString& value);

}
