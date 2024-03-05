#include "ckernel.h"
#include<netapi/mediator/INetMediator.h>
#include<QFileInfo>
#include<QApplication>
#include<QSettings>
#include<QMessageBox>
#include<windows.h>
#include<packdef.h>
#include <iostream>
#include<string.h>
#include<QThread>
#include<QCoreApplication>
#include<QDir>
#define FunsMap(a) m_funs[a-_DEF_PACK_BASE]
//
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
    m_clientMediator->OpenNet(_DEF_SERVER_IP);
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
    setSystemPath();
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
    FunsMap(_DEF_PACK_UPLOAD_FILE_RS)=&Ckernel::DealUpFileRs;

    FunsMap(_DEF_PACK_FILE_CONTENT_RS)=&Ckernel::slot_dealFileContentRs;
    FunsMap(_DEF_PACK_GET_FILE_INFO_RS)=&Ckernel::slot_dealGetFileInfoRs;
    FunsMap(_DEF_PACK_FILE_HEADER_RQ)=&Ckernel::slot_dealFileHeaderRq;
    FunsMap(_DEF_PACK_FILE_CONTENT_RQ)=&Ckernel::slot_dealFileContentRq;

    FunsMap(_DEF_PACK_ADD_FOLDER_RS)=&Ckernel::slot_dealAddFolderRs;

     FunsMap(_DEF_PACK_QUICK_UPLOAD_RS)=&Ckernel::slot_dealQuickUploadRs;
     FunsMap(_DEF_PACK_SHARE_FILE_RS)=&Ckernel::slot_dealShareFileRs;
     FunsMap(_DEF_PACK_MY_SHARE_RS)=&Ckernel::slot_dealMyShareRs;
     FunsMap(_DEF_PACK_GET_SHARE_RS)=&Ckernel::slot_dealGetShareRs;
     FunsMap(_DEF_PACK_FOLDER_HEADER_RQ)=&Ckernel::slot_dealFolderHeaderRq;
     FunsMap(_DEF_PACK_DELETE_FILE_RS)=&Ckernel::slot_dealDeleteFileRs;
}

void Ckernel::SendData(char *buf, int size)
{
    //TODO:base64编码
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
        connect(this,SIGNAL(SIG_updateUploadFileProgress(int,int)),
                m_maindialog,SLOT(slot_updateUploadFileProgress(int,int)));
        connect(m_maindialog,SIGNAL(SIG_changeDir(QString)),this,SLOT(slot_changeDir(QString)));
        connect(m_maindialog,SIGNAL(SIG_downloadFile(int,QString)),
               this,SLOT(slot_downloadFile(int,QString)));
        connect(m_maindialog,SIGNAL(SIG_downloadFolder(int,QString)),
               this,SLOT(slot_downloadFolder(int,QString)));
        connect(this,SIGNAL(SIG_updateDownLoadFileProgress(int,int)),m_maindialog,SLOT(slot_updateDownloadFileProgress(int,int)));
        connect(m_maindialog,SIGNAL(SIG_addFolder(QString,QString)),this,SLOT(slot_addFolder(QString,QString)));
        connect(m_maindialog,SIGNAL(SIG_changeDir(QString)),this,SLOT(slot_changeDir(QString)));
        connect(m_maindialog,SIGNAL(SIG_uploadFolder(QString,QString)),this,SLOT(slot_uploadFolder(QString,QString)));
        connect(m_maindialog,SIGNAL(SIG_getShareByLink(int,QString)),this,SLOT(slot_getShareByLink(int,QString)));
        connect(m_maindialog,SIGNAL(SIG_deleteFile(QVector<int>,QString)),this,SLOT(slot_deleteFile(QVector<int>,QString)));

        connect(m_maindialog,SIGNAL(SIG_setUploadPause(int,int)),this,SLOT(slot_setUploadPause(int,int)));
        connect(m_maindialog,SIGNAL(SIG_setDownloadPause(int,int)),this,SLOT(slot_setDownloadPause(int,int)));
        connect(m_maindialog,SIGNAL(SIG_shareFile(QVector<int>,QString)),this,SLOT(slot_shareFile(QVector<int>,QString)));
        m_maindialog->setWindowTitle("网盘");
        m_maindialog->setWindowFlags(Qt::WindowMinMaxButtonsHint|Qt::WindowCloseButtonHint);
        m_maindialog->show();
        m_id=loginRs.userid;
        m_curDir=m_maindialog->GetCurDir();
        slot_getCurFileList();
        slot_getMyshare();
}
        break;
    case password_error:
        QMessageBox::about(m_ploginWindow,"提示","密码错误,请重新输入");
        break;
    }

}

