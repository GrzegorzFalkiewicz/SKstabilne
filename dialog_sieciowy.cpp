#include "dialog_sieciowy.h"
#include "ui_dialog_sieciowy.h"
#include <QHostAddress>
#include <QTimer>
#include <QDebug>

dialog_sieciowy::dialog_sieciowy(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_sieciowy)
    , serwer(new QTcpServer(this))
    , klient(new QTcpSocket(this))
    , polaczoneGniazdo(nullptr)
    , migacz(new QTimer(this))
    , reconnector(new QTimer(this))
{
    ui->setupUi(this);
    trybSieciowyWlaczony = false;
    ustawKontrolke("gray");
    ui->groupTryb->hide();
    pokazElementySerwera(false);
    pokazElementyKlienta(false);
    rozlaczonoRecznie = false;
    symulator = nullptr;

    connect(serwer, &QTcpServer::newConnection, this, &dialog_sieciowy::nowyKlient);
    connect(klient, &QTcpSocket::connected, this, &dialog_sieciowy::polaczonoZSerwerem);
    connect(klient, &QTcpSocket::disconnected, this, &dialog_sieciowy::rozlaczonoZSerwerem);
    connect(klient, &QTcpSocket::errorOccurred, this, &dialog_sieciowy::bladPolaczenia);
    connect(migacz, &QTimer::timeout, this, &dialog_sieciowy::resetujKontrolke);

    reconnector->setInterval(3000);
    connect(reconnector, &QTimer::timeout, this, [=]() {
        if (trybSieciowyWlaczony && ui->radioKlient->isChecked() && klient->state() == QAbstractSocket::UnconnectedState) {
            QString ip = ui->lineIP->text();
            quint16 port = ui->linePortKlient->text().toUShort();
            klient->connectToHost(ip, port);
            ustawKontrolke("orange");
            wyswietlKomunikat("Ponawianie połączenia z serwerem...");
        }
    });
}

dialog_sieciowy::~dialog_sieciowy()
{
    delete ui;
}

void dialog_sieciowy::ustawSymulator(Symulator *s)
{
    symulator = s;
}

void dialog_sieciowy::on_btnWlaczSiec_clicked()
{
    if (!trybSieciowyWlaczony) {
        trybSieciowyWlaczony = true;
        ui->btnWlaczSiec->setText("Wyłącz sieć");
        ui->groupTryb->show();
        ustawKontrolke("orange");
        wyswietlKomunikat("Sieć włączona. Wybierz tryb.");
    } else {
        trybSieciowyWlaczony = false;
        ui->btnWlaczSiec->setText("Włącz sieć");
        ui->groupTryb->hide();
        pokazElementySerwera(false);
        pokazElementyKlienta(false);
        ustawKontrolke("gray");
        wyswietlKomunikat("Tryb sieciowy wyłączony.");

        if (serwer->isListening()) serwer->close();
        if (polaczoneGniazdo && polaczoneGniazdo->isOpen()) polaczoneGniazdo->disconnectFromHost();
        if (klient->isOpen()) klient->disconnectFromHost();

        reconnector->stop();
        ui->radioSerwer->setEnabled(true);
        ui->radioKlient->setEnabled(true);
        ui->btnStartSerwer->setEnabled(true);
        ui->btnPolaczKlient->setEnabled(true);
        ui->lineIP->setEnabled(true);
        ui->linePortKlient->setEnabled(true);
        ui->linePortSerwer->setEnabled(true);
    }
}

void dialog_sieciowy::zakonczObaTryby()
{
    if (serwer->isListening()) {
        if (polaczoneGniazdo && polaczoneGniazdo->isOpen()) {
            polaczoneGniazdo->disconnectFromHost();
        }
        serwer->close();
    }

    if (klient->state() == QAbstractSocket::ConnectedState || klient->state() == QAbstractSocket::ConnectingState) {
        klient->abort();
    }

    reconnector->stop();

    ui->btnPolaczKlient->setEnabled(true);
    ui->btnStartSerwer->setEnabled(true);
    ui->lineIP->setEnabled(true);
    ui->linePortKlient->setEnabled(true);
    ui->linePortSerwer->setEnabled(true);

    ustawKontrolke("orange");
    wyswietlKomunikat("Zmieniono tryb. Poprzedni został zakończony.");
}

