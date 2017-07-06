#ifndef CMOTORMODBUS_H
#define CMOTORMODBUS_H

#include "ctools/modbushandler.h"
#include "fanmotor/fpublic.h"

#include <QFrame>
#include <QSharedPointer>
#include <QVector>
#include <QMap>



namespace Ui {
class CMotorModbus;
}

class CMotorModbus : public QFrame
{
    Q_OBJECT

public:
    explicit CMotorModbus(QWidget *parent = 0);
    ~CMotorModbus();
    void updateMotorUi(quint16 registerAddress);
    void monitorTimer_update();
private slots:
    void mbReadResult(int serverAddress, const QModbusDataUnit &unit);
    void on_searchButton_clicked();
    void on_connectButton_clicked();
    void on_readPIBtn_clicked();
    void on_setPIBtn_clicked();
    void on_startMotor_btn_clicked();
    void on_stopMotor_btn_clicked();

    void on_readBtn_clicked();
    void on_setBtn_clicked();
    void on_checkBox_realTime1_stateChanged(int arg1);

    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void addGraph();
    void removeSelectedGraph();
    void removeAllGraphs();
    void mouseWheel();
    void mousePress();
    void selectionChanged();
    void on_SaveBtn_clicked();
    void on_openExcelBtn_clicked();

    void on_checkBox_speed_stateChanged(int arg1);
    void on_checkBox_id_stateChanged(int arg1);
    void on_checkBox_iq_stateChanged(int arg1);
    void on_checkBox_reserve1_stateChanged(int arg1);
    void on_checkBox_reserve2_stateChanged(int arg1);
    void on_checkBox_reserve3_stateChanged(int arg1);

private:
    Ui::CMotorModbus *ui;
    QSharedPointer<ModbusHandler> mModbusHandler;
    QMotor mMotor;
    quint16 mPeriod = 0;

    QVector<double>    yAxis1;
    QMap<QString, QVector<double>> mAxis;

    void initUi();


    void addOneGraph(const QString &name);
};

#endif // CMOTORMODBUS_H