void Ckernel::DealUpFileRs(unsigned int lSendIP, char *buf, int nlen)
{
    qDebug()<<__func__;
   //拆包
   STRU_UPLOAD_FILE_RS*rs=(STRU_UPLOAD_FILE_RS*)buf;
//    rs->fileid;//文件id 更新
//    rs->result;//返回结果 判断是否为真
//    rs->timestamp;//时间戳
   //首先看结果是否为真
   if(!rs->result){
       qDebug()<<"上传失败,"<<rs->result<<"为结果"<<endl;
       return;
   }//为真
       //获取文件信息
           //三种写法，count find 下边引用去取
           //count和find不会创造节点，下标引用会创建一个空对象，不推荐使用
       if(map_TimeIdToFileinfo.count(rs->timestamp)==0){
           qDebug()<<"not find"<<endl;
           return;
       }
       //使用引用改变info的值
       FileInfo &info=map_TimeIdToFileinfo[rs->timestamp];
       //更新文件id,
       info.fileid=rs->fileid;

       //插入上传信息到“上传中”的控件中
       m_maindialog->slot_insertUploadFile(info);


       //发送文件块（内容）请求
       STRU_FILE_CONTENT_RQ rq;
       rq.fileid=rs->fileid;
       rq.timestamp=rs->timestamp;
       rq.userid=m_id;
       rq.len=fread(rq.content,1,_DEF_BUFFER,info.pFile);//缓冲区，次数，大小，文件指针
       SendData((char*)&rq,sizeof(rq));
}

