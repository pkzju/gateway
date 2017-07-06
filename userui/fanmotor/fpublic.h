#ifndef FPUBLIC_H
#define FPUBLIC_H

#include "qmotor.h"

const int gGroupnum = 4; //Number of all groups
const int gMotorMaxnum = 3;

const int g_responseTimeout = 50;

const int g_numberOfTry = 0;

enum CommunicationMode:char{
    Init,
    Tcp,
    Modbus,
    CANbus
};

class FanGroupInfo{

public:
    FanGroupInfo(quint16 id):
      m_groupID(id)
      ,m_startAddress((id-1)*gMotorMaxnum + 1)
      ,m_fanMaxNumber(gMotorMaxnum)
    {
        for(int i = 0; i < m_fanMaxNumber; i++){
            m_motors.push_back(new QMotor(m_startAddress + i));
        }
    }

public:

    quint16 m_groupID;
    quint16 m_startAddress;
    quint16 m_fanMaxNumber;
    QVector<QMotor *> m_motors;

};

class RoomInfo{

public:
    RoomInfo();
    explicit RoomInfo(int _id, int firstAdd, int lastAdd);

    bool isInRoom(int add);
    NMotor* findMotor(int add);
    void setMotorNum(int n);
    void addMotor();
    void removeMotor();

public:
    int id;
    int firstAddress;
    int lastAddress;
    int number;

    int temperature;
    int humidity;
    int pm2d5;
    int sensorAddress;
    QVector<NMotor> motors;
};

#endif // FPUBLIC_H
