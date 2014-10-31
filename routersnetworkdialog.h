#ifndef ROUTERSNETWORKDIALOG_H
#define ROUTERSNETWORKDIALOG_H

#include <QDialog>
#include "mainwindow.h"
namespace Ui {
class RoutersNetworkDialog;
}

class RoutersNetworkDialog : public QDialog
{
    friend class MainWindow;
    Q_OBJECT

public:
    explicit RoutersNetworkDialog(QWidget *parent = 0);
    ~RoutersNetworkDialog();

private:
    Ui::RoutersNetworkDialog *ui;
private slots:
    void selectionChanged();
};

#endif // ROUTERSNETWORKDIALOG_H
