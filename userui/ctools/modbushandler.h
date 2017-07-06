#ifndef MODBUSHANDLER_H
#define MODBUSHANDLER_H

#include <QObject>

#include <QModbusDataUnit>

#include "serialportsettings.h"

QT_BEGIN_NAMESPACE
class QModbusClient;
class QModbusReply;

QT_END_NAMESPACE

class ModbusHandler : public QObject
{
    Q_OBJECT
public:
    explicit ModbusHandler(QObject *parent = 0);
    bool close();
    bool isOpen();
    bool open(const SerialPortSettings &settings, int timeout, int numberOfRetries);

    void read(int serverAddress, int startAddress, int numberOfEntries);
    bool write(int serverAddress, const QModbusDataUnit &unit);

signals:
    void errorOccurred(int serverAddress, QString error);
    void readResult(int serverAddress, const QModbusDataUnit &unit);

public slots:

    void readReady();
    void onStateChanged(int state);


private:
    QModbusClient *modbusRtuDevice;
    QModbusDataUnit mReceivedUnit;
};

#endif // MODBUSHANDLER_H
