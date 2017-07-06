#include "cmotormodbus.h"
#include "ui_cmotormodbus.h"
#include "ctools/ctools.h"
#include <QSerialPortInfo>
#include <QTimer>
#include <QDir>
#include "../excelbase/ExcelBase.h"
#include <QElapsedTimer>

#include<vector>

#define PLOT_POINT 100
#define PERIOD (1.145*(8+(1+1+1+2*12+2)+3.5*2))

CMotorModbus::CMotorModbus(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CMotorModbus),
    mModbusHandler(new ModbusHandler),
    yAxis1(PLOT_POINT, 0)
{
    ui->setupUi(this);

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

    initUi();

}

void CMotorModbus::initUi()
{
    CTools tools;
    tools.updateOptions(QDir::currentPath() + "/json/modbusui.json", "BaudRate", ui->baudCombo);
    tools.updateOptions(QDir::currentPath() + "/json/modbusui.json", "DataBits", ui->dataBitsCombo);
    tools.updateOptions(QDir::currentPath() + "/json/modbusui.json", "Parity", ui->parityCombo);
    tools.updateOptions(QDir::currentPath() + "/json/modbusui.json", "StopBits", ui->stopBitsCombo);
    tools.updateOptions(QDir::currentPath() + "/json/modbusui.json", "StopBits", ui->stopBitsCombo);
    tools.updateOptions(QDir::currentPath() + "/json/modbusui.json", "comboBox_state", ui->comboBox_state);

    QMap<QString,QString> map;
    tools.readSettings(QDir::currentPath() + "/Catcher.ini", "CMotorModbus", map);
    ui->serialPortInfoListBox->setCurrentText(map.value("serialPortInfoListBox", ""));
    ui->baudCombo->setCurrentIndex(map.value("baudCombo", "0").toInt());
    ui->dataBitsCombo->setCurrentIndex(map.value("dataBitsCombo", "0").toInt());
    ui->parityCombo->setCurrentIndex(map.value("parityCombo", "0").toInt());
    ui->stopBitsCombo->setCurrentIndex(map.value("stopBitsCombo", "0").toInt());
    ui->timeoutSpinner->setValue(map.value("timeoutSpinner", "50").toInt());
    ui->retriesSpinner->setValue(map.value("retriesSpinner", "1").toInt());
    ui->serverEdit->setValue(map.value("serverEdit", "1").toInt());

    ui->serialPortInfoListBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        list << info.portName();
        ui->serialPortInfoListBox->addItem(list.first(), list);
    }

    //! Plot init
    //! set some pens, brushes and backgrounds:
//    ui->customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
//    ui->customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
//    ui->customPlot->xAxis->setTickPen(QPen(Qt::white, 1));
//    ui->customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
//    ui->customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
//    ui->customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
//    ui->customPlot->xAxis->setTickLabelColor(Qt::white);
//    ui->customPlot->yAxis->setTickLabelColor(Qt::white);
//    ui->customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ui->customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//    ui->customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ui->customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//    ui->customPlot->xAxis->grid()->setSubGridVisible(true);
//    ui->customPlot->yAxis->grid()->setSubGridVisible(true);
//    ui->customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ui->customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//    ui->customPlot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    ui->customPlot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//    QLinearGradient plotGradient;
//    plotGradient.setStart(0, 0);
//    plotGradient.setFinalStop(0, 350);
//    plotGradient.setColorAt(0, QColor(80, 80, 80));
//    plotGradient.setColorAt(1, QColor(50, 50, 50));
//    ui->customPlot->setBackground(plotGradient);
//    QLinearGradient axisRectGradient;
//    axisRectGradient.setStart(0, 0);
//    axisRectGradient.setFinalStop(0, 350);
//    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
//    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
//    ui->customPlot->axisRect()->setBackground(axisRectGradient);

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
void CMotorModbus::addOneGraph(const QString &name)
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

void CMotorModbus::addGraph()
{
    QString name;
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
    {
        name = contextAction->data().toString();
    }
    if(mAxis.contains(name)){
        addOneGraph(name);
    }
    else{
        addOneGraph(name + tr("Ref"));
        addOneGraph(name + tr("Fbk"));
    }

}


