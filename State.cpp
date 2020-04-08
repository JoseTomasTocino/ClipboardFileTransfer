#include "State.h"

QString stateToString(State state)
{
    QMap<State, QString> stateStrings;

    stateStrings[State::Initial] = "Initial";
    stateStrings[State::WaitingForReceiver] = "WaitingForReceiver";
    stateStrings[State::WaitingForSendAccept] = "WaitingForSendAccept";
    stateStrings[State::WaitingForSender] = "WaitingForSender";
    stateStrings[State::Sending] = "Sending";
    stateStrings[State::WaitingForAck] = "WaitingForAck";
    stateStrings[State::Receiving] = "Receiving";
    stateStrings[State::FinishedSending] = "FinishedSending";
    stateStrings[State::FinishedReceiving] = "FinishedReceiving";

    return stateStrings[state];
}
