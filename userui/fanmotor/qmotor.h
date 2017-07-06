/****************************************************************************
**
** Copyright (C) 2016 pkzju
**
**
** Version	: 0.1.1.0
** Author	: pkzju
** Website	: https://github.com/pkzju
** Project	: https://github.com/pkzju/QSuperConsole
** 
****************************************************************************/

#ifndef QMOTOR_H
#define QMOTOR_H

#include <QObject>
#include "fanmotor/fanmotor.h"
#include <QLabel>
#include <QLCDNumber>
#include "lamp/qcw_indicatorlamp.h"

enum FmotorCommand:unsigned short{
    m_connectToServer = 1,
    m_readMotorRegister

};

const quint16 g_motorCommandAddress = 0x002b;
const quint16 g_commonCommandCount = 1;
const quint16 g_readCommandAddress = 3;
struct FCommandRegister{
    FmotorCommand  m_command;    // 0x002b
    quint16  m_registerAdd;     // 0x002c
    quint16 m_count;
};


class QMotor
{

public:
    explicit QMotor(int address = 1);
    ~QMotor();

    quint16 *findRegister(quint16 registerAddress);
    void update();

public:
    int m_address;
    FanMotorSettings  m_initSetttings;     //0x0040
    FanMotorController  m_motorController; //0x0060
    FanPIParameters  m_PIPara;             //0x0080
    FanCommunicationState  m_communicationState;
    FanMotorCustom m_custom;     //0x00A0

    QLabel  *m_motorAddressLabel;


    QLCDNumber  *m_ratedPowerLCD;
    QLCDNumber  *m_targetPowerLCD;
    QLCDNumber  *m_nowPowerLCD;

    QLCDNumber  *m_ratedSpeedLCD;
    QLCDNumber  *m_speedRefLCD;
    QLCDNumber  *m_speedFbkLCD;

    QcwIndicatorLamp  *m_runLamp;
    QcwIndicatorLamp  *m_commLamp;

    QLabel  *m_message;

    bool isMonitor = true;


};

class NMotor
{

public:
    explicit NMotor(int address = 1)
    {
        m_address = address;
    }

    quint16 *findRegister(quint16 registerAddress);
    void update();

public:
    int m_address;
    FanMotorSettings  m_initSetttings;     //0x0040
    FanMotorController  m_motorController; //0x0060
    FanPIParameters  m_PIPara;             //0x0080
    FanMotorCustom m_custom;     //0x00A0
    FanCommunicationState  m_communicationState;

    bool isMonitor = true;
};

#endif // QMOTOR_H
