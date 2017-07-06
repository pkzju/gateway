#include "newhome.h"
#include "ui_newhome.h"

#include "ctools/ctools.h"
#include "../excelbase/ExcelBase.h"
#include <QSerialPortInfo>

#include <QDir>

#include <QDebug>

#define PLOT_POINT 100
#define PERIOD (1.145*(8+(1+1+1+2*12+2)+3.5*2))
#define POLLING_PERIOD 2000

newHome::newHome(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::newHome)
  ,mModbusHandler(new ModbusHandler)
  ,yAxis1(PLOT_POINT, 0)
{
    ui->setupUi(this);

    initUi();
    initData();
    initConnections();

    connect(&pollingTimer, SIGNAL(timeout()), this, SLOT(pollingState()));
    pollingTimer.start(POLLING_PERIOD);
}

void newHome::initConnections()
{
    mRoomBtns.addButton(ui->enterR1_btn, 0);
    mRoomBtns.addButton(ui->enterR2_btn, 1);
    mRoomBtns.addButton(ui->enterR3_btn, 2);
    mRoomBtns.addButton(ui->enterR4_btn, 3);
    mRoomBtns.addButton(ui->enterR5_btn, 4);
    connect(&mRoomBtns, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            [=](int id){ enterRoom(id); });

    mMotorBtns.addButton(ui->pushButton_fan1, 0);
    mMotorBtns.addButton(ui->pushButton_fan2, 1);
    mMotorBtns.addButton(ui->pushButton_fan3, 2);
    mMotorBtns.addButton(ui->pushButton_fan4, 3);
    mMotorBtns.addButton(ui->pushButton_fan5, 4);
    mMotorBtns.addButton(ui->pushButton_fan6, 5);
    mMotorBtns.addButton(ui->pushButton_fan7, 6);
    mMotorBtns.addButton(ui->pushButton_fan8, 7);
    mMotorBtns.addButton(ui->pushButton_fan9, 8);
    connect(&mMotorBtns, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            [=](int id){ enterMotor(id); });

    comLamps.push_back(ui->lamp_c1);
    comLamps.push_back(ui->lamp_c2);
    comLamps.push_back(ui->lamp_c3);
    comLamps.push_back(ui->lamp_c4);
    comLamps.push_back(ui->lamp_c5);
    comLamps.push_back(ui->lamp_c6);
    comLamps.push_back(ui->lamp_c7);
    comLamps.push_back(ui->lamp_c8);
    comLamps.push_back(ui->lamp_c9);

    runLamps.push_back(ui->lamp_r1);
    runLamps.push_back(ui->lamp_r2);
    runLamps.push_back(ui->lamp_r3);
    runLamps.push_back(ui->lamp_r4);
    runLamps.push_back(ui->lamp_r5);
    runLamps.push_back(ui->lamp_r6);
    runLamps.push_back(ui->lamp_r7);
    runLamps.push_back(ui->lamp_r8);
    runLamps.push_back(ui->lamp_r9);
}

const int NMotorNumber = 9;

void newHome::initData()
{
    for(quint16 i = 0; i < 5; ++i){
        mRooms.push_back(RoomInfo(i, i*NMotorNumber + 1, (i+1)*NMotorNumber));
    }

    currentRoom = &mRooms[0];
    currentMotor = &currentRoom->motors[0];
    currentRoomId = 0;
    currentMotorId = 1;

    for(quint16 i = 0; i < PLOT_POINT; ++i){
        yAxis1[i] = i;
    }
    mAxis.insert("SpeedRef", QVector<double>(PLOT_POINT, 0));
    mAxis.insert("SpeedFbk", QVector<double>(PLOT_POINT, 0));
    mAxis.insert("IdRef",    QVector<double>(PLOT_POINT, 0));
    mAxis.insert("IdFbk",    QVector<double>(PLOT_POINT, 0));
    mAxis.insert("IqRef",    QVector<double>(PLOT_POINT, 0));
    mAxis.insert("IqFbk",    QVector<double>(PLOT_POINT, 0));
    mAxis.insert("Reserve1", QVector<double>(PLOT_POINT, 0));
    mAxis.insert("Reserve2", QVector<double>(PLOT_POINT, 0));
    mAxis.insert("Reserve3", QVector<double>(PLOT_POINT, 0));


}