void Ckernel::slot_dealFileContentRs(unsigned int lSendIP, char *buf, int nLen)
{
     qDebug()<<__func__;
     //拆包
     STRU_FILE_CONTENT_RS*rs=(STRU_FILE_CONTENT_RS*)buf;
//     rs->fileid;
//     rs->len;
//     rs->result;
//     rs->timestamp;
     //找信息结构体
     if(map_TimeIdToFileinfo.count(rs->timestamp)==0){
         qDebug()<<"file not found";
         return;
     }
     FileInfo & info=map_TimeIdToFileinfo[rs->timestamp];


     //判断是否暂停
     while(info.isPause){
         qDebug()<<"pause";
         //sleep();
         QThread::msleep(100);
         //写成线程传输
         //为了避免阻塞窗口窗口线程，影响事件循环，加入下面的处理 将信号取出并执行
         QCoreApplication::processEvents(QEventLoop::AllEvents,100);

//         if(m_quit){
//             return;
//         }
         //优化:当两个窗口都关闭时函数应该结束
        if(!m_maindialog&&!m_ploginWindow){
            return;
        }
     }

     //结果
     if(!rs->result){
        //如果为 假 跳回
        fseek(info.pFile,-1*rs->len,SEEK_CUR);
     }else{
         //如果为 真 pos+len
         info.pos+=rs->len;
         //TODO 更新上传进度（控件）
         //方案1：写信号槽，考虑多线程
         //方案2：直接调用 当前函数一定在主线程
         Q_EMIT SIG_updateUploadFileProgress(info.timestamp,info.pos);

         //文件 大小 时间 恢复
         //表单项写到map中
         //把信息写入表格项QTableWidget的每一项QTableWidgetItem

         //判断是否结束
         if(info.pos>=info.size){
             //是     关闭文件 回收，返回
            fclose(info.pFile);
            map_TimeIdToFileinfo.erase(rs->timestamp);
            //回收节点会销毁对象，所以不回收
            m_maindialog->slot_deleteAllFileInfo();
            slot_getCurFileList();
            return;
         }
     }
     //发文件块
     STRU_FILE_CONTENT_RQ rq;
     rq.fileid=rs->fileid;
     rq.timestamp=rs->timestamp;
     rq.userid=m_id;
     rq.len=fread(rq.content,1,_DEF_BUFFER,info.pFile);//缓冲区，次数，大小，文件指针
     SendData((char*)&rq,sizeof(rq));

}
void Ckernel::slot_getCurFileList()
{
    qDebug()<<__func__;
    //向服务器发送获取当前目录文件列表
    STRU_GET_FILE_INFO_RQ rq;
    rq.userid=m_id;
    //兼容中文
    std::string strDir=m_curDir.toStdString().c_str();
    strcpy(rq.dir,strDir.c_str());
    SendData((char*)&rq,sizeof(rq));
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
#include<QDateTime>

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

    //FILE* pfile= fopen(gbPath,"rb");
    fileinfo.pFile=fopen(gbPath,"rb");
    if(fileinfo.pFile==nullptr){
        qDebug()<<"获取文件失败";
    }

    fileinfo.name = qfileinfo.fileName();
    fileinfo.dir =dir;
    fileinfo.time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    fileinfo.md5 =QString::fromStdString(getFileMD5(path));
    //文件类型为"file"
    fileinfo.type = "file";
    //对fileinfo结构体进行赋值
    fileinfo.absolutePath =path;
    //发送请求
    STRU_UPLOAD_FILE_RQ uploadRq;
    uploadRq.userid = m_id;
    uploadRq.size = fileinfo.size;
    //获取当前时间戳:
    qint64 time=QDateTime::currentDateTime().currentSecsSinceEpoch();
    uploadRq.timestamp = static_cast<int>(time);//ms to s
    map_TimeIdToFileinfo[uploadRq.timestamp]=fileinfo;
    strcpy(uploadRq.fileName, fileinfo.name.toStdString().c_str());
    strcpy(uploadRq.dir, dir.toStdString().c_str());
    strcpy(uploadRq.md5, fileinfo.md5.toStdString().c_str());
    strcpy(uploadRq.fileType, "file");
    strcpy(uploadRq.time, fileinfo.time.toStdString().c_str());
    SendData((char*)&uploadRq,sizeof(uploadRq));
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
    return md.toString();
}

//获取文件列表
void Ckernel::slot_dealGetFileInfoRs(unsigned int lSendIP,char* buf,int nLen)
{
    qDebug()<<__func__<<endl;
    //拆包
    STRU_GET_FILE_INFO_RS*rs=(STRU_GET_FILE_INFO_RS*)buf;
    //rs->dir;
    if(m_curDir!=QString::fromStdString(rs->dir)){
        return;
    }

    //先删除原来的列表，再更新
    m_maindialog->slot_deleteAllFileInfo();

    //获取元素
    int count=rs->count;
    for(int i=0;i<count;i++){
        FileInfo info;
        info.fileid=rs->fileinfo[i].fileid;
        info.type=QString::fromStdString(rs->fileinfo[i].filetype);
        info.name=QString::fromStdString(rs->fileinfo[i].name);
        info.size=rs->fileinfo[i].size;
        info.time=QString::fromStdString(rs->fileinfo[i].time);

        //插入到控件中
        m_maindialog->slot_insertFileInfo(info);


    }

}

void Ckernel::slot_downloadFile(int fileid, QString dir)
{
    qDebug()<<__func__<<this->metaObject()->className();
    //写请求
    STRU_DOWNLOAD_FILE_RQ rq;
    //兼容中文
    string strDir=dir.toStdString().c_str();
    strcpy(rq.dir,strDir.c_str());
    rq.fileid=fileid;
    int timestamp=QDateTime::currentDateTime().toString("hhmmsszzz").toInt();

    //bug修复 反复检测时间戳是否存在
    //防止时间戳冲突
    while( map_TimeIdToFileinfo.count(timestamp)>0){
        timestamp++;
    }
    rq.timestamp=timestamp;
    rq.userid=m_id;
    SendData((char*)&rq,sizeof(rq));
}

