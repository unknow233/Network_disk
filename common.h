#ifndef COMMON_H
#define COMMON_H
#include<QString>
#include<iostream>
#include<memory>
#include"net/packdef.h"
////////////////////文件信息/////////////////
typedef  int PackType;
struct FileInfo
{

    FileInfo():fileid(0) , size(0),pFile( nullptr )
      , pos(0) , isPause(0){

    }

    int fileid;
    QString name;
    QString dir;
    QString time;
    int size;
    QString md5;
    QString type;
    QString absolutePath;

    int pos; //上传或下载到什么位置
    int timestamp;//时间戳，很重要，用于区分哪个文件
    int isPause; //暂停  0 1

    //文件指针
   FILE* pFile;

  static QString getsize(int size){
      QString res;
      int count=0;
      int tmp=size;
      while(tmp!=0){
          tmp/=1024;
          if(tmp!=0)count++;
      }
      switch(count){
      case 0:  //KB
          res=QString("0.%1KB").arg((int)(size%1024/1024.0*100),2,10,QChar('0'));
          //arg的参数，第二个：宽度，第三个：进制，第四个：不够宽时缺省的参数
          if(size!=0&&res=="0.00KB")
              res=="0.01KB";
          break;
      case 1:
          res=QString("%1.%2KB").arg(size/1024).arg((int)(size%1024/1024.0*100),2,10,QChar('0'));
          //arg的参数，第二个：宽度，第三个：进制，第四个：不够宽时缺省的参数
          break;
      case 2://MB
          res=QString("%1.%2MB").arg(size/1024/1024).arg((int)(size/1024%1024/1024.0*100),2,10,QChar('0'));
          break;
          break;
      case 3://GB
          res=QString("%1.%2GB").arg(size/1024/1024).arg((int)(size/1024%1024/1024.0*100),2,10,QChar('0'));
          break;
      default://过大
          break;
      }
      return res;
  }
};


////////////////////文件上传/////////////////
//上传文件请求
#define _DEF_PACK_UPLOAD_FILE_RQ       (_DEF_PACK_BASE + 4 )
//上传文件回复
#define _DEF_PACK_UPLOAD_FILE_RS       (_DEF_PACK_BASE + 5 )

//文件内容请求
#define _DEF_PACK_FILE_CONTENT_RQ       (_DEF_PACK_BASE + 6 )
//文件内容回复
#define _DEF_PACK_FILE_CONTENT_RS       (_DEF_PACK_BASE + 7 )
#define _MAX_PATH_SIZE                  (260)
#define _DEF_BUFFER                     (4096)
#define _DEF_CONTENT_SIZE               (1024)
#define _MAX_SIZE                       (40)






//上传文件请求
struct STRU_UPLOAD_FILE_RQ
{
    STRU_UPLOAD_FILE_RQ():type(_DEF_PACK_UPLOAD_FILE_RQ)
      ,userid(0),size(0),timestamp(0){
        memset( fileName , 0, sizeof(fileName) );
        memset( dir , 0, sizeof(dir) );
        memset( md5 , 0, sizeof(md5) );
        memset( fileType , 0, sizeof(fileType) );
        memset( time , 0, sizeof(time) );
    }
    PackType type;
    int timestamp;//时间戳用于区分不同任务
    int userid; //服务器与时间戳配合,区分不同任务
    char fileName[_MAX_PATH_SIZE]; //上传文件名字
    int size;//大小
    char dir[_MAX_PATH_SIZE];//上传到什么目录
    char md5[_MAX_SIZE]; //上传文件的md5, 用于验证文件是否完整无误
    char fileType[_MAX_SIZE];//文件类型
    char time[_MAX_SIZE]; //上传时间
};

//上传文件回复
struct STRU_UPLOAD_FILE_RS
{
    STRU_UPLOAD_FILE_RS(): type(_DEF_PACK_UPLOAD_FILE_RS)
      , userid(0), fileid(0),result(1),timestamp(0){

    }
    PackType type;
    int timestamp;//时间戳用于区分不同任务
    int userid;//用户id
    int fileid; //文件id
    int result; //结果
};


//文件内容请求
struct STRU_FILE_CONTENT_RQ
{
    STRU_FILE_CONTENT_RQ():type(_DEF_PACK_FILE_CONTENT_RQ),
        userid(0),fileid(0),len(0),timestamp(0){
        memset( content , 0 , sizeof(content));
    }
    PackType type;
    int timestamp;//时间戳用于区分不同任务
    int userid;//用户id
    int fileid;//文件id
    char content[_DEF_BUFFER];//文件内容 也叫文件块   _DEF_BUFFER  4096
    int len;//文件内容长度
};

