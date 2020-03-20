#include "mainwindow.h"
#include "settingdialog.h"
#include "ui_mainwindow.h"
#include "ui_settingdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , trayIcon(new QSystemTrayIcon(this))
{
    QCoreApplication::setApplicationName("QtSSR");
    this->setWindowTitle("QtSSR");

    auto menu = this->createMenu();
    this->trayIcon->setContextMenu(menu);

    // App icon
    auto appIcon = QIcon(":/Resources/icon.png");
    this->trayIcon->setIcon(appIcon);
    this->setWindowIcon(appIcon);

    // Displaying the tray icon
    this->trayIcon->show();     // Note: without explicitly calling show(), QSystemTrayIcon::activated signal will never be emitted!

    // Interaction
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    ui->setupUi(this);

    connect(ui->update_subscription,SIGNAL(triggered()),this,SLOT(update_subscription()));
    connect(ui->ssr_listwidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(server_double_clicked(QListWidgetItem*)));
    connect(ui->ssr_listwidget, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(show_server_info(QListWidgetItem*)));
    connect(ui->quit,SIGNAL(triggered()),this,SLOT(quit()));
    connect(ui->about,SIGNAL(triggered()),this,SLOT(about()));
    connect(ui->setting_action,SIGNAL(triggered()),this,SLOT(show_setting_dialog()));
    while(load_config()==-1){
         config->changed_flag=0;
         if(show_setting_dialog_()!=QDialog::Rejected){
             if(config->changed_flag==1){
                 int code=request_subscription("update_subscription");
                 if(code!=0){
                    QMessageBox::about(NULL,"错误", "订阅失败");
               }
             }
         }else{
             exit(0);
         }
    }
    config->set_subscription_url(config_json["ssr_subscription_url"].get<std::string>());
    config->set_socks_port(config_json["local_port"].get<std::string>());
    config->set_http_port(config_json["local_http_port"].get<std::string>());

    show_list();
    start_socks2http();
}

int MainWindow::show_setting_dialog_(){
    SettingDialog *sDlg = new SettingDialog(config);
    sDlg->show();
    connect(sDlg, &SettingDialog::finished, sDlg, &SettingDialog::deleteLater);
    return sDlg->exec();
    //this->setEnabled(false);
}

void MainWindow::show_setting_dialog(){
    config->changed_flag=0;
    if(show_setting_dialog_()!=QDialog::Rejected){
        if(config->changed_flag==1){
            if(request_subscription("update_subscription")!=0){
                QMessageBox::about(this,"错误", "订阅失败，更改未保存。");
            }
            else{
                QMessageBox::about(this,"成功", "设置在重启程序后生效。");
            }
        }
    }
}

void MainWindow::start_socks2http(){
    std::string socks_local_address=config_json["local_address"].get<std::string>();
    std::string socks_local_port=config_json["local_port"].get<std::string>();
    std::string http_local_port=config_json["local_http_port"].get<std::string>();
    std::string socks_server=socks_local_address+":"+socks_local_port;
    std::string http_server=socks_local_address+":"+http_local_port;
    char** argv=new char*[6];
    argv[0]=strdup("socks2http");
    argv[1]=strdup("-s");
    argv[2]=strdup(socks_server.data());
    argv[3]=strdup("-l");
    argv[4]=strdup(http_server.data());
    argv[5]=0;
    sockstohttp_pid=popen2("./socks2http",argv,NULL,NULL);
    delete [] argv;
}
MainWindow::~MainWindow()
{
    quit();
    delete ui;
}

QMenu* MainWindow::createMenu()
{
  auto menu = new QMenu(this);
  server_menu=menu->addMenu("Server");
  connect(server_menu,SIGNAL(triggered(QAction*)),this,SLOT(server_action_triggered(QAction*)));

  auto quitAction = new QAction("&Quit", this);
  connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
  menu->addAction(quitAction);
  return menu;
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason_)
{
  switch (reason_) {
    case QSystemTrayIcon::Trigger:
      //this->trayIcon->showMessage("Hello", "You clicked me!");
      if(this->isHidden()){
          show();
      }
      else{
          hide();
      }
      break;
    default:
      ;
  }
}

void MainWindow::about(){
    QMessageBox::about(this,"关于", "Qt编写的SSR客户端，调用ssrr-local和socks2http。");
}

int MainWindow::request_subscription(std::string cmd){
    char tmp[300];
    std::string subscription_url=config->get_subscription_url();
    std::string socks_port_=config->get_socks_port();
    std::string http_port_=config->get_http_port();
    sprintf(tmp,"python3 ./update_subscription.py %s \"%s\" %s %s",cmd.data(),subscription_url.data(),socks_port_.data(),http_port_.data());
    return system(tmp);
}

