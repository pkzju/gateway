#include "modbushandler.h"

#include <qsettings.h>
#include <QModbusRtuSerialMaster>

ModbusHandler::ModbusHandler(QObject *parent) : QObject(parent)
  , modbusRtuDevice(Q_NULLPTR)
{

}

bool ModbusHandler::open(const SerialPortSettings &settings, int timeout, int numberOfRetries)
{
    //![1] Make sure modbus device was not created yet
    if (modbusRtuDevice) {
        modbusRtuDevice->disconnectDevice();
        modbusRtuDevice->deleteLater();
        modbusRtuDevice = Q_NULLPTR;
    }

    //![2] Create modbus RTU client device and monitor error,state
    modbusRtuDevice = new QModbusRtuSerialMaster(this);
    connect(modbusRtuDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        emit errorOccurred(0, modbusRtuDevice->errorString());
    });
    if (!modbusRtuDevice) {
        emit errorOccurred(0, tr("Could not create Modbus client."));
        return false;
    } else {
        connect(modbusRtuDevice, &QModbusClient::stateChanged,
                this, &ModbusHandler::onStateChanged);//!< Monitor modbus device state
    }

    //![3] Set serialport parameter and connect device
    if (modbusRtuDevice->state() != QModbusDevice::ConnectedState) {

        modbusRtuDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                                                settings.name());
        modbusRtuDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                                                settings.baudRate());
        modbusRtuDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                                                settings.dataBits());
        modbusRtuDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                                settings.parity());
        modbusRtuDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                                                settings.stopBits());

        modbusRtuDevice->setTimeout(timeout);//!< Set response timeout
        modbusRtuDevice->setNumberOfRetries(numberOfRetries);//!< Set retry number


        //! Now try connect modbus device
        if (!modbusRtuDevice->connectDevice()) {
           // emit errorOccurred(0, tr("Modbus connect failed: ") + modbusRtuDevice->errorString());
        } else {
            //!< Connect  successfully
            return true;
        }
    }
    return false;
}

bool ModbusHandler::close()
{
    if(!modbusRtuDevice)
        return false;
    if(modbusRtuDevice->state() == QModbusDevice::ConnectedState){
        modbusRtuDevice->disconnectDevice();
    }

    return true;
}

bool ModbusHandler::isOpen()
{
    if (!modbusRtuDevice)
        return false;
    return (modbusRtuDevice->state() == QModbusDevice::ConnectedState);
}

void ModbusHandler::read(int serverAddress, int startAddress, int numberOfEntries)
{
    if (!modbusRtuDevice || !isOpen())
        return;

    QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);

    if (auto *reply = modbusRtuDevice->sendReadRequest(unit, serverAddress)) {
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, &ModbusHandler::readReady);
        }
        else
            delete reply; // broadcast replies return immediately
    } else {
        emit errorOccurred(reply->serverAddress(), tr("Read error: ") + modbusRtuDevice->errorString());
    }
}

bool ModbusHandler::write(int serverAddress, const QModbusDataUnit &unit)
{
    if(!modbusRtuDevice || !isOpen())
        return false;

    //! Send write request to server (adress: )
    if (auto *reply = modbusRtuDevice->sendWriteRequest(unit, serverAddress)) {
        if (!reply->isFinished()) {

            connect(reply, &QModbusReply::finished, this, [this, reply]() {

                if(reply->serverAddress() != 0){//! Not broadcast
                    if (reply->error() == QModbusDevice::ProtocolError) {
                         errorOccurred(reply->serverAddress(), tr("Write response error: %1 (Mobus exception: 0x%2)")
                                                .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16)
                                                );
                    } else if (reply->error() != QModbusDevice::NoError) {
                         errorOccurred(reply->serverAddress(), tr("Write response error: %1 (code: 0x%2)").
                                                arg(reply->errorString()).arg(reply->error(), -1, 16));
                    }
                }
                reply->deleteLater();
            });
        } else {//!< Broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
         errorOccurred(reply->serverAddress(), tr("Write error: ") + modbusRtuDevice->errorString());
        return false;
    }

    return true;
}

void ModbusHandler::readReady()
{
    QModbusReply *reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        mReceivedUnit = reply->result();
        emit readResult(reply->serverAddress(), mReceivedUnit);

    } else if (reply->error() == QModbusDevice::ProtocolError) {
        emit errorOccurred(reply->serverAddress(), tr("Read response error: %1 (Mobus exception: 0x%2)")\
                           .arg(reply->errorString())
                           .arg(reply->rawResult().exceptionCode(), -1, 16));
    } else {
        emit errorOccurred(reply->serverAddress(), tr("Read response error: %1 (code: 0x%2)")
                           .arg(reply->errorString()).arg(reply->error(), -1, 16));
    }

    reply->deleteLater();


}


void ModbusHandler::onStateChanged(int state)
{
    bool connected = (state != 0);

    connected = (state == QModbusDevice::ConnectedState);
    if(connected){


    }
    else{


    }

}


