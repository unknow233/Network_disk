#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
signals:
    void SIG_REGIST(QString name,QString tel,QString password);
    void SIG_LOGIN(QString tel,QString password);
private slots:
    void on_pb_clean_regist_clicked();

    void on_pb_commit_regist_clicked();

    void on_pb_clean_login_clicked();

    void on_pb_commit_login_clicked();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
