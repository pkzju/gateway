#include "ctools.h"
#include <QDebug>
CTools::CTools(QObject *parent) : QObject(parent)
{

}

void CTools::updateOptions(const QString &file, const QString &name, QComboBox *ob)
{
    QFile loadFile(file);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file.");
        return ;
    }
    QByteArray jsonString = loadFile.readAll();
    loadFile.close();

    QJsonDocument loadDoc(QJsonDocument::fromJson(jsonString));
    QJsonObject json = loadDoc.object();
    QJsonObject list = json[name].toObject();

    ob->clear();
    foreach (QString key, list.keys()) {
        if(!list[key].toString().isEmpty())
            ob->addItem(key, list[key].toString());
        else
            ob->addItem(key);
    }
}

void CTools::saveSettings(const QString &fileName, const QString &group, QMap<QString, QString> &settings)
{
    QSettings iniSettings(fileName, QSettings::IniFormat);
    iniSettings.beginGroup(group);

    foreach (QString key, settings.keys()) {
        iniSettings.setValue(key, settings.value(key, key));
        qDebug() << group << "save" << key << settings.value(key);
    }
    iniSettings.endGroup();
}

void CTools::readSettings(const QString &fileName, const QString &group, QMap<QString, QString> &settings)
{
    QSettings iniSettings(fileName, QSettings::IniFormat);
    iniSettings.beginGroup(group);

    foreach (QString key, iniSettings.allKeys()) {
        settings.insert(key, iniSettings.value(key, "0").toString());
        qDebug()<< group << "read"<< key << iniSettings.value(key);
    }
    iniSettings.endGroup();
}