//文件内容回复
struct STRU_FILE_CONTENT_RS
{
    STRU_FILE_CONTENT_RS():type(_DEF_PACK_FILE_CONTENT_RS),
        userid(0),fileid(0),result(1),len(0),timestamp(0){

    }
    PackType type;
    int timestamp;//时间戳用于区分不同任务
    int userid;//用户id
    int fileid;//文件id
    int result;//结果
    int len;//文件内容长度

};
/////////////文件信息//////////////////
//文件内容请求
#define _DEF_PACK_GET_FILE_INFO_RQ       (_DEF_PACK_BASE + 8 )
//文件内容回复
#define _DEF_PACK_GET_FILE_INFO_RS       (_DEF_PACK_BASE + 9 )

#define _DEF_TYPE_LEN (10)

struct STRU_GET_FILE_INFO_RQ{
    STRU_GET_FILE_INFO_RQ():type(_DEF_PACK_GET_FILE_INFO_RQ),userid(0){
        memset(dir,0,sizeof(dir));
    }
    PackType type;
    int userid;
    char dir[_MAX_PATH_SIZE];

};
struct STRU_FILE_INFO{
    STRU_FILE_INFO():fileid(0){
        memset(name,0,sizeof(name));
        memset(time,0,sizeof(time));
        memset(filetype,0,sizeof(filetype));
    }
    int fileid;
    char name[_MAX_PATH_SIZE];
    char time[_MAX_SIZE];
    int size;
    char filetype[_DEF_TYPE_LEN];
};

struct STRU_GET_FILE_INFO_RS{
    STRU_GET_FILE_INFO_RS():type(_DEF_PACK_GET_FILE_INFO_RS),count(0)
    {
        memset(dir,0,sizeof(dir));
    }
    void init(){//柔性数组，不能用new；所以不能用结构体的初始化，需要手动实现构造函数并调用
        type=_DEF_PACK_GET_FILE_INFO_RS;
        count=0;
        memset(dir,0,sizeof(dir));
    }
    PackType type;
    char dir[_MAX_PATH_SIZE];
    int count;
    //文件信息数组
    STRU_FILE_INFO fileinfo[ ];//柔性数组
};

////////////////////文件下载/////////////////
//下载文件请求
#define _DEF_PACK_DOWNLOAD_FILE_RQ			(_DEF_PACK_BASE + 10 )

//下载文件夹请求
#define _DEF_PACK_DOWNLOAD_FOLDER_RQ		(_DEF_PACK_BASE + 11 )

//下载文件回复
#define _DEF_PACK_DOWNLOAD_FILE_RS			(_DEF_PACK_BASE + 12 )

//下载文件头请求
#define _DEF_PACK_FILE_HEADER_RQ			(_DEF_PACK_BASE + 13 )

//下载文件头回复
#define _DEF_PACK_FILE_HEADER_RS			(_DEF_PACK_BASE + 14 )



//下载文件请求
struct STRU_DOWNLOAD_FILE_RQ
{
    STRU_DOWNLOAD_FILE_RQ():type(_DEF_PACK_DOWNLOAD_FILE_RQ)
      ,userid(0),fileid(0),timestamp(0){
        memset( dir , 0, sizeof(dir) );
    }
    PackType type;
    int timestamp;//时间戳用于区分不同任务
    int userid; //服务器与时间戳配合,区分不同任务
    int fileid; //文件id
    char dir[ _MAX_PATH_SIZE ]; //文件所属目录

};

//下载文件夹请求
struct STRU_DOWNLOAD_FOLDER_RQ
{
    STRU_DOWNLOAD_FOLDER_RQ():type(_DEF_PACK_DOWNLOAD_FOLDER_RQ)
      ,userid(0),fileid(0),timestamp(0){
        memset( dir , 0, sizeof(dir) );
    }
    PackType type;
    int timestamp;//时间戳用于区分不同任务
    int userid; //服务器与时间戳配合,区分不同任务
    int fileid; //文件id
    char dir[ _MAX_PATH_SIZE ]; //文件所属目录

};