void Ckernel::slot_downloadFolder(int fileid, QString dir)
{
    //写请求
    qDebug()<<__func__<<endl;

    STRU_DOWNLOAD_FOLDER_RQ rq;
    string strDir=dir.toStdString();
    strcpy(rq.dir,strDir.c_str());

    rq.fileid=fileid;
    int timestamp=QDateTime::currentDateTime().toString("hhmmsszzz").toInt();
    while( map_TimeIdToFileinfo.count(timestamp)>0){
        timestamp++;
    }
    rq.timestamp=timestamp;
    rq.userid=m_id;
    SendData((char*)&rq,sizeof(rq));
}

void Ckernel::slot_addFolder(QString name, QString dir)
{
    qDebug()<<__func__;
    //发请求包
    STRU_ADD_FOLDER_RQ rq;
//    rq.dir;
//    rq.fileName;
//    rq.time;
//    rq.timestamp;
//    rq.userid;

    string strDir=dir.toStdString();
    strcpy(rq.dir,strDir.c_str());
    string strName=name.toStdString();
    strcpy(rq.fileName,strName.c_str());
    string strTime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
    strcpy(rq.time,strTime.c_str());

    //时间戳不放到map中，不创建结构体
    rq.timestamp=QDateTime::currentDateTime().toString("hhmmsszzz").toInt();
    rq.userid=m_id;

    SendData((char*)&rq,sizeof(rq));
}

void Ckernel::slot_changeDir(QString dir)
{
    qDebug()<<__func__<<endl;
    //更新当前的目录
    m_curDir=dir;
//    //刷新列表
    m_maindialog->slot_deleteAllFileInfo();
    slot_getCurFileList();
}


void Ckernel::slot_getMyshare(){
   qDebug()<<__func__<<endl;
    STRU_MY_SHARE_RQ rq;
    rq.userid=m_id;
    qDebug()<<"注册回复时我的id"<<m_id;
    SendData((char*)&rq,sizeof(rq));

}

void Ckernel::slot_deleteFile(QVector<int> fileidArray, QString dir)
{
    qDebug()<<__func__<<endl;
    //发送请求
    int packlen=sizeof(STRU_DELETE_FILE_RQ)+fileidArray.size()*sizeof(int);
    STRU_DELETE_FILE_RQ*rq=(STRU_DELETE_FILE_RQ*)malloc(packlen);
    rq->init();
    string strDir=dir.toStdString();
    strcpy(rq->dir,strDir.c_str());
    rq->fileCount=fileidArray.size();

    rq->userid=m_id;
    for(int i=0;i<rq->fileCount;i++){
        rq->fileidArray[i]=fileidArray[i];
    }

    SendData((char*)rq,packlen);
    free(rq);
}

void Ckernel::slot_setUploadPause(int timestamp, int isPause)
{
    qDebug()<<__func__<<endl;
    //ispause 1 从正在下载变为暂停 isPause 0从暂停变为正在下载
    //需要找到文件信息结构体
    if(map_TimeIdToFileinfo.count(timestamp)>0){
        //map里面有程序未退出的情况,直接置位
        map_TimeIdToFileinfo[timestamp].isPause=isPause;
    }else{
        //map[]中没有，程序已退出
        //断点续传 todo

    }
}

