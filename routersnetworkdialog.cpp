#include "routersnetworkdialog.h"
#include "ui_routersnetworkdialog.h"

RoutersNetworkDialog::RoutersNetworkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoutersNetworkDialog)
{
    ui->setupUi(this);
    //Removes "What's it?" Button
    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
}

RoutersNetworkDialog::~RoutersNetworkDialog()
{
    delete ui;
}
