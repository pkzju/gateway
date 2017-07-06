#include "canevs.h"
#include "ui_canevs.h"
#include "QDebug"
#include "QDir"

#include <ctools/ctools.h>
#include "../excelbase/ExcelBase.h"

#include "qusbcan/qusbcan.h"

#define PLOT_POINT 100
#define PERIOD (1.145*(8+(1+1+1+2*12+2)+3.5*2))

CanEVS::CanEVS(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CanEVS)
  ,mUsbCan{new QUsbCan(this)}
  ,yAxis1(PLOT_POINT, 0)
{
    ui->setupUi(this);
    ui->textBrowser->document()->setMaximumBlockCount(50);

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

void CanEVS::initUi()
{
    CTools ctools;

    ctools.updateOptions(QDir::currentPath()+"/json/canevs.json", "speed_comB", ui->speed_comB);
    ctools.updateOptions(QDir::currentPath() + "/json/canevs.json", "comboBox_state", ui->comboBox_state);

    ui->speed_comB->setCurrentIndex(2);

    //! Plot init
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

    ui->checkBox_speed_2->setCheckState(Qt::Checked);

}

CanEVS::~CanEVS()
{
    if(ui->pushButton_Close->isEnabled())
        on_pushButton_Close_clicked();
    delete ui;
}

void CanEVS::receiveHandler()
{
    if(!mUsbCan)
        return;

    QCanBusFrame frame = mUsbCan->readFrame();
    if(!frame.isValid())
        return;

    const qint32 id = frame.frameId();
    const QByteArray payload = frame.payload();
    const qint8 dataLength = payload.size();

    qint32 r_id = 0;
    if(ui->Hex_recieve->isChecked()){
        bool ok = false;
        r_id = ui->idEdit_recive->text().toInt(&ok, 16);
    }
    else
        r_id = ui->idEdit_recive->text().toInt();

    if(id == r_id || r_id == 0){
        QString view;
        view += QLatin1String("Received: ");
        view += QLatin1String("Id: ");
        view += QString::number(id, 16);
        view += QLatin1String(" bytes: ");
        view += QString::number(dataLength, 10);
        view += QLatin1String(" data: ");
        view += payload.toHex();
        ui->textBrowser->setTextColor(Qt::darkGreen);
        ui->textBrowser->append(view);
    }

    if(dataLength < 8)
        return;

    can_J1939_frame *f = mMotor.findFrame(id);

    if(!f)
        return;

    f->Byte.Byte0 = payload.at(0);
    f->Byte.Byte1 = payload.at(1);
    f->Byte.Byte2 = payload.at(2);
    f->Byte.Byte3 = payload.at(3);
    f->Byte.Byte4 = payload.at(4);
    f->Byte.Byte5 = payload.at(5);
    f->Byte.Byte6 = payload.at(6);
    f->Byte.Byte7 = payload.at(7);


    if(readPICount>0 && (id == gSpeedIdPIRecieveID || id == gIqPIRecieveID)){
        ui->SpinBox_speedPRW->setValue((double) mMotor.speedIdPI.speedIdPI.SpeedP * 0.001);
        ui->SpinBox_speedIRW->setValue((double) mMotor.speedIdPI.speedIdPI.SpeedI * 0.001);
        ui->SpinBox_idPRW->setValue((double) mMotor.speedIdPI.speedIdPI.IdP * 0.001);
        ui->SpinBox_idIRW->setValue((double) mMotor.speedIdPI.speedIdPI.IdI * 0.001);
        ui->SpinBox_iqPRW->setValue((double) mMotor.IqPI.IqPI.IqP * 0.001);
        ui->SpinBox_iqIRW->setValue((double) mMotor.IqPI.IqPI.IqI * 0.001);
        --readPICount;
    }

    if(readCount>0 && (id == gCustomSendID || id == gSpeedTorqueRecieveID)){

        ui->SpinBox_speed->setValue((double) mMotor.speedTorque.speedTorque.SpeedRef);
        ui->SpinBox_Torque->setValue((double) mMotor.speedTorque.speedTorque.TargetTorque);

        --readCount;
    }

    if(id == gCustomSendID){
        ui->lcdNumber_T->display(mMotor.custom.custom.Custom3* 0.01);//tem
        ui->lcdNumber_err->display(mMotor.custom.custom.Custom2);//err
    }

    quint16 state = mMotor.speedTorque.speedTorque.Status;

    if(state == gMotorRun)
        ui->runLamp->setLampState(QcwIndicatorLamp::lamp_green);
    else if(state == gMotorStop)
        ui->runLamp->setLampState(QcwIndicatorLamp::lamp_yellow);
    else{
        ui->runLamp->setLampState(QcwIndicatorLamp::lamp_red);
    }

    ui->comboBox_state->setCurrentIndex(ui->comboBox_state->findData(\
                                                QString::number(state)));

    ui->comLamp->setLampState(QcwIndicatorLamp::lamp_green);

    if(id == gSpeedTorqueRecieveID){
        mAxis["SpeedRef"].pop_front();
        mAxis["SpeedRef"].push_back(mMotor.speedTorque.speedTorque.SpeedRef);
        mAxis["SpeedFbk"].pop_front();
        mAxis["SpeedFbk"].push_back(mMotor.speedTorque.speedTorque.SpeedFbk);

        ui->lcdNumber_S->display(mMotor.speedTorque.speedTorque.SpeedFbk);
    }

    if(id == gIdIqRecieveID)
    {
        mAxis["IdRef"].pop_front();
        mAxis["IdRef"].push_back(mMotor.IdIq.IdIq.IdRef);
        mAxis["IdFbk"].pop_front();
        mAxis["IdFbk"].push_back(mMotor.IdIq.IdIq.IdFbk);
        mAxis["IqRef"].pop_front();
        mAxis["IqRef"].push_back(mMotor.IdIq.IdIq.IqRef);
        mAxis["IqFbk"].pop_front();
        mAxis["IqFbk"].push_back(mMotor.IdIq.IdIq.IqFbk);

        ui->lcdNumber_Id->display(mMotor.IdIq.IdIq.IdFbk);
        ui->lcdNumber_Iq->display(mMotor.IdIq.IdIq.IqFbk);
    }

    if(id == gWatchRecieveID)
    {
        mAxis["Reserve1"].pop_front();
        mAxis["Reserve1"].push_back(mMotor.watch.watch.Watch1);
        mAxis["Reserve2"].pop_front();
        mAxis["Reserve2"].push_back(mMotor.watch.watch.Watch1);
        mAxis["Reserve3"].pop_front();
        mAxis["Reserve3"].push_back(mMotor.watch.watch.Watch1);

        ui->lcdNumber_1->display(mMotor.watch.watch.Watch1);
        ui->lcdNumber_2->display(mMotor.watch.watch.Watch2);
    }

    if(id == gSpeedTorqueRecieveID || id == gIdIqRecieveID || id == gWatchRecieveID)
    {
        for(auto i = 0; i < ui->customPlot->graphCount(); ++i){
            QString name = ui->customPlot->graph(i)->name();
            ui->customPlot->graph(i)->setData(yAxis1, mAxis[name]);
        }
        ui->customPlot->replot();
    }
}

void CanEVS::on_pushButton_Open_clicked()
{
    if(!mUsbCan)
        return;

    ui->textBrowser->append(tr("Opening ...."));

    if(!mUsbCan->connectDevice())
    {
        mUsbCan->disconnectDevice();
        ui->textBrowser->append(tr("Open fail!"));
        return;
    }

    if(mUsbCan->ConnectedState == QCanBusDevice::ConnectedState)
    {
        mUsbCan->setBitRate(ui->speed_comB->currentData().toInt());
        connect(mUsbCan.data(), SIGNAL(framesReceived()), this, SLOT(receiveHandler()));
        ui->pushButton_Open->setEnabled(false);
        ui->pushButton_Close->setEnabled(true);

        ui->textBrowser->append(tr("Opened!"));
        ui->backend_comB_2->setEnabled(false);
    }

}

void CanEVS::on_pushButton_Close_clicked()
{
    if(!mUsbCan)
        return;

    ui->textBrowser->append(tr("Closing ...."));

    mUsbCan->disconnectDevice();

    if(mUsbCan->state() != QCanBusDevice::ConnectedState)
    {
        ui->pushButton_Open->setEnabled(true);
        ui->pushButton_Close->setEnabled(false);
        ui->textBrowser->append(tr("Closed!"));
        ui->backend_comB_2->setEnabled(true);
    }
}

void CanEVS::on_pushButton_ClearAll_clicked()
{
    ui->textBrowser->clear();
}

void CanEVS::on_pushButton_Send_clicked()
{

    qint32 id = 0;
    if(ui->Hex->isChecked()){
        bool ok = false;
        id = ui->idEdit->text().toInt(&ok, 16);
    }
    else
        id = ui->idEdit->text().toInt();
    QString str = ui->lineEdit->text();
    QCanBusFrame frame{0};
    QByteArray payload;
    QString subStr;

    for(auto i = str.begin(); i <= str.end(); ++i){
        if(*i != ' ' && i!=str.end())
            subStr.append(*i);
        else if(!subStr.isEmpty()){
            quint8 num = 0;
            if(ui->Hex->isChecked()){
                bool ok = false;
                num = subStr.toInt(&ok, 16);
            }
            else
                num = subStr.toInt();
            qDebug()<<num;
            payload.append(num);
            subStr.clear();
        }
    }

    frame.setFrameId(id);
    frame.setExtendedFrameFormat(ui->EXT->checkState());
    frame.setPayload(payload);

    mUsbCan->writeFrame(frame);

    QString view;
    view += QLatin1String("Send: ");
    view += QLatin1String("Id: ");
    view += QString::number(frame.frameId(), 16);
    view += QLatin1String(" bytes: ");
    view += QString::number(payload.size(), 10);
    view += QLatin1String(" data: ");
    view += payload.toHex();
    ui->textBrowser->setTextColor(Qt::blue);
    ui->textBrowser->append(view);
}

void CanEVS::on_setPIBtn_clicked()
{
    if(!mUsbCan)
        return;

    can_J1939_frame *f = &mMotor.speedIdPI;
    f->speedIdPI.SpeedP = (quint16)(ui->SpinBox_speedPRW->value() * 1000);
    f->speedIdPI.SpeedI = (quint16)(ui->SpinBox_speedIRW->value() * 1000);
    f->speedIdPI.IdP = (quint16)(ui->SpinBox_idPRW->value() * 1000);
    f->speedIdPI.IdI = (quint16)(ui->SpinBox_idIRW->value() * 1000);

    QCanBusFrame frame{0};
    QByteArray payload, payload2;
    payload.append(f->Byte.Byte0);
    payload.append(f->Byte.Byte1);
    payload.append(f->Byte.Byte2);
    payload.append(f->Byte.Byte3);
    payload.append(f->Byte.Byte4);
    payload.append(f->Byte.Byte5);
    payload.append(f->Byte.Byte6);
    payload.append(f->Byte.Byte7);

    frame.setFrameId(gSpeedIdPISendID);
    frame.setExtendedFrameFormat(true);
    frame.setPayload(payload);
    mUsbCan->writeFrame(frame);

    f = &mMotor.IqPI;
    f->IqPI.IqP = (quint16)(ui->SpinBox_iqPRW->value() * 1000);
    f->IqPI.IqI = (quint16)(ui->SpinBox_iqIRW->value() * 1000);

    payload2.append(f->Byte.Byte0);
    payload2.append(f->Byte.Byte1);
    payload2.append(f->Byte.Byte2);
    payload2.append(f->Byte.Byte3);
    payload2.append(f->Byte.Byte4);
    payload2.append(f->Byte.Byte5);
    payload2.append(f->Byte.Byte6);
    payload2.append(f->Byte.Byte7);

    frame.setFrameId(gIqPISendID);
    frame.setExtendedFrameFormat(true);
    frame.setPayload(payload2);
    mUsbCan->writeFrame(frame);

}

void CanEVS::on_setBtn_clicked()
{
    if(!mUsbCan)
        return;

    can_J1939_frame *f = &mMotor.speedTorque;

    f->speedTorque.SpeedRef = (quint16)(ui->SpinBox_speed->value());
    f->speedTorque.TargetTorque = (quint16)(ui->SpinBox_Torque->value());
 //   mMotor.speedIdPI.speedTorque.Status = (quint16)(ui->SpinBox_idPRW->value() * 1000);

    QCanBusFrame frame{0};
    QByteArray payload;
    payload.append(f->Byte.Byte0);
    payload.append(f->Byte.Byte1);
    payload.append(f->Byte.Byte2);
    payload.append(f->Byte.Byte3);
    payload.append(f->Byte.Byte4);
    payload.append(f->Byte.Byte5);
    payload.append(f->Byte.Byte6);
    payload.append(f->Byte.Byte7);

    frame.setFrameId(gSpeedTorqueSendID);
    frame.setExtendedFrameFormat(true);
    frame.setPayload(payload);
    mUsbCan->writeFrame(frame);



}

void CanEVS::on_setBtn2_clicked()
{
    if(!mUsbCan)
        return;

    can_J1939_frame *f = &mMotor.custom;

    f->custom.Custom1 = (quint16)(ui->SpinBox_Id->value());//id
    f->custom.Reserve = (quint16)(ui->SpinBox_Iq->value());//iq

    QCanBusFrame frame{0};
    QByteArray  payload2;
    payload2.append(f->Byte.Byte0);
    payload2.append(f->Byte.Byte1);
    payload2.append(f->Byte.Byte2);
    payload2.append(f->Byte.Byte3);
    payload2.append(f->Byte.Byte4);
    payload2.append(f->Byte.Byte5);
    payload2.append(f->Byte.Byte6);
    payload2.append(f->Byte.Byte7);

    frame.setFrameId(gCustomSendID);
    frame.setExtendedFrameFormat(true);
    frame.setPayload(payload2);
    mUsbCan->writeFrame(frame);

}

void CanEVS::on_startMotor_btn_clicked()
{
    if(!mUsbCan)
        return;

    can_J1939_frame *f = &mMotor.speedTorque;

    f->speedTorque.Status = gMotorRun;

    QCanBusFrame frame{0};
    QByteArray payload;
    payload.append(f->Byte.Byte0);
    payload.append(f->Byte.Byte1);
    payload.append(f->Byte.Byte2);
    payload.append(f->Byte.Byte3);
    payload.append(f->Byte.Byte4);
    payload.append(f->Byte.Byte5);
    payload.append(f->Byte.Byte6);
    payload.append(f->Byte.Byte7);

    frame.setFrameId(gSpeedTorqueSendID);
    frame.setExtendedFrameFormat(true);
    frame.setPayload(payload);
    mUsbCan->writeFrame(frame);
}

void CanEVS::on_stopMotor_btn_clicked()
{
    if(!mUsbCan)
        return;

    can_J1939_frame *f = &mMotor.speedTorque;

    f->speedTorque.Status = gMotorStop;

    QCanBusFrame frame{0};
    QByteArray payload;
    payload.append(f->Byte.Byte0);
    payload.append(f->Byte.Byte1);
    payload.append(f->Byte.Byte2);
    payload.append(f->Byte.Byte3);
    payload.append(f->Byte.Byte4);
    payload.append(f->Byte.Byte5);
    payload.append(f->Byte.Byte6);
    payload.append(f->Byte.Byte7);

    frame.setFrameId(gSpeedTorqueSendID);
    frame.setExtendedFrameFormat(true);
    frame.setPayload(payload);
    mUsbCan->writeFrame(frame);
}

void CanEVS::on_readPIBtn_clicked()
{
    readPICount = 4;
}

void CanEVS::addOneGraph(const QString &name)
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

void CanEVS::addGraph()
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

void CanEVS::contextMenuRequest(QPoint pos)
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

void CanEVS::moveLegend()
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

void CanEVS::removeSelectedGraph()
{
    if (ui->customPlot->selectedGraphs().size() > 0)
    {
        ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
        ui->customPlot->replot();
    }
}

void CanEVS::removeAllGraphs()
{
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();
}

void CanEVS::selectionChanged()
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

void CanEVS::mousePress()
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

void CanEVS::mouseWheel()
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

void CanEVS::on_SaveBtn_clicked()
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

void CanEVS::on_openExcelBtn_clicked()
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

void CanEVS::on_checkBox_speed_2_stateChanged(int arg1)
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

void CanEVS::on_checkBox_id_2_stateChanged(int arg1)
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

void CanEVS::on_checkBox_iq_2_stateChanged(int arg1)
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

void CanEVS::on_checkBox_reserve1_2_stateChanged(int arg1)
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

void CanEVS::on_checkBox_reserve2_2_stateChanged(int arg1)
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

void CanEVS::on_checkBox_reserve3_2_stateChanged(int arg1)
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

void CanEVS::on_backend_comB_2_currentIndexChanged(int index)
{
    QUsbCan::setChannelIndex(index);

    //qDebug()<<channelIndex;

}


