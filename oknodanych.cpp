// Lokalne nagłówki
#include "oknodanych.h"
#include "ui_oknodanych.h"
#include "plikijson.h"

// Qt NetworkinG
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

// Qt JSON
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// Qt GUI
#include <QMessageBox>
#include <QFileDialog>

// Qt Data & Time
#include <QDateTime>

// Qt Pliki i katalogi
#include <QFile>
#include <QDir>

// Qt Charts
#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>

// Qt Aplikacja
#include <QApplication>

using namespace std;

/**
 * @brief Konstruktor okna danych.
 * Inicjalizuje interfejs, ukrywa etykiety oraz wysyła zapytanie o dane dla danego czujnika.
 */
oknodanych::oknodanych(QString nazwamiasta, QString nazwaczujnika, int IDczujnika , QWidget *parent)
    : QWidget(parent),
    IDczujnika(IDczujnika),
    nazwaczujnika(nazwaczujnika),
    nazwamiasta(nazwamiasta),
    ui(new Ui::oknodanych)
{
    ui->setupUi(this);

    // Przygotowanie układu wykresu
    wykreslayout = new QVBoxLayout(ui->wykres);
    ui->wykres->setLayout(wykreslayout);

    // Ukrycie tekstów do czasu załadowania danych
    ui->napis->hide();
    ui->srednia->hide();
    ui->trend->hide();
    ui->minmax->hide();

    // Inicjalizacja managera zapytań i wysłanie żądania do API
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &oknodanych::odebranodane);

    QString url = "https://api.gios.gov.pl/pjp-api/rest/data/getData/" + QString::number(IDczujnika);
    manager->get(QNetworkRequest(QUrl(url)));
}

/**
 * @brief Destruktor klasy – zwalnia zasoby interfejsu.
 */
oknodanych::~oknodanych()
{
    delete ui;
}

/**
 * @brief Slot odbierający dane z API i zapisujący je jako QJsonDocument.
 * @param reply Odpowiedź HTTP z danymi z czujnika.
 */
void oknodanych::odebranodane(QNetworkReply *reply)
{
    //qDebug() << "Funkcja odebranodane() została wywołana";

    if (reply->error()) {
        ui->napis->setText("Błąd pobierania danych.");
        reply->deleteLater();
        return;
    }

    // Parsowanie JSON z odpowiedzi
    QByteArray odpowiedz = reply->readAll();
    QJsonDocument dokument = QJsonDocument::fromJson(odpowiedz);

    if (!dokument.isObject()) {
        QMessageBox::warning(this, "Błąd", "Niepoprawna odpowiedź z API.");
        reply->deleteLater();
        return;
    }

    pobranydokument = dokument;
    QMessageBox::information(this, "Sukces", "Dane zostały pobrane. Kliknij 'Wyświetl dane', aby je zobaczyć.");
    reply->deleteLater();
}

/**
 * @brief Ustawia okno poprzednie, by umożliwić powrót.
 * @param okno Wskaźnik do poprzedniego QWidgeta.
 */
void oknodanych::ustawoknopoprzednie(QWidget* okno)
{
    oknopoprzednie = okno;
}

/**
 * @brief Slot zapisujący pobrane dane do pliku JSON.
 */
void oknodanych::on_zapiszdane_clicked()
{
    try {
        if (pobranydokument.isNull())
            throw runtime_error("Brak danych do zapisania");

        if (datypomiarowprivate.isEmpty())
            throw runtime_error("Brak daty pomiaru — nie można zapisać danych.");

        // Tworzenie daty z pierwszego pomiaru
        QDateTime dataczas = QDateTime::fromString(datypomiarowprivate[0], "yyyy-MM-dd HH:mm:ss");
        if (!dataczas.isValid())
            throw runtime_error("Niepoprawny format daty pomiaru.");

        // Tworzenie obiektu odpowiedzialnego za zapis
        plikijson zapis;
        QJsonObject dane = pobranydokument.object();

        if (!zapis.zapisz(dane, nazwamiasta, nazwaczujnika, dataczas))
            throw runtime_error("Nie udało się zapisać danych.");

        QMessageBox::information(this, "Sukces", "Dane zapisane do pliku JSON");

    } catch (const exception& error) {
        QMessageBox::warning(this, "Błąd", QString("Błąd: ") + error.what());
    } catch (...) {
        QMessageBox::critical(this, "Błąd krytyczny", "Wystąpił nieznany błąd podczas zapisu danych.");
    }
}

