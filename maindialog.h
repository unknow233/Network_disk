#ifndef MAINDIALOG_H
#define MAINDIALOG_H
#include<common.h>
#include <QDialog>
#include<QMenu>
QT_BEGIN_NAMESPACE
namespace Ui { class MainDialog; }
QT_END_NAMESPACE

class MainDialog : public QDialog
{
    Q_OBJECT
public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();

    void closeEvent(QCloseEvent *) override;
    void slot_insertUploadFile(FileInfo &info);
    void slot_deleteAllFileInfo();
    void slot_insertFileInfo(FileInfo &info);
    QString GetCurDir();
    void slot_insertDownloadFile(FileInfo &info);
    void slot_deleteAllShareInfo();
    void slot_insertShareFileInfo(QString name, int size, QString time, int shareLink);
    void slot_insertUploadComplete(FileInfo info);
signals:
    //什么路径下创建什么名字的文件夹
    void SIG_addFolder(QString name,QString dir);
    void SIG_mainClose();
    void SIG_UpFile(QString path, QString dir);
    void SIG_uploadFolder(QString path,QString dir);
    void SIG_changeDir(QString dir);
    //什么文件id的文件，什么目录下的文件，下载
    void SIG_downloadFile(int fileid,QString dir);
    //什么文件id的文件，什么目录下的文件夹，下载
    void SIG_downloadFolder(int fileid,QString dir);
    //分享 什么目录下面的文件列表
    void SIG_shareFile(QVector<int>fileidArray,QString dir);
    //获取什么分享码的文件，添加到什么目录
    void SIG_getShareByLink(int code,QString dir);
    //删除什么目录下的一系列文件（文件id，数组）
    void SIG_deleteFile(QVector<int>fileidArray,QString dir);
    //设置上传暂停 0开始 1暂停
    void SIG_setUploadPause(int timestamp,int isPause);
    //设置下载暂停 0开始 1暂停
    void SIG_setDownloadPause(int timestamp,int isPause);
private slots:
    void on_pb_file_clicked();

    void on_pb_share_clicked();

    void on_pb_translate_clicked();

    void on_pb_favorite_clicked();

    void on_pb_create_clicked();

    void solt_PushButtonaddFolder();
    void solt_PushButtonupFolder();
    void solt_PushButtonupFile();

    void slot_updateUploadFileProgress(int timestamp, int pos);

    void on_tb_info_cellDoubleClicked(int row, int column);

    void on_tb_info_cellClicked(int row, int column);

    void on_tb_info_over_customContextMenuRequested(const QPoint &pos);

    void on_tb_info_over_cellDoubleClicked(int row, int column);

    void on_tb_info_customContextMenuRequested(const QPoint &pos);

    void slot_downloadFile(bool flag);
    void slot_shareFile(bool flag);
    void slot_getShare(bool flag);
    void slot_deleteFile(bool flag);
    void on_pb_pre_clicked();

    void slot_uploadPause(bool flag);
    void slot_downloadResume(bool flag);
    void slot_downloadPause(bool flag);
    void slot_uploadResume(bool flag);
    void on_tb_info_loadown_cellDoubleClicked(int row, int column);

    void on_tb_info_loadown_customContextMenuRequested(const QPoint &pos);

    void on_tb_info_up_cellDoubleClicked(int row, int column);

    void slot_updateDownloadFileProgress(int timestamp, int pos);
private:
    Ui::MainDialog *ui;
    QMenu* m_pAddMenu;
    QMenu m_menuFileInfo;
    QMenu m_menudownload;
    QMenu m_menuupload;
    void slot_deleteUploadFileByRow(int row);
    void slot_insertDownloadComplete(FileInfo info);

    void slot_deleteDownloadFileByRow(int row);
};

#endif // MAINDIALOG_H
