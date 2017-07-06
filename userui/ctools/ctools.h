#ifndef CTOOLS_H
#define CTOOLS_H

#include <QObject>
#include <QSettings>
#include <QMap>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QComboBox>

class CTools : public QObject
{
    Q_OBJECT
public:
    explicit CTools(QObject *parent = 0);

    void updateOptions(const QString &file, const QString &name, QComboBox *ob);

    void saveSettings(const QString &fileName, const QString &group, QMap<QString, QString> &settings);
    void readSettings(const QString &fileName, const QString &group, QMap<QString, QString> &settings);

signals:

public slots:
};

#endif // CTOOLS_H
