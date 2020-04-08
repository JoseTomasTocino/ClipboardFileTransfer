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

#ifndef WIDGET_H
#define WIDGET_H

#include <QFile>
#include <QTimer>
#include <QWidget>

#include "Receiver.h"
#include "Sender.h"
#include "State.h"

class QClipboard;

namespace Ui
{
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget * parent = nullptr);
    ~Widget();

public slots:
    void updateProgress(quint16 current, quint16 total);

private slots:
    void on_btnSend_clicked();
    void on_btnReceive_clicked();

    void enableButtons();
    void disableButtons();

private:
    void handleState();
    void sendCurrentChunk();

    Ui::Widget * ui;

    State mState = State::Initial;
    QClipboard * mClipboard = nullptr;

    QList<QByteArray> mReceivedChunks;

    Mode mMode = Mode::None;
    QSharedPointer<Receiver> mReceiver;
    QSharedPointer<Sender> mSender;
};

#endif // WIDGET_H