void newHome::initUi()
{
    CTools tools;
    tools.updateOptions(QDir::currentPath() + "/json/modbusui.json", "BaudRate", ui->baudCombo);
    tools.updateOptions(QDir::currentPath() + "/json/modbusui.json", "comboBox_state", ui->comboBox_state);

    QMap<QString,QString> map;
    tools.readSettings(QDir::currentPath() + "/Catcher.ini", "newHome", map);
    ui->serialPortInfoListBox->setCurrentText(map.value("serialPortInfoListBox", ""));
    ui->baudCombo->setCurrentIndex(map.value("baudCombo", "0").toInt());

    ui->serialPortInfoListBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        list << info.portName();
        ui->serialPortInfoListBox->addItem(list.first(), list);
    }

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->customPlot->xAxis->setRange(0, 100);
    ui->customPlot->yAxis->setRange(0, 655);
    ui->customPlot->axisRect()->setupFullAxesBox();

    ui->customPlot->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(ui->customPlot, "Oscilloscope", QFont("sans", 17, QFont::Bold));
    ui->customPlot->plotLayout()->addElement(0, 0, title);

    ui->customPlot->xAxis->setLabel("x Axis");
    ui->customPlot->yAxis->setLabel("y Axis");
    ui->customPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setSelectedFont(legendFont);
    ui->customPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    ui->customPlot->rescaleAxes();

    ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);

    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
    connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

    ui->checkBox_speed->setCheckState(Qt::Checked);
}

newHome::~newHome()
{
    QMap<QString,QString> set;
    set.insert("serialPortInfoListBox", ui->serialPortInfoListBox->currentText());
    set.insert("baudCombo", QString::number(ui->baudCombo->currentIndex()));

    CTools tools;
    tools.saveSettings(QDir::currentPath() + "/Catcher.ini", "newHome", set);

    if(mModbusHandler->isOpen()){
        mModbusHandler->close();
        ui->connectButton->setText(tr("Open"));
        disconnect(mModbusHandler.data(), &ModbusHandler::readResult, this, &newHome::mbReadResult);
    }
    delete ui;
}

void newHome::enterRoom(int id)
{
    ui->stackedWidget->setCurrentIndex(1);
    currentRoomId = mRooms[id].id;
    currentRoom = &mRooms[currentRoomId];
    ui->roomNum_lcd->display(currentRoomId + 1);
    ui->spinBox_motorNum->setValue(mRooms[currentRoomId].number);
    ui->sensorAddress_lcd->display(currentRoom->sensorAddress);

    for(int lampId = 0; lampId < currentRoom->number; ++lampId){

        NMotor *motor = &currentRoom->motors[lampId];
        if(!motor)
            return;

        if(motor->m_motorController.m_runState == FanMotorState::m_run)
            runLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_green);
        else if(motor->m_motorController.m_runState == FanMotorState::m_stop)
            runLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_yellow);
        else{
            runLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_red);
        }

        if(motor->m_communicationState == FanCommunicationState::m_connect)
            comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_green);
        else if(motor->m_communicationState == FanCommunicationState::m_disconnect)
            comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_red);
        else if(motor->m_communicationState == FanCommunicationState::m_comError)
            comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_yellow);
        else
            comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_grey);
    }
}

void newHome::enterMotor(int id)
{
    ui->stackedWidget->setCurrentIndex(2);
    currentMotorId = currentRoom->firstAddress + id;
    currentMotor = currentRoom->findMotor(currentMotorId);

    ui->motorId_lcd->display(currentMotorId);

    NMotor *motor = currentMotor;

    if(motor->m_motorController.m_runState == FanMotorState::m_run)
        ui->runLamp->setLampState(QcwIndicatorLamp::lamp_green);
    else if(motor->m_motorController.m_runState == FanMotorState::m_stop)
        ui->runLamp->setLampState(QcwIndicatorLamp::lamp_yellow);
    else{
        ui->runLamp->setLampState(QcwIndicatorLamp::lamp_red);
    }

    if(motor->m_communicationState == FanCommunicationState::m_connect)
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_green);
    else if(motor->m_communicationState == FanCommunicationState::m_disconnect)
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_red);
    else if(motor->m_communicationState == FanCommunicationState::m_comError)
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_yellow);
    else
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_grey);

}