//下载文件回复 ( 一般也不会出现问题, 所以这个包也不用 )
struct STRU_DOWNLOAD_FILE_RS
{
    STRU_DOWNLOAD_FILE_RS():type(_DEF_PACK_DOWNLOAD_FILE_RS)
    ,timestamp(0),userid(0),fileid(0),result(1){
    }
    PackType type;
    int timestamp;//时间戳用于区分不同任务
    int userid; //服务器与时间戳配合,区分不同任务
    int fileid; //文件id
    int result; //结果
};

//文件头请求
struct STRU_FILE_HEADER_RQ
{
    STRU_FILE_HEADER_RQ():type(_DEF_PACK_FILE_HEADER_RQ)
      ,fileid(0),size(0),timestamp(0){
        memset( fileName , 0, sizeof(fileName) );
        memset( dir , 0, sizeof(dir) );
        memset( md5 , 0, sizeof(md5) );
        memset( fileType , 0, sizeof(fileType) );
    }
    PackType type;
    int timestamp;
    int fileid;
    char fileName[_MAX_PATH_SIZE];
    int size;//大小
    char dir[_MAX_PATH_SIZE];//路径
    char md5[_MAX_SIZE];
    char fileType[_MAX_SIZE];//文件类型
};

//文件头回复
struct STRU_FILE_HEADER_RS
{
    STRU_FILE_HEADER_RS(): type(_DEF_PACK_FILE_HEADER_RS)
      , userid(0), fileid(0),result(1),timestamp(0){

    }
    PackType type;
    int timestamp;
    int userid;
    int fileid;
    int result;

};

//////////////////新建文件夹/////////////////////
//新建文件夹请求
#define _DEF_PACK_ADD_FOLDER_RQ       (_DEF_PACK_BASE + 15 )
//新建文件夹回复
#define _DEF_PACK_ADD_FOLDER_RS       (_DEF_PACK_BASE + 16 )

//新建文件夹请求
struct STRU_ADD_FOLDER_RQ
{
    STRU_ADD_FOLDER_RQ():type(_DEF_PACK_ADD_FOLDER_RQ)
      ,timestamp(0),userid(0){
        memset( fileName , 0, sizeof(fileName) );
        memset( dir , 0, sizeof(dir) );
        memset( time , 0, sizeof(time) );
    }
    PackType type;
    int timestamp;
    int userid;
    char fileName[_MAX_PATH_SIZE];
    char dir[_MAX_PATH_SIZE];//路径
    char time[_MAX_SIZE]; //上传时间
};

//新建文件夹回复
struct STRU_ADD_FOLDER_RS
{
    STRU_ADD_FOLDER_RS(): type(_DEF_PACK_ADD_FOLDER_RS)
     ,timestamp(0) ,userid(0), result(1){

    }
    PackType type;
    int timestamp;
    int userid;
    int result;
};

//////////////////////秒传（快传）/////////////
//秒传回复
#define _DEF_PACK_QUICK_UPLOAD_RS (_DEF_PACK_BASE+17)

struct STRU_QUICK_UPLOAD_RS{
    STRU_QUICK_UPLOAD_RS():type(_DEF_PACK_QUICK_UPLOAD_RS),timestamp(0),userid(0),result(1){}
    PackType type;
    int timestamp;
    int userid;
    int result;

};


/////////////////分享文件///////////////////
////分享文件请求
#define _DEF_PACK_SHARE_FILE_RQ (_DEF_PACK_BASE+18)
////分享文件回复
#define _DEF_PACK_SHARE_FILE_RS (_DEF_PACK_BASE+19)
//分享文件请求：包含   谁  分享  什么目录下面的 那些文件 (文件id,柔性数组) 分享时间
struct STRU_SHARE_FILE_RQ{
    void init(){
        type=_DEF_PACK_SHARE_FILE_RQ;
        userid=0;
        memset(dir,0,sizeof(dir));
        memset(shareTime,0,sizeof(shareTime));
        itemCount=0;
    }
    PackType type;
    int userid;
    char dir[_MAX_PATH_SIZE];
    char shareTime[_MAX_SIZE];
    int itemCount;
    int fileidArray[];
};
//////收到回复，就刷新分享列表
struct STRU_SHARE_FILE_RS{
    STRU_SHARE_FILE_RS():type(_DEF_PACK_SHARE_FILE_RS),result(0){}
    PackType type;
    int result;
};

