#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_networkconfigdialog.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QScroller>
#include <QNetworkConfigurationManager>
#include <QList>
#include <QNetworkConfiguration>
#include <QItemSelectionModel>
#include <QSettings>
#include <QMessageBox>
#include <QtXml>
#include <QDomDocument>
#include <QDomNodeList>
#include <QDomNode>
#include "networkconfigdialog.h"
#include "aboutdialog.h"
#include "version.h"
#include "build_version.h"
#include "routersnetworkdialog.h"
#include "ui_routersnetworkdialog.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QProgressBar>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->manager = NULL;
    this->reply = 0;
    this->makeconnections();
    this->ui->advanced_frame->setVisible(false);
    //Disables Configuration Group Box and Buttons Frame
    this->ui->configuration_groupBox->setEnabled(false);
    this->ui->buttons_frame->setEnabled(false);
    //Disables Save Button
    this->ui->save_pushButton->setEnabled(false);
    //Default SubDrive Address
    //this->default_address = "http://192.168.240.1";
    this->default_address = "http://localhost/data.php?group=0";
    this->loadUerPreferences();
}

void MainWindow::makeconnections(){
    connect(this->ui->actionAboutApp,SIGNAL(triggered()),this,SLOT(aboutApp()));
    connect(this->ui->actionNetwork_Config,SIGNAL(triggered()),this,SLOT(networkConfigDialogSlot()));
    connect(this->ui->actionExit,SIGNAL(triggered()),this,SLOT(close()));
    connect(&this->requestTimeOut,SIGNAL(timeout()),this,SLOT(abortRequest()));
    connect(this->ui->retrieve_pushButton,SIGNAL(clicked()),this,SLOT(listCurrentConfig()));
    connect(this->ui->save_pushButton,SIGNAL(clicked()),this,SLOT(saveConfig()));
    connect(this->ui->connect_pushButton,SIGNAL(clicked()),this,SLOT(connectSubDrive()));
}


void MainWindow::connectSubDrive(){
//this->ui->connect_pushButton->setEnabled(false);
        listCurrentConfig();
}
void MainWindow::listCurrentConfig(){
    //Disables Configuration Group Box and Buttons Frame
    this->ui->configuration_groupBox->setEnabled(false);
    this->ui->buttons_frame->setEnabled(false);
    //Send Read Configuration Request
    QByteArray xml_request;
    QXmlStreamWriter stream(&xml_request);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("CCL");
    //stream.writeAttribute("href", "http://qt-projekkkkkkkkkkkkkct.org/");
    //stream.writeTextElement("title", "Qt Project");
    stream.writeCharacters("1");
    stream.writeEndElement(); // bookmark
    //stream.writeEndDocument();
    this->request(xml_request);
}