void newHome::on_previous1_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void newHome::on_next1_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void newHome::on_previous2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void newHome::on_searchButton_clicked()
{
    ui->serialPortInfoListBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        list << info.portName();
        ui->serialPortInfoListBox->addItem(list.first(), list);
    }
    ui->serialPortInfoListBox->showPopup();
}

void newHome::on_connectButton_clicked()
{
    if(mModbusHandler->isOpen()){
        mModbusHandler->close();
        ui->connectButton->setText(tr("Open"));
        disconnect(mModbusHandler.data(), &ModbusHandler::readResult, this, &newHome::mbReadResult);
    }
    else{
        SerialPortSettings settings;
        settings.setName(ui->serialPortInfoListBox->currentText());
        settings.setParity(QSerialPort::NoParity);
        settings.setBaudRate(QSerialPort::Baud19200);
        settings.setDataBits(QSerialPort::Data8);
        settings.setStopBits(QSerialPort::OneStop);

        int timeout = 50;
        int numberOfretries = 0;
        mModbusHandler->open(settings, timeout, numberOfretries);

        ui->connectButton->setText(tr("Close"));

        connect(mModbusHandler.data(), &ModbusHandler::readResult, this, &newHome::mbReadResult);
        connect(mModbusHandler.data(), &ModbusHandler::errorOccurred, this, &newHome::mbError);

    }
}

void newHome::on_spinBox_motorNum_valueChanged(int arg1)
{
    currentRoom->setMotorNum(arg1);

    for(int i = arg1; i < NMotorNumber; ++i)//!< For reduce group
    {
        mMotorBtns.button(i)->setEnabled(false);
        comLamps[i]->setLampState(QcwIndicatorLamp::lamp_grey);
        runLamps[i]->setLampState(QcwIndicatorLamp::lamp_grey);
    }
    for(int i = 0; i < arg1; ++i)        //!< For add group
    {
        mMotorBtns.button(i)->setEnabled(true);
    }
}

//! plot functions
void newHome::addOneGraph(const QString &name)
{
    for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
        if(ui->customPlot->graph(i)->name() == name){
            ui->customPlot->removeGraph(i);
        }
    }

    ui->customPlot->addGraph();
    ui->customPlot->graph()->setName(name);
    ui->customPlot->graph()->setData(yAxis1, mAxis[name]);
    QPen graphPen;
    //        ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
    graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
    graphPen.setWidthF(2);
    ui->customPlot->graph()->setPen(graphPen);

    ui->customPlot->replot();
}

void newHome::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (ui->customPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {
        menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
        menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
        menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
        menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
        menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
    }
    else  // general context menu on graphs requested
    { }

    menu->popup(ui->customPlot->mapToGlobal(pos));
}

void newHome::moveLegend()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
    {
        bool ok;
        int dataInt = contextAction->data().toInt(&ok);
        if (ok)
        {
            ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
            ui->customPlot->replot();
        }
    }
}

void newHome::selectionChanged()
{
    // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<ui->customPlot->graphCount(); ++i)
    {
        QCPGraph *graph = ui->customPlot->graph(i);
        QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            item->setSelected(true);
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }
}

void newHome::mousePress()
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void newHome::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
    else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
    else
        ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void newHome::mbReadResult(int serverAddress, const QModbusDataUnit &unit)
{
    RoomInfo *room = nullptr;
    for(int i =0; i < mRooms.size(); ++i){
        if(mRooms[i].isInRoom(serverAddress)){
            room = &mRooms[i];
            break;
        }
    }

    if(!room)
        return;

    NMotor *motor = room->findMotor(serverAddress);
    if(!motor)
        return;

    quint16 startAddress = unit.startAddress();
    quint16 valueCount = unit.valueCount();

    //![1] Check which registers
    quint16 *buff = motor->findRegister(startAddress);

    if(!buff)
        return;

    //![2] Update data to motor
    for (uint i = 0; i < valueCount; i++) {
        *buff++ = unit.value(i);
    }

    motor->m_communicationState = FanCommunicationState::m_connect;//!< Update communication state

    //![3] update ui
    if(serverAddress == currentMotorId)
        updateMotorUi(startAddress);

    if(currentRoom->isInRoom(serverAddress)){
        if(startAddress == g_mRealTimeRegisterAddress){

            int lampId = serverAddress - currentRoom->firstAddress;
            if(motor->m_motorController.m_runState == FanMotorState::m_run)
                runLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_green);
            else if(motor->m_motorController.m_runState == FanMotorState::m_stop)
                runLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_yellow);
            else{
                runLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_red);
            }

            if(motor->m_communicationState == FanCommunicationState::m_connect)
                comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_green);
            else if(motor->m_communicationState == FanCommunicationState::m_disconnect)
                comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_red);
            else if(motor->m_communicationState == FanCommunicationState::m_comError)
                comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_yellow);
            else
                comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_grey);

        }
    }

}

