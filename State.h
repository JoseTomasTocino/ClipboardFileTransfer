/**
 ** This file is part of the ClipboardFileTransfer project.
 **
 ** Copyright (C) 2020 NAVANTIA Sistemas
 **
 ** This is propietary software. Unauthorized copying of this file
 ** via any medium is strictly prohibited. Propietary and confidential.
 **
 ** Authors:
 **   abr. 2020 - José Tomás Tocino García <elimco.jttocinog@navantia.es>
 **/

#ifndef STATE_H
#define STATE_H

enum class State {
    Initial,
    WaitingForReceiver,
    WaitingForSendAccept,
    WaitingForSender,
    Sending,
    WaitingForAck,
    Receiving,
    FinishedSending,
    FinishedReceiving
};

#endif // STATE_H