void Ckernel::slot_dealFileHeaderRq(unsigned int lSendIP, char *buf, int nLen)
{
    //拆包
    qDebug()<<__func__;
    STRU_FILE_HEADER_RQ*rq=(STRU_FILE_HEADER_RQ*)buf;
//    rq->dir;
//    rq->fileid;
//    rq->fileName;
//    rq->fileType;
//    rq->md5;
//    rq->size;
//    rq->timestamp;
    //创建文件信息结构体 赋值
    FileInfo info;
    //默认位置/路径
    // NetDisk sysPath(不含最后的'/')+dir+name

    //dir可能有很多层 需要循环创建目录 TODO
    QString tmpDir=QString::fromStdString(rq->dir);// /NetDisk/111   1.txt   把111文件夹创建出来才能创建1.txt文件
    QStringList dirList=tmpDir.split("/");//切分  分割函数  Netdisk 111

//    for(int i=0;i<dirList.size();i++){
//        if(dirList.at(i).is){

//        }
//    }
    QString pathsum=m_sysPath;
    for(QString & node:dirList){
        if(!node.isEmpty()){
            pathsum+="/";
            pathsum+=node;

            QDir dir;
            if(!dir.exists(pathsum)){
                dir.mkdir(pathsum);
            }
        }
    }

    info.name=QString::fromStdString(rq->fileName);
    info.dir=QString::fromStdString(rq->dir);
    info.absolutePath=m_sysPath+info.dir+info.name;
    //QString("%1%2%3").arg(m_sysPath).arg(info.dir).arg(info.name);
    info.fileid=rq->fileid;
    info.md5=QString::fromStdString(rq->md5);


    info.size=rq->size;
    info.time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    info.timestamp=rq->timestamp;
    info.type="file";

    //打开文件
    char pathBuf[1000]="";
    Utf8ToGB2312(pathBuf,1000,info.absolutePath);
    info.pFile=fopen(pathBuf,"wb");//
    if(!info.pFile){
        qDebug()<<"file open fail";
        return;
    }


    //TODO ：前端控件:保存下载信息到控件
    m_maindialog->slot_insertDownloadFile(info);


    //时间戳存在说明还在上传,应该等上传完毕
    while( map_TimeIdToFileinfo.count(rq->timestamp)>0){
        rq->timestamp++;
    }
    //保存到map中
    map_TimeIdToFileinfo[rq->timestamp]=info;

    //写回复
    STRU_FILE_HEADER_RS rs;
    rs.fileid=rq->fileid;
    rs.result=1;
    rs.timestamp=rq->timestamp;
    rs.userid=m_id;
    SendData((char*)&rs,sizeof(rs));
}

void Ckernel::setSystemPath()
{
    QString path=QCoreApplication::applicationDirPath()+"/NetDisk";

    QDir dir;
    //没有文件夹，创建
    if(!dir.exists(path)){
        dir.mkdir(path);//只能创建一层
    }
    //默认路径
    m_sysPath=path;
}

void Ckernel::slot_dealFileContentRq(unsigned int lSendIP, char *buf, int nLen)
{
    qDebug()<<__func__;
    //拆包
    STRU_FILE_CONTENT_RQ *rq=(STRU_FILE_CONTENT_RQ *)buf;
    //拿到文件信息结构体
    if(map_TimeIdToFileinfo.count(rq->timestamp)==0)return ;
    FileInfo&info=map_TimeIdToFileinfo[rq->timestamp];

    //判断是否暂停
    while(info.isPause){
        //sleep();
        QThread::msleep(100);
        //写成线程传输
        //为了避免阻塞窗口窗口线程，影响事件循环，加入下面的处理 将信号取出并执行
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        //避免程序退出后，此任务不退出，一直卡住
        if(!m_maindialog&&!m_ploginWindow){
            return;
        }
    }
    //写文件
    STRU_FILE_CONTENT_RS rs;
        //缓冲区，字节数，次数，文件指针
        int len=fwrite(rq->content,1,rq->len,info.pFile);
        //不成功
        if(len!=rq->len){
            rs.result=0;
            fseek(info.pFile,-1*len,SEEK_CUR);
            qDebug()<<"本地写入失败";
        }
        else{//成功    pos+len
            rs.result=1;
            info.pos+=len;
            //TODO:更新进度  信号
            Q_EMIT SIG_updateDownLoadFileProgress(rq->timestamp,info.pos);

            //要看 有没有到末尾 是否结束
            if(info.pos>=info.size){
                //结束 关闭文件 回收
                fclose(info.pFile);
                map_TimeIdToFileinfo.erase(rq->timestamp);
            }


        }
    //写回复
    rs.fileid=rq->fileid;
    rs.len=rq->len;
    rs.timestamp=rq->timestamp;
    rs.userid=m_id;
    //发送
    SendData((char*)&rs,sizeof(rs));

}