void newHome::mbError(int serverAddress, QString error)
{
    Q_UNUSED(error)

    RoomInfo *room = nullptr;
    for(int i =0; i < mRooms.size(); ++i){
        if(mRooms[i].isInRoom(serverAddress)){
            room = &mRooms[i];
            break;
        }
    }

    if(!room)
        return;

    NMotor *motor = room->findMotor(serverAddress);
    if(!motor)
        return;

    motor->m_communicationState = FanCommunicationState::m_disconnect;

    if(serverAddress == currentMotorId){

        if(currentMotor->m_communicationState == FanCommunicationState::m_connect)
            ui->comLamp->setLampState(QcwIndicatorLamp::lamp_green);
        else if(currentMotor->m_communicationState == FanCommunicationState::m_disconnect)
            ui->comLamp->setLampState(QcwIndicatorLamp::lamp_red);
        else if(currentMotor->m_communicationState == FanCommunicationState::m_comError)
            ui->comLamp->setLampState(QcwIndicatorLamp::lamp_yellow);
        else
            ui->comLamp->setLampState(QcwIndicatorLamp::lamp_grey);
    }

    if(currentRoom->isInRoom(serverAddress)){
        int lampId = serverAddress - currentRoom->firstAddress;
        if(motor->m_communicationState == FanCommunicationState::m_connect)
            comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_green);
        else if(motor->m_communicationState == FanCommunicationState::m_disconnect)
            comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_red);
        else if(motor->m_communicationState == FanCommunicationState::m_comError)
            comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_yellow);
        else
            comLamps[lampId]->setLampState(QcwIndicatorLamp::lamp_grey);


    }
}

