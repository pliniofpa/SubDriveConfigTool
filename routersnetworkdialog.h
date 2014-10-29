#ifndef ROUTERSNETWORKDIALOG_H
#define ROUTERSNETWORKDIALOG_H

#include <QDialog>

namespace Ui {
class RoutersNetworkDialog;
}

class RoutersNetworkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoutersNetworkDialog(QWidget *parent = 0);
    ~RoutersNetworkDialog();

private:
    Ui::RoutersNetworkDialog *ui;
};

#endif // ROUTERSNETWORKDIALOG_H
