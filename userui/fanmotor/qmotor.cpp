#include "fanmotor/qmotor.h"
#include <QObject>

QMotor::QMotor(int address) :
    m_motorAddressLabel{new QLabel(QString("%1").arg(address))},
    m_ratedPowerLCD{new QLCDNumber},
    m_targetPowerLCD{new QLCDNumber},
    m_nowPowerLCD{new QLCDNumber},
    m_ratedSpeedLCD{new QLCDNumber},
    m_speedRefLCD{new QLCDNumber},
    m_speedFbkLCD{new QLCDNumber},
    m_runLamp{new QcwIndicatorLamp},
    m_commLamp{new QcwIndicatorLamp},
    m_message{new QLabel(QString("stop"))}
{
    m_address = address;
    m_initSetttings = {0};
    m_motorController ={0};
    m_PIPara = {0};
}
QMotor::~QMotor()
{
    delete m_motorAddressLabel;

    delete m_ratedPowerLCD;
    delete m_targetPowerLCD;
    delete m_nowPowerLCD;

    delete m_ratedSpeedLCD;
    delete m_speedRefLCD;
    delete m_speedFbkLCD;

    delete m_runLamp;
    delete m_commLamp;
}

quint16 *QMotor::findRegister(quint16 registerAddress)
{
    quint16 *buff = Q_NULLPTR;

    if(registerAddress == g_mSettingsRegisterAddress){//!< Make sure is Init data
        buff = (quint16 *)&this->m_initSetttings;//!< Get current motor's initSetttings pointer
    }
    else if(registerAddress == g_mRealTimeRegisterAddress){
        buff = (quint16 *)&this->m_motorController;
        buff += 2;
    }
    else if(registerAddress == g_mControllerRegisterAddress){
        buff = (quint16 *)&this->m_motorController;
    }
    else if(registerAddress == g_mPIParaRegisterAddress){//!< Make sure is PI data
        buff = (quint16 *)&this->m_PIPara;
    }
    else if(registerAddress == g_mComStateAddress){
        buff = (quint16 *)&this->m_communicationState;
    }
    else if(registerAddress == g_mCustomAddress){
        buff = (quint16 *)&this->m_custom;
    }
    else{
        buff = Q_NULLPTR;
    }

    return buff;
}

void QMotor::update()
{
    m_ratedPowerLCD->display(m_motorController.m_ratedPower*0.01);
    m_targetPowerLCD->display(m_motorController.m_targetpower*0.01);
    m_nowPowerLCD->display(m_motorController.m_nowpower*0.01);

    m_ratedSpeedLCD->display(m_motorController.m_ratedSpeed);
    m_speedRefLCD->display(m_motorController.m_speedRef);
    m_speedFbkLCD->display(m_motorController.m_speedFbk);

    if(m_motorController.m_runState == FanMotorState::m_run)
        m_runLamp->setLampState(QcwIndicatorLamp::lamp_green);
    else if(m_motorController.m_runState == FanMotorState::m_stop)
        m_runLamp->setLampState(QcwIndicatorLamp::lamp_yellow);
    else{
        m_runLamp->setLampState(QcwIndicatorLamp::lamp_red);

        if(m_motorController.m_runState == FanMotorState::m_stall){
            m_message->setText(QObject::tr("stall"));
        }
        else if(m_motorController.m_runState == FanMotorState::m_overSpd){
            m_message->setText(QObject::tr("over speed"));
        }
        else if(m_motorController.m_runState == FanMotorState::m_error){

            if(m_motorController.m_runError == FanMotorError::m_overCur){
                m_message->setText(QObject::tr("over current"));
            }
            else if(m_motorController.m_runError == FanMotorError::m_overSpd_){
                m_message->setText(QObject::tr("over speed"));
            }
            else if(m_motorController.m_runError == FanMotorError::m_stall_){
                m_message->setText(QObject::tr("stall"));
            }
            else if(m_motorController.m_runError == FanMotorError::m_lowVolt){
                m_message->setText(QObject::tr("low voltage "));
            }
            else if(m_motorController.m_runError == FanMotorError::m_highVolt){
                m_message->setText(QObject::tr("high voltage "));
            }
            else
                m_message->setText(QObject::tr("no error"));
        }
    }

    if(m_communicationState == FanCommunicationState::m_connect){
        isMonitor = true;
        m_commLamp->setLampState(QcwIndicatorLamp::lamp_green);
    }
    else if(m_communicationState == FanCommunicationState::m_disconnect){
        isMonitor = false;
        m_commLamp->setLampState(QcwIndicatorLamp::lamp_red);
    }
    else if(m_communicationState == FanCommunicationState::m_comError){
        m_commLamp->setLampState(QcwIndicatorLamp::lamp_yellow);
    }
    else{
        m_commLamp->setLampState(QcwIndicatorLamp::lamp_grey);
    }

}

quint16 *NMotor::findRegister(quint16 registerAddress)
{
    quint16 *buff = Q_NULLPTR;

    if(registerAddress == g_mSettingsRegisterAddress){//!< Make sure is Init data
        buff = (quint16 *)&this->m_initSetttings;//!< Get current motor's initSetttings pointer
    }
    else if(registerAddress == g_mRealTimeRegisterAddress){
        buff = (quint16 *)&this->m_motorController;
        buff += 2;
    }
    else if(registerAddress == g_mControllerRegisterAddress){
        buff = (quint16 *)&this->m_motorController;
    }
    else if(registerAddress == g_mPIParaRegisterAddress){//!< Make sure is PI data
        buff = (quint16 *)&this->m_PIPara;
    }
    else if(registerAddress == g_mComStateAddress){
        buff = (quint16 *)&this->m_communicationState;
    }
    else if(registerAddress == g_mCustomAddress){
        buff = (quint16 *)&this->m_custom;
    }
    else{
        buff = Q_NULLPTR;
    }

    return buff;
}
