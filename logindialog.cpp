#include "logindialog.h"
#include "ui_logindialog.h"
#include<QMessageBox>
#include<QRegularExpression>
LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_pb_clean_regist_clicked()
{
    ui->le_name_regist->setText("");
    ui->le_password_check->setText("");
    ui->le_password_regist->setText("");
    ui->le_tel_regist->setText("");
}


void LoginDialog::on_pb_commit_regist_clicked()
{
//检查手机号是否合法
    QString tel=ui->le_tel_regist->text();
    if(tel.isEmpty()){
        QMessageBox::about(this,"提醒","手机号不能为空");
        return;
    }
    QRegularExpression reg("^1[23579][0-9]{9}$");
    QRegularExpressionMatch match=reg.match(tel);
    if(!match.hasMatch()){
        QMessageBox::about(this,"提醒","请输入正确的手机号");
        return;
    }

//检查密码位数
    QString password=ui->le_password_regist->text();
    QString passwordCheck=ui->le_password_check->text();

    if(password.length()<6){
        QMessageBox::about(this,"提醒","密码应该大于等于6位");
        return;
    }
//检查密码是否一致
    if(password!=passwordCheck){
        QMessageBox::about(this,"提醒","两次密码输入不一致");
        return;
    }
QString name=ui->le_name_regist->text();
    //检查name中的敏感词
    //...
    //name长度不允许超过10
if(name.length()>10){
     QMessageBox::about(this,"提醒","name长度不允许超过10");
}
    //信号发送
    emit SIG_REGIST(name,tel,password);
}




void LoginDialog::on_pb_clean_login_clicked()
{
    ui->le_password_login->setText("");
    ui->le_tel_login->setText("");
}


void LoginDialog::on_pb_commit_login_clicked()
{
    //检查手机号是否合法
        QString tel=ui->le_tel_login->text();
        if(tel.isEmpty()){
            QMessageBox::about(this,"提醒","手机号不能为空");
            return;
        }
        QRegularExpression reg("^1[23579][0-9]{9}$");
        QRegularExpressionMatch match=reg.match(tel);
        if(!match.hasMatch()){
            QMessageBox::about(this,"提醒","请输入正确的手机号");
            return;
        }

    //检查密码位数
        QString password=ui->le_password_login->text();
        if(password.length()<6){
            QMessageBox::about(this,"提醒","密码应该大于等于6位");
            return;
        }
        //发送信号
        emit SIG_LOGIN(tel,password);
}

