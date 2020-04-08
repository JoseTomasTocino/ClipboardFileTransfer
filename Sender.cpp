#include "Sender.h"

#include <QDebug>
#include <QFileDialog>
#include <QGuiApplication>
#include <QMessageBox>
#include <QTimer>
#include <QtMath>

Sender::Sender(QWidget * parent)
    : QObject(parent)
{
    // Attach to clipboard changes
    mClipboard = QGuiApplication::clipboard();
    mClipboard->clear();

    connect(mClipboard, &QClipboard::dataChanged, this, &Sender::clipboardDataChanged);
}

void Sender::start(QWidget * parent)
{
    mParentWindow = parent;

    // Select the file to send
    QString selectedFile = QFileDialog::getOpenFileName(mParentWindow, "Select file to send");

    if (selectedFile.isEmpty())
        return;

    // Open the selected file
    mTheFile.setFileName(selectedFile);
    bool openStatus = mTheFile.open(QFile::ReadOnly);

    if (!openStatus)
    {
        QMessageBox::warning(mParentWindow, "Cannot open file", "Cannot open file: " + selectedFile);
        return;
    }

    // Start waiting for receiver
    mState = State::WaitingForReceiver;

    emit status(QString("Selected file %1, waiting for receiver").arg(selectedFile));

    handleState();
}

void Sender::clipboardDataChanged()
{
    QTimer::singleShot(0, this, &Sender::handleState);
}

void Sender::handleState()
{
    QString data = mClipboard->text();
    emit status(QString("Current state: %1").arg(stateToString(mState)));
    emit status(QString("New clipboard data: %1").arg(data.left(50)));

    if (mState == State::WaitingForReceiver)
    {
        if (data.startsWith("waiting_for_sender"))
        {
            QFileInfo fileInfo(mTheFile);

            mChunkCount = qCeil(mTheFile.size() / mChunkSize);

            emit status("Detected receiver");
            mClipboard->setText(
                QString("begin_send %1 %2 %3").arg(mTheFile.size()).arg(mChunkCount).arg(fileInfo.fileName()));

            mBytesSent = 0;
            mBytesTotal = mTheFile.size();

            emit status(QString("Set clipboard to: begin_send %1 %2").arg(mTheFile.size()).arg(fileInfo.fileName()));

            mState = State::WaitingForSendAccept;
        }
    }

    else if (mState == State::WaitingForSendAccept)
    {
        if (data.startsWith("accept_send"))
        {
            emit status("Receiver accepted, starting to send");
            mCurrentChunk = 0;
            mState = State::Sending;

            sendCurrentChunk();
        }
    }

    else if (mState == State::Sending)
    {
    }

    else if (mState == State::FinishedSending)
    {
        if (data.startsWith("end_receive"))
        {
            emit status("Finished sending data");
            mState = State::Initial;
            mTheFile.close();

            emit finishedSending();
        }
    }

    else if (mState == State::WaitingForAck)
    {
        if (data.startsWith("received_chunk"))
        {
            QStringList dataParts = data.split(" ");
            quint16 chunkNumber = dataParts[1].toUInt();

            if (chunkNumber < mCurrentChunk)
            {
                emit status(
                    QString(
                        "Receiver was left behind, rewinding to chunk number %i ####################################")
                        .arg(chunkNumber + 1));
                mTheFile.seek((chunkNumber + 1) * mChunkSize);
                mCurrentChunk = chunkNumber;
            }

            mCurrentChunk++;
            sendCurrentChunk();
        }
    }
}

void Sender::sendCurrentChunk()
{
    emit status("Sending chunk");

    QByteArray rawData = mTheFile.read(mChunkSize);
    QByteArray chunkData = rawData.toBase64();
    mBytesSent += rawData.size();

    emit status(QString("Raw data size: %1, chunk data size: %2").arg(rawData.size()).arg(chunkData.size()));
    emit progress(mCurrentChunk, mChunkCount);

    QString chunkDataString(chunkData);

    if (chunkData.size() == 0)
    {
        mClipboard->setText("end_send");
        emit status("Finished sending file");

        mState = State::FinishedSending;

        return;
    }

    mState = State::WaitingForAck;
    mClipboard->setText(QString("send_chunk %1 %2").arg(mCurrentChunk).arg(chunkDataString));
    emit status(QString("%1 / %2").arg(mBytesSent).arg(mBytesTotal));
}
