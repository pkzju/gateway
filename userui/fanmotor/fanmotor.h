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

#ifndef FANMOTOR_H
#define FANMOTOR_H

typedef signed char qint8;         // 8 bit signed
typedef unsigned char quint8;      // 8 bit unsigned
typedef short qint16;              // 16 bit signed
typedef unsigned short quint16;    // 16 bit unsigned
typedef int qint32;                // 32 bit signed
typedef unsigned int quint32;      // 32 bit unsigned


enum FanMotorError:char {
    m_noError = 0,
    m_overCur,
    m_overSpd_,
    m_stall_,
    m_lowVolt,
    m_highVolt
};

enum FanMotorState:char{  
    m_stop = 0,
    m_error,
    m_run,
    m_stall,
    m_overSpd

};

enum FanCommunicationState:unsigned short{
    m_unknownCS = 0,
    m_disconnect,
    m_connect,
    m_comError

};


const quint16 g_mSettingsRegisterAddress = 0x0040;
const quint16 g_mSettingsRegisterAddress2 = 0x004C;
const quint16 g_mSettingsRegisterCount = 13;
const quint16 g_mSettingsRegisterCount2 = 1;

const quint16 g_mControllerRegisterAddress = 0x0060;
const quint16 g_mRatedRegisterCount = 2;

const quint16 g_mRealTimeRegisterAddress = 0x0062;
const quint16 g_mRealTimeRegisterStateCount = 1;
const quint16 g_mTorqueSpeed = 3;
const quint16 g_mRealTimeRegisterCount = 5;
const quint16 g_mRealTimeRegisterMoreCount = 9;
const quint16 g_mRealTimeRegisterAllCount = 12;

const quint16 g_mPIParaRegisterAddress = 0x0080;
const quint16 g_mPIParaRegisterCount = 6;

const quint16 g_mComStateAddress = 0x0090;
const quint16 g_mComStateRegisterCount = 1;

const quint16 g_mCustomAddress = 0x00A0;
const quint16 g_mCCustomRegisterCount = 3;
/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct FanMotorSettings{
    quint16 m_dryHighPower:16;    //2 byte  address:0x0040 true value : *0.01
    quint16 m_dryHighSpeed:16;    //4 byte
    quint16 m_wetHighPower:16;    //6 byte    true value : *0.01
    quint16 m_wetHighSpeed:16;    //8 byte

    quint16 m_dryMiddlePower:16;  //10 byte   true value : *0.01
    quint16 m_dryMiddleSpeed:16;  //12 byte
    quint16 m_wetMiddlePower:16;  //14 byte   true value : *0.01
    quint16 m_wetMiddleSpeed:16;  //16 byte

    quint16 m_dryLowPower:16;     //18 byte   true value : *0.01
    quint16 m_dryLowSpeed:16;     //20 byte
    quint16 m_wetLowPower:16;     //22 byte   true value : *0.01
    quint16 m_wetLowSpeed:16;     //24 byte  0x004b

    quint16 m_motorType:4;//0x004c
    quint16 m_staticPressure:2;
    quint16 m_regulation:2;       //25 byte
    quint16 m_control:1;
    quint16 m_address:7;          //26 byte  0x004c
};
#pragma pack(pop)

/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct FanPIParameters{
    quint16  m_speedKp;    // * 10000  true value : *0.001
    quint16  m_speedKi;    // * 10000
    quint16  m_idKp;       // * 10000
    quint16  m_idKi;       // * 10000
    quint16  m_iqKp;       // * 10000
    quint16  m_iqKi;       // * 10000
};
#pragma pack(pop)

/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct FanPIController{
    struct FanPIParameters m_PI;     // PI parameters
    qint16  m_ref;   		// reference set-point
    qint16  m_fbk;   		// feedback
    qint16  m_out;   		// Output: controller output
};
#pragma pack(pop)

/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct FanMotorController{
    quint16 m_ratedPower;     //      address:0x0060
    quint16 m_ratedSpeed;     //

    enum FanMotorState m_runState;	  // Motor run state address:0x0062
    enum FanMotorError m_runError;	  // Motor run Error

    quint16 m_targetpower;    //    address:0x0063
    qint16 m_speedRef;        //    address:0x0064

    quint16 m_nowpower; //                     0x0065
    qint16 m_speedFbk;

    qint16 m_idRef;
    qint16 m_idFbk;
    qint16 m_iqRef;
    qint16 m_iqFbk;//0x006a

    qint16 m_reserve1;//0x006B
    qint16 m_reserve2;
    qint16 m_reserve3;

};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
struct FanMotorCustom{
    quint16 m_custom1;     //      address:0x00A0
    quint16 m_custom2;     //
    quint16 m_custom3;    //    address:0x00A3
};
#pragma pack(pop)



/********************
 *
 *******************/
#pragma pack(push)
#pragma pack(1)
struct FanMotor{
    struct FanMotorSettings m_initSetttings;     //0x0040
    struct FanMotorController m_motorController; //0x0060
    struct FanPIParameters m_PIPara;             //0x0080
    enum FanCommunicationState m_communicationState; //0x0090
    struct FanMotorCustom m_custom;     //0x00A0
};
#pragma pack(pop)



#endif // QMOTOR_H
