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

#include "State.h"

class QClipboard;

namespace Ui {
class Widget;
}

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget* parent = nullptr);
    ~Widget();

private slots:
    void clipboardDataChanged();

    void on_btnSend_clicked();

    void on_btnReceive_clicked();

private:
    void handleState();
    void sendCurrentChunk();

    Ui::Widget* ui;

    State mState = State::Initial;
    QClipboard* mClipboard = nullptr;

    QTimer mReceiverHeartbeat;

    QFile mTheFile;

    QString mReceivedFileName;
    QString mReceivedFileFullPath;
    quint64 mReceivedFileSize;

    quint64 mBytesTotal = 0;
    quint64 mBytesSent = 0;

    quint8 mCurrentChunk;
    QList<QByteArray> mReceivedChunks;
};

#endif // WIDGET_H
