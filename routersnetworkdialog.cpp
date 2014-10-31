#include "routersnetworkdialog.h"
#include "ui_routersnetworkdialog.h"
#include <QPushButton>
RoutersNetworkDialog::RoutersNetworkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RoutersNetworkDialog)
{
    ui->setupUi(this);
    //Removes "What's it?" Button
    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    //Creates Table Headers
    this->ui->router_networks_tableWidget->setColumnCount(3);
    QStringList headers = QString("SSID,RSSI,SECURITY").split(",");
    this->ui->router_networks_tableWidget->setHorizontalHeaderLabels(headers);
    this->ui->router_networks_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->router_networks_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->router_networks_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    //Disables OK button
    this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    //Connects selectionChanged
    connect(this->ui->router_networks_tableWidget,SIGNAL(itemSelectionChanged()),this,SLOT(selectionChanged()));
}

RoutersNetworkDialog::~RoutersNetworkDialog()
{
    delete ui;
}
void RoutersNetworkDialog::selectionChanged(){
    if(this->ui->router_networks_tableWidget->selectionModel()->hasSelection()){
        //Enables OK button
        this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }else{
        //Disables OK button
        this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}