/**
 * @brief Powrót do poprzedniego okna po kliknięciu przycisku "Wróć".
 */
void oknodanych::on_wroc_clicked()
{
    if (oknopoprzednie)
        oknopoprzednie->show();

    this->close();
}

/**
 * @brief Przetwarza dokument JSON i wyświetla dane w GUI oraz jako wykres.
 * @param dokument Obiekt JSON zawierający dane pomiarowe.
 */
void oknodanych::wyswietldanezdokumentu(const QJsonDocument& dokument)
{
    if (!dokument.isObject()) {
        QMessageBox::warning(this, "Błąd", "Niepoprawna odpowiedź z API.");
        return;
    }

    QJsonArray dane = dokument.object()["values"].toArray();

    // Pokazanie opisu
    ui->napis->setText("Wyświetlam dane z czujnika: " + nazwaczujnika + "\n" + "z miasta: " + nazwamiasta);
    ui->napis->show();
    ui->srednia->show();
    ui->trend->show();
    ui->minmax->show();

    QVector<double> wartoscipomiarow;
    QVector<QString> datypomiarow;

    // Parsowanie wartości pomiarów
    for (int i = 0; i < dane.size(); ++i) {
        QJsonObject obiekt = dane[i].toObject();
        QString data = obiekt["date"].toString();

        if (!obiekt["value"].isNull()) {
            double wartosc = obiekt["value"].toDouble();
            wartoscipomiarow.append(wartosc);
            datypomiarow.append(data);
        }
    }


    datypomiarowprivate = datypomiarow;

    // Obliczenie średniej
    double suma = std::accumulate(wartoscipomiarow.begin(), wartoscipomiarow.end(), 0.0);
    if (wartoscipomiarow.isEmpty()) {
        ui->srednia->setText("Błędne dane");
    } else {
        double srednia = suma / wartoscipomiarow.size();
        ui->srednia->setText("Średnia wartość pomiarów: " + QString::number(srednia, 'f', 2));
    }

    // Min, max, trend
    double min = wartoscipomiarow[0];
    double max = wartoscipomiarow[0];
    QString datamin = datypomiarow[0];
    QString datamax = datypomiarow[0];
    for (int i = 1; i < wartoscipomiarow.size(); i++) {
        if (wartoscipomiarow[i] < min) {
            min = wartoscipomiarow[i];
            datamin = datypomiarow[i];
        }
        if (wartoscipomiarow[i] > max) {
            max = wartoscipomiarow[i];
            datamax = datypomiarow[i];
        }
    }

    QString trend = "Trend";
    if (wartoscipomiarow.first() > wartoscipomiarow.last())
        trend += " rosnący";
    else if (wartoscipomiarow.first() < wartoscipomiarow.last())
        trend += " malejący";
    else
        trend += " stabilny";

    ui->minmax->setText("Minimalna wartość: " + QString::number(min, 'f', 2) + " Data: (" + datamin + ")" +
                        "\n\nMaksymalna wartość: " + QString::number(max, 'f', 2) + " Data: (" + datamax + ")");
    ui->trend->setText(trend);

    // WYKRES

    QLineSeries *seria = new QLineSeries();
    seria->setPointsVisible(true);
    seria->setMarkerSize(3);

    for (int i = 0; i < wartoscipomiarow.size(); ++i) {
        QDateTime czaspomiaru = QDateTime::fromString(datypomiarow[i], "yyyy-MM-dd HH:mm:ss");
        seria->append(czaspomiaru.toMSecsSinceEpoch(), wartoscipomiarow[i]);
    }

    QChart *wykres = new QChart();
    wykres->addSeries(seria);
    wykres->setTitle("Wartości pomiarów w czasie");
    wykres->legend()->hide();

    // Oś X
    QDateTimeAxis *osX = new QDateTimeAxis;
    osX->setFormat("dd.MM HH:mm");
    osX->setTitleText("Czas");
    wykres->addAxis(osX, Qt::AlignBottom);
    seria->attachAxis(osX);

    QDateTime mindata = QDateTime::fromString(datypomiarow[0], "yyyy-MM-dd HH:mm:ss");
    QDateTime maxdata = mindata;

    for (int i = 1; i < datypomiarow.size(); ++i) {
        QDateTime data = QDateTime::fromString(datypomiarow[i], "yyyy-MM-dd HH:mm:ss");
        if (data < mindata) mindata = data;
        if (data > maxdata) maxdata = data;
    }

    qint64 margines = (maxdata.toMSecsSinceEpoch() - mindata.toMSecsSinceEpoch()) * 0.05;
    osX->setRange(
        QDateTime::fromMSecsSinceEpoch(mindata.toMSecsSinceEpoch() - margines),
        QDateTime::fromMSecsSinceEpoch(maxdata.toMSecsSinceEpoch() + margines)
        );

    // Oś Y
    QValueAxis *osY = new QValueAxis;
    osY->setLabelFormat("%.1f");
    osY->setTitleText("Wartości pomiaru");
    wykres->addAxis(osY, Qt::AlignLeft);
    seria->attachAxis(osY);

    double minY = *std::min_element(wartoscipomiarow.begin(), wartoscipomiarow.end());
    double maxY = *std::max_element(wartoscipomiarow.begin(), wartoscipomiarow.end());
    double marginesY = (maxY - minY) * 0.1;
    osY->setRange(minY - marginesY, maxY + marginesY);

    // Dodanie wykresu do layoutu
    QChartView *widokwykresu = new QChartView(wykres);
    widokwykresu->setRenderHint(QPainter::Antialiasing);

    QLayoutItem* item;
    while ((item = wykreslayout->takeAt(0)) != nullptr) { //usuwanie poprzednich wykresow
        delete item->widget();
        delete item;
    }

    wykreslayout->addWidget(widokwykresu);
}

