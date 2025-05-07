#include "SiecUAR.h"

SiecUAR::SiecUAR(QObject *parent)
    : QObject(parent)
    , serwer(nullptr)
    , gniazdo(nullptr)
    , trybSerwer(false)
{
    serwer = new QTcpServer(this);
    connect(serwer, &QTcpServer::newConnection, this, &SiecUAR::nowePolaczenie);
}

void SiecUAR::startSerwer(quint16 port)
{
    trybSerwer = true;
    serwer->listen(QHostAddress::Any, port);
}

void SiecUAR::polaczZSerwerem(const QString &adres, quint16 port)
{
    trybSerwer = false;

    if (gniazdo) {
        gniazdo->disconnectFromHost();
        gniazdo->deleteLater();
    }

    gniazdo = new QTcpSocket(this);
    connect(gniazdo, &QTcpSocket::readyRead, this, &SiecUAR::daneNadchodzace);
    connect(gniazdo, &QTcpSocket::connected, this, &SiecUAR::polaczenieNawiazane);
    connect(gniazdo, &QTcpSocket::disconnected, this, &SiecUAR::rozlaczono);
    connect(gniazdo, &QTcpSocket::connected, this, &SiecUAR::obsluzPolaczenie);
    gniazdo->connectToHost(adres, port);
}

void SiecUAR::nowePolaczenie()
{
    if (gniazdo) {
        gniazdo->disconnectFromHost();
        gniazdo->deleteLater();
    }

    gniazdo = serwer->nextPendingConnection();
    connect(gniazdo, &QTcpSocket::readyRead, this, &SiecUAR::daneNadchodzace);
    connect(gniazdo, &QTcpSocket::disconnected, this, &SiecUAR::rozlaczono);

    emit polaczenieNawiazane();
}

void SiecUAR::obsluzPolaczenie()
{
    qDebug() << "[SIEC] Połączenie nawiązane!";
    emit polaczenieNawiazane();  // sygnał o gotowości
}

void SiecUAR::daneNadchodzace()
{
    while (gniazdo && gniazdo->bytesAvailable() >= sizeof(UARData)) {
        UARData pakiet;
        gniazdo->read(reinterpret_cast<char*>(&pakiet), sizeof(UARData));

        if (pakiet.typ == 0) {
            emit odebranoU(pakiet.wartosc);
        } else if (pakiet.typ == 1) {
            emit odebranoY(pakiet.wartosc);
        }
    }
}

void SiecUAR::wyslijU(double u)
{
    qDebug() << "[SIEC] wyslijU: u =" << u;
    if (gniazdo && gniazdo->state() == QAbstractSocket::ConnectedState) {
        UARData pakiet = { u, 0 };
        gniazdo->write(reinterpret_cast<const char*>(&pakiet), sizeof(UARData));
    }
}

void SiecUAR::wyslijY(double y)
{
    if (gniazdo && gniazdo->state() == QAbstractSocket::ConnectedState) {
        UARData pakiet = { y, 1 };
        gniazdo->write(reinterpret_cast<const char*>(&pakiet), sizeof(UARData));
    }
}

void SiecUAR::rozlacz()
{
    if (gniazdo) {
        gniazdo->disconnectFromHost();
    }
    if (serwer->isListening()) {
        serwer->close();
    }
}

bool SiecUAR::polaczono() const {
    return gniazdo && gniazdo->state() == QAbstractSocket::ConnectedState;
}

void SiecUAR::rozlaczono()
{
    emit polaczenieZerwane();
}
