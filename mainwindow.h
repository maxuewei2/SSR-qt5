#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QMessageBox>
#include <string>
#include <fstream>
#include <unistd.h>
#include <signal.h>
#include <QSystemTrayIcon>
#include "json.hpp"
#include "config.h"
//#include "base64.hpp"
using json = nlohmann::json;

#define READ 0
#define WRITE 1

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

pid_t popen2(const char *command, char** argv, int *infp, int *outfp);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:

private slots:
    void update_subscription();
    void server_double_clicked(QListWidgetItem*);
    void quit();
    void about();
    void iconActivated(QSystemTrayIcon::ActivationReason);
    void show_server_info(QListWidgetItem* item);
    void server_action_triggered(QAction* s);
    void show_setting_dialog();

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
    QMenu* server_menu;

    std::string config_filename="ssr_config.json";
    Config* config=new Config();
    json config_json;
    pid_t ssrr_local_pid=-1;
    pid_t sockstohttp_pid=-1;

    QMenu* createMenu();
    void change_ssr(std::string text);
    int request_subscription(std::string cmd);
    int load_config();
    void show_list();
    void start_socks2http();
    int show_setting_dialog_();

};
#endif // MAINWINDOW_H
