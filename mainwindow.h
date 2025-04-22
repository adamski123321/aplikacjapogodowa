#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt GUI
#include <QMainWindow>
#include <QTableWidgetItem>

// Qt Networking
#include <QNetworkAccessManager>
#include <QNetworkReply>

// Qt JSON
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

// Qt File I/O
#include <QFile>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Główne okno aplikacji do monitorowania jakości powietrza.
 *
 * Klasa odpowiada za pobieranie listy stacji pomiarowych z API GIOŚ,
 * wyświetlanie ich na liście, wybór jednej ze stacji oraz przejście
 * do szczegółowego widoku czujników.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor głównego okna aplikacji.
     * @param parent Wskaźnik do obiektu nadrzędnego (domyślnie nullptr).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor klasy MainWindow.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Wysyła zapytanie GET do API GIOŚ w celu pobrania listy stacji pomiarowych.
     */
    void pobierzStacje();

    /**
     * @brief Przetwarza odpowiedź z API GIOŚ i wypełnia listę stacji.
     * @param reply Obiekt zawierający odpowiedź sieciową.
     */
    void odebranoDane(QNetworkReply *reply);

    /**
     * @brief Ustawia identyfikator wybranej stacji na liście.
     */
    void ustawWybraneId();

    /**
     * @brief Otwiera nowe okno z czujnikami dla wybranej stacji pomiarowej.
     */
    void otworzoknostacji();

private:
    Ui::MainWindow *ui; ///< Interfejs użytkownika wygenerowany przez Qt Designer.
    QNetworkAccessManager *manager; ///< Obiekt odpowiedzialny za wykonywanie zapytań HTTP.
    int wybraneId = -1; ///< Przechowuje ID aktualnie wybranej stacji.
};

#endif // MAINWINDOW_H