void newHome::updateMotorUi(quint16 registerAddress)
{
    //! Monitor data update
    if(registerAddress == g_mRealTimeRegisterAddress){
        if(!ui->checkBox_realTime1->isChecked()){
            ui->SpinBox_speed->setValue((double) currentMotor->m_motorController.m_speedRef * 0.01);
            ui->SpinBox_Torque->setValue((double) currentMotor->m_motorController.m_targetpower * 0.01);
        }
        else{
            mAxis["SpeedRef"].pop_front();
            mAxis["SpeedRef"].push_back(currentMotor->m_motorController.m_speedRef);
            mAxis["SpeedFbk"].pop_front();
            mAxis["SpeedFbk"].push_back(currentMotor->m_motorController.m_speedFbk);
            mAxis["IdRef"].pop_front();
            mAxis["IdRef"].push_back(currentMotor->m_motorController.m_idRef);
            mAxis["IdFbk"].pop_front();
            mAxis["IdFbk"].push_back(currentMotor->m_motorController.m_idFbk);
            mAxis["IqRef"].pop_front();
            mAxis["IqRef"].push_back(currentMotor->m_motorController.m_iqRef);
            mAxis["IqFbk"].pop_front();
            mAxis["IqFbk"].push_back(currentMotor->m_motorController.m_iqFbk);
            mAxis["Reserve1"].pop_front();
            mAxis["Reserve1"].push_back(currentMotor->m_motorController.m_reserve1);
            mAxis["Reserve2"].pop_front();
            mAxis["Reserve2"].push_back(currentMotor->m_motorController.m_reserve2);
            mAxis["Reserve3"].pop_front();
            mAxis["Reserve3"].push_back(currentMotor->m_motorController.m_reserve3);

            for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
                QString name = ui->customPlot->graph(i)->name();
                ui->customPlot->graph(i)->setData(yAxis1, mAxis[name]);
            }
            ui->customPlot->replot();
        }


        if(currentMotor->m_motorController.m_runState == FanMotorState::m_run)
            ui->runLamp->setLampState(QcwIndicatorLamp::lamp_green);
        else if(currentMotor->m_motorController.m_runState == FanMotorState::m_stop)
            ui->runLamp->setLampState(QcwIndicatorLamp::lamp_yellow);
        else{
            ui->runLamp->setLampState(QcwIndicatorLamp::lamp_red);
        }
        ui->comboBox_state->setCurrentIndex(ui->comboBox_state->findData(\
                                                QString::number((currentMotor->m_motorController.m_runState << 8)|
                                                                currentMotor->m_motorController.m_runError)));

    }
    //PI
    if(registerAddress == g_mPIParaRegisterAddress){
        ui->SpinBox_speedPRW->setValue((double) currentMotor->m_PIPara.m_speedKp * 0.001);
        ui->SpinBox_speedIRW->setValue((double) currentMotor->m_PIPara.m_speedKi * 0.001);
        ui->SpinBox_idPRW->setValue((double) currentMotor->m_PIPara.m_idKp * 0.001);
        ui->SpinBox_idIRW->setValue((double) currentMotor->m_PIPara.m_idKi * 0.001);
        ui->SpinBox_iqPRW->setValue((double) currentMotor->m_PIPara.m_iqKp * 0.001);
        ui->SpinBox_iqIRW->setValue((double) currentMotor->m_PIPara.m_iqKi * 0.001);
    }

    //Custom
    if(registerAddress == g_mCustomAddress){
        ui->SpinBox_custom1->setValue(currentMotor->m_custom.m_custom1* 0.01);
        ui->SpinBox_custom2->setValue(currentMotor->m_custom.m_custom2* 0.01);
        ui->SpinBox_custom3->setValue(currentMotor->m_custom.m_custom3* 0.01);
    }


    if(currentMotor->m_communicationState == FanCommunicationState::m_connect)
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_green);
    else if(currentMotor->m_communicationState == FanCommunicationState::m_disconnect)
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_red);
    else if(currentMotor->m_communicationState == FanCommunicationState::m_comError)
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_yellow);
    else
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_grey);

}

void newHome::on_readPIBtn_clicked()
{
    mModbusHandler->read(currentMotorId,g_mPIParaRegisterAddress,g_mPIParaRegisterCount);
}

void newHome::on_setPIBtn_clicked()
{
    FanPIParameters &_PIPara = currentMotor->m_PIPara;
    _PIPara.m_speedKp = (quint16)(ui->SpinBox_speedPRW->value() * 1000);
    _PIPara.m_speedKi = (quint16)(ui->SpinBox_speedIRW->value() * 1000);
    _PIPara.m_idKp = (quint16)(ui->SpinBox_idPRW->value() * 1000);
    _PIPara.m_idKi = (quint16)(ui->SpinBox_idIRW->value() * 1000);
    _PIPara.m_iqKp = (quint16)(ui->SpinBox_iqPRW->value() * 1000);
    _PIPara.m_iqKi = (quint16)(ui->SpinBox_iqIRW->value() * 1000);

    quint16 *buff = (quint16 *)&_PIPara;

    int startAddress = g_mPIParaRegisterAddress;
    int numberOfEntries = g_mPIParaRegisterCount;
    QModbusDataUnit unit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
    for (uint i = 0; i < unit.valueCount(); ++i) {

        unit.setValue(i, *buff++);
    }
    mModbusHandler->write(currentMotorId, unit);



}

void newHome::on_checkBox_realTime1_stateChanged(int arg1)
{
    if(arg1){
        pollingTimer.stop();
        QTimer::singleShot(0, [this]() { monitorTimer_update();});
    }
    else
        pollingTimer.start(POLLING_PERIOD);
}

void newHome::monitorTimer_update()
{
    mModbusHandler->read(currentMotorId,g_mRealTimeRegisterAddress,g_mRealTimeRegisterAllCount);
    if(ui->checkBox_realTime1->isChecked())
        QTimer::singleShot(PERIOD, [this]() { monitorTimer_update();});
}

