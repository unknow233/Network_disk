#include "ckernel.h"
#include<QFileInfo>
#include<QApplication>
#include<QSettings>
#include<QMessageBox>
Ckernel::Ckernel(QObject *parent) : QObject(parent)
{
    m_maindialog=new MainDialog;
    //绑定信号
    connect(m_maindialog,SIGNAL(SIG_close()),
            this,SLOT(slot_close()));
    //创建中介者对象
    m_serverMediator=new TcpServerMediator;
    connect(m_serverMediator,SIGNAL(SIG_ReadyData( unsigned int, char*, int)),
            this,SLOT(slot_ReadyData( unsigned int, char*, int)));

    connect(m_serverMediator,SIGNAL(SIG_disConnect()),
              this,SLOT(slot_disConnect()));
    //TODO:缺少定义&实现槽函数
     m_serverMediator->OpenNet();
    m_clientMediator=new TcpClientMediator;
    connect(m_clientMediator,SIGNAL(SIG_disConnect()),
            this,SLOT(slot_disConnect()));
    connect(m_clientMediator,SIGNAL(SIG_ReadyData( unsigned int, char*, int)),
            this,SLOT(slot_ReadyData( unsigned int, char*, int)));
    m_clientMediator->OpenNet("192.168.166.4");
    m_maindialog->show();
    loadIniFile();
    //发送

    char mes[1024]="hello test";
    qDebug()<<mes;
    if(m_clientMediator->IsConnected())
    m_clientMediator->SendData(0,mes,strlen(mes)+1);
    else qDebug()<<"connected fail";
}

Ckernel::~Ckernel()
{

}

void Ckernel::loadIniFile()
{
    //配置文件应该放在exe文件目录下
    //获取exe目录
     QString path=QApplication::applicationDirPath()+"/config.ini";
    QFileInfo inifile(path);
   if(inifile.exists()){
       QSettings setting(path,QSettings::IniFormat);
       //打开组
      setting.beginGroup("net");
      //设置key和value
      m_ip=setting.value("ip","").toString();
      m_port=setting.value("port","").toString();
      //关闭组
      setting.endGroup();
   }else{
        QSettings setting(path,QSettings::IniFormat);
        //打开组
       setting.beginGroup("net");
       //设置key和value
       setting.setValue("ip","192.25.7.0");
       setting.setValue("port","4001");
       //关闭组
       setting.endGroup();
   }
}

void Ckernel::slot_close()
{
    qDebug()<<__func__;
    delete m_maindialog;
    m_maindialog=nullptr;
}

void Ckernel::slot_ReadyData(unsigned int lSendIP, char *buf, int nlen)
{
    qDebug()<<__func__;
    //显示一个消息弹窗

    QString msg=QString("EN:%1").arg(QString::fromStdString(buf));
    QMessageBox::about(nullptr,"提示",msg);
}

void Ckernel::slot_disConnect()
{

}
