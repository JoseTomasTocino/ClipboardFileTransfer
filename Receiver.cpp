#include "Receiver.h"

#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QStandardPaths>
#include <QTimer>

Receiver::Receiver(QObject * parent)
    : QObject(parent)
{
    // Attach to clipboard changes
    mClipboard = QGuiApplication::clipboard();
    mClipboard->clear();

    connect(mClipboard, &QClipboard::dataChanged, this, &Receiver::clipboardDataChanged);

    mHeartbeat.setInterval(1000);
    connect(&mHeartbeat, &QTimer::timeout, this, &Receiver::resendClipboard);
}

void Receiver::start(QWidget * parent)
{
    mParentWindow = parent;

    emit status("Waiting for file...");

    setClipboard("waiting_for_sender");
    mState = State::WaitingForSender;

    handleState();
}

void Receiver::clipboardDataChanged()
{
    QTimer::singleShot(0, this, &Receiver::handleState);
}

void Receiver::handleState()
{
    QString data = mClipboard->text();
    emit status(QString("Current state: %1").arg(stateToString(mState)));
    emit status(QString("New clipboard data: %1").arg(data.left(50)));

    if (mState == State::WaitingForSender)
    {
        if (data.startsWith("begin_send"))
        {
            QStringList dataParts = data.split(" ");
            mReceivedFileSize = dataParts[1].toUInt();
            mReceivedFileChunkCount = dataParts[2].toUInt();
            mReceivedFileName = dataParts[3];

            const QString downloadsFolder = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

            mReceivedFileFullPath = QDir::cleanPath(downloadsFolder + QDir::separator() + mReceivedFileName);

            emit status(
                QString("Starting to receive %1 bytes, file name: %2").arg(mReceivedFileSize).arg(mReceivedFileName));
            emit status(QString("Storing in %1").arg(mReceivedFileFullPath));

            mTheFile.setFileName(mReceivedFileFullPath);
            mTheFile.open(QFile::WriteOnly);

            setClipboard(QString("accept_send %1 %2").arg(mReceivedFileSize).arg(mReceivedFileName));
            qDebug() << QString("Set clipboard to: accept_send %1 %2").arg(mReceivedFileSize).arg(mReceivedFileName);

            mState = State::Receiving;
        }
    }

    else if (mState == State::Receiving)
    {
        if (data.startsWith("send_chunk"))
        {
            emit status("Receiving chunk");

            QStringList dataParts = data.split(" ");
            quint16 chunkNumber = dataParts[1].toUInt();

            emit progress(chunkNumber, mReceivedFileChunkCount);

            mTheFile.write(QByteArray::fromBase64(dataParts[2].toUtf8()));

            setClipboard(QString("received_chunk %1").arg(chunkNumber));
            qDebug() << QString("Set clipboard to: received_chunk %1").arg(chunkNumber);
        }

        else if (data.startsWith("end_send"))
        {
            emit status("Finished receiving file");

            setClipboard("end_receive");
            qDebug() << "Set clipboard to: end_receive";

            mTheFile.close();
            mState = State::Initial;
            mHeartbeat.stop();

            emit finishedReceiving();
        }
    }
}

void Receiver::resendClipboard()
{
    emit status(QString("RESENDING COMMAND %1 ##############################################################")
                    .arg(mLastSentCommand));
    mClipboard->setText(mLastSentCommand);
}

void Receiver::setClipboard(const QString & command)
{
    mLastSentCommand = command;
    mClipboard->setText(command);
    mHeartbeat.start();
}
