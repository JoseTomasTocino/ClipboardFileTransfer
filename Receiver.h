#ifndef RECEIVER_H
#define RECEIVER_H

#include <QClipboard>
#include <QFile>
#include <QObject>
#include <QTimer>
#include <QWidget>

#include "State.h"

class Receiver : public QObject
{
    Q_OBJECT
public:
    explicit Receiver(QObject * parent = nullptr);

    void start(QWidget * parent);

signals:
    void status(const QString & message);
    void progress(quint16 current, quint16 total);
    void finishedReceiving();

private slots:
    void clipboardDataChanged();
    void handleState();

    void resendClipboard();

private:
    void setClipboard(const QString & command);

    QWidget * mParentWindow = nullptr;
    QClipboard * mClipboard = nullptr;
    State mState = State::Initial;
    QFile mTheFile;

    QString mReceivedFileName;
    QString mReceivedFileFullPath;
    quint64 mReceivedFileSize;
    quint64 mReceivedFileChunkCount;

    QString mLastSentCommand;
    QTimer mHeartbeat;
};

#endif // RECEIVER_H