void MainWindow::saveConfig(){
    //Disables Configuration Group Box and Buttons Frame
    this->ui->configuration_groupBox->setEnabled(false);
    this->ui->buttons_frame->setEnabled(false);

    //Send Read Configuration Request
    QByteArray xml_request;
    QXmlStreamWriter stream(&xml_request);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("SSID");
    stream.writeCharacters(this->ui->router_ssid_lineEdit_2->text());
    stream.writeEndElement(); // SSID
    stream.writeStartElement("PWD");
    stream.writeCharacters(this->ui->router_pwd_lineEdit_3->text());
    stream.writeEndElement(); // PWD
    stream.writeStartElement("SWSID");
    stream.writeCharacters(this->ui->subdrive_web_id_lineEdit->text());
    stream.writeEndElement(); // SWSID
    stream.writeStartElement("SCI");
    stream.writeCharacters(QString::number(this->ui->server_comm_interval_spinBox->value()));
    stream.writeEndElement(); // SCI
    stream.writeStartElement("SADD");
    stream.writeCharacters(this->ui->server_address_lineEdit_4->text());
    stream.writeEndElement(); // SADD
    //stream.writeEndDocument();
    this->request(xml_request);
    //Call Function of Clicking Edit Button
    this->on_edit_pushButton_2_clicked();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::request(QByteArray xml_request){
    if(this->reply==0 || (this->reply && this->reply->isFinished())){
        if(this->manager==NULL){
            this->manager = new QNetworkAccessManager(this);
            //this->manager->setConfiguration(this->ui->networks_tableWidget->sele);
        }
        qDebug()<<manager->configuration().name();
        //Deletes Reply and sets its pointer to 0
        if(this->reply){
            delete this->reply;
            this->reply =0;
        }
        QUrl *url;
        if(this->custom_address_enabled){
            url = new QUrl(this->subdrive_custom_address+"/gainspan/profile/mcu");
        }else{
            //url = new QUrl(QString(this->default_address+"/gainspan/profile/mcu"));
            url = new QUrl(QString(this->default_address));
        }
        QNetworkRequest request(*url);
        //Sets Content Type for request
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/xml");
        //Makes de POST Request
        this->reply = manager->post(request,xml_request);
        //Starts TimeOut Timer
        requestTimeOut.start(this->timeout*1000);//Start Timeout Timer
        QObject::connect(reply, SIGNAL(finished()), this, SLOT(parseXML()));
    }else{
        QTimer::singleShot(500,this,SLOT(listCurrentConfig()));
    }
}
void MainWindow::abortRequest(){
    if(this->reply){
        this->reply->abort();
    }
    QMessageBox::critical(0, qApp->tr("Time out!"),
                          qApp->tr("Impossible to establish connection to %1\n"
                                   "Check you connection for errors or try to\n"
                                   "use a timeout greater than %2 seconds").arg(this->manager->configuration().name()).arg(this->timeout), QMessageBox::Ok);
}

void MainWindow::parseXML()
{
    QDomDocument *doc = new QDomDocument();
    QString *error = new QString();
    int *a = new int(-1);
    int *b = new int(-1);
    requestTimeOut.stop();

    if(reply->error()==QNetworkReply::NoError){
        //Enables Configuration Buttons Frame
        this->ui->buttons_frame->setEnabled(true);
    }else{
        QMessageBox::critical(0, qApp->tr("Response Error"),
                              qApp->tr("An error happened retrieving the response from %1.\n"
                                       "Is there a SubDrive running on this netowrk?").arg(this->manager->configuration().name()), QMessageBox::Ok);
        return;//Interrupt this function in case of error
    }
    QByteArray replyByteArray = reply->readAll();

    qDebug()<< Q_FUNC_INFO << replyByteArray;


    if (!doc->setContent(replyByteArray,error,a,b)){
        QMessageBox::critical(0, qApp->tr("Error Parsing XML File"),
                              qApp->tr("An error occurred while parsing XML Response File.\n"
                                       "Try to get the Response again Clicking on 'Refresh' button.\n"
                                       "ERROR: %1").arg(*error), QMessageBox::Ok);

    }else{
        //Fill Configuration Form with current configuration retrievered from SubDrive
        if (doc->elementsByTagName("SSID").length()) {
            this->ui->router_ssid_lineEdit_2->setText(doc->elementsByTagName("SSID").at(0).toElement().text());
        }
        if (doc->elementsByTagName("PWD").length()) {
            this->ui->router_pwd_lineEdit_3->setText(doc->elementsByTagName("PWD").at(0).toElement().text());
        }
        if (doc->elementsByTagName("SWSID").length()) {
            this->ui->subdrive_web_id_lineEdit->setText(doc->elementsByTagName("SWSID").at(0).toElement().text());
        }
        if (doc->elementsByTagName("SCI").length()) {
            this->ui->server_comm_interval_spinBox->setValue(doc->elementsByTagName("SCI").at(0).toElement().text().toInt());
        }
        if (doc->elementsByTagName("SADD").length()) {
            this->ui->server_address_lineEdit_4->setText(doc->elementsByTagName("SADD").at(0).toElement().text());
        }
        //In case of Responsing with Routers Networks
        if(doc->elementsByTagName("NETWORKS").length()){
            QTableWidget *routerNetworkTable = this->routerNetworkDialog->ui->router_networks_tableWidget;
            QDomNodeList networks = doc->elementsByTagName("NETWORKS").at(0).childNodes();
            for(int i=0;i<networks.length();++i){
                routerNetworkTable->setRowCount(routerNetworkTable->rowCount()+1);
                QTableWidgetItem *newItem = new QTableWidgetItem(networks.at(i).toElement().elementsByTagName("SSID").at(0).toElement().text());
                routerNetworkTable->setItem(routerNetworkTable->rowCount()-1,0,newItem);
                newItem = 0;
                newItem =  new QTableWidgetItem(networks.at(i).toElement().elementsByTagName("RSSI").at(0).toElement().text().remove("-"));
                routerNetworkTable->setItem(routerNetworkTable->rowCount()-1,1,newItem);
                //Set Progress Bar
                QProgressBar *progress_bar = new QProgressBar();
                progress_bar->setMaximumHeight(20);
                progress_bar->setMinimum(0);
                progress_bar->setMaximum(100);
                progress_bar->setValue(newItem->text().toInt());
                routerNetworkTable->setCellWidget(routerNetworkTable->rowCount()-1,1,progress_bar);
                newItem = 0;
                newItem = new QTableWidgetItem(networks.at(i).toElement().elementsByTagName("SECURITY").at(0).toElement().text());
                routerNetworkTable->setItem(routerNetworkTable->rowCount()-1,2,newItem);
                newItem = 0;
            }
            routerNetworkTable->resizeColumnsToContents();
            routerNetworkTable->resizeRowsToContents();
            routerNetworkTable->setSortingEnabled(true);
        }
    }
}

void MainWindow::networkConfigDialogSlot(){    
    NetworkConfigDialog dialog(this);
    dialog.ui->customAdress_checkBox->setChecked(this->custom_address_enabled);
    dialog.ui->customAddresslineEdit->setText(this->subdrive_custom_address);
    dialog.ui->timeout_spinBox->setValue(this->timeout);
    if(dialog.exec()){
        this->custom_address_enabled = dialog.ui->customAdress_checkBox->isChecked();
        this->subdrive_custom_address = dialog.ui->customAddresslineEdit->text().remove("/").remove("http:").prepend("http://");
        this->timeout = dialog.ui->timeout_spinBox->value();
    }
}

void MainWindow::on_advanced_pushButton_clicked()
{
    if(!this->ui->advanced_frame->isVisible()){
        this->ui->advanced_frame->setVisible(true);
        this->ui->advanced_pushButton->setText("Basic");
    }else{
        this->ui->advanced_frame->setVisible(false);
        this->ui->advanced_pushButton->setText("Advanced...");
    }
}


void MainWindow::aboutApp(){
    AboutDialog about(this);
    about.setVersionText(QString("<html><head/><body><p><span style='font-size:10pt; font-weight:600;'>Version: </span><span style='font-size:10pt;'>%1</span></p></body></html>").arg(QString(VERSION_ABOUT)+QString(" Build: %1").arg(BUILD_VER)));
#ifdef Q_OS_ANDROID
    about.showMaximized();//Show Maximized for Android Devices
#endif
    about.exec();
}

void MainWindow::saveUserPreferences(){
    QSettings toSave(QSettings::IniFormat,QSettings::UserScope,VER_COMPANYNAME_STR,VER_FILEDESCRIPTION_STR);
    toSave.beginGroup("MainWindow");
    toSave.setValue("size",this->size());
    toSave.setValue("pos",this->pos());
    toSave.endGroup();
    toSave.beginGroup("NetworkConfig");
    toSave.setValue("timeout",this->timeout);
    toSave.setValue("custom_enabled",this->custom_address_enabled);
    toSave.setValue("custom_address",this->subdrive_custom_address);
    toSave.endGroup();
}
void MainWindow::loadUerPreferences(){
    QSettings toLoad(QSettings::IniFormat,QSettings::UserScope,VER_COMPANYNAME_STR,VER_FILEDESCRIPTION_STR);
    toLoad.beginGroup("MainWindow");
    this->resize(toLoad.value("size", QSize(400,250)).toSize());
    this->move(toLoad.value("pos", QPoint(200, 200)).toPoint());
    toLoad.endGroup();
    toLoad.beginGroup("NetworkConfig");
    this->timeout = toLoad.value("timeout", 20).toInt();
    this->custom_address_enabled = toLoad.value("custom_enabled", false).toBool();
    this->subdrive_custom_address = toLoad.value("custom_address", this->default_address).toString();
    toLoad.endGroup();
}
void MainWindow::closeEvent(QCloseEvent *e){
    this->saveUserPreferences();
    e->accept();
}


void MainWindow::on_edit_pushButton_2_clicked()
{
    if(this->ui->edit_pushButton_2->text()=="Edit"){
        //Set Cancel Icon
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/1413781093_Delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        this->ui->edit_pushButton_2->setIcon(icon1);
        //Changes Button Text
        this->ui->edit_pushButton_2->setText("Cancel Edit");
        //Enables Configuration Form fields
        this->ui->configuration_groupBox->setEnabled(true);
        //Disables Retrieve Info Button
        this->ui->retrieve_pushButton->setEnabled(false);
        //Enables Save Button
        this->ui->save_pushButton->setEnabled(true);
    }else{
        //Set Edit Icon
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/pencil3.png"), QSize(), QIcon::Normal, QIcon::Off);
        this->ui->edit_pushButton_2->setIcon(icon1);
        //Changes button Texxt
        this->ui->edit_pushButton_2->setText("Edit");
        this->ui->configuration_groupBox->setEnabled(false);
        //Retrieves Current Config from SubDrive
        this->listCurrentConfig();
        //Enables Retrieve Info Button
        this->ui->retrieve_pushButton->setEnabled(true);
        //Disables Save Button
        this->ui->save_pushButton->setEnabled(false);
    }
}

void MainWindow::on_list_pushButton_4_clicked()
{

    QString temp = this->default_address;
    this->default_address = "http://localhost/data.php?group=7";
    this->routerNetworkDialog = new RoutersNetworkDialog(this);
    //Send Read Configuration Request
    QByteArray xml_request;
    QXmlStreamWriter stream(&xml_request);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("LST_NTW");
    stream.writeCharacters("1");
    stream.writeEndElement(); // LST_NTW
    this->request(xml_request);
    //Shows Model Dialog
    if(routerNetworkDialog->exec()){
        QTableWidget *curTable = this->routerNetworkDialog->ui->router_networks_tableWidget;
        int selected_row = curTable->selectionModel()->currentIndex().row();
        //Set SSID on the Router SSID Field
        this->ui->router_ssid_lineEdit_2->setText(curTable->item(selected_row,0)->text());
        if(curTable->item(selected_row,2)->text()=="NONE"){
            this->ui->router_pwd_lineEdit_3->setText("NONE");
            this->ui->router_pwd_lineEdit_3->setEnabled(false);
        }else{
            this->ui->router_pwd_lineEdit_3->clear();
            this->ui->router_pwd_lineEdit_3->setEnabled(true);
        }
    }
    //Deletes Dialog Object
    delete this->routerNetworkDialog;
    this->default_address = temp;
}

