// Lokalne nagłówki
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "oknostacji.h"
#include "oknodanych.h"

// Qt GUI
#include <QMessageBox>     // do komunikatów dla użytkownika
#include <QTableWidget>    // obsługa tabeli w GUI

// Qt File I/O
#include <QFile>           // do zapisu pliku
#include <QTextStream>     // do zapisu tekstu

using namespace std;

/**
 * @brief Konstruktor głównego okna aplikacji.
 * Inicjalizuje interfejs i łączy przyciski z odpowiednimi slotami.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this); // ustawienie interfejsu użytkownika z pliku .ui

    manager = new QNetworkAccessManager(this); // menedżer do wysyłania zapytań HTTP

    // Po otrzymaniu odpowiedzi z API, uruchom slot odebranoDane()
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::odebranoDane);

    // Kliknięcie "Pełna lista stacji" pobiera dane z API
    connect(ui->przyciskpelnalista, &QPushButton::clicked, this, &MainWindow::pobierzStacje);

    // Kliknięcie elementu listy ustawia wybrane ID i otwiera okno stacji
    connect(ui->przycisklistastacji, &QPushButton::clicked, this, &MainWindow::ustawWybraneId);
    connect(ui->przycisklistastacji, &QPushButton::clicked, this, &MainWindow::otworzoknostacji);
}

/**
 * @brief Destruktor głównego okna – zwalnia pamięć interfejsu.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Wysyła zapytanie do API o listę wszystkich stacji w Polsce.
 */
void MainWindow::pobierzStacje()
{
    QString url = "https://api.gios.gov.pl/pjp-api/rest/station/findAll";
    manager->get(QNetworkRequest(QUrl(url))); // wykonanie zapytania GET
}

/**
 * @brief Slot przetwarzający odpowiedź z API GIOŚ.
 * Jeśli nie ma połączenia – proponuje otwarcie danych historycznych.
 */
void MainWindow::odebranoDane(QNetworkReply *reply)
{
    if (reply->error()) {
        // Jeśli wystąpił błąd – zapytaj, czy użytkownik chce użyć danych historycznych
        QMessageBox::StandardButton odp = QMessageBox::question(this, "Błąd", "Nie udało się pobrać danych z API, czy chcesz skorzystać z danych historycznych?", QMessageBox::Yes | QMessageBox::No);

        if (odp == QMessageBox::Yes) {
            oknodanych* okno = new oknodanych("", "", -1); // otwieramy puste okno, użytkownik wczyta dane ręcznie
            okno->setAttribute(Qt::WA_DeleteOnClose); //automatycznie usuwanie okna po kliknieciu X
            okno->ustawoknopoprzednie(this); //ustawiamy okno poprzednie na to w razie potrzeby powrotu przyciskiem wróć
            this->hide(); //chowamy to okno
            okno->show(); //otwieramy nowe okno
        }

        reply->deleteLater(); //zwalniamy pamiec
        return;
    }

    // Odczyt danych JSON z odpowiedzi
    QByteArray odpowiedz = reply->readAll(); //odczytujemy wszystko z reply
    QJsonDocument dokument = QJsonDocument::fromJson(odpowiedz); //zamiana odpowiedzi w formacie json na format QJsonDocument zeby potem na nim pracowac

    // Sprawdzenie czy odpowiedź to tablica JSON
    if (!dokument.isArray()) {
        QMessageBox::warning(this, "Błąd", "Odpowiedź API nie jest listą.");
        reply->deleteLater();
        return;
    }

    QJsonArray stacje = dokument.array();        // pobrana lista stacji zamiana na tablice
    ui->listastacji->clear();                     // czyścimy listę przed dodaniem nowych pozycji

    // Iteracja po stacjach i dodanie ich do listy w interfejsie
    for (int i = 0; i < stacje.size(); ++i) {
        QJsonObject stacja = stacje[i].toObject(); //konwersja z tablicy na obiekt Json
        QString miasto = stacja["city"].toObject()["name"].toString(); //z obiektu stacja wybiramy pole city i następnie pobieramy nazwe
        QString nazwastacji = stacja["stationName"].toString(); //zamieniamy stationname na string
        int idstacji = stacja["id"].toInt(); //z obiektu stacja pobieramy pole id i przypisujemy idstacji

        QString opis = miasto + " – " + nazwastacji; //tworzymy opis aby pozniej go wyswietlic
        ui->listastacji->addItem(opis, idstacji); // dodajemy opis i ID do listy (dane ukryte)
    }

    reply->deleteLater(); // sprzątanie
}

/**
 * @brief Ustawia ID aktualnie wybranej stacji z listy.
 */
void MainWindow::ustawWybraneId()
{
    int index = ui->listastacji->currentIndex();
    wybraneId = ui->listastacji->itemData(index).toInt(); // pobieramy przypisane ID
}

/**
 * @brief Otwiera okno z czujnikami wybranej stacji.
 */
void MainWindow::otworzoknostacji()
{
    QString nazwamiasta = ui->listastacji->currentText(); // pobieramy opis stacji (nazwa miasta)

    if (nazwamiasta.isNull()) {
        QMessageBox::warning(this, "Błąd", "Nie wybrano stacji");
        return;
    }

    // Tworzymy i pokazujemy okno z czujnikami
    oknostacji *okno = new oknostacji(wybraneId, nazwamiasta); //tworzenie nowego okna
    okno->setAttribute(Qt::WA_DeleteOnClose); //usuwa okno jak kliknie sie X
    okno->ustawoknopoprzednie(this); // umożliwia powrót
    this->hide(); //chowa okno aktualne
    okno->show(); //pokazuje nowe okno
}
