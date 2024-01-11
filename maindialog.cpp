#include "maindialog.h"
#include "ui_maindialog.h"
#include<QMessageBox>
#include<QCloseEvent>
#include<QDebug>
MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    //为其设置默认的窗口
    ui->sw_page->setCurrentIndex(0);
    ui->tw_translate->setCurrentIndex(1);
    m_pAddMenu=new QMenu;
    QAction* addFolder=new QAction("新建文件夹");
    QAction* upFile=new QAction("上传文件");
    QAction* upFolder=new QAction("上传文件夹");
    //这里只能传指针, 即使对对象取地址也不行
    //猜测:菜单的显示会访问对应的内存,使用的空间应该在堆区, 且中途不能被释放

    m_pAddMenu->addAction(addFolder);
    m_pAddMenu->addSeparator();//加入分隔符
    m_pAddMenu->addAction(upFile);
    m_pAddMenu->addAction(upFolder);
    //实验:在这里delete addFolder  对应的那一栏就不显示了

    //绑定槽函数
    connect(addFolder,SIGNAL(triggered(bool)),
            this,SLOT(solt_addFolder()));
            connect(upFile,SIGNAL(triggered(bool)),
                    this,SLOT(solt_upFile()));
            connect(upFolder,SIGNAL(triggered(bool)),
                    this,SLOT(solt_upFolder()));
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
        SIG_mainClose();

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


void MainDialog::on_pb_create_clicked()
{
    qDebug()<<__func__;
    m_pAddMenu->exec(QCursor::pos());//鼠标坐标
}

void MainDialog::solt_addFolder()
{
    qDebug()<<__func__;
}

void MainDialog::solt_upFolder()
{
    qDebug()<<__func__;
}

void MainDialog::solt_upFile()
{
qDebug()<<__func__;
}