void newHome::on_startMotor_btn_clicked()
{
    FanMotorController &motorController = currentMotor->m_motorController;
    motorController.m_runState = FanMotorState::m_run;
    quint16 *buff = (quint16 *)&motorController;
    buff += 2;

    int startAddress = g_mRealTimeRegisterAddress;
    int numberOfEntries = g_mRealTimeRegisterStateCount;
    QModbusDataUnit unit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
    for (uint i = 0; i < unit.valueCount(); i++) {
        unit.setValue(i, *buff++);
    }
    mModbusHandler->write(currentMotorId, unit);
    mModbusHandler->read(currentMotorId,startAddress,numberOfEntries);
}

void newHome::on_stopMotor_btn_clicked()
{
    FanMotorController &motorController = currentMotor->m_motorController;
    motorController.m_runState = FanMotorState::m_stop;
    quint16 *buff = (quint16 *)&motorController;
    buff += 2;

    int startAddress = g_mRealTimeRegisterAddress;
    int numberOfEntries = g_mRealTimeRegisterStateCount;
    QModbusDataUnit unit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
    for (uint i = 0; i < unit.valueCount(); i++) {
        unit.setValue(i, *buff++);
    }
    mModbusHandler->write(currentMotorId, unit);
    mModbusHandler->read(currentMotorId,startAddress,numberOfEntries);
}

void newHome::on_readBtn_clicked()
{
    mModbusHandler->read(currentMotorId, g_mCustomAddress, g_mCCustomRegisterCount);
    mModbusHandler->read(currentMotorId, g_mRealTimeRegisterAddress, g_mTorqueSpeed);
}

void newHome::on_setBtn_clicked()
{
    FanMotorCustom &custom = currentMotor->m_custom;
    custom.m_custom1 = (quint16) (ui->SpinBox_custom1->value()*100);
    custom.m_custom2 = (quint16) (ui->SpinBox_custom2->value()*100);
    custom.m_custom3 = (quint16) (ui->SpinBox_custom3->value()*100);
    quint16 *buff = (quint16 *)&custom;

    int startAddress = g_mCustomAddress;
    int numberOfEntries = g_mCCustomRegisterCount;
    QModbusDataUnit unit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
    for (uint i = 0; i < unit.valueCount(); ++i) {
        unit.setValue(i, *buff++);
    }
    mModbusHandler->write(currentMotorId, unit);

    FanMotorController &ts = currentMotor->m_motorController;
    ts.m_targetpower = (quint16) (ui->SpinBox_Torque->value()*100);
    ts.m_speedRef = (quint16) (ui->SpinBox_speed->value()*100);
    buff = (quint16 *)&ts;
    buff += 2;

    startAddress = g_mRealTimeRegisterAddress;
    numberOfEntries = g_mTorqueSpeed;
    QModbusDataUnit unit2 = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
    for (uint i = 0; i < unit2.valueCount(); ++i) {
        unit2.setValue(i, *buff++);
    }
    mModbusHandler->write(currentMotorId, unit2);
}

void newHome::on_SaveBtn_clicked()
{
    QString xlsFile = QFileDialog::getSaveFileName(this,QString(),QString(),"excel(*.xls *.xlsx)");// = QFileDialog::getExistingDirectory(this);
    if(xlsFile.isEmpty())
        return;

    ExcelBase m_xls;
    m_xls.create(xlsFile);

    QList< QList<QVariant> > m_datas;

    QList<QVariant> rows;
    rows.append(tr("Index"));
    for(auto k : mAxis.keys()){
        rows.append(k);
    }
    m_datas.append(rows);
    for(auto i = 0; i< PLOT_POINT; ++i){
        QList<QVariant> rows;
        rows.append(i + 1);
        for(auto k : mAxis.keys()){
            rows.append(mAxis[k].at(i));
        }
        m_datas.append(rows);
    }

    m_xls.setCurrentSheet(1);
    m_xls.writeCurrentSheet(m_datas);
    m_xls.save();
}

