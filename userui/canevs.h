#ifndef CANEVS_H
#define CANEVS_H

#include "qusbcan/qusbcan.h"
#include "fanmotor/canmotor.h"

#include <QSharedPointer>
#include <QFrame>
#include <QVector>
#include <QMap>

namespace Ui {
class CanEVS;
}

class CanEVS : public QFrame
{
    Q_OBJECT

public:
    explicit CanEVS(QWidget *parent = 0);
    ~CanEVS();

private slots:
    void receiveHandler();
    void on_pushButton_Open_clicked();
    void on_pushButton_Close_clicked();
    void on_pushButton_ClearAll_clicked();
    void on_pushButton_Send_clicked();
    void on_setPIBtn_clicked();
    void on_setBtn_clicked();
    void on_startMotor_btn_clicked();
    void on_stopMotor_btn_clicked();

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

    void on_readPIBtn_clicked();

    void on_checkBox_speed_2_stateChanged(int arg1);

    void on_checkBox_id_2_stateChanged(int arg1);

    void on_checkBox_iq_2_stateChanged(int arg1);

    void on_checkBox_reserve1_2_stateChanged(int arg1);

    void on_checkBox_reserve2_2_stateChanged(int arg1);

    void on_checkBox_reserve3_2_stateChanged(int arg1);

    void on_backend_comB_2_currentIndexChanged(int index);

    void on_setBtn2_clicked();

private:
    Ui::CanEVS *ui;

    QSharedPointer<QUsbCan> mUsbCan;

    canMotor mMotor;
    QVector<double>    yAxis1;
    QMap<QString, QVector<double>> mAxis;

    int readPICount = 2;
    int readCount = 2;

    void initUi();


    void addOneGraph(const QString &name);

};

#endif // CANEVS_H
