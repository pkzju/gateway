#ifndef SERIALPORTSETTINGS_H
#define SERIALPORTSETTINGS_H

#include <QObject>

#include <QSerialPort>

class SerialPortSettings : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortSettings(QObject *parent = 0);
    QString name() const;
    void setName(const QString &name);
    qint32 baudRate() const;
    void setBaudRate(const qint32 &baudRate);
    QSerialPort::DataBits dataBits() const;
    void setDataBits(const QSerialPort::DataBits &dataBits);
    QSerialPort::Parity parity() const;
    void setParity(const QSerialPort::Parity &parity);
    QSerialPort::StopBits stopBits() const;
    void setStopBits(const QSerialPort::StopBits &stopBits);
    QSerialPort::FlowControl flowControl() const;
    void setFlowControl(const QSerialPort::FlowControl &flowControl);

private:
    QString mName;
    qint32 mBaudRate;
    QSerialPort::DataBits mDataBits;
    QSerialPort::Parity mParity;
    QSerialPort::StopBits mStopBits;
    QSerialPort::FlowControl mFlowControl;
};

#endif // SERIALPORTSETTINGS_H
