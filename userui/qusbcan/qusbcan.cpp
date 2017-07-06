#include "qusbcan.h"
#include <QVector>
#include <QVariant>
#include <QDebug>

int QUsbCan::channelIndex = 0;

class CanPrivateThread : public QThread
{
public:
    CanPrivateThread(QUsbCan *d, QObject *parent)
        : QThread(parent)
        , dptr(d)
    {
    }

    void run()
    {
        CANMessage message{0};
        QCanBusFrame frame;
        while(true)
        {
            //! Receive message
            ::memset(&message, 0, sizeof(message)); //!< Clear
            if(::usbCanReceive(QUsbCan::getChannelIndex(), &message)){       //!< Has message?
                frame.setFrameId(message.cob_id);
                QByteArray payload{message.len, 0};
                ::memcpy(payload.data(), message.data, message.len);
                frame.setPayload(payload);
                dptr->enqueueReceivedFrame(frame);  //!< Enqueue, emit framesReceived()
            }


            //! Send message
            if(dptr->hasOutgoingFrames()){          //!< Has Frame to send?
                frame = dptr->dequeueOutgoingFrame();
                QByteArray payload = frame.payload();
                ::memset(&message, 0, sizeof(message));

                if (payload.size() > int(sizeof(message.data))) {
                    qWarning("Impossible to write the message with unacceptable data size\
                             %d, ignored", payload.size());
                } else {
                    message.cob_id = frame.frameId();
                    message.len = payload.size();
                    message.extend = frame.hasExtendedFrameFormat();
                    message.rtr = (frame.frameType() == QCanBusFrame::RemoteRequestFrame);
                    ::memcpy(message.data, payload.constData(), message.len);
                    if (!::usbCanSend(QUsbCan::getChannelIndex(), &message))
                        dptr->setError(tr("Send fail"), QCanBusDevice::CanBusError::WriteError);
                }
            }

        }
    }

private:
    QUsbCan *dptr;

};



QUsbCan::QUsbCan(QObject *parent)
    : QCanBusDevice(parent)
    , canThd(Q_NULLPTR)
{

}

bool QUsbCan::open()
{
    qDebug() << channelIndex;
    if (!::usbCanOpen(channelIndex)) {
        close();
        return false;
    }

    //! Apply all stored configurations
    foreach (int key, configurationKeys()) {
        const QVariant param = configurationParameter(key);
        setConfigurationParameter(key, param);
    }

    setState(QCanBusDevice::ConnectedState);

    if(!canThd){
        canThd = new CanPrivateThread(this, this);
    }
    canThd->start();

    return true;
}

void QUsbCan::close()
{
    if(canThd){
        canThd->quit();
        qDebug()<<"can thread quit";
    }

    if (!::usbCanClose(channelIndex))
        setError(QUsbCan::tr("Close fail"), QCanBusDevice::CanBusError::ConnectionError);
    else{
        setState(QCanBusDevice::UnconnectedState);
    }
}

bool QUsbCan::writeFrame(const QCanBusFrame &frame)
{
    if (state() != QCanBusDevice::ConnectedState)
        return false;

    if (!frame.isValid()) {
        setError(tr("Cannot write invalid QCanBusFrame"), QCanBusDevice::WriteError);
        return false;
    }

    if (frame.frameType() != QCanBusFrame::DataFrame
            && frame.frameType() != QCanBusFrame::RemoteRequestFrame
            && frame.frameType() != QCanBusFrame::ErrorFrame) {
        setError(tr("Unable to write a frame with unacceptable type"),
                 QCanBusDevice::WriteError);
        return false;
    }

    if (frame.payload().size() > 8) {
        setError(tr("CanFD frame format not supported."), QCanBusDevice::WriteError);
        return false;
    }

    QMutexLocker locker(&outgoingFramesGuard);
    QCanBusDevice::enqueueOutgoingFrame(frame);

    return true;
}

QString QUsbCan::interpretErrorFrame(const QCanBusFrame &errorFrame)
{
    Q_UNUSED(errorFrame);

    return QString();
}



void QUsbCan::setConfigurationParameter(int key, const QVariant &value)
{
    switch (key) {
    case QCanBusDevice::BitRateKey:
        setBitRate(value.toInt());
    default:
        setError(QUsbCan::tr("Unsupported configuration key"), QCanBusDevice::ConfigurationError);
    }
    QCanBusDevice::setConfigurationParameter(key, value);
}

bool QUsbCan::setBitRate(int bitrate)
{
    if (state() == QCanBusDevice::ConnectedState) {
        if (!::usbCanChangeBaudrate(channelIndex, bitrate)) {
            setError(QUsbCan::tr("Set bitrate fail"), QCanBusDevice::CanBusError::ConfigurationError);
            return false;
        }
    }

    return true;
}

void QUsbCan::setError(const QString &errorText, QCanBusDevice::CanBusError errorId)
{
    QCanBusDevice::setError(errorText, errorId);
}

void QUsbCan::enqueueReceivedFrame(QCanBusFrame &newFrame)
{
    QVector<QCanBusFrame> frames;
    frames.append(newFrame);
    QCanBusDevice::enqueueReceivedFrames(frames);

}

QCanBusFrame QUsbCan::dequeueOutgoingFrame()
{
    QMutexLocker locker(&outgoingFramesGuard);
    return QCanBusDevice::dequeueOutgoingFrame();
}

bool QUsbCan::hasOutgoingFrames()
{
    QMutexLocker locker(&outgoingFramesGuard);
    return QCanBusDevice::hasOutgoingFrames();
}

int QUsbCan::getChannelIndex()
{
    return channelIndex;
}

void QUsbCan::setChannelIndex(int i)
{
    channelIndex = i;
}
