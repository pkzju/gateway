/****************************************************************************
**
** Copyright (C) 2014 dragondjf
**
** QFramer is a frame based on Qt5.3, you will be more efficient with it. 
** As an Qter, Qt give us a nice coding experience. With user interactive experience(UE) 
** become more and more important in modern software, deveployers should consider business and UE.
** So, QFramer is born. QFramer's goal is to be a mature solution 
** which you only need to be focus on your business but UE for all Qters.
**
** Version	: 0.2.5.0
** Author	: dragondjf
** Website	: https://github.com/dragondjf
** Project	: https://github.com/dragondjf/QCFramer
** Blog		: http://my.oschina.net/dragondjf/home/?ft=atme
** Wiki		: https://github.com/dragondjf/QCFramer/wiki
** Lincence: LGPL V2
** QQ: 465398889
** Email: dragondjf@gmail.com, ding465398889@163.com, 465398889@qq.com
** 
****************************************************************************/

#include "centerwindow.h"
#include <QHBoxLayout>
#include <qgridlayout.h>
#include "../qframer/ftabwidget.h"

//#include "userui/canui.h"
//#include "userui/mplotui.h"
//#include "userui/modbusui.h"
//#include "userui/serialportui.h"
//#include "userui/tcpclientframe.h"
//#include "userui/tcpserverframe.h"
#include "../userui/fanhomeframe.h"
#include "../userui/newhome.h"

#include "../userui/canevs.h"
#include "../userui/cmotormodbus.h"

CenterWindow::CenterWindow(QWidget *parent) :
    FCenterWindow(parent)
{
    qDebug("CenterWindow init");

    initData();
    initUI();
    initConnect();
}

CenterWindow::~CenterWindow()
{
//    CANUi::deleteInstance();
//    MPlotUi::deleteInstance();
//    ModbusUi::deleteInstance();

    qDebug("CenterWindow exit");
}

void CenterWindow::initData()
{
    while(motorGroups.count() < gGroupnum){
        int _groupID = motorGroups.count()+1;
        FanGroupInfo *group = new FanGroupInfo(_groupID);
        motorGroups.push_back(group);
    }
}

void CenterWindow::initUI()
{
    setObjectName(tr("needBorder"));

 //   FanHomeFrame *fanHome = new FanHomeFrame;
 //   fanHome->setGroup(&motorGroups, 0, 2);
 //   addWidget(tr("Home%1").arg(1), "Home", fanHome);

 //   newHome *nh = new newHome;
//        addWidget(tr("Home%1").arg(1), "Home", nh);

    CanEVS *canevs = new CanEVS(this);
    addWidget(tr("CAN"), "serialportBtn", canevs);

    CMotorModbus *cmmodbus = new CMotorModbus(this);
    addWidget(tr("Modbus"), "serialportBtn", cmmodbus);




//    homewindow *m_home{homewindow::getInstance()};
//    addWidget(tr("Home"), "Home", m_home);
//    CANUi *m_canUi{CANUi::getS_Instance()};
//    addWidget(tr("Canbus"),"Communication", m_canUi);
//    MPlotUi *m_plotUi{MPlotUi::getInstance()};
//    addWidget(tr("Scope"), "MathPlot", m_plotUi);
//    ModbusUi *m_modbusui{ModbusUi::getInstance()};
//    addWidget(tr("Modbus"),"Communication", m_modbusui);
//    SerialPortUi *serialport{new SerialPortUi(this)};
//    addWidget(tr("COMPort"),"serialportBtn", serialport);
//    TcpClientFrame *_TcpClientFrame = new TcpClientFrame(this);
//    TcpServerFrame *_TcpServerFrame = new TcpServerFrame(this);
//    addWidget(tr("TcpClient"),"serialportBtn", _TcpClientFrame);
//    addWidget(tr("TcpServer"),"serialportBtn", _TcpServerFrame);

//    for(int i=0; i<gGroupnum; i+=2){
//        FanHomeFrame *fanHome = new FanHomeFrame;
//        fanHome->setGroup(&motorGroups, i, 2);
//        addWidget(tr("Home%1").arg(i/2+1), "Home", fanHome);
//    }

    setAlignment(TopCenter);
}

void CenterWindow::initConnect()
{

}

QVector<FanGroupInfo *> &CenterWindow::getGroups()
{
    return motorGroups;

}