void MainWindow::update_subscription(){
    if(request_subscription("update_subscription")!=0){
        std::string notify_cmd="notify-send \"更新订阅失败\"";
        system(notify_cmd.data());
        return ;
    }
    load_config();
    show_list();

    std::string notify_cmd="notify-send \"更新订阅成功\"";
    system(notify_cmd.data());
}

void MainWindow::show_server_info(QListWidgetItem* item){
    std::string server_name=item->text().toUtf8().data();
    std::string info=config_json["ssr_dict"][server_name].dump(4);
    ui->server_info->setText(info.data());
}

int MainWindow::load_config(){
    if(access(config_filename.data(), F_OK ) != -1 ) {
        std::ifstream i(config_filename);
        json j;
        i >> j;
        config_json=j;
        return 0;
    } else {
        return -1;
    }
}

void MainWindow::show_list()
{
    ui->ssr_listwidget->clear();
    server_menu->clear();
    json ssr_dict=config_json["ssr_dict"];
    for (json::iterator it = ssr_dict.begin(); it != ssr_dict.end(); ++it) {
        const char* server_name=it.key().data();
        new QListWidgetItem(tr(server_name), ui->ssr_listwidget);

        std::string server_address=it.value()["server"].get<std::string>();
        std::string server_port=it.value()["server_port"].get<std::string>();
        char tmp[100];
        sprintf(tmp, "%s (%s:%s)", server_name, server_address.data(), server_port.data());
        auto server_action=server_menu->addAction(tmp);
        server_action->setToolTip(it.key().data());
    }
}

pid_t popen2(const char *command, char** argv, int *infp, int *outfp)
{
    int p_stdin[2], p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
        return -1;

    pid = fork();

    if (pid < 0)
        return pid;
    else if (pid == 0)
    {
        close(p_stdin[WRITE]);
        dup2(p_stdin[READ], READ);
        close(p_stdout[READ]);
        dup2(p_stdout[WRITE], WRITE);

        execvp(command, argv);
        perror("execvp");
        exit(1);
    }

    if (infp == NULL)
        close(p_stdin[WRITE]);
    else
        *infp = p_stdin[WRITE];

    if (outfp == NULL)
        close(p_stdout[READ]);
    else
        *outfp = p_stdout[READ];

    return pid;
}

void MainWindow::change_ssr(std::string server_name){
    if(ssrr_local_pid>0){
        kill(ssrr_local_pid,SIGKILL);
        ssrr_local_pid=-1;
    }
    std::map<std::string,std::string> local_param_dict;
    local_param_dict["-s"]="server";
    local_param_dict["-p"]="server_port";
    local_param_dict["-k"]="password";
    local_param_dict["-m"]="method";
    local_param_dict["-O"]="protocol";
    local_param_dict["-o"]="obfs";
    local_param_dict["-g"]="obfs_param";
    local_param_dict["-b"]="local_address";
    local_param_dict["-l"]="local_port";
    char** argv=new char*[20];
    argv[0]=strdup("ssrr-local");
    int i=1;
    for (std::map<std::string,std::string>::iterator it = local_param_dict.begin(); it != local_param_dict.end(); ++it) {
        const char* arg_name=(it->first).data();
        argv[i]=strdup(arg_name);
        i++;
        const char* arg=(it->second).data();
        std::string s=config_json["ssr_dict"][server_name][arg].get<std::string>();
        if(strcmp(arg_name,"-b")==0){
            s=config_json["local_address"].get<std::string>();
        }
        if(strcmp(arg_name,"-l")==0){
            s=config_json["local_port"].get<std::string>();
        }
        argv[i]=strdup(s.data());
        i++;
    }
    argv[i]=0;

    pid_t pid=popen2("ssrr-local",argv,NULL,NULL);
    delete [] argv;
    ssrr_local_pid=pid;
    std::string notify_cmd="notify-send \"changed to "+server_name+"\"";
    system(notify_cmd.data());
}

void MainWindow::server_double_clicked(QListWidgetItem* item){
    std::string server_name=item->text().toUtf8().data();
    change_ssr(server_name);
}

void MainWindow::server_action_triggered(QAction* s){
    std::string server_name=s->toolTip().toUtf8().data();
    change_ssr(server_name);
}

void MainWindow::quit(){
    if(ssrr_local_pid>0){
        kill(ssrr_local_pid,SIGKILL);
    }
    if(sockstohttp_pid>0){
        kill(sockstohttp_pid,SIGKILL);
    }
    this->close();
}