void Ckernel::slot_dealAddFolderRs(unsigned int lSendIP, char *buf, int nLen)
{
    //拆包
    STRU_ADD_FOLDER_RS*rs=(STRU_ADD_FOLDER_RS*)buf;

    //判断是否成功
    if(rs->result!=1)return;

    //先删除原来的列表，再更新
    //由于并发问题，写在获取文件列表当中
    m_maindialog->slot_deleteAllFileInfo();
    //更新文件列表
    slot_getCurFileList();

}
void Ckernel::slot_uploadFolder(QString path, QString dir)
{
    qDebug()<<__func__;

    QFileInfo info(path);//用于获取文件的名字
    QDir dr(path);
    //当前文件夹的处理 addfolder c:/项目  下面是有：/0321/ /0523/  1.txt 上传到 /05
    //第一个：创建的子目录/子文件夹 ;第二个：当前目录下/文件夹
    qDebug()<<"folder: "<<info.fileName()<<",dir: "<<dir<<endl;
    slot_addFolder(info.fileName(),dir);
    //获取文件夹下面一层 所有文件的路径(文件信息)
    QFileInfoList lst=dr.entryInfoList();//获取路径下所有文件的文件信息列表
    //遍历所有文件
    QString newDir=dir+info.fileName()+"/";
    for(int i=0;i<lst.size();i++){
        QFileInfo file=lst.at(i);
        //如果是 . 继续
        if(file.fileName()=="."){
            continue;
        }
        //如果是 .. 继续
        if(file.fileName()==".."){
            continue;
        }
        //如果是文件 uploadfile  -》路径 文件信息的绝对路径 传到什么目录 /05/项目
        //路径不变，上传的东西一直在变化，因为上传的文件夹内可能有多个文件
        if(file.isFile()){
            //文件路径    在哪个文件夹下创建
            qDebug()<<"file: "<<file.absoluteFilePath()<<",dir: "<<newDir<<endl;
            slot_UpFile(file.absoluteFilePath(),newDir);//dir: /05/项目

        }
        //如果是文件夹 slot_uploadfolder 递归
        if(file.isDir()){
            qDebug()<<"file: "<<file.absoluteFilePath()<<",dir: "<<newDir<<endl;
            slot_uploadFolder(file.absoluteFilePath(),newDir);
        }
    }
}
void Ckernel::slot_shareFile(QVector<int> fileidArray, QString dir)
{
    qDebug()<<__func__<<this->metaObject()->className();
    //打包
    int packLen=sizeof(STRU_SHARE_FILE_RQ)+
            sizeof(int)*fileidArray.size();
    STRU_SHARE_FILE_RQ*rq=(STRU_SHARE_FILE_RQ*)malloc(packLen);
    rq->init();
    //STRU_SHARE_FILE_RQ*rq=(STRU_SHARE_FILE_RQ*)
    rq->itemCount=fileidArray.size();
    for(int i=0;i<fileidArray.size();i++){
        rq->fileidArray[i]=fileidArray[i];
    }
    rq->userid=m_id;
    std::string strDir=dir.toStdString();
    //rq->dir;
    strcpy(rq->dir,strDir.c_str());
    QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //rq->shareTime;
    strcpy(rq->shareTime,time.toStdString().c_str());


    SendData((char*)rq,packLen);
    free(rq);
}
void Ckernel::slot_dealShareFileRs(unsigned int lSendIP, char *buf, int nLen)
{
    //拆包
    qDebug()<<__func__<<endl;
    STRU_SHARE_FILE_RS *rs=(STRU_SHARE_FILE_RS*)buf;
    //判断是否成功
    if(rs->result!=1)return;

    //刷新 发获取请求
    slot_getMyshare();

}
void Ckernel::slot_dealMyShareRs(unsigned int lSendIP, char *buf, int nLen)
{
    qDebug()<<__func__<<endl;
    //拆包
    STRU_MY_SHARE_RS*rs=(STRU_MY_SHARE_RS*)buf;
    int count=rs->itemCount;
    //rs->items;

    //遍历分享文件的信息，添加到控件上面

    m_maindialog->slot_deleteAllShareInfo();
    for(int i=0;i<count;i++){
        m_maindialog->slot_insertShareFileInfo(
        rs->items[i].name,
        rs->items[i].size,
        rs->items[i].time,
        rs->items[i].shareLink);
    }

}
void Ckernel::slot_getShareByLink(int code, QString dir)
{
    qDebug()<<__func__<<_DEF_PACK_GET_SHARE_RQ<<endl;
    //发请求
    STRU_GET_SHARE_RQ rq;
    string tmpdir=dir.toStdString();
    strcpy(rq.dir,tmpdir.c_str());
    rq.shareLink=code;
    string time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
    strcpy(rq.time,time.c_str());
    rq.userid=m_id;
    SendData((char*)&rq,sizeof(rq));

}
void Ckernel::slot_dealGetShareRs(unsigned int lSendIP, char *buf, int nLen)
{
    //拆包
    STRU_GET_SHARE_RS * rs=(STRU_GET_SHARE_RS *)buf;
    //按结果处理
    if(rs->result==0){
        //错误返回提示
        QMessageBox::about(this->m_maindialog,"提示","获取分享失败");
    }else{//正确刷新
        if(QString::fromStdString(rs->dir)==m_curDir){
            slot_getCurFileList();
        }
    }
}
void Ckernel::slot_dealDeleteFileRs(unsigned int lSendIP, char *buf, int nLen)
{
    //拆包
    STRU_DELETE_FILE_RS * rs=(STRU_DELETE_FILE_RS *)buf;

    //看是否刷新
    if(rs->result==1){
        if(QString::fromStdString(rs->dir)==m_curDir){
            m_maindialog->slot_deleteAllFileInfo();
            slot_getCurFileList();
        }
    }
}
void Ckernel::slot_dealFolderHeaderRq(unsigned int lSendIP, char *buf, int nLen)
{
    //拆包
    STRU_FOLDER_HEADER_RQ *rq=(STRU_FOLDER_HEADER_RQ *)buf;
    //创建目录

    //dir可能有很多层 需要循环创建目录 TODO
    QString tmpDir=QString::fromStdString(rq->dir);// /NetDisk/111   1.txt   把111文件夹创建出来才能创建1.txt文件
    QStringList dirList=tmpDir.split("/");//切分  分割函数  Netdisk 111

//    for(int i=0;i<dirList.size();i++){
//        if(dirList.at(i).is){

//        }
//    }
    QString pathsum=m_sysPath;
    for(QString & node:dirList){
        if(!node.isEmpty()){
            pathsum+="/";
            pathsum+=node;

            QDir dir;
            if(!dir.exists(pathsum)){
                dir.mkdir(pathsum);
            }
        }
    }
    pathsum+="/";
    pathsum+=QString::fromStdString(rq->fileName);
    QDir dir;
    if(!dir.exists(pathsum)){
        dir.mkdir(pathsum);
    }
}


