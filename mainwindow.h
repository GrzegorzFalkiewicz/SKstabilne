#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qtcharts>
#include "symulator.h"
#include <QTimer>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QTimer>
#include "dialog_arx.h"
#include "dialog_sieciowy.h"
#include "SiecUAR.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void advance();

    void on_ustawA_valueChanged(double arg1);

    void on_ustawP_valueChanged(double arg1);

    void on_ustawT_valueChanged(int arg1);

    void on_ustawKpid_valueChanged(double arg1);

    void on_ustawTi_valueChanged(double arg1);

    void on_ustawTd_valueChanged(double arg1);

    void dodajSerie();

    void resetMaksMin();

    void usunSerie();

    void utworzSerie();

    void utworzOsie();

    void ustawNazwy();

    void ustawZakres();

    void dodacDoSerii();

    void ustawMin();

    void ustawMax();

    void usun_charty();

    void on_Spbox_inter_valueChanged(double arg1);

    //slot do pobierania danych z okienka
    void Pobiezdane_ARX();

    void ZakresWykresu(double &y, QLineSeries * &seria, bool=true);

    void on_checkBox_stateChanged(int arg1);

    void on_Spbox_Stala_valueChanged(double arg1);

    void on_StartStop_clicked();

    void on_Reset_clicked();

    void on_ustaw_S_valueChanged(double arg1);

    void on_Sygnal_currentTextChanged(const QString &arg1);

    void on_chboxCalka_stateChanged(int arg1);

    void on_pidReset_clicked();

    void on_edytujARX_clicked();

    void on_btnPolaczenieSieciowe_clicked();

private:
    Ui::MainWindow *ui;


    // pole z klasą symulatora
    Symulator sym;
    int inter;
    friend Symulator;
    void sig();

    //charty do wykresów 1:Uchyb; 2:Wartości Zadana i Regulowana; 3:Wartości regulatora PID
    QChart* chart1=new QChart();
    QChart* chart2=new QChart();
    QChart* chart3=new QChart();

    //Serie dla wykresów series:Uchyb; series2:Wartość Zadana; series3:Wartość Regulowana; series4:P series5:I; series 6:D; series7:wartość sumaryczna regulatora
    QLineSeries* series = new QLineSeries();
    QLineSeries* series2 = new QLineSeries();
    QLineSeries* series3 = new QLineSeries();
    QLineSeries* series4 = new QLineSeries();
    QLineSeries* series5 = new QLineSeries();
    QLineSeries* series6 = new QLineSeries();
    QLineSeries* series7 = new QLineSeries();

    //stan pracy programu
    bool working=false;


    QTimer* timer=nullptr;
    //wartości maksymalne i minimalnezakresów wykresów
    double maks_y1=-1;
    double maks_y2=-1;
    double maks_y3=-1;
    double min_y1=0;
    double min_y2=0;
    double min_y3=0;
    //Okno dialogowe do regulacji ARX
    Dialog_ARX* edit_ARX;
    //minimalna wartość na osi x na wykresach
    int x=0;

    dialog_sieciowy *oknoSieciowe;
    bool trybSieciowy = false;
};
#endif // MAINWINDOW_H
