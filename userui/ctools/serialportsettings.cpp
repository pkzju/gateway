#include "serialportsettings.h"

SerialPortSettings::SerialPortSettings(QObject *parent) : QObject(parent)
{

}

QSerialPort::FlowControl SerialPortSettings::flowControl() const
{
    return mFlowControl;
}

void SerialPortSettings::setFlowControl(const QSerialPort::FlowControl &flowControl)
{
    mFlowControl = flowControl;
}

QSerialPort::StopBits SerialPortSettings::stopBits() const
{
    return mStopBits;
}

void SerialPortSettings::setStopBits(const QSerialPort::StopBits &stopBits)
{
    mStopBits = stopBits;
}

QSerialPort::Parity SerialPortSettings::parity() const
{
    return mParity;
}

void SerialPortSettings::setParity(const QSerialPort::Parity &parity)
{
    mParity = parity;
}

QSerialPort::DataBits SerialPortSettings::dataBits() const
{
    return mDataBits;
}

void SerialPortSettings::setDataBits(const QSerialPort::DataBits &dataBits)
{
    mDataBits = dataBits;
}

qint32 SerialPortSettings::baudRate() const
{
    return mBaudRate;
}

void SerialPortSettings::setBaudRate(const qint32 &baudRate)
{
    mBaudRate = baudRate;
}

QString SerialPortSettings::name() const
{
    return mName;
}

void SerialPortSettings::setName(const QString &name)
{
    mName = name;
}
