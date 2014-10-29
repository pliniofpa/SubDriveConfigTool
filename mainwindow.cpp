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
#include "networkconfigdialog.h"
#include "aboutdialog.h"
#include "version.h"
#include "build_version.h"
#include "multdriversconfig.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->manager = NULL;
    this->reply = 0;
    this->makeconnections();
    this->ui->advanced_frame->setVisible(false);
    //Disables Configuration Group Box
    this->ui->configuration_groupBox->setEnabled(false);
    //Default SubDrive Address
    //this->default_address = "http://192.168.240.1";
    this->default_address = "http://localhost";
    this->loadUerPreferences();
    //Create Network Table
    createNetworksTable();
    //Disables Select button
    this->ui->select_pushButton_3->setEnabled(false);
    //Fills created tables with available networks
    listNetworks();
}
void MainWindow::selectionChanged(){
    if(this->ui->networks_tableWidget->selectedItems().size()!=0){
        this->ui->select_pushButton_3->setEnabled(true);
    }
}

void MainWindow::makeconnections(){
    connect(this->ui->actionAboutApp,SIGNAL(triggered()),this,SLOT(aboutApp()));
    connect(this->ui->actionNetwork_Config,SIGNAL(triggered()),this,SLOT(networkConfigDialogSlot()));
    connect(this->ui->actionExit,SIGNAL(triggered()),this,SLOT(close()));
    connect(this->ui->updateList_pushButton,SIGNAL(clicked()),this,SLOT(listNetworks()));
    connect(this->ui->only_subdrive_checkBox,SIGNAL(clicked()),this,SLOT(listNetworks()));
    connect(this->ui->select_pushButton_3,SIGNAL(clicked()),this,SLOT(selectNetwork()));
    connect(&this->requestTimeOut,SIGNAL(timeout()),this,SLOT(abortRequest()));
    connect(this->ui->networks_tableWidget,SIGNAL(itemSelectionChanged()),this,SLOT(selectionChanged()));
}

void MainWindow::createNetworksTable(){
    QTableWidget *curTable;
    curTable = this->ui->networks_tableWidget;
    //Sets number of columns in the table to 2
    curTable->setColumnCount(2);
    //Creates the headers.
    curTable->setHorizontalHeaderLabels(QStringList(QString(tr("Network Name,#")).split(",")));
    curTable->hideColumn(1);
    //Scroll Configuration
    curTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    QScroller::grabGesture(curTable->viewport(), QScroller::LeftMouseButtonGesture);
    //Resizes Table Columns and Rows
    curTable->resizeColumnsToContents();
    curTable->resizeRowsToContents();
    //Sets Headers Movable
    //curTable->horizontalHeader()->setSectionsMovable(true);
    //Sets selection behaviior and mode
    curTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    curTable->setSelectionMode(QAbstractItemView::SingleSelection);
    curTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    curTable = 0;
}

void MainWindow::listNetworks(){
    //Disable Select Button
    this->ui->select_pushButton_3->setEnabled(false);
    //Changes Update button to disable and "Updating..."
    QString button_text = this->ui->updateList_pushButton->text();
    this->ui->updateList_pushButton->setText("Updating...");
    this->ui->updateList_pushButton->setEnabled(false);
    QNetworkConfigurationManager manager;
    //List available networks
    QList<QNetworkConfiguration> temp = manager.allConfigurations(QNetworkConfiguration::Undefined);
    temp.append(manager.allConfigurations(QNetworkConfiguration::Discovered));
    networks = temp;
    QTableWidget *curTable = this->ui->networks_tableWidget;
    //Clear table
    curTable->clearContents();
    curTable->setRowCount(0);
    //Stop sorting before insert more items
    curTable->setSortingEnabled(false);
    //Inserts avaliable networks in the table
    for(int i=0;i<networks.length();++i){
        if(networks.at(i).bearerType()==QNetworkConfiguration::BearerWLAN){
            if(this->ui->only_subdrive_checkBox->isChecked()){
                QString network_name = networks.at(i).name();
                if(network_name.contains("FECNCT")){
                    //qDebug()<<"Here!!!";
                    //Creates new table row
                    curTable->setRowCount(curTable->rowCount()+1);
                    QTableWidgetItem *curItem = new QTableWidgetItem(network_name);
                    curTable->setItem(i,0,curItem);
                    curItem = new QTableWidgetItem(QString::number(i));
                    curTable->setItem(i,1,curItem);
                }
            }else{
                //Creates new table row
                curTable->setRowCount(curTable->rowCount()+1);
                QTableWidgetItem *curItem = new QTableWidgetItem(networks.at(i).name());
                curTable->setItem(i,0,curItem);
                curItem = new QTableWidgetItem(QString::number(i));
                curTable->setItem(i,1,curItem);
            }

        }
    }
    //Actives sorting before insert more items
    curTable->setSortingEnabled(true);
    //Resizes Table Columns and Rows
    curTable->resizeColumnsToContents();
    curTable->resizeRowsToContents();
    //Changes Update button to disable and sets default text
    this->ui->updateList_pushButton->setText(button_text);
    this->ui->updateList_pushButton->setEnabled(true);
}

