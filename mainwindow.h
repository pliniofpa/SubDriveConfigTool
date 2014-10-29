/**
 * @file mainwindow.h
 * This Class defines application Main Window.
 * @brief Header file for MainWindow class definition.
 *
 * This class contains code related to application Main Window and most of the code related to application operation.
 *
 * @author Plinio Andrade &lt;PAndrade@fele.com&gt;
 * @version 1.0.0.0 (Qt: 5.3.1)
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QScrollerProperties>
#include <QBrush>
#include <QNetworkConfiguration>
class QDomDocument;
class QNetworkAccessManager;
class QNetworkReply;
class QTableWidget;
class QScroller;
class MultDriversConfig;
namespace Ui {
class MainWindow;

}

/**
 * @brief The MainWindow class defines application Main Window.
 *
 * \image html mainwindow-dialog.png
 * This class contains code related to application Main Window and most of the code related to application operation.
 *
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs this class with the given parent widget.
     *
     * @param parent A pointer to parent widget.
     */
    explicit MainWindow(QWidget *parent = 0);
    /**
     * @brief Destructs this class.
     *
     */
    ~MainWindow();
private slots:
    void selectionChanged();
    void networkConfigDialogSlot();
    /**
     * @brief Shows application About dialog.
     *
     * Shows application About dialog showing the dialog created by AboutDialog.
     *
     */
    void aboutApp();


    /**
     * @brief Requests data from network.
     *
     * This slot is called in intervals defined in requestTimer. However, the requestTimer is disconnected in this member
     * and connected again either timeoutTimer timeout() signal is emitted or this request is finished.
     *
     */
    void request(QByteArray xml_request);
    /**
     * @brief Aborts a request.
     *
     * Aborts a request and connects requestTimer to request() slot again. This slot is connected to timeoutTimer timeout() signal.
     */
    void abortRequest();
    /**
     * @brief Parses data received in request() member.
     *
     * Parses data from request() member, call the appropried member according to the group of data received, and connects
     * requestTimer to request() slot again.
     */
    void parseXML();

    void on_advanced_pushButton_clicked();

    /**
     * @brief Produces a list of availiable networks.
     *
     */
    void listNetworks();
    void selectNetwork();

private:
    //MainWindow Ui
    /**
     * @brief ui object was created by Qt Creator.
     *
     * It contains the graphical elements created using Qt Design.
     */
    Ui::MainWindow *ui;

     void closeEvent(QCloseEvent * e);


    /**
     * @brief Saves user preferences.
     *
     * Saves user preferences such as window position, window size, user modified fields. This function is called always
     * that this windows is closed using the Ok button. For more information about how it works see the QSettings documentation.
     */
    void saveUserPreferences();

    /**
     * @brief Loads user preferences.
     *
     * Loads user preferences such as window position, window size, user modified fields. This function is called always
     * that this windows is closed using the Ok button. For more information about how it works see the QSettings documentation.
     */
    void loadUerPreferences();

    /**
     * @brief networks Contains network configurations.
     *
     * The networks configuration comes from QNetworkConfigurationManager::allConfigurations().
     */
    QList<QNetworkConfiguration> networks;

    /**
     * @brief Creates table with found networks.
     *
     */
    void createNetworksTable();


    //About Scroller
    QScrollerProperties scrollprop; /**< Properties for the scoll screen feature. For more information see QScroller documentation */

    //About Network
    QNetworkAccessManager * manager; /**< Pointer to network manager for this application. See QNetworkAccessManager documentation.*/
    QNetworkReply* reply; /**< Pointer to network reply for this application. See QNetworkReply documentation. */

    /**
     * @brief Request timout Timer
     *
     * The timeout() signal of this timer is connected to abortRequest() slot.
     */
    QTimer requestTimeOut;

    //Helper variable for Request Timer
    bool stoptimer; /**< True if stop buttom was pressed and the requestTimer should be stopped. False otherwise. */

    //Internal variables
    /**
     * @brief Creates all connections between signals and slots for this application.
     *
     */
    void makeconnections();
    //Network Configuration
    QString default_address;
    QString subdrive_custom_address;
    int timeout;
    bool custom_address_enabled;

};
#endif // MAINWINDOW_H

