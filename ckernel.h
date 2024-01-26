#ifndef CKERNEL_H
#define CKERNEL_H
#include"maindialog.h"
#include"packdef.h"
#include<QDebug>
#include <QObject>
#include"TcpClientMediator.h"
#include"TcpServerMediator.h"
#include"md5.h"
#include"logindialog.h"
#include<common.h>
#include<QTextCodec>
#include<string>
//协议映射表:没有协议映射表的话,处理相应数据会使用switch,代码量大
class Ckernel;
typedef void(Ckernel::*FUN)(unsigned int lSendIP , char* buf , int nlen );
//单例模式
//#define server
class Ckernel : public QObject
{
    Q_OBJECT
public:
   static Ckernel* GetInstance(){
           static Ckernel m_kernel;
        return &m_kernel;
    }
   // QString -> char* gb2312
   static void Utf8ToGB2312( char* gbbuf , int nlen ,QString& utf8);

   // char* gb2312 --> QString utf8
   static QString GB2312ToUtf8( char* gbbuf );

   static string getFileMD5 ( QString path);

   static const string GetMd5(string str);
private:
    explicit Ckernel(QObject *parent = nullptr);
    ~Ckernel();
   void loadIniFile();
   //初始化协议映射表
   void initFunMap();
   //封装发送函数
   void SendData(char* buf,int size);
   //注册回复
    void DealRegistRs(unsigned int lSendIP , char* buf , int nlen );
    //login 回复
    void DealLoginRs(unsigned int lSendIP , char* buf , int nlen );
    //处理文件上传回复
    void DealUpFileRs(unsigned int lSendIP , char* buf , int nlen );
signals:

private:

    //所有操作都由kernel完成, maindialog作为kernel的成员
    MainDialog* m_maindialog;
    QString m_ip;
    QString m_port;
    TcpClientMediator* m_clientMediator;
    FUN m_funs[_DEF_PACK_COUNT];
    LoginDialog* m_ploginWindow;
    std::map<long long,FileInfo> map_TimeIdToFileinfo;
    int m_id;

#ifdef server
    TcpServerMediator* m_serverMediator;
#endif
private slots:
    void slot_loginClose();
    void slot_mainClose();
    void slot_ReadyData( unsigned int lSendIP , char* buf , int nlen );
    void slot_disConnect();
    //处理loginDialog的注册信号
    void slot_registRq(QString name,QString tel,QString password );
    //处理loginDialog的login信号
    void slot_loginRq(QString tel,QString password );
    //处理mainDialog的upfile信号
    void slot_UpFile(QString path, QString dir);
    
    
};

#endif // CKERNEL_H
