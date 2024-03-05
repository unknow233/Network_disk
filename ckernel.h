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
   void slot_getCurFileList();
   void setSystemPath();
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
    void slot_dealFileContentRs(unsigned int lSendIP,char* buf,int nLen);
    void slot_dealGetFileInfoRs(unsigned int lSendIP,char* buf,int nLen);
    void slot_dealFileHeaderRq(unsigned int lSendIP,char* buf,int nLen);
    void slot_dealFileContentRq(unsigned int lSendIP,char* buf,int nLen);
    void slot_dealAddFolderRs(unsigned int lSendIP,char* buf,int nLen);
    void slot_dealQuickUploadRs(unsigned int lSendIP,char* buf,int nLen);
    void slot_dealShareFileRs(unsigned int lSendIP,char* buf,int nLen);
    void slot_dealMyShareRs(unsigned int lSendIP,char* buf,int nLen);
    void slot_dealGetShareRs(unsigned int lSendIP,char* buf,int nLen);
    void slot_dealFolderHeaderRq(unsigned int lSendIP,char* buf,int nLen);
    void slot_dealDeleteFileRs(unsigned int lSendIP,char* buf,int nLen);

signals:
    void SIG_updateUploadFileProgress(int timestamp,int pos);
    void SIG_updateDownLoadFileProgress(int timestamp,int pos);
private:

    //所有操作都由kernel完成, maindialog作为kernel的成员
    MainDialog* m_maindialog;
    QString m_ip;
    QString m_port;
    TcpClientMediator* m_clientMediator;
    FUN m_funs[_DEF_PACK_COUNT];
    LoginDialog* m_ploginWindow;
    std::map<int64_t,FileInfo> map_TimeIdToFileinfo;
    int m_id;
    QString m_curDir;//网盘当前的目录
    QString m_sysPath;//
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
    void slot_downloadFile(int fileid,QString dir);
    //什么文件id的文件，什么目录下的文件夹，下载
    void slot_downloadFolder(int fileid,QString dir);
    //什么路径下创建什么名字的文件夹
    void slot_addFolder(QString name,QString dir);
    //改变路径
    void slot_changeDir(QString dir);
    //分享 什么目录下面的文件列表
    void slot_shareFile(QVector<int>fileidArray,QString dir);
    //获取个人目录所有分享
    void slot_getMyshare();
    //获取分享码文件，添加到目录
    void slot_getShareByLink(int code,QString dir);
    //删除什么目录下的一系列文件（文件id，数组）
    void slot_deleteFile(QVector<int>fileidArray,QString dir);
    //设置上传暂停 0开始 1暂停
    void slot_setUploadPause(int timestamp,int isPause);
    //设置下载暂停 0开始 1暂停
    void slot_setDownloadPause(int timestamp,int isPause);
    void slot_uploadFolder(QString path, QString dir);
};

#endif // CKERNEL_H