void dialog_sieciowy::on_radioSerwer_toggled(bool checked)
{
    if (checked) {
        zakonczObaTryby();
        pokazElementySerwera(true);
        pokazElementyKlienta(false);
        wyswietlKomunikat("Wybrano tryb: Serwer");
        ustawKontrolke("orange");
    }
}

void dialog_sieciowy::on_radioKlient_toggled(bool checked)
{
    if (checked) {
        zakonczObaTryby();
        pokazElementySerwera(false);
        pokazElementyKlienta(true);
        wyswietlKomunikat("Wybrano tryb: Klient");
        ustawKontrolke("orange");
    }
}

void dialog_sieciowy::on_btnStartSerwer_clicked()
{
    ui->linePortSerwer->setEnabled(false);
    ui->btnStartSerwer->setEnabled(false);
    ui->radioSerwer->setEnabled(false);
    ui->radioKlient->setEnabled(false);
    quint16 port = ui->linePortSerwer->text().toUShort();

    if (!serwer->listen(QHostAddress::Any, port)) {
        ustawKontrolke("red");
        wyswietlKomunikat("Błąd: nie można uruchomić serwera.");
        return;
    }

    if (!siecUAR) siecUAR = new SiecUAR(this);
    siecUAR->startSerwer(port);

    if (symulator) {
        symulator->setSiec(siecUAR);
        symulator->setTrybSieciowy(true);
    }

    connect(siecUAR, &SiecUAR::odebranoU, this, [=](double u){
        if (symulator) symulator->odbierzU(u);
    });

    connect(siecUAR, &SiecUAR::polaczenieZerwane, this, [=](){
        ui->labelStatus->setText("Utracono połączenie (serwer)");
        if (symulator) symulator->setTrybSieciowy(false);
    });

    ustawKontrolke("green");
    wyswietlKomunikat("Serwer nasłuchuje na porcie " + QString::number(port));
}

void dialog_sieciowy::nowyKlient()
{
    polaczoneGniazdo = serwer->nextPendingConnection();
    QString ip = polaczoneGniazdo->peerAddress().toString();
    if (ip.startsWith("::ffff:")) ip = ip.mid(7);
    wyswietlKomunikat("Klient połączony. Adres: " + ip);
    migajKontrolka("green", "gray");

    connect(polaczoneGniazdo, &QTcpSocket::disconnected, this, &dialog_sieciowy::klientRozlaczyl);
}

void dialog_sieciowy::klientRozlaczyl()
{
    wyswietlKomunikat("Klient został rozłączony.");
    ustawKontrolke("orange");
    QTimer::singleShot(600, this, [=]() { ustawKontrolke("green"); });
}

void dialog_sieciowy::on_btnStopSerwer_clicked()
{
    ui->linePortSerwer->setEnabled(true);
    ui->btnStartSerwer->setEnabled(true);
    ui->radioSerwer->setEnabled(true);
    ui->radioKlient->setEnabled(true);

    if (serwer->isListening()) serwer->close();
    if (polaczoneGniazdo && polaczoneGniazdo->isOpen()) polaczoneGniazdo->disconnectFromHost();

    ustawKontrolke("orange");
    wyswietlKomunikat("Serwer został wyłączony.");
}