void CMotorModbus::contextMenuRequest(QPoint pos)
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
    } else  // general context menu on graphs requested
    {
        //        QSet<QString> now;
        //        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
        //            now.insert(ui->customPlot->graph(i)->name());
        //        }
        //        if(!now.contains("SpeedRef"))
        //            menu->addAction("Add Speed graph", this, SLOT(addGraph()))->setData("Speed");
        //        if(!now.contains("IdRef"))
        //            menu->addAction("Add Id graph", this, SLOT(addGraph()))->setData("Id");
        //        if(!now.contains("IqRef"))
        //            menu->addAction("Add Iq graph", this, SLOT(addGraph()))->setData("Iq");
        //        if(!now.contains("Reserve1"))
        //            menu->addAction("Add Reserve1 graph", this, SLOT(addGraph()))->setData("Reserve1");
        //        if(!now.contains("Reserve2"))
        //            menu->addAction("Add Reserve2 graph", this, SLOT(addGraph()))->setData("Reserve2");
        //        if(!now.contains("Reserve3"))
        //            menu->addAction("Add Reserve3 graph", this, SLOT(addGraph()))->setData("Reserve3");
        //        if (ui->customPlot->selectedGraphs().size() > 0)
        //            menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
        //        if (ui->customPlot->graphCount() > 0)
        //            menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
    }

    menu->popup(ui->customPlot->mapToGlobal(pos));
}

void CMotorModbus::moveLegend()
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

void CMotorModbus::removeSelectedGraph()
{
    if (ui->customPlot->selectedGraphs().size() > 0)
    {
        ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
        ui->customPlot->replot();

    }
}
void CMotorModbus::removeAllGraphs()
{
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();
}

void CMotorModbus::selectionChanged()
{
    /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

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

void CMotorModbus::mousePress()
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

void CMotorModbus::mouseWheel()
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


CMotorModbus::~CMotorModbus()
{
    QMap<QString,QString> set;
    set.insert("serialPortInfoListBox", ui->serialPortInfoListBox->currentText());
    set.insert("baudCombo", QString::number(ui->baudCombo->currentIndex()));
    set.insert("dataBitsCombo", QString::number(ui->dataBitsCombo->currentIndex()));
    set.insert("parityCombo", QString::number(ui->parityCombo->currentIndex()));
    set.insert("stopBitsCombo", QString::number(ui->stopBitsCombo->currentIndex()));
    set.insert("timeoutSpinner", QString::number(ui->timeoutSpinner->value()));
    set.insert("retriesSpinner", QString::number(ui->retriesSpinner->value()));
    set.insert("serverEdit", QString::number(ui->serverEdit->value()));
    CTools tools;
    tools.saveSettings(QDir::currentPath() + "/Catcher.ini", "CMotorModbus", set);

    if(mModbusHandler->isOpen()){
        mModbusHandler->close();
        ui->connectButton->setText(tr("Open"));
        disconnect(mModbusHandler.data(), &ModbusHandler::readResult, this, &CMotorModbus::mbReadResult);
    }

    delete ui;
}

void CMotorModbus::mbReadResult(int serverAddress, const QModbusDataUnit &unit)
{
    Q_UNUSED(serverAddress)
    quint16 startAddress = unit.startAddress();
    quint16 valueCount = unit.valueCount();

    //![1] Check which registers
    quint16 *buff = mMotor.findRegister(startAddress);

    if(!buff)
        return;

    //![2] Update data to motor
    for (uint i = 0; i < valueCount; i++) {
        *buff++ = unit.value(i);
    }

    mMotor.m_communicationState = FanCommunicationState::m_connect;//!< Update communication state

    //![3] update ui
    updateMotorUi(startAddress);
}

void CMotorModbus::updateMotorUi(quint16 registerAddress)
{
    //! Monitor data update
    if(registerAddress == g_mRealTimeRegisterAddress){
        if(!ui->checkBox_realTime1->isChecked()){
            ui->SpinBox_speed->setValue((double) mMotor.m_motorController.m_speedRef);
            ui->SpinBox_Torque->setValue((double) mMotor.m_motorController.m_targetpower);
        }


        mAxis["SpeedRef"].pop_front();
        mAxis["SpeedRef"].push_back(mMotor.m_motorController.m_speedRef);
        mAxis["SpeedFbk"].pop_front();
        mAxis["SpeedFbk"].push_back(mMotor.m_motorController.m_speedFbk);
        mAxis["IdRef"].pop_front();
        mAxis["IdRef"].push_back(mMotor.m_motorController.m_idRef);
        mAxis["IdFbk"].pop_front();
        mAxis["IdFbk"].push_back(mMotor.m_motorController.m_idFbk);
        mAxis["IqRef"].pop_front();
        mAxis["IqRef"].push_back(mMotor.m_motorController.m_iqRef);
        mAxis["IqFbk"].pop_front();
        mAxis["IqFbk"].push_back(mMotor.m_motorController.m_iqFbk);
        mAxis["Reserve1"].pop_front();
        mAxis["Reserve1"].push_back(mMotor.m_motorController.m_reserve1);
        mAxis["Reserve2"].pop_front();
        mAxis["Reserve2"].push_back(mMotor.m_motorController.m_reserve2);
        mAxis["Reserve3"].pop_front();
        mAxis["Reserve3"].push_back(mMotor.m_motorController.m_reserve3);

        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
            QString name = ui->customPlot->graph(i)->name();
            ui->customPlot->graph(i)->setData(yAxis1, mAxis[name]);
        }
        ui->customPlot->replot();


        if(mMotor.m_motorController.m_runState == FanMotorState::m_run)
            ui->runLamp->setLampState(QcwIndicatorLamp::lamp_green);
        else if(mMotor.m_motorController.m_runState == FanMotorState::m_stop)
            ui->runLamp->setLampState(QcwIndicatorLamp::lamp_yellow);
        else{
            ui->runLamp->setLampState(QcwIndicatorLamp::lamp_red);
        }
        ui->comboBox_state->setCurrentIndex(ui->comboBox_state->findData(\
                                                QString::number((mMotor.m_motorController.m_runState << 8)|
                                                                mMotor.m_motorController.m_runError)));

    }
    //PI
    if(registerAddress == g_mPIParaRegisterAddress){
        ui->SpinBox_speedPRW->setValue((double) mMotor.m_PIPara.m_speedKp * 0.001);
        ui->SpinBox_speedIRW->setValue((double) mMotor.m_PIPara.m_speedKi * 0.001);
        ui->SpinBox_idPRW->setValue((double) mMotor.m_PIPara.m_idKp * 0.001);
        ui->SpinBox_idIRW->setValue((double) mMotor.m_PIPara.m_idKi * 0.001);
        ui->SpinBox_iqPRW->setValue((double) mMotor.m_PIPara.m_iqKp * 0.001);
        ui->SpinBox_iqIRW->setValue((double) mMotor.m_PIPara.m_iqKi * 0.001);
    }

    //Custom
    if(registerAddress == g_mCustomAddress){
        ui->SpinBox_custom1->setValue(mMotor.m_custom.m_custom1* 0.01);
        ui->SpinBox_custom2->setValue(mMotor.m_custom.m_custom2* 0.01);
        ui->SpinBox_custom3->setValue(mMotor.m_custom.m_custom3* 0.01);
    }


    if(mMotor.m_communicationState == FanCommunicationState::m_connect)
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_green);
    else if(mMotor.m_communicationState == FanCommunicationState::m_disconnect)
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_red);
    else if(mMotor.m_communicationState == FanCommunicationState::m_comError)
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_yellow);
    else
        ui->comLamp->setLampState(QcwIndicatorLamp::lamp_grey);

}

