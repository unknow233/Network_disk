#include "ckernel.h"
#include<QFileInfo>
#include<QApplication>
#include<QSettings>
#include<QMessageBox>
#include<windows.h>
#include<packdef.h>
#include <iostream>
#define FunsMap(a) m_funs[a-_DEF_PACK_BASE]

Ckernel::Ckernel(QObject *parent) : QObject(parent)
{
    //m_maindialog=new MainDialog;//放在这里会导致只允许一个用户登录
    //正常情况应该是再登录成功以后再创建,信号的绑定也应该随着变化
    //注释代码需要改动


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
    connect(m_ploginWindow,SIGNAL(SIG_loginClose()),
            this,SLOT(slot_loginClose()));
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
    case login_success:{
        QMessageBox::about(m_ploginWindow,"提示","登录成功");
        m_maindialog=new MainDialog;
        connect(m_maindialog,&MainDialog::SIG_mainClose,
                this,&Ckernel::slot_mainClose);
        connect(m_maindialog,&MainDialog::SIG_UpFile,
                this,&Ckernel::slot_UpFile);
        m_maindialog->setWindowTitle("网盘");
        m_maindialog->setWindowFlags(Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint);
        m_maindialog->show();
}
        break;
    case password_error:
        QMessageBox::about(m_ploginWindow,"提示","密码错误,请重新输入");
        break;
    }
}

void Ckernel::slot_loginClose()
{
    qDebug()<<__func__;
    delete m_ploginWindow;
    m_ploginWindow=nullptr;
}

void Ckernel::slot_mainClose()
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
        qDebug()<<"type is not requested, error type:"<<type;
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
#include<QFile>
#include<QTimeLine>
void Ckernel::slot_UpFile(QString path, QString dir)
{
    qDebug()<<Q_FUNC_INFO;
    QFileInfo qfileinfo(path);
    //设置文件信息
    FileInfo fileinfo;
    fileinfo.size = qfileinfo.size();
    //utf-8转为gb2312
    char gbPath[1000];
    Utf8ToGB2312(gbPath,1000,path);
    fileinfo.pFile = fopen(gbPath,"rb");
    if(fileinfo.pFile==nullptr){
        qDebug()<<"获取文件失败";
    }
    fileinfo.name = qfileinfo.fileName();
    fileinfo.dir =dir;
    //fileinfo.time = ;
    string fileMD5=getFileMD5(path);//这里的路径用原来的即可, 转为gb2312在函数内部实现
    fileinfo.md5 =QString::fromStdString(fileMD5);
    qDebug()<<fileinfo.md5;
    fileinfo.type = "text";
    fileinfo.absolutePath = "/home/user/example.txt";

    //用map保存文件信息key=id+时间戳
    //map_TimeIdToFileinfo[...]=fileinfo;
    //发送请求
    STRU_UPLOAD_FILE_RQ uploadRq;
    uploadRq.userid = 1;
    uploadRq.size = 1024;
    uploadRq.timestamp = 1629876543;
    strcpy(uploadRq.fileName, "example.txt");
    strcpy(uploadRq.dir, "/home/user/uploads");
    strcpy(uploadRq.md5, "d41d8cd98f00b204e9800998ecf8427e");
    strcpy(uploadRq.fileType, "text");
    strcpy(uploadRq.time, "2022-08-23 12:34:56");

}

// QString -> char* gb2312
 void Ckernel::Utf8ToGB2312( char* gbbuf , int nlen ,QString& utf8)
{
    //转码的对象
    QTextCodec * gb2312code = QTextCodec::codecForName( "gb2312");
    //QByteArray char 类型数组的封装类 里面有很多关于转码 和 写IO的操作
    QByteArray ba = gb2312code->fromUnicode( utf8 );// Unicode -> 转码对象的字符集

    strcpy_s ( gbbuf , nlen , ba.data() );
}

// char* gb2312 --> QString utf8
QString Ckernel::GB2312ToUtf8( char* gbbuf )
{
    //转码的对象
    QTextCodec * gb2312code = QTextCodec::codecForName( "gb2312");
    //QByteArray char 类型数组的封装类 里面有很多关于转码 和 写IO的操作
    return gb2312code->toUnicode( gbbuf );// 转码对象的字符集 -> Unicode
}

string Ckernel::getFileMD5(QString path)
{
    //打开文件，读取文件内容，读到md5类，生成md5
    FILE* pFile = nullptr;
    //fopen如果有中文支持ANSI编码使用ascii码
    //path里面是utf8( qt默认的)编码
    char buf[1000] = "";
    Utf8ToGB2312( buf , 1000 , path );
    pFile = fopen( buf , "rb" );//二进制只读
    if( !pFile ){
    qDebug() << "file md5 open fail";
    return string();
}
    int len = 0;
    MD5 md;
    do{
        //缓冲区，一次读多少，读多少次，文件指针返回值读成功次数
        len = fread( buf ,1 , 1000 , pFile );
        md.update(buf,len);

    }while( len > 0 );
    fclose(pFile);
    qDebug()<<QString::fromStdString( md.toString());
}