/**
 * @brief Wczytuje dane z pliku JSON i przekazuje do funkcji wyświetlającej.
 */
void oknodanych::on_wczytajdane_clicked()
{
    QString sciezkapliku = QFileDialog::getOpenFileName(this, "Wybierz plik JSON", QDir::homePath(), "Pliki JSON (*.json)"); //otwiera okno z wybieraniem pliku, domyślnie homepath użytkownika
    if (sciezkapliku.isEmpty()) return; //jesli uzytkownik nic nie kliknął koniec funkcji

    QFile plik(sciezkapliku);
    if (!plik.open(QIODevice::ReadOnly)) { //jak sie nie da otworzyć to jest false
        QMessageBox::warning(this, "Błąd", "Nie można otworzyć pliku");
        return;
    }

    QByteArray dane = plik.readAll(); //Przypisujemy dane z pliku do zmiennej
    plik.close(); //zamykamy plik

    QJsonParseError blad; // obiekt z informacją o ewnetualnym błędzie parsowania
    QJsonDocument dokument = QJsonDocument::fromJson(dane, &blad); //Parsujemy do Json, i przypisujemy informacje do zmiennej błąd

    if (blad.error != QJsonParseError::NoError) { //sprawdzamy czy zmienna błąd ma jakiś error, jeśli nie idziemy dalej
        QMessageBox::warning(this, "Błąd", "Niepoprawny format JSON: " + blad.errorString());
        return;
    }

    if (!dokument.isObject()) {
        QMessageBox::warning(this, "Błąd", "Plik nie zawiera obiektu głównego JSON.");
        return;
    }

    pobranydokument = dokument;
    QJsonObject obiekt = dokument.object();
    nazwaczujnika = obiekt["czujnik"].toString(); //odczytujemy nazweczujnika i miasta do wyświetlenia napisu
    nazwamiasta = obiekt["miasto"].toString();
    wyswietldanezdokumentu(pobranydokument); //wywołujemy funkcję wyswietldane
}

/**
 * @brief Wywołuje analizę i wykres z załadowanych danych po kliknięciu przycisku.
 */
void oknodanych::on_wyswietldane_clicked()
{
    if (pobranydokument.isNull()) {
        QMessageBox::warning(this, "Błąd", "Nie pobrano jeszcze danych.");
        return;
    }

    wyswietldanezdokumentu(pobranydokument);
}
