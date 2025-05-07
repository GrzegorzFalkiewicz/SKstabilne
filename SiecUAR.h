#ifndef SIECUAR_H
#define SIECUAR_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

#pragma pack(push, 1)
struct UARData {
    double wartosc;  // wartość U lub Y
    quint8 typ;      // 0 = U (od regulatora), 1 = Y (od obiektu)
};
#pragma pack(pop)

class SiecUAR : public QObject
{
    Q_OBJECT
public:
    explicit SiecUAR(QObject *parent = nullptr);

    void startSerwer(quint16 port);
    void polaczZSerwerem(const QString &adres, quint16 port);
    void wyslijU(double u);
    void wyslijY(double y);
    void rozlacz();
    bool polaczono() const;

signals:
    void odebranoU(double u);
    void odebranoY(double y);
    void polaczenieNawiazane();
    void polaczenieZerwane();

private slots:
    void nowePolaczenie();
    void daneNadchodzace();
    void rozlaczono();
    void obsluzPolaczenie();

private:
    QTcpServer *serwer;
    QTcpSocket *gniazdo;
    bool trybSerwer;
};

#endif // SIECUAR_H
