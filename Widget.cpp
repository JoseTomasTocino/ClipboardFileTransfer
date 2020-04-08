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

Widget::Widget(QWidget * parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::updateProgress(quint16 current, quint16 total)
{
    ui->progressBar->setMaximum(total);
    ui->progressBar->setValue(current);
}

void Widget::on_btnSend_clicked()
{
    disableButtons();

    // Create sender
    mMode = Mode::Sending;

    // Create sender
    mSender.reset(new Sender);
    connect(mSender.data(), &Sender::status, ui->txtLog, &QPlainTextEdit::appendPlainText);
    connect(mSender.data(), &Sender::finishedSending, this, &Widget::enableButtons);
    connect(mSender.data(), &Sender::progress, this, &Widget::updateProgress);

    // Start sending
    mSender->start(this);
}

void Widget::on_btnReceive_clicked()
{
    disableButtons();

    // Set mode
    mMode = Mode::Receiving;

    // Create receiver
    mReceiver.reset(new Receiver);
    connect(mReceiver.data(), &Receiver::status, ui->txtLog, &QPlainTextEdit::appendPlainText);
    connect(mReceiver.data(), &Receiver::finishedReceiving, this, &Widget::enableButtons);
    connect(mReceiver.data(), &Receiver::progress, this, &Widget::updateProgress);

    // Start receiving
    mReceiver->start(this);
}

void Widget::enableButtons()
{
    ui->btnSend->setEnabled(true);
    ui->btnReceive->setEnabled(true);
}

void Widget::disableButtons()
{
    ui->btnSend->setEnabled(false);
    ui->btnReceive->setEnabled(false);
}