////////////////刷新分享列表///////////////////
//获取自己的分享请求
#define _DEF_PACK_MY_SHARE_RQ (_DEF_PACK_BASE+20)
//获取自己的分享回复
#define _DEF_PACK_MY_SHARE_RS (_DEF_PACK_BASE+21)
struct STRU_MY_SHARE_RQ{
    STRU_MY_SHARE_RQ():type(_DEF_PACK_MY_SHARE_RQ),userid(0){}
    PackType type;
    int userid;
    //考虑加入时间 获取指定时间范围内的分享

};
//分享文件信息：名字 大小 分享时间 链接 (密码 todo)
struct STRU_MY_SHARE_FILE{
    char name[_MAX_PATH_SIZE];
    int size;
    char time[_MAX_SIZE];
    int shareLink;
};
//获取自己的分享回复 分享文件的列表 文件：名字 大小 分享时间 链接（密码 TODO）

struct STRU_MY_SHARE_RS{
    void init(){
        type=_DEF_PACK_MY_SHARE_RS;
        itemCount=0;

    }
    PackType type;
    int itemCount;
    STRU_MY_SHARE_FILE items[];

};

/////////////////////////////获取分享//////////////////////////////////
//获取分享请求
#define _DEF_PACK_GET_SHARE_RQ       (_DEF_PACK_BASE + 22 )
//获取分享回复
#define _DEF_PACK_GET_SHARE_RS       (_DEF_PACK_BASE + 23 )

//获取分享
struct STRU_GET_SHARE_RQ
{
    STRU_GET_SHARE_RQ():type(_DEF_PACK_GET_SHARE_RQ)
      ,userid(0), shareLink(0){
        memset(dir , 0 , sizeof(dir));
        memset(time , 0 , sizeof(time));
    }
    PackType type;
    int userid;
    int shareLink; // 9位 首位是1-9 数字
    char dir[_MAX_PATH_SIZE];
    char time[_MAX_SIZE];
    //直接加载这个路径下面
};

//获取分享回复 :收到刷新
struct STRU_GET_SHARE_RS
{
    STRU_GET_SHARE_RS():type(_DEF_PACK_GET_SHARE_RS)
      ,result(0) {
        memset(dir , 0 , sizeof(dir));
    }
    PackType type;
    int result;
    char dir[_MAX_PATH_SIZE];
};

////////////////////文件夹下载/////////////////

//下载文件头请求
#define _DEF_PACK_FOLDER_HEADER_RQ			(_DEF_PACK_BASE + 24 )

////下载文件头回复
//#define _DEF_PACK_FOLDER_HEADER_RS			(_DEF_PACK_BASE + 25 )


//文件头请求
struct STRU_FOLDER_HEADER_RQ
{
    STRU_FOLDER_HEADER_RQ():type(_DEF_PACK_FOLDER_HEADER_RQ)
      ,fileid(0),timestamp(0){
        memset( fileName , 0, sizeof(fileName) );
        memset( dir , 0, sizeof(dir) );
        memset( fileType , 0, sizeof(fileType) );
    }
    PackType type;
    int timestamp;
    int fileid;
    char fileName[_MAX_PATH_SIZE];
    char dir[_MAX_PATH_SIZE];//路径
    char fileType[_MAX_SIZE];//文件类型
};

////文件头回复
//struct STRU_FOLDER_HEADER_RS
//{
//    STRU_FOLDER_HEADER_RS(): type(_DEF_PACK_FOLDER_HEADER_RS)
//      , userid(0), fileid(0),result(1),timestamp(0){

//    }
//    PackType type;
//    int timestamp;
//    int userid;
//    int fileid;
//    int result;

//};

//////////////////删除文件///////////////////
//删除文件请求
#define _DEF_PACK_DELETE_FILE_RQ       (_DEF_PACK_BASE + 25 )
//删除文件回复
#define _DEF_PACK_DELETE_FILE_RS       (_DEF_PACK_BASE + 26 )

//删除文件请求 : 某人 删除某路径下的 某文件 fileid数组
struct STRU_DELETE_FILE_RQ
{
    void init()
    {
        type = _DEF_PACK_DELETE_FILE_RQ;
        userid = 0;
        fileCount = 0;
        memset( dir , 0 , sizeof(dir) );
    }
    PackType type;
    int userid;
    char dir[_MAX_PATH_SIZE];
    int fileCount;
    int fileidArray[];
};

//删除文件回复
struct STRU_DELETE_FILE_RS
{
    STRU_DELETE_FILE_RS():type(_DEF_PACK_DELETE_FILE_RS)
      ,result(1){
        memset( dir , 0 , sizeof(dir) );
    }
    PackType type;
    int result;
    char dir[_MAX_PATH_SIZE];
};
#endif // COMMON_H
