#include "ckernel.h"
#include<QFileInfo>
#include<QApplication>
#include<QSettings>
#include<QMessageBox>
#include<windows.h>
#include<packdef.h>

#define FunsMap(a) m_funs[a-_DEF_PACK_BASE]

Ckernel::Ckernel(QObject *parent) : QObject(parent)
{
    m_maindialog=new MainDialog;
    //绑定信号
    connect(m_maindialog,SIGNAL(SIG_close()),
            this,SLOT(slot_close()));
    //创建中介者对象
    #ifdef server
    m_serverMediator=new TcpServerMediator;
    connect(m_serverMediator,SIGNAL(SIG_ReadyData( unsigned int, char*, int)),
            this,SLOT(slot_ReadyData( unsigned int, char*, int)));

    connect(m_serverMediator,SIGNAL(SIG_disConnect()),
              this,SLOT(slot_disConnect()));

     m_serverMediator->OpenNet();
    #endif
    m_clientMediator=new TcpClientMediator;
    connect(m_clientMediator,SIGNAL(SIG_disConnect()),
            this,SLOT(slot_disConnect()));
    connect(m_clientMediator,SIGNAL(SIG_ReadyData( unsigned int, char*, int)),
            this,SLOT(slot_ReadyData( unsigned int, char*, int)));
    m_clientMediator->OpenNet("192.168.107.136");
    //m_maindialog->show();
    loadIniFile();
    //测试发送
   // STRU_LOGIN_RQ loginRq;
//    if(m_clientMediator->IsConnected())
//            m_clientMediator->SendData(0,(char*)&loginRq,sizeof (loginRq)+1);

//    else qDebug()<<"connected fail";
    memset(m_funs,0,_DEF_PACK_COUNT*sizeof(FUN));
    m_ploginWindow=new LoginDialog;
    m_ploginWindow->show();
    connect(m_ploginWindow,SIGNAL(SIG_REGIST(QString ,QString ,QString )),
            this,SLOT(slot_registRq(QString ,QString ,QString )));
    connect(m_ploginWindow,SIGNAL(SIG_LOGIN(QString ,QString )),
            this,SLOT(slot_loginRq(QString , QString )));
    initFunMap();
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
       setting.setValue("ip","192.168.166.4");
       setting.setValue("port","4001");
       //关闭组
       setting.endGroup();
   }
}

void Ckernel::initFunMap()
{

    //采用宏定义
    FunsMap(_DEF_PACK_REGISTER_RS)=&Ckernel::DealRegistRs;
    FunsMap(_DEF_PACK_LOGIN_RS)=&Ckernel::DealLoginRs;

}

void Ckernel::SendData(char *buf, int size)
{
    m_clientMediator->SendData(0,buf,size);
}

const string Ckernel::GetMd5(std::string str)
{
    MD5 md5(str);
    return md5.toString();
}

void Ckernel::DealRegistRs(unsigned int lSendIP, char *buf, int nlen)
{
//包大小底层应该处理过了
    STRU_REGISTER_RS registRs=*(STRU_REGISTER_RS*)buf;
    switch(registRs.result){
    case user_is_exist:
        QMessageBox::about(m_ploginWindow,"提示","该手机号已经存在");
        break;
    case register_success:
         QMessageBox::about(m_ploginWindow,"提示","注册成功");
        break;

    }

}

void Ckernel::DealLoginRs(unsigned int lSendIP, char *buf, int nlen)
{
    STRU_LOGIN_RS loginRs=*(STRU_LOGIN_RS*)buf;
    switch (loginRs.result) {
    case user_not_exist:
        QMessageBox::about(m_ploginWindow,"提示","用户不存在");
        break;
    case login_success:
        QMessageBox::about(m_ploginWindow,"提示","登录成功");
        break;
    case password_error:
        QMessageBox::about(m_ploginWindow,"提示","密码错误,请重新输入");
        break;
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
    int type=*(int*)buf;

    int index=type-_DEF_PACK_BASE;
    //判断type的合法性
    if(index>_DEF_PACK_COUNT||index<0){
        qDebug()<<"type is not requested";
    }
    else{ //即使type合法, 对应的协议表中也不一定有
        FUN pfun=FunsMap(type);
        if(pfun){
            (this->*pfun)(lSendIP,buf,nlen);
         }
        else{
            qDebug()<<"type is requested But can't to deal with";
        }
    }


}

void Ckernel::slot_disConnect()
{

}

void Ckernel::slot_registRq(QString name,QString tel,QString password)
{
    qDebug()<<__func__;
    STRU_REGISTER_RQ registerRq;
    strcpy(registerRq.name,name.toStdString().c_str());
    strcpy(registerRq.tel,tel.toStdString().c_str());
    strcpy(registerRq.password,GetMd5(password.toStdString()).c_str());
    string passwordTest=GetMd5(password.toStdString());
    qDebug()<<QString::fromStdString(passwordTest);

    SendData((char*)&registerRq,sizeof(registerRq));
}

void Ckernel::slot_loginRq(QString tel, QString password)
{
    qDebug()<<__func__;
    STRU_LOGIN_RQ loginRq;
    strcpy(loginRq.tel,tel.toStdString().c_str());
    strcpy(loginRq.password,GetMd5(password.toStdString()).c_str());
    string passwordTest=GetMd5(password.toStdString());
    qDebug()<<QString::fromStdString(passwordTest);
    SendData((char*)&loginRq,sizeof(loginRq));
}


