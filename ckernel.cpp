#include "ckernel.h"
#include<QFileInfo>
#include<QApplication>
#include<QSettings>
Ckernel::Ckernel(QObject *parent) : QObject(parent)
{
    m_maindialog=new MainDialog;
    //绑定信号
    connect(m_maindialog,SIGNAL(SIG_close()),
            this,SLOT(slot_close()));
    m_maindialog->show();
    loadIniFile();
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