void CMotorModbus::on_searchButton_clicked()
{
    ui->serialPortInfoListBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        list << info.portName();
        ui->serialPortInfoListBox->addItem(list.first(), list);
    }
    ui->serialPortInfoListBox->showPopup();
}

void CMotorModbus::on_connectButton_clicked()
{
    if(mModbusHandler->isOpen()){
        mModbusHandler->close();
        ui->connectButton->setText(tr("Open"));
        disconnect(mModbusHandler.data(), &ModbusHandler::readResult, this, &CMotorModbus::mbReadResult);
    }
    else{
        SerialPortSettings settings;
        settings.setName(ui->serialPortInfoListBox->currentText());
        settings.setParity(static_cast<QSerialPort::Parity>(
                               ui->parityCombo->itemData(ui->parityCombo->currentIndex()).toInt()));
        if (ui->baudCombo->currentIndex() == 4) {

            settings.setBaudRate(ui->baudCombo->currentText().toInt());
        }else{
            settings.setBaudRate(static_cast<QSerialPort::BaudRate>(
                                     ui->baudCombo->itemData(ui->baudCombo->currentIndex()).toInt()));
        }
        settings.setDataBits(static_cast<QSerialPort::DataBits>(
                                 ui->dataBitsCombo->itemData(ui->dataBitsCombo->currentIndex()).toInt()));
        settings.setStopBits(static_cast<QSerialPort::StopBits>(
                                 ui->stopBitsCombo->itemData(ui->stopBitsCombo->currentIndex()).toInt()));

        int timeout = ui->timeoutSpinner->value();
        int numberOfretries = ui->retriesSpinner->value();
        mModbusHandler->open(settings, timeout, numberOfretries);

        ui->connectButton->setText(tr("Close"));

        connect(mModbusHandler.data(), &ModbusHandler::readResult, this, &CMotorModbus::mbReadResult);

        mPeriod = PERIOD * (9600.0/ui->baudCombo->currentText().toInt()) *2;
    }
}

