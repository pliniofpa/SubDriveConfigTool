/**
 * @file networkconfigdialog.h
 * This Class defines application Network Configuration dialog window.
 * @brief Header file for NetworkConfigDialog class definition.
 *
 * This dialog window is created through <B>Configuration->Network Config.</B> menu.
 *
 * @author Plinio Andrade &lt;PAndrade@fele.com&gt;
 * @version 1.0.0.0 (Qt: 5.3.1)
 */
#include "mainwindow.h"
#ifndef NETWORKCONFIGDIALOG_H
#define NETWORKCONFIGDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class NetworkConfigDialog;
}

/**
 * @brief The NetworkConfigDialog class defines application Network Configuration dialog window.
 *
 * \image html network-config-dialog.png
 * This dialog window is created through <B>Configuration->Network Config.</B> menu.
 */
class NetworkConfigDialog : public QDialog
{
    friend class MainWindow;
    Q_OBJECT

public:
    /**
     * @brief Constructs this class with the given parent widget.
     *
     * @param parent A pointer to parent widget.
     */
    explicit NetworkConfigDialog(QWidget *parent = 0);
    /**
     * @brief Destructs this class.
     *
     */
    ~NetworkConfigDialog();
private slots:
    /**
     * @brief This member is called always that Address lineEdit text is changed.
     *
     * @param text The new text.
     */
    void addressTextChanged(QString text);
    /**
     * @brief Saves user preferences.
     *
     * Saves user preferences such as window position, window size, user modified fields. This function is called always
     * that this windows is closed using the Ok button. For more information about how it works see the QSettings documentation.
     */
    void saveUserPreferences();
    /**
     * @brief Custom Address checkbox class slot.
     *
     * This slot is connected to Custom Address checkbox and it's called always that Custum Address checkbox state (Checked/Unchecked)
     * is changed.
     *
     * @param checked True if checkbox is checked. False if it's unchecked.
     */
    void checkBoxToggled(bool checked);
private:
    void closeEvent(QCloseEvent * e);

    /**
     * @brief ui object was created by Qt Creator.
     *
     * It contains the graphical elements created using Qt Design.
     */
    Ui::NetworkConfigDialog *ui;
    /**
     * @brief Loads user preferences.
     *
     * Loads user preferences such as window position, window size, user modified fields. This function is called always
     * that this windows is closed using the Ok button. For more information about how it works see the QSettings documentation.
     */
    void loadUerPreferences();
};

#endif // NETWORKCONFIGDIALOG_H
