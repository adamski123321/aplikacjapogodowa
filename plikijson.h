#ifndef PLIKIJSON_H
#define PLIKIJSON_H

// Qt Core
#include <QString>
#include <QJsonObject>
#include <QDateTime>

/**
 * @class plikijson
 * @brief Klasa odpowiedzialna za zapisywanie danych pomiarowych do lokalnej bazy danych w formacie JSON.
 *
 * Tworzy strukturę folderów wg schematu: baza_danych/miasto/czujnik/,
 * a następnie zapisuje dane do pliku o nazwie zawierającej datę i nazwę czujnika/miasta.
 */
class plikijson
{
public:
    /**
     * @brief Konstruktor klasy plikijson.
     */
    plikijson();

    /**
     * @brief Zapisuje dane pomiarowe do pliku JSON w odpowiedniej strukturze katalogów.
     *
     * @param zapisywanedane Obiekt JSON zawierający dane pomiarowe (zawiera klucz "values").
     * @param miasto Nazwa miasta, z którego pochodzi czujnik.
     * @param czujnik Nazwa czujnika.
     * @param data Data i czas pomiaru – używane do nazwy pliku.
     * @return true jeśli zapis się powiódł, false w przeciwnym przypadku.
     */
    bool zapisz(const QJsonObject& zapisywanedane, QString& miasto, QString& czujnik, QDateTime& data);
};

#endif // PLIKIJSON_H
