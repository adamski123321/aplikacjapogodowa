// Lokalne nagłówki
#include "oknostacji.h"
#include "ui_oknostacji.h"
#include "oknodanych.h"

// Qt Networking
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

// Qt JSON
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// Qt GUI  debugowanie
#include <QMessageBox>
#include <QDebug>

/**
 * @brief Konstruktor okna stacji.
 * Inicjalizuje interfejs, ustawia nazwę miasta, wysyła zapytanie o czujniki i łączy przyciski.
 */
oknostacji::oknostacji(int idStacji, const QString& nazwamiasta, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::oknostacji)
{
    manager = new QNetworkAccessManager(this);           // Inicjalizacja managera do komunikacji z API
    nazwamiastaprivate = nazwamiasta;                    // Zapamiętanie nazwy miasta
    ui->setupUi(this);                                   // Załadowanie interfejsu graficznego z .ui

    connect(manager, &QNetworkAccessManager::finished, this, &oknostacji::odebranoczujniki); // Po otrzymaniu odpowiedzi wywołaj slot

    // Po kliknięciu w przycisk wybranostanowisko, otwieramy nowe okno z danymi
    connect(ui->wybranostanowisko, &QPushButton::clicked, this, &oknostacji::otworzoknodanych);

    // Ustawienie etykiet w GUI
    ui->tekststacja->setText("Wybrałeś stację: " + nazwamiasta);
    ui->wyswietlnapis->setText("Lista czujników w stacji " + nazwamiasta);

    // Wyśrodkowanie tekstu
    ui->tekststacja->setAlignment(Qt::AlignCenter);
    ui->wyswietlnapis->setAlignment(Qt::AlignCenter);

    pobierzczujniki(idStacji); // Wysłanie zapytania do API o czujniki
}

/**
 * @brief Destruktor – usuwa interfejs użytkownika.
 */
oknostacji::~oknostacji()
{
    delete ui;
}

/**
 * @brief Wysyła zapytanie HTTP GET do API GIOŚ, by pobrać czujniki danej stacji.
 */
void oknostacji::pobierzczujniki(int idStacji)
{
    QString url = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + QString::number(idStacji); // Tworzenie URL
    manager->get(QNetworkRequest(QUrl(url))); // Wysłanie zapytania GET
}

/**
 * @brief Slot przetwarzający odpowiedź z API – lista czujników.
 */
void oknostacji::odebranoczujniki(QNetworkReply *reply)
{
    if (reply->error()) {
        QMessageBox::warning(this, "Błąd", "Nie udało się pobrać danych o czujnikach."); // Komunikat o błędzie
        reply->deleteLater();
        return;
    }

    QByteArray odpowiedz = reply->readAll(); // Odczyt danych z odpowiedzi
    QJsonDocument dokument = QJsonDocument::fromJson(odpowiedz); // Parsowanie JSONa

    if (!dokument.isArray()) {
        QMessageBox::warning(this, "Błąd", "Odpowiedź API nie jest listą."); // Jeśli odpowiedź nie jest tablicą
        reply->deleteLater();
        return;
    }

    QJsonArray czujniki = dokument.array(); // Pobranie tablicy czujników
    ui->listaczujnikow->clear(); // Wyczyść listę przed dodaniem nowych danych

    for (int i = 0; i < czujniki.size(); ++i) {
        QJsonValue wartosc = czujniki[i];
        QJsonObject czujnik = wartosc.toObject();
        QString nazwaparametru = czujnik["param"].toObject()["paramName"].toString(); // Nazwa parametru pomiarowego
        //qDebug() << "Parametr:" << nazwaparametru;
        int id = czujnik["id"].toInt(); // ID czujnika

        ui->listaczujnikow->addItem(nazwaparametru, id); // Dodanie czujnika do listy
    }

    reply->deleteLater(); // Zwolnienie zasobów odpowiedzi
}

/**
 * @brief Otwiera okno z danymi pomiarowymi dla wybranego czujnika.
 */
void oknostacji::otworzoknodanych()
{
    QString nazwaczujnika = ui->listaczujnikow->currentText();         // Nazwa wybranego czujnika z listy
    int IDczujnika = ui->listaczujnikow->currentData().toInt();        // ID wybranego czujnika

    if (IDczujnika <= 0) {
        QMessageBox::warning(this, "Błąd", "Nie wybrano czujnika."); // Komunikat jeśli nic nie zaznaczono
        return;
    }

    //qDebug() << "Otwieram dane dla czujnika ID:" << IDczujnika << " Nazwa:" << nazwaczujnika;

    oknodanych* okno = new oknodanych(nazwamiastaprivate, nazwaczujnika, IDczujnika); // Tworzymy nowe okno
    okno->setAttribute(Qt::WA_DeleteOnClose); // Okno automatycznie się usuwa po zamknięciu
    okno->ustawoknopoprzednie(this);          // Ustawiamy to okno jako poprzednie

    this->hide(); // Ukryj bieżące okno
    okno->show(); // Pokaż nowe
}

/**
 * @brief Ustawia poprzednie okno, do którego można wrócić.
 */
void oknostacji::ustawoknopoprzednie(QWidget* okno)
{
    oknopoprzednie = okno;
}

/**
 * @brief Obsługuje kliknięcie przycisku "Wróć" – zamyka okno i pokazuje poprzednie.
 */
void oknostacji::on_wroc_clicked()
{
    if (oknopoprzednie) {
        oknopoprzednie->show(); // pokaż poprzednie okno (np. MainWindow)
    }
    this->close(); // Zamknij to okno
}