void Ckernel::slot_dealQuickUploadRs(unsigned int lSendIP, char *buf, int nLen)
{
    //拆包
    STRU_QUICK_UPLOAD_RS*rs=(STRU_QUICK_UPLOAD_RS*)buf;
    //获取文件信息
    if(map_TimeIdToFileinfo.count(rs->timestamp==0)){
        return;
    }
    FileInfo&info=map_TimeIdToFileinfo[rs->timestamp];
    //关闭文件（上传请求打开了文件，需要关闭）
    if(info.pFile){
       fclose(info.pFile);
    }

    //写入上传已完成信息
    m_maindialog->slot_insertUploadComplete(info);

    //发送刷新文件列表
    if(m_curDir==info.dir){//判断是不是当前目录
        m_maindialog->slot_deleteAllFileInfo();
        slot_getCurFileList();
    }
    //删除节点
    map_TimeIdToFileinfo.erase(rs->timestamp);

}


void Ckernel::slot_setDownloadPause(int timestamp, int isPause)
{
    //ispause 1 从正在下载变为暂停 isPause 0从暂停变为正在下载
    //需要找到文件信息结构体
    if(map_TimeIdToFileinfo.count(timestamp)>0){
        //map里面有程序未退出的情况,直接置位
        map_TimeIdToFileinfo[timestamp].isPause=isPause;
    }else{
        //map[]中没有，程序已退出
        //断点续传 todo

    }
}