void newHome::on_openExcelBtn_clicked()
{
    QString xlsFile = QFileDialog::getOpenFileName(this,QString(),QString(),"excel(*.xls *.xlsx)");
    if(xlsFile.isEmpty())
        return;

    ExcelBase m_xls;
    m_xls.open(xlsFile);
    m_xls.setCurrentSheet(1);
    QList< QList<QVariant> > m_datas;
    m_xls.readAll(m_datas);

    for(auto i = 1; i <= PLOT_POINT; ++i){
        QList<QVariant> rows = m_datas.at(i);
        for(auto j = 1;j < rows.size(); ++j){
            auto k = m_datas[0][j].toString();
            if(mAxis.contains(k)){
                mAxis[k][i-1] = rows[j].toDouble();
                qDebug()<<rows[j].toDouble();}
        }
    }
    for(auto k : mAxis.keys()){
        //addOneGraph(k);
        if(k == "SpeedRef")
            ui->checkBox_speed->setCheckState(Qt::Checked);
        else if(k == "IdRef")
            ui->checkBox_speed->setCheckState(Qt::Checked);
        else if(k == "IqRef")
            ui->checkBox_speed->setCheckState(Qt::Checked);
        else if(k == "Reserve1")
            ui->checkBox_speed->setCheckState(Qt::Checked);
        else if(k == "Reserve2")
            ui->checkBox_speed->setCheckState(Qt::Checked);
        else if(k == "Reserve3")
            ui->checkBox_speed->setCheckState(Qt::Checked);
    }
}

void newHome::on_checkBox_speed_stateChanged(int arg1)
{
    if(arg1){
        addOneGraph(tr("SpeedRef"));
        addOneGraph(tr("SpeedFbk"));
    }
    else{
        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
            if(ui->customPlot->graph(i)->name() == "SpeedRef")
                ui->customPlot->removeGraph(i);
        }
        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
            if(ui->customPlot->graph(i)->name() == "SpeedFbk")
                ui->customPlot->removeGraph(i);
        }
    }
    ui->customPlot->replot();
}

void newHome::on_checkBox_id_stateChanged(int arg1)
{
    if(arg1){
        addOneGraph(tr("IdRef"));
        addOneGraph(tr("IdFbk"));
    }
    else{
        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
            if(ui->customPlot->graph(i)->name() == "IdRef")
                ui->customPlot->removeGraph(i);
        }
        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
            if(ui->customPlot->graph(i)->name() == "IdFbk")
                ui->customPlot->removeGraph(i);
        }
    }
    ui->customPlot->replot();
}

void newHome::on_checkBox_iq_stateChanged(int arg1)
{
    if(arg1){
        addOneGraph(tr("IqRef"));
        addOneGraph(tr("IqFbk"));
    }
    else{
        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
            if(ui->customPlot->graph(i)->name() == "IqRef")
                ui->customPlot->removeGraph(i);
        }
        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
            if(ui->customPlot->graph(i)->name() == "IqFbk")
                ui->customPlot->removeGraph(i);
        }
    }
    ui->customPlot->replot();
}

void newHome::on_checkBox_reserve1_stateChanged(int arg1)
{
    if(arg1){
        addOneGraph(tr("Reserve1"));

    }
    else{
        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
            if(ui->customPlot->graph(i)->name() == "Reserve1")
                ui->customPlot->removeGraph(i);
        }
    }
    ui->customPlot->replot();
}

void newHome::on_checkBox_reserve2_stateChanged(int arg1)
{
    if(arg1){
        addOneGraph(tr("Reserve2"));

    }
    else{
        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
            if(ui->customPlot->graph(i)->name() == "Reserve2")
                ui->customPlot->removeGraph(i);
        }
    }
    ui->customPlot->replot();
}

void newHome::on_checkBox_reserve3_stateChanged(int arg1)
{
    if(arg1){
        addOneGraph(tr("Reserve3"));

    }
    else{
        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
            if(ui->customPlot->graph(i)->name() == "Reserve3")
                ui->customPlot->removeGraph(i);
        }
    }
    ui->customPlot->replot();
}

void newHome::pollingState()
{

    for(int j = 0; j < currentRoom->number; ++j){
        mModbusHandler->read(currentRoom->firstAddress + j, \
                             g_mRealTimeRegisterAddress, g_mRealTimeRegisterStateCount);

    }
}
