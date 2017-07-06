#include "fanmotor/fpublic.h"


#define sensorStartAddress 100
RoomInfo::RoomInfo()
{
    firstAddress = 0;
    lastAddress = 0;
    number = 0;

    temperature = 0;
    humidity = 0;
    pm2d5 = 0;
    sensorAddress = sensorStartAddress + id;
}

RoomInfo::RoomInfo(int _id, int firstAdd, int lastAdd):
    id(_id)
  ,firstAddress(firstAdd)
  ,lastAddress(lastAdd)
{
    temperature = 0;
    humidity = 0;
    pm2d5 = 0;
    sensorAddress = sensorStartAddress + id;
    if(firstAddress <= 0 || lastAddress <= 0 || lastAddress < firstAddress)
    {
        firstAddress = 0;
        lastAddress = 0;
        number = 0;
        return;
    }
    number = lastAddress - firstAddress + 1;
    for(int i = firstAddress; i <= lastAddress; ++i){
        motors.push_back(NMotor(i));
    }
}

bool RoomInfo::isInRoom(int add)
{
    return (add >= firstAddress && add <= lastAddress);
}

NMotor* RoomInfo::findMotor(int add)
{
    if(isInRoom(add))
        return &motors[add - firstAddress];
    else
        return nullptr;
}

void RoomInfo::setMotorNum(int n)
{
    while(n < number){
        removeMotor();
    }
    while(n > number){
        addMotor();
    }
}

void RoomInfo::addMotor()
{
    ++lastAddress;
    ++number;
    motors.push_back(NMotor(lastAddress));
}

void RoomInfo::removeMotor()
{
    if(lastAddress > 0 && motors.size() > 0){
        --lastAddress;
        --number;
        motors.pop_back();
    }
}
