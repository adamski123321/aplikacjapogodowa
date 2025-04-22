#ifndef OKNOSTACJI_H
#define OKNOSTACJI_H

// Qt GUI
#include <QWidget>
#include <QListWidget>
#include <QMessageBox>

// Qt Networking
#include <QNetworkAccessManager>
#include <QNetworkReply>

// Qt JSON
#include <QJsonDocument>
#include <QJsonArray>

namespace Ui {
class oknostacji;
}

/**
 * @class oknostacji
 * @brief Okno wyświetlające czujniki pomiarowe dla wybranej stacji.
 *
 * Umożliwia pobranie listy czujników z API GIOŚ oraz otwarcie szczegółowego widoku danych z wybranego czujnika.
 */
class oknostacji : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor okna stacji.
     * @param idStacji Identyfikator wybranej stacji pomiarowej.
     * @param nazwamiasta Nazwa miasta, w którym znajduje się stacja.
     * @param parent Wskaźnik do obiektu nadrzędnego (domyślnie nullptr).
     */
    explicit oknostacji(int idStacji, const QString& nazwamiasta, QWidget *parent = nullptr);

    /**
     * @brief Destruktor klasy oknostacji.
     */
    ~oknostacji();

    /**
     * @brief Pobiera czujniki dostępne w stacji o podanym ID.
     * @param idStacji Identyfikator stacji pomiarowej.
     */
    void pobierzczujniki(int idStacji);

    /**
     * @brief Ustawia poprzednie okno (np. MainWindow), do którego można wrócić.
     * @param okno Wskaźnik do poprzedniego okna.
     */
    void ustawoknopoprzednie(QWidget* okno);

private slots:
    /**
     * @brief Obsługuje odpowiedź z API dotyczącą listy czujników w stacji.
     * @param reply Obiekt zawierający odpowiedź sieciową.
     */
    void odebranoczujniki(QNetworkReply *reply);

    /**
     * @brief Otwiera okno z danymi dla wybranego czujnika.
     */
    void otworzoknodanych();

    /**
     * @brief Powraca do poprzedniego okna po kliknięciu przycisku "Wróć".
     */
    void on_wroc_clicked();

private:
    QWidget* oknopoprzednie = nullptr; ///< Wskaźnik do poprzedniego okna (np. MainWindow).

    Ui::oknostacji *ui; ///< Interfejs użytkownika wygenerowany przez Qt Designer.

    QNetworkAccessManager *manager; ///< Obiekt odpowiedzialny za zapytania sieciowe do API.

    QString nazwamiastaprivate; ///< Nazwa miasta, do którego należy stacja.

    int IDczujnika; ///< ID wybranego czujnika.
};

#endif // OKNOSTACJI_H
