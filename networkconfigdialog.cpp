/**
 * @file networkconfigdialog.cpp
 * This Class defines application Network Configuration dialog window.
 * @brief Source file for NetworkConfigDialog class definition.
 *
 * This dialog window is created through <B>Configuration->Network Config.</B> menu.
 *
 * @author Plinio Andrade &lt;PAndrade@fele.com&gt;
 * @version 1.0.0.0 (Qt: 5.3.1)
 */
#include "networkconfigdialog.h"
#include "ui_networkconfigdialog.h"
#include <QPushButton>
#include <QSettings>
#include "version.h"
//#include <QDebug>
NetworkConfigDialog::NetworkConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkConfigDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    this->ui->customAddress_groupBox->setEnabled(false);
    QObject::connect(this->ui->customAdress_checkBox,SIGNAL(toggled(bool)),this,SLOT(checkBoxToggled(bool)));
    QObject::connect(this->ui->customAddresslineEdit, SIGNAL(textChanged(QString)), this, SLOT(addressTextChanged(QString)));
    this->loadUerPreferences();
}

NetworkConfigDialog::~NetworkConfigDialog()
{
    delete ui;
}
void NetworkConfigDialog::addressTextChanged(QString text){
    if(this->ui->customAdress_checkBox->isChecked() && text.isEmpty()){
        this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }else{
        this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void NetworkConfigDialog::saveUserPreferences(){
    QSettings toSave(QSettings::IniFormat,QSettings::UserScope,VER_COMPANYNAME_STR,VER_FILEDESCRIPTION_STR);
    toSave.beginGroup("NetworkConfig");
    toSave.setValue("size",this->size());
    toSave.setValue("pos",this->pos());    
    toSave.endGroup();
}
void NetworkConfigDialog::loadUerPreferences(){
    QSettings toLoad(QSettings::IniFormat,QSettings::UserScope,VER_COMPANYNAME_STR,VER_FILEDESCRIPTION_STR);
    toLoad.beginGroup("NetworkConfig");
    this->resize(toLoad.value("size", QSize(400,250)).toSize());
    this->move(toLoad.value("pos", QPoint(200, 200)).toPoint());
    toLoad.endGroup();
}
void NetworkConfigDialog::checkBoxToggled(bool checked){
    this->ui->customAddress_groupBox->setEnabled(checked);
    if(checked){
        emit this->ui->customAddresslineEdit->textChanged(this->ui->customAddresslineEdit->text());
    }else{
        this->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);

    }
}
void NetworkConfigDialog::closeEvent(QCloseEvent *e){
    this->saveUserPreferences();
    e->accept();
}