void dialog_sieciowy::on_btnPolaczKlient_clicked()
{
    ui->lineIP->setEnabled(false);
    ui->linePortKlient->setEnabled(false);
    ui->btnPolaczKlient->setEnabled(false);
    ui->radioSerwer->setEnabled(false);
    ui->radioKlient->setEnabled(false);

    QString ip = ui->lineIP->text();
    quint16 port = ui->linePortKlient->text().toUShort();

    klient->connectToHost(ip, port);
    ustawKontrolke("orange");
    wyswietlKomunikat("Łączenie z serwerem...");
    reconnector->start();

    if (!siecUAR) siecUAR = new SiecUAR(this);
    siecUAR->polaczZSerwerem(ip, port);

    if (symulator) {
        symulator->setSiec(siecUAR);
        symulator->setTrybSieciowy(true);
    }

    connect(siecUAR, &SiecUAR::odebranoY, this, [=](double y){
        qDebug() << "ODEBRANO Y W KLIENCIE:" << y;
        if (symulator) symulator->set_Y(y);
    });

    connect(siecUAR, &SiecUAR::polaczenieZerwane, this, [=](){
        ui->labelStatus->setText("Utracono połączenie (klient)");
        if (symulator) symulator->setTrybSieciowy(false);
    });
}

void dialog_sieciowy::polaczonoZSerwerem()
{
    reconnector->stop();
    ustawKontrolke("green");
    QString ip = klient->peerAddress().toString();
    if (ip.startsWith("::ffff:")) ip = ip.mid(7);
    wyswietlKomunikat("Połączono z serwerem: " + ip);
}

void dialog_sieciowy::rozlaczonoZSerwerem()
{
    ui->lineIP->setEnabled(true);
    ui->linePortKlient->setEnabled(true);
    ustawKontrolke("red");
    wyswietlKomunikat("Rozłączono przez serwer.");
    QTimer::singleShot(2000, this, [=]() { ustawKontrolke("gray"); });
    if (!rozlaczonoRecznie) reconnector->start();
}

void dialog_sieciowy::on_btnRozlaczKlient_clicked()
{
    ui->lineIP->setEnabled(true);
    ui->linePortKlient->setEnabled(true);
    rozlaczonoRecznie = true;
    reconnector->stop();
    ui->btnPolaczKlient->setEnabled(true);
    ui->radioSerwer->setEnabled(true);
    ui->radioKlient->setEnabled(true);
    klient->disconnectFromHost();
    ustawKontrolke("orange");
    wyswietlKomunikat("Rozłączono klienta.");
}

void dialog_sieciowy::bladPolaczenia(QAbstractSocket::SocketError)
{
    ustawKontrolke("red");
    wyswietlKomunikat("Błąd połączenia z serwerem.");
}

void dialog_sieciowy::migajKontrolka(const QString &kolor1, const QString &kolor2)
{
    Q_UNUSED(kolor2);
    ustawKontrolke(kolor1);
    QTimer::singleShot(150, this, [=]() {
        ustawKontrolke("#006400");
        QTimer::singleShot(150, this, [=]() {
            ustawKontrolke(kolor1);
        });
    });
}

void dialog_sieciowy::resetujKontrolke()
{
    ustawKontrolke(aktualnyKolor);
    migacz->stop();
}

void dialog_sieciowy::ustawKontrolke(const QString &kolor)
{
    aktualnyKolor = kolor;
    ui->kontrolkaStatus->setStyleSheet("background-color: " + kolor);
}

void dialog_sieciowy::wyswietlKomunikat(const QString &tekst)
{
    ui->labelStatus->setText(tekst);
}

void dialog_sieciowy::pokazElementySerwera(bool widoczne)
{
    ui->linePortSerwer->setVisible(widoczne);
    ui->btnStartSerwer->setVisible(widoczne);
    ui->btnStopSerwer->setVisible(widoczne);
}

void dialog_sieciowy::pokazElementyKlienta(bool widoczne)
{
    ui->lineIP->setVisible(widoczne);
    ui->linePortKlient->setVisible(widoczne);
    ui->btnPolaczKlient->setVisible(widoczne);
    ui->btnRozlaczKlient->setVisible(widoczne);
}