void MainWindow::selectNetwork(){
    if(this->ui->select_pushButton_3->text()=="Select"){
        //Disables elements for selecting a Network
        this->ui->networks_tableWidget->setEnabled(false);
        this->ui->only_subdrive_checkBox->setEnabled(false);
        this->ui->updateList_pushButton->setEnabled(false);
        //Send Read Configuration Request
        QByteArray xml_request;
        QXmlStreamWriter stream(&xml_request);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();
        stream.writeStartElement("CCL");
        //stream.writeAttribute("href", "http://qt-project.org/");
        //stream.writeTextElement("title", "Qt Project");
        stream.writeCharacters("1");
        stream.writeEndElement(); // bookmark
        stream.writeEndDocument();

        qDebug()<<xml_request;

        this->request(xml_request);
        this->ui->select_pushButton_3->setText("Change Network");
    }else{
        //Enables elements for selecting a Network
        this->ui->networks_tableWidget->setEnabled(true);
        this->ui->only_subdrive_checkBox->setEnabled(true);
        this->ui->updateList_pushButton->setEnabled(true);
        this->ui->select_pushButton_3->setText("Select");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::request(QByteArray xml_request){
    if(this->manager==NULL){
        this->manager = new QNetworkAccessManager(this);
        //this->manager->setConfiguration(this->ui->networks_tableWidget->sele);
    }
    //Connect to the selected Network
    QItemSelectionModel *select = this->ui->networks_tableWidget->selectionModel();
    int row = select->currentIndex().row();
    QTableWidgetItem *item = this->ui->networks_tableWidget->item(row,1);
    int index = item->text().toInt();
    manager->setConfiguration(networks.at(index));
    qDebug()<<manager->configuration().name();

    if(this->reply){
        delete this->reply;
        this->reply =0;
    }
    QUrl *url;
    if(this->custom_address_enabled){
        url = new QUrl(this->subdrive_custom_address+"/gainspan/profile/mcu");
    }else{
        url = new QUrl(QString(this->default_address+"/gainspan/profile/mcu"));
    }
    QNetworkRequest request(*url);
    //Sets Content Type for request
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/xml");
    //Makes de POST Request
    this->reply = manager->post(request,xml_request);
    //Starts TimeOut Timer
    requestTimeOut.start(this->timeout*1000);//Start Timeout Timer
    QObject::connect(reply, SIGNAL(finished()), this, SLOT(parseXML()));
}
void MainWindow::abortRequest(){
    if(this->reply){
        this->reply->abort();
    }
    qDebug()<<"Aborted";
    QMessageBox::critical(0, qApp->tr("Time out!"),
                          qApp->tr("Impossible to establish connection to %1\n"
                                   "Check you connection for errors or try to\n"
                                   "use a timeout greater than %2 seconds").arg(this->manager->configuration().name()).arg(this->timeout), QMessageBox::Cancel);
}

void MainWindow::parseXML()
{
    //Enables Configuration Group Box
    this->ui->configuration_groupBox->setEnabled(true);
    requestTimeOut.stop();
    if(reply->error()==QNetworkReply::NoError){

    }else{
        return;//Interrupt this function in case of error
    }
    QDomDocument* doc = new QDomDocument();
    QString *error = new QString();
    int *a = new int(-1);
    int *b = new int(-1);
    QByteArray replyByteArray = reply->readAll();
    if (!doc->setContent(replyByteArray,error,a,b)) {

    }else{

        if (doc->elementsByTagName("GRUP").at(0).toElement().text().toInt()) {

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
        this->ui->advanced_pushButton->setText("Advanced");
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
    toSave.setValue("only_subdrive_networks",this->ui->only_subdrive_checkBox->isChecked());
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
    this->ui->only_subdrive_checkBox->setChecked(toLoad.value("only_subdrive_networks", false).toBool());
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
    this->saveGeometry();
}