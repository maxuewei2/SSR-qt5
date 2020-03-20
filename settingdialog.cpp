#include "mainwindow.h"
#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <QPushButton>

SettingDialog::SettingDialog(Config* config, QMainWindow *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    this->config=config;
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingDialog::onAccepted);

    if(config->get_subscription_url().length()!=0){
        ui->subscription_url_edit->setText(config->get_subscription_url().data());
        ui->socks_port_edit->setText(config->get_socks_port().data());
        ui->http_port_edit->setText(config->get_http_port().data());
    }

    this->adjustSize();
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::onAccepted()
{
    std::string subscription_url_text=ui->subscription_url_edit->text().toUtf8().data();
    std::string socks_port_text=ui->socks_port_edit->text().toUtf8().data();
    std::string http_port_text=ui->http_port_edit->text().toUtf8().data();
    if((strncmp(subscription_url_text.data(),"https://",8)!=0) && (strncmp(subscription_url_text.data(),"http://",7)!=0)){
         QMessageBox::about(this,"错误", "订阅地址不合法");
         return;
    }
    try {
         std::stoi(socks_port_text);
    } catch (std::invalid_argument&) {
         QMessageBox::about(this,"错误", "Socks端口不合法");
         return;
    }
    try {
         std::stoi(http_port_text);
    } catch (std::invalid_argument&) {
         QMessageBox::about(this,"错误", "Http端口不合法");
         return;
    }
    config->set_subscription_url(subscription_url_text);
    config->set_socks_port(socks_port_text);
    config->set_http_port(http_port_text);
    config->changed_flag=1;

    this->accept();
}

void SettingDialog::onChanged()
{

}
