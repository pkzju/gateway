#ifndef QUSBCAN_H
#define QUSBCAN_H

#include <QCanBusDevice>
#include <QThread>
#include <QtCore/qmutex.h>
#include <QSharedPointer>

#include "pusbcan.h"



class QUsbCan : public QCanBusDevice
{
private:
    static int channelIndex;
public:
    explicit QUsbCan(QObject *parent = 0);

    void setConfigurationParameter(int key, const QVariant &value) override;

    bool writeFrame(const QCanBusFrame &newData) override;

    QString interpretErrorFrame(const QCanBusFrame &errorFrame) override;

    bool setBitRate(int bitrate);

    void setError(const QString &errorText, QCanBusDevice::CanBusError errorId);
    void enqueueReceivedFrame(QCanBusFrame &newFrame);
    QCanBusFrame dequeueOutgoingFrame();
    bool hasOutgoingFrames();

    static int getChannelIndex();
    static void setChannelIndex(int i);

private:
    bool open() override;
    void close() override;
private:
    QThread *canThd;
    QMutex outgoingFramesGuard;

};



#endif // QUSBCAN_H
