#include "maindialog.h"
#include "ui_maindialog.h"
#include<QMessageBox>
#include<QCloseEvent>
MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    //为其设置默认的窗口
    ui->sw_page->setCurrentIndex(0);
    ui->tw_translate->setCurrentIndex(1);
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


void MainDialog::on_pb_file_clicked()
{
    ui->sw_page->setCurrentIndex(0);
}


void MainDialog::on_pb_share_clicked()
{
 ui->sw_page->setCurrentIndex(1);
}


void MainDialog::on_pb_translate_clicked()
{
 ui->sw_page->setCurrentIndex(2);
}


void MainDialog::on_pb_favorite_clicked()
{
 ui->sw_page->setCurrentIndex(3);
}

