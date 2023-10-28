#include "maindialog.h"
#include "ui_maindialog.h"
#include<QMessageBox>
#include<QCloseEvent>
MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);
}

MainDialog::~MainDialog()
{
    delete ui;
}

void MainDialog::closeEvent(QCloseEvent *event)
{
    //确认关闭窗口
    if(QMessageBox::Yes==QMessageBox::question(this,"窗口","是否确认关闭")){
        event->accept();
        SIG_close();
    }else{
        event->ignore();
    }
}

