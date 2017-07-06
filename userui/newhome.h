#ifndef NEWHOME_H
#define NEWHOME_H

#include "ctools/modbushandler.h"
#include "fanmotor/fpublic.h"
#include "lamp/qcw_indicatorlamp.h"

#include <QFrame>
#include <QButtonGroup>
#include <QSharedPointer>
#include <QTimer>

namespace Ui {
class newHome;
}

class newHome : public QFrame
{
    Q_OBJECT

public:
    explicit newHome(QWidget *parent = 0);
    ~newHome();

private slots:
    void on_next1_clicked();
    void on_previous1_clicked();
    void on_previous2_clicked();
    void on_searchButton_clicked();
    void on_connectButton_clicked();
    void on_spinBox_motorNum_valueChanged(int arg1);

    void mbReadResult(int serverAddress, const QModbusDataUnit &unit);
    void mbError(int serverAddress, QString error);
    void on_readPIBtn_clicked();
    void on_setPIBtn_clicked();
    void on_startMotor_btn_clicked();
    void on_stopMotor_btn_clicked();
    void on_readBtn_clicked();
    void on_setBtn_clicked();
    void on_checkBox_realTime1_stateChanged(int arg1);

    void contextMenuRequest(QPoint pos);
    void moveLegend();
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

    void pollingState();

private:
    void enterRoom(int id);
    void enterMotor(int id);
    void initConnections();
    void initData();
    void initUi();

    void updateMotorUi(quint16 registerAddress);
    void monitorTimer_update();
    void addOneGraph(const QString &name);

private:
    Ui::newHome *ui;

    QButtonGroup mRoomBtns;
    QVector<RoomInfo> mRooms;
    QButtonGroup mMotorBtns;
    QSharedPointer<ModbusHandler> mModbusHandler;
    RoomInfo *currentRoom;
    NMotor *currentMotor;
    int currentRoomId;
    int currentMotorId;

    QVector<double>    yAxis1;
    QMap<QString, QVector<double>> mAxis;

    QVector<QcwIndicatorLamp*> comLamps;
    QVector<QcwIndicatorLamp*> runLamps;

    QTimer pollingTimer;

};

#endif // NEWHOME_H
