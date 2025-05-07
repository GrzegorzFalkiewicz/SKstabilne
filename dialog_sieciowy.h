#pragma once

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include "SiecUAR.h"
#include "symulator.h"

namespace Ui {
class dialog_sieciowy;
}

class dialog_sieciowy : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_sieciowy(QWidget *parent = nullptr);
    ~dialog_sieciowy();

    void ustawSymulator(Symulator *s);  // ← DODANE

private slots:
    void on_btnWlaczSiec_clicked();
    void on_radioSerwer_toggled(bool checked);
    void on_radioKlient_toggled(bool checked);
    void on_btnStartSerwer_clicked();
    void on_btnStopSerwer_clicked();
    void on_btnPolaczKlient_clicked();
    void on_btnRozlaczKlient_clicked();
    void resetujKontrolke();
    void nowyKlient();
    void klientRozlaczyl();
    void polaczonoZSerwerem();
    void rozlaczonoZSerwerem();
    void bladPolaczenia(QAbstractSocket::SocketError);

private:
    Ui::dialog_sieciowy *ui;
    QTcpServer *serwer;
    QTcpSocket *klient;
    QTcpSocket *polaczoneGniazdo;
    QTimer *migacz;
    QTimer *reconnector;
    bool trybSieciowyWlaczony;
    bool rozlaczonoRecznie = false;
    QString aktualnyKolor = "gray";
    SiecUAR *siecUAR = nullptr;
    Symulator *symulator = nullptr;  // ← DODANE

    void ustawKontrolke(const QString &kolor);
    void wyswietlKomunikat(const QString &tekst);
    void migajKontrolka(const QString &kolor1, const QString &kolor2);
    void zakonczObaTryby();
    void pokazElementySerwera(bool widoczne);
    void pokazElementyKlienta(bool widoczne);
};
