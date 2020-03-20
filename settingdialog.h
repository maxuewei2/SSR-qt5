#ifndef SETTING_H
#define SETTING_H
#include <QDialog>
#include <QMainWindow>
#include "config.h"

namespace Ui {
    class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(Config* config, QMainWindow *parent = 0);
    ~SettingDialog();

private:
    Ui::SettingDialog *ui;
    Config* config;

private slots:
    void onAccepted();
    void onChanged();
};
#endif // SETTING_H