void CMotorModbus::on_readPIBtn_clicked()
{
    mModbusHandler->read(ui->serverEdit->value(),g_mPIParaRegisterAddress,g_mPIParaRegisterCount);

}

void CMotorModbus::on_setPIBtn_clicked()
{
    FanPIParameters _PIPara;
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
    mModbusHandler->write(ui->serverEdit->value(), unit);



}

void CMotorModbus::on_checkBox_realTime1_stateChanged(int arg1)
{
    if(arg1){
        QTimer::singleShot(0, [this]() { monitorTimer_update();});
    }
}

void CMotorModbus::monitorTimer_update()
{
    mModbusHandler->read(ui->serverEdit->value(),g_mRealTimeRegisterAddress,g_mRealTimeRegisterAllCount);
    if(ui->checkBox_realTime1->isChecked())
        QTimer::singleShot(mPeriod, [this]() { monitorTimer_update();});
}

void CMotorModbus::on_startMotor_btn_clicked()
{
    FanMotorController &motorController = mMotor.m_motorController;
    motorController.m_runState = FanMotorState::m_run;
    quint16 *buff = (quint16 *)&motorController;
    buff += 2;

    int startAddress = g_mRealTimeRegisterAddress;
    int numberOfEntries = g_mRealTimeRegisterStateCount;
    QModbusDataUnit unit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
    for (uint i = 0; i < unit.valueCount(); i++) {
        unit.setValue(i, *buff++);
    }
    mModbusHandler->write(ui->serverEdit->value(), unit);
    mModbusHandler->read(ui->serverEdit->value(),startAddress,numberOfEntries);
}

void CMotorModbus::on_stopMotor_btn_clicked()
{
    FanMotorController &motorController = mMotor.m_motorController;
    motorController.m_runState = FanMotorState::m_stop;
    quint16 *buff = (quint16 *)&motorController;
    buff += 2;

    int startAddress = g_mRealTimeRegisterAddress;
    int numberOfEntries = g_mRealTimeRegisterStateCount;
    QModbusDataUnit unit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
    for (uint i = 0; i < unit.valueCount(); i++) {
        unit.setValue(i, *buff++);
    }
    mModbusHandler->write(ui->serverEdit->value(), unit);
    mModbusHandler->read(ui->serverEdit->value(),startAddress,numberOfEntries);
}

void CMotorModbus::on_readBtn_clicked()
{
    mModbusHandler->read(ui->serverEdit->value(), g_mCustomAddress, g_mCCustomRegisterCount);
    mModbusHandler->read(ui->serverEdit->value(), g_mRealTimeRegisterAddress, g_mTorqueSpeed);
}

void CMotorModbus::on_setBtn_clicked()
{
    FanMotorCustom &custom = mMotor.m_custom;
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
    mModbusHandler->write(ui->serverEdit->value(), unit);

    FanMotorController &ts = mMotor.m_motorController;
    ts.m_targetpower = (quint16) (ui->SpinBox_Torque->value());
    ts.m_speedRef = (quint16) (ui->SpinBox_speed->value());
    buff = (quint16 *)&ts;
    buff += 2;

    startAddress = g_mRealTimeRegisterAddress;
    numberOfEntries = g_mTorqueSpeed;
    QModbusDataUnit unit2 = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numberOfEntries);
    for (uint i = 0; i < unit2.valueCount(); ++i) {
        unit2.setValue(i, *buff++);
    }
    mModbusHandler->write(ui->serverEdit->value(), unit2);
}

void CMotorModbus::on_SaveBtn_clicked()
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

void CMotorModbus::on_openExcelBtn_clicked()
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
        addOneGraph(k);
    }
}



void CMotorModbus::on_checkBox_speed_stateChanged(int arg1)
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

void CMotorModbus::on_checkBox_id_stateChanged(int arg1)
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

void CMotorModbus::on_checkBox_iq_stateChanged(int arg1)
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

void CMotorModbus::on_checkBox_reserve1_stateChanged(int arg1)
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

void CMotorModbus::on_checkBox_reserve2_stateChanged(int arg1)
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

void CMotorModbus::on_checkBox_reserve3_stateChanged(int arg1)
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
