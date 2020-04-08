#ifndef SENDER_H
#define SENDER_H

#include <QClipboard>
#include <QFile>
#include <QObject>
#include <QWidget>

#include "State.h"

class Sender : public QObject
{
    Q_OBJECT
public:
    explicit Sender(QWidget * parent = nullptr);

    void start(QWidget * parent);

signals:
    void status(const QString & message);
    void progress(quint16 current, quint16 total);
    void finishedSending();

private slots:
    void clipboardDataChanged();

private:
    void handleState();
    void sendCurrentChunk();

    QWidget * mParentWindow = nullptr;
    QClipboard * mClipboard = nullptr;
    State mState = State::Initial;
    QFile mTheFile;

    quint64 mBytesTotal = 0;
    quint64 mBytesSent = 0;
    quint16 mChunkSize = 1024 * 10;
    quint16 mChunkCount = 0;
    quint16 mCurrentChunk = 0;
};

#endif // SENDER_H
