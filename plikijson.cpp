// Lokalne nagłówki
#include "plikijson.h"

// Qt Pliki i katalogi
#include <QDir>
#include <QFile>

// Qt JSON
#include <QJsonDocument>

// Qt Debugowanie
#include <QDebug>

/**
 * @brief Konstruktor domyślny klasy plikijson.
 */
plikijson::plikijson() {}

/**
 * @brief Zapisuje dane pomiarowe do pliku JSON w odpowiedniej strukturze folderów.
 *
 * @param zapisywanedane Obiekt JSON z danymi do zapisania.
 * @param miasto Nazwa miasta – używana do struktury katalogów i nazwy pliku.
 * @param czujnik Nazwa czujnika – używana do struktury katalogów i nazwy pliku.
 * @param data Data pomiaru – wykorzystywana w nazwie pliku.
 * @return true jeśli zapis się powiódł, false w przeciwnym razie.
 */
bool plikijson::zapisz(const QJsonObject& zapisywanedane, QString& miasto, QString& czujnik, QDateTime& data)
{
    // Tworzenie ścieżek folderów: baza_danych/miasto/czujnik
    QString sciezkabazowa = "baza_danych";
    QString sciezkamiasto = sciezkabazowa + "/" + miasto;
    QString sciezkaczujnik = sciezkamiasto + "/" + czujnik;

    QDir katalog;

    // Tworzenie folderów, jeśli nie istnieją
    if (!katalog.mkpath(sciezkaczujnik)) { // mkpath tworzy pełną ścieżkę katalogów
        qWarning() << "Nie udalo sie utworzyć folderów: " << sciezkaczujnik;
        return false;
    }

    // Tworzenie nazwy pliku: czujnik_miasto_data.json
    QString datatekst = data.toString("yyyy-MM-dd_HH-mm-ss");
    QString nazwapliku = czujnik + "_" + miasto + "_" + datatekst + ".json";
    QString pelnasciezka = sciezkaczujnik + "/" + nazwapliku;

    // Tworzenie i otwarcie pliku do zapisu
    QFile plik(pelnasciezka);
    if (!plik.open(QIODevice::WriteOnly)) { //otwiera plik w trybie write only, jak sie nie da zwróci false czyli jest błąd
        qWarning() << "Nie można otworzyć pliku do zapisu:" << pelnasciezka;
        return false;
    }

    // Tworzenie struktury JSON do zapisu
    QJsonObject obiektdozapisu;
    obiektdozapisu["czujnik"] = czujnik; //dopisana nazwa czujnika
    obiektdozapisu["miasto"] = miasto; //dopisana nazwa miasta do JSON
    obiektdozapisu["values"] = zapisywanedane["values"]; // tylko sekcja "values" z oryginalnego JSON jest przekazywana

    // Zapis do pliku w formacie JSON
    QJsonDocument dokument(obiektdozapisu);
    QByteArray daneJson = dokument.toJson(); // zamiana na domyślny format
    plik.write(daneJson);
    plik.close();

    return true; // Zapis zakończony sukcesem
}
