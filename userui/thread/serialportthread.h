#ifndef SERIALPORTTHREAD_H
#define SERIALPORTTHREAD_H
#include <QThread>
#include <QMutex>
#include "../userui/serialportsettingsdialog.h"

class SerialPortThread : public QThread
{
    Q_OBJECT

public:
    SerialPortThread(QObject *parent = 0);
    ~SerialPortThread();
    static SerialPortThread* getInstance();
    static void deleteInstance();
    void mStart(SerialPortSettings::Settings __settings);
    void mStop();
    void run();
    void mSet(SerialPortSettings::Settings __settings);
    void mSendData(QByteArray __data);

signals:
    void message(const QString &s);
    void message(const QByteArray &data);

private:
    static SerialPortThread* s_instance;
    QMutex m_serialPortMutex;
    bool m_isStopped;
    bool m_isSettingsChanged;

    SerialPortSettings::Settings m_currentSettings;
    QByteArray m_dataToSend;


};

#endif // SERIALPORTTHREAD_H
