#ifndef OKNODANYCH_H
#define OKNODANYCH_H

//  Qt GUI
#include <QWidget>
#include <QVBoxLayout>

// Qt Networking
#include <QNetworkAccessManager>
#include <QNetworkReply>

// Qt JSON
#include <QJsonDocument>

namespace Ui {
class oknodanych;
}


/**
 * @class oknodanych
 * @brief Okno odpowiedzialne za pobieranie, wyświetlanie i zapisywanie danych z czujnika pomiarowego.
 *
 * Umożliwia pobieranie danych z API GIOŚ lub wczytywanie ich z lokalnego pliku JSON.
 * Prezentuje dane w formie wykresu oraz wykonuje prostą analizę (średnia, min/max, trend).
 */
class oknodanych : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor okna danych.
     * @param nazwamiasta Nazwa miasta, z którego pochodzi czujnik.
     * @param nazwaczujnika Nazwa czujnika pomiarowego.
     * @param IDczujnika Identyfikator czujnika.
     * @param parent Wskaźnik do obiektu nadrzędnego (domyślnie nullptr).
     */
    explicit oknodanych(QString nazwamiasta, QString nazwaczujnika, int IDczujnika, QWidget *parent = nullptr);

    /**
     * @brief Destruktor klasy oknodanych.
     */
    ~oknodanych();

    /**
     * @brief Ustawia poprzednie okno, do którego można wrócić.
     * @param okno Wskaźnik do poprzedniego okna (np. oknostacji).
     */
    void ustawoknopoprzednie(QWidget* okno);

    /**
     * @brief Wyświetla dane z przekazanego dokumentu JSON (np. z pliku).
     * @param dokument Dokument JSON zawierający dane pomiarowe.
     */
    void wyswietldanezdokumentu(const QJsonDocument& dokument);

private slots:
    /**
     * @brief Obsługuje odpowiedź z API z danymi pomiarowymi.
     * @param reply Obiekt zawierający odpowiedź sieciową.
     */
    void odebranodane(QNetworkReply* reply);

    /**
     * @brief Obsługuje kliknięcie przycisku "Zapisz dane" – zapisuje dane do pliku JSON.
     */
    void on_zapiszdane_clicked();

    /**
     * @brief Obsługuje kliknięcie przycisku "Wróć" – powrót do poprzedniego okna.
     */
    void on_wroc_clicked();

    /**
     * @brief Obsługuje kliknięcie przycisku "Wczytaj dane" – otwiera dane z lokalnego pliku JSON.
     */
    void on_wczytajdane_clicked();

    /**
     * @brief Obsługuje kliknięcie przycisku "Wyświetl dane" – pokazuje wykres i analizę.
     */
    void on_wyswietldane_clicked();

private:
    QWidget* oknopoprzednie = nullptr; ///< Wskaźnik do poprzedniego okna (np. oknostacji).

    QVector<QString> datypomiarowprivate; ///< Przechowuje daty pomiarów z czujnika.

    QString nazwaczujnika; ///< Nazwa aktualnego czujnika.

    QString nazwamiasta; ///< Nazwa miasta, z którego pochodzi czujnik.

    QJsonDocument pobranydokument; ///< Dokument JSON zawierający pobrane lub wczytane dane.

    int IDczujnika = IDczujnika; ///< Identyfikator czujnika pomiarowego.

    Ui::oknodanych *ui; ///< Interfejs użytkownika wygenerowany przez Qt Designer.

    QNetworkAccessManager *manager; ///< Obiekt odpowiedzialny za pobieranie danych z sieci.

    QVBoxLayout *wykreslayout = nullptr; ///< Layout odpowiedzialny za wykres.
};

#endif // OKNODANYCH_H
