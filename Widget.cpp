/**
 ** This file is part of the DelfosUI project.
 **
 ** Copyright (C) 2020 NAVANTIA Sistemas
 **
 ** This is propietary software. Unauthorized copying of this file
 ** via any medium is strictly prohibited. Propietary and confidential.
 **
 ** Authors:
 **   abr. 2020 - José Tomás Tocino García <elimco.jttocinog@navantia.es>
 **/

#include "Widget.h"
#include "ui_Widget.h"

#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QThread>

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    mClipboard = QGuiApplication::clipboard();
    connect(mClipboard, &QClipboard::dataChanged, this, &Widget::clipboardDataChanged);

    mClipboard->clear();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::clipboardDataChanged()
{
    ui->txtLog->appendPlainText("Clipboard data changed to:" + mClipboard->text().left(40));

    qDebug() << "Clipboard changed:" << mClipboard->text().left(40);
    QTimer::singleShot(0, this, &Widget::handleState);
}

void Widget::on_btnSend_clicked()
{
    ui->btnSend->setEnabled(false);
    ui->btnReceive->setEnabled(false);

    // Select the file to send
    QString selectedFile = QFileDialog::getOpenFileName(this, "Select file to send");

    if (selectedFile.isEmpty())
        return;

    mTheFile.setFileName(selectedFile);
    bool openStatus = mTheFile.open(QFile::ReadOnly);

    if (!openStatus) {
        QMessageBox::warning(this, "Cannot open file", "Cannot open file: " + selectedFile);
        return;
    }

    mState = State::WaitingForReceiver;

    ui->txtLog->appendPlainText(QString("Selected file %1, waiting for receiver").arg(selectedFile));

    handleState();
}

void Widget::on_btnReceive_clicked()
{
    ui->btnSend->setEnabled(false);
    ui->btnReceive->setEnabled(false);

    mState = State::WaitingForSender;

    ui->txtLog->appendPlainText("Waiting for file...");
    mClipboard->setText("waiting_to_receive");
    qDebug() << "Set clipboard to:"
             << "waiting_to_receive";

    handleState();
}

void Widget::handleState()
{
    QString data = mClipboard->text();

    if (mState == State::WaitingForReceiver) {
        if (data.startsWith("waiting_to_receive")) {
            QFileInfo fileInfo(mTheFile);

            ui->txtLog->appendPlainText("Detected receiver");
            mClipboard->setText(QString("begin_send %1 %2").arg(mTheFile.size()).arg(fileInfo.fileName()));

            mBytesSent = 0;
            mBytesTotal = mTheFile.size();

            qDebug() << QString("Set clipboard to: begin_send %1 %2").arg(mTheFile.size()).arg(fileInfo.fileName());

            mState = State::WaitingForSendAccept;
        }
    }

    else if (mState == State::WaitingForSendAccept) {
        if (data.startsWith("accept_send")) {
            ui->txtLog->appendPlainText("Receiver accepted, starting to send");
            mCurrentChunk = 0;
            mState = State::Sending;

            sendCurrentChunk();
        }
    }

    else if (mState == State::Sending) {

    }

    else if (mState == State::FinishedSending) {
        if (data.startsWith("end_receive")) {
            ui->txtLog->appendPlainText("Receiver finished receiving data");
            mState = State::Initial;
            mTheFile.close();

            ui->btnReceive->setEnabled(true);
            ui->btnSend->setEnabled(true);
        }
    }

    else if (mState == State::WaitingForAck) {
        if (data.startsWith("received_chunk")) {
            mCurrentChunk++;
            sendCurrentChunk();
        }
    }

    else if (mState == State::WaitingForSender) {
        if (data.startsWith("begin_send")) {
            QStringList dataParts = data.split(" ");
            mReceivedFileSize = dataParts[1].toUInt();
            mReceivedFileName = dataParts[2];

            const QString downloadsFolder = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

            mReceivedFileFullPath = QDir::cleanPath(downloadsFolder + QDir::separator() + mReceivedFileName);

            ui->txtLog->appendPlainText(QString("Starting to receive %1 bytes, file name: %2").arg(mReceivedFileSize).arg(mReceivedFileName));
            ui->txtLog->appendPlainText(QString("Storing in %1").arg(mReceivedFileFullPath));

            mTheFile.setFileName(mReceivedFileFullPath);
            mTheFile.open(QFile::WriteOnly);

            mClipboard->setText(QString("accept_send %1 %2").arg(mReceivedFileSize).arg(mReceivedFileName));
            qDebug() << QString("Set clipboard to: accept_send %1 %2").arg(mReceivedFileSize).arg(mReceivedFileName);

            mState = State::Receiving;
        }
    }

    else if (mState == State::Receiving) {
        if (data.startsWith("send_chunk")) {
            ui->txtLog->appendPlainText("Receiving chunk");

            QStringList dataParts = data.split(" ");
            quint16 chunkNumber = dataParts[1].toUInt();

            mTheFile.write(QByteArray::fromBase64(dataParts[2].toUtf8()));

            mClipboard->setText(QString("received_chunk %1").arg(chunkNumber));
            qDebug() << QString("Set clipboard to: received_chunk %1").arg(chunkNumber);
        }

        else if (data.startsWith("end_send")) {
            ui->txtLog->appendPlainText("Finished receiving file");

            mClipboard->setText("end_receive");
            qDebug() << "Set clipboard to: end_receive";

            mTheFile.close();
            mState = State::Initial;
            ui->btnReceive->setEnabled(true);
            ui->btnSend->setEnabled(true);
        }
    }
}

void Widget::sendCurrentChunk()
{
    ui->txtLog->appendPlainText("Sending chunk");

    QByteArray rawData = mTheFile.read(1024 * 10);
    QByteArray chunkData = rawData.toBase64();
    mBytesSent += rawData.size();

    QString chunkDataString(chunkData);

    if (chunkData.size() == 0) {
        mClipboard->setText("end_send");
        qDebug() << "Set clipboard to: end_send";

        mState = State::FinishedSending;

        return;
    }

    mState = State::WaitingForAck;
    mClipboard->setText(QString("send_chunk %1 %2").arg(mCurrentChunk).arg(chunkDataString));
    ui->txtLog->appendPlainText(QString("%1 / %2").arg(mBytesSent).arg(mBytesTotal));
}
