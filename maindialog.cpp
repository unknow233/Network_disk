#include<QMessageBox>
#include<QCloseEvent>
#include<QDebug>
#include<QFileDialog>
#include"mytablewidgetitem.h"
#include "maindialog.h"
#include "ui_maindialog.h"
MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    //为其设置默认的窗口
    ui->sw_page->setCurrentIndex(0);
    ui->tw_translate->setCurrentIndex(1);
    ui->tb_info->setContextMenuPolicy(Qt::CustomContextMenu);
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
            this,SLOT(solt_PushButtonaddFolder()));

            connect(upFile,SIGNAL(triggered(bool)),
                    this,SLOT(solt_PushButtonupFile()));
            connect(upFolder,SIGNAL(triggered(bool)),
                    this,SLOT(solt_PushButtonupFolder()));

            QAction* action_downloadFile=new QAction("下载文件");
            QAction* action_sharedFile=new QAction("分享文件");
            QAction* action_deleteFile=new QAction("删除文件");
            QAction* action_getShare=new QAction("获取分享");//获取分享到本地目录
            QAction* action_honar=new QAction("收藏");

            m_menuFileInfo.addAction(addFolder);
            m_menuFileInfo.addSeparator();//加入分隔符
            m_menuFileInfo.addAction(action_downloadFile);
            m_menuFileInfo.addAction(action_sharedFile);
            m_menuFileInfo.addAction(action_deleteFile);
            m_menuFileInfo.addSeparator();//加入分隔符
            m_menuFileInfo.addAction(action_honar);
            m_menuFileInfo.addSeparator();//加入分隔符
            m_menuFileInfo.addAction(action_getShare);

            connect(action_downloadFile,SIGNAL(triggered(bool))
                    ,this,SLOT(slot_downloadFile(bool)));
            connect(action_sharedFile,SIGNAL(triggered(bool))
                    ,this,SLOT(slot_shareFile(bool)));
            connect(action_deleteFile,SIGNAL(triggered(bool))
                    ,this,SLOT(slot_deleteFile(bool)));
            connect(action_getShare,SIGNAL(triggered(bool))
                    ,this,SLOT(slot_getShare(bool)));

            //ui->table_downloa

            //添加右键显示菜单 lambda表达式 匿名函数
            //[]捕捉列表 ()函数参数列表 { }函数体
            //connect(action_getShare,SIGNAL(triggered(bool)),this,SLOT(slot_getShare(bool)));
            connect(ui->tb_info_loadown,&QTableWidget::customContextMenuRequested,this,
            [this](QPoint){this->m_menudownload.exec(QCursor::pos());
            } );
            connect(ui->tb_info_up,&QTableWidget::customContextMenuRequested,this,
            [this](QPoint){this->m_menuupload.exec(QCursor::pos());
            } );

            //添加菜单项
            QAction * actionUploadPause=new QAction("暂停");
            QAction * actionUploadResume=new QAction("恢复");
            QAction * actionDownloadPause=new QAction("暂停");
            QAction * actionDownloadResume=new QAction("开始");
            m_menuupload.addAction(actionUploadPause);
            m_menuupload.addAction(actionUploadResume);
            m_menuupload.addAction("全部暂停");
            m_menuupload.addAction("全部开始");

            m_menudownload.addAction(actionDownloadPause);
            m_menudownload.addAction(actionDownloadResume);
            m_menudownload.addAction("全部暂停");
            m_menudownload.addAction("全部开始");
            connect(actionUploadPause,SIGNAL(triggered(bool))
                    ,this,SLOT(slot_uploadPause(bool)));
            connect(actionUploadResume,SIGNAL(triggered(bool))
                    ,this,SLOT(slot_uploadResume(bool)));
            connect(actionDownloadPause,SIGNAL(triggered(bool))
                    ,this,SLOT(slot_downloadPause(bool)));
            connect(actionDownloadResume,SIGNAL(triggered(bool))
                    ,this,SLOT(slot_downloadResume(bool)));

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
#include<QInputDialog>
void MainDialog::solt_PushButtonaddFolder()
{
    qDebug()<<__func__;
    //弹出输入窗口
    QString name=QInputDialog::getText(this,"新建文件夹","输入名称");

    QString tmp=name;

    if(name.isEmpty()||tmp.remove(" ").isEmpty()||name.length()>100){
        QMessageBox::about(this,"提示","名字非法");
        return;
    }
    //排除不文明的名字

    //数据库，配置文件，文档导入不文明库
    //使用正则表达式匹配

    //特殊字符屏蔽: \ / : ? * < > | "
    if(name.contains("\\")||name.contains("/")||
            name.contains("<")||name.contains(">")||
            name.contains(":")||name.contains("|")||
            name.contains("*")||name.contains("?")
            ||name.contains("\"")){
        QMessageBox::about(this,"提示","名字非法");
        return;
    }
    //TODO:判断是否已经存在

    QString dir=ui->lb_curPath->text();
    Q_EMIT SIG_addFolder(name,dir);
}

void MainDialog::solt_PushButtonupFolder()
{
    qDebug()<<__func__;

    //点击 弹出文件选择对话框 选择路径
    QString path=QFileDialog::getExistingDirectory(this,"选择文件夹","./");
    //判断非空
    if(path.isEmpty()){
        return;
    }
    //TODO 过滤，是否正在传

    //发信号 上传什么目录下面什么名字的
    //上传什么路径的文件夹，到什么目录下面
    Q_EMIT SIG_uploadFolder(path,ui->lb_curPath->text());
}


void MainDialog::solt_PushButtonupFile()
{
    qDebug()<<"MainDialog::"<<__func__;
    QString path=QFileDialog::getOpenFileName(this,"选择文件","D:\\");
    QString dir=ui->lb_curPath->text();
    SIG_UpFile( path,  dir);//path为本地路径, dir为网盘路径
}

#include<QProgressBar>
void MainDialog::slot_insertUploadFile(FileInfo &info)
{
    //表格插入信息
    qDebug()<<__func__;
    //列：文件名  大小  时间  速率  进度  按钮
    //1.新增一行 获取当前行+1 设置行数

//    int rows=ui->tb_info_up->rowCount()+1;
//    ui->tb_info_up->setRowCount(rows);//改错：获取当前行号后从0开始，所以变量row应该是你设置的行号-1
    int rows=ui->tb_info_up->rowCount();
    ui->tb_info_up->setRowCount(rows+1);
    //2.设置每一行的控件（添加对象）
    MyTableWidgetItem * item0=new MyTableWidgetItem;
    item0->slot_setInfo(info);
    ui->tb_info_up->setItem(rows,0,item0);


    QTableWidgetItem * item1=new QTableWidgetItem(FileInfo::getsize(info.size));
    ui->tb_info_up->setItem(rows,1,item1);

    QTableWidgetItem * item2=new QTableWidgetItem(info.time);
    ui->tb_info_up->setItem(rows,2,item2);
    //大小，传kb，先写字节数
    QTableWidgetItem * item3=new QTableWidgetItem("0KB/s");
    ui->tb_info_up->setItem(rows,3,item3);

    //进度条
    //如果创建局部变量，则函数结束之后变量被回收，所以new创建指针变量，在堆区创建
    QProgressBar *progress=new QProgressBar;
    //为什么不用比例：分母是否为0
    progress->setMaximum(info.size);
    ui->tb_info_up->setCellWidget(rows,4,progress);

    //ui->tb_info_up->setItem(rows,5,item5);
    QPushButton * button=new QPushButton;
    if(info.isPause==0){
        button->setText("暂停");
    }else{
        button->setText("开始");
    }
    ui->tb_info_up->setCellWidget(rows,5,button);
}
void MainDialog::slot_updateUploadFileProgress(int timestamp, int pos)
{
    //遍历所有项 第0列
    qDebug()<<__func__;
    int row=ui->tb_info_up->rowCount();//
    for(int i=0;i<row;++i){
        //取到所有文件信息的时间戳，看是否一致
        MyTableWidgetItem * item0=(MyTableWidgetItem *)ui->tb_info_up->item(i,0);
        if(item0->m_info.timestamp==timestamp){
            //一致，更新进度
            QProgressBar * item4=(QProgressBar *)ui->tb_info_up->cellWidget(i,4);
            item0->m_info.pos=pos;
            item4->setValue(pos);
            //看是否结束
            if(item4->value()>=item4->maximum()){
                //是 删除这一项 添加到完成
                slot_deleteUploadFileByRow(i);
                //当函数参数为引用类型时进入函数后,参数不好使了
                slot_insertUploadComplete(item0->m_info);
                return;
            }
        }
    }

}
void MainDialog::slot_insertUploadComplete(FileInfo info)
{
    //列：文件名  大小  时间  上传完成
    //新增一行 获取当前行+1  设置行数
//    int rows=ui->tb_info_over->rowCount()+1;
//    ui->tb_info_up->setRowCount(rows);

    qDebug()<<__func__;
     qDebug()<<"m_info "<<info.size<<" "<<info.name<<" "<<info.time;
    int rows=ui->tb_info_over->rowCount();
    ui->tb_info_over->setRowCount(rows+1);
    //2.设置每一行的控件（添加对象）
    MyTableWidgetItem * item0=new MyTableWidgetItem;
    item0->slot_setInfo(info);
    ui->tb_info_over->setItem(rows,0,item0);


    QTableWidgetItem * item1=new QTableWidgetItem(FileInfo::getsize(info.size));
    ui->tb_info_over->setItem(rows,1,item1);

    QTableWidgetItem * item2=new QTableWidgetItem(info.time);
    ui->tb_info_over->setItem(rows,2,item2);

    QTableWidgetItem * item3=new QTableWidgetItem("上传已完成");
    ui->tb_info_over->setItem(rows,3,item3);

}

void MainDialog::slot_deleteAllFileInfo()
{
    qDebug()<<__func__;
    //ui->tb_info->clear();//删文字 行数不变
    int rows=ui->tb_info->rowCount();
    for(int i=rows-1;i>=0;i--){
        ui->tb_info->removeRow(i);
    }

}
void MainDialog::slot_deleteUploadFileByRow(int row)
{
    qDebug()<<__func__;
    //TODO
    ui->tb_info_up->removeRow(row);
}

void MainDialog::slot_insertDownloadComplete(FileInfo info)
{
    //列：文件名  大小  时间  按钮
    //新增一行 获取当前行+1  设置行数

//    int rows=ui->tb_info_over->rowCount()+1;
//    ui->tb_info_up->setRowCount(rows);

    qDebug()<<__func__;
    int rows=ui->tb_info_over->rowCount();
    ui->tb_info_over->setRowCount(rows+1);
    //2.设置每一行的控件（添加对象）
    MyTableWidgetItem * item0=new MyTableWidgetItem;
    item0->slot_setInfo(info);
    ui->tb_info_over->setItem(rows,0,item0);


    QTableWidgetItem * item1=new QTableWidgetItem(FileInfo::getsize(info.size));
    ui->tb_info_over->setItem(rows,1,item1);

    QTableWidgetItem * item2=new QTableWidgetItem(info.time);
    ui->tb_info_over->setItem(rows,2,item2);

    //QTableWidgetItem * item3=new QTableWidgetItem("下载已完成");
    QPushButton*button=new QPushButton;
    button->setIcon(QIcon(":/images/folder.png"));
    //设置扁平
    button->setFlat(true);
    //拿到路径方式：1.重写拿到路径
    //2.tooltip
    button->setToolTip(info.absolutePath);

    connect(button,SIGNAL(clicked(bool)),this,SLOT(slot_openPath(bool)));

    ui->tb_info_over->setCellWidget(rows,3,button);


}
void MainDialog::slot_insertFileInfo(FileInfo &info)
{
    qDebug()<<__func__<<endl;
    //列：文件名  大小  时间  上传完成、
    //新增一行 获取当前行+1  设置行数
//    int rows=ui->table_complete->rowCount()+1;
//    ui->tb_info_up->setRowCount(rows);
    //qDebug()<<__func__;

    int rows=ui->tb_info->rowCount();
    ui->tb_info->setRowCount(rows+1);

    //2.设置每一行的控件（添加对象）
    MyTableWidgetItem * item0=new MyTableWidgetItem;
    item0->slot_setInfo(info);
    //////////////文件夹不显示大小//////////
    QString strSize;
    if(info.type=="file"){
        strSize=FileInfo::getsize(info.size);
    }else{
        QIcon icon(":/resouce/images/folder.png");
        item0->setIcon(icon);
        strSize=" ";
        qDebug()<<strSize<<endl;
    }

    ui->tb_info->setItem(rows,0,item0);
    QTableWidgetItem * item1=new QTableWidgetItem(FileInfo::getsize(info.size));
                //QString::number(info.size));

    ui->tb_info->setItem(rows,1,item1);

    QTableWidgetItem * item2=new QTableWidgetItem(info.time);
    ui->tb_info->setItem(rows,2,item2);


}

QString MainDialog::GetCurDir()
{
    return ui->lb_curPath->text();
}

void MainDialog::on_tb_info_cellDoubleClicked(int row, int column)
{
    qDebug()<<__func__<<endl;
    //首先 拿到双击那行的文件的名字
    MyTableWidgetItem* item0=(MyTableWidgetItem*)ui->tb_info->item(row,0);
    //判断是不是文件夹  是文件夹，可以跳转（是文件 考虑打开文件TODO）
    if(item0->m_info.type!="file"){
        //是文件夹 路径拼接
        QString dir=ui->lb_curPath->text()+item0->m_info.name+"/";
        //设置路径 lb_curPath->text
        ui->lb_curPath->setText(dir);
        //发送信号-》更新当前的目录-》刷新文件列表
        Q_EMIT SIG_changeDir(dir);

    }
}


void MainDialog::on_tb_info_cellClicked(int row, int column)
{
    qDebug()<<__func__;
    //查看当前状态，切换打勾/未打勾
    MyTableWidgetItem*item0=(MyTableWidgetItem*)ui->tb_info->item(row,0);
    if(item0->checkState()==Qt::Checked){
        item0->setCheckState(Qt::Unchecked);
    }else{
        item0->setCheckState(Qt::Checked);
    }
}


void MainDialog::on_tb_info_over_customContextMenuRequested(const QPoint &pos)
{
    //弹出菜单
    m_menuFileInfo.exec(QCursor::pos());
}


void MainDialog::on_tb_info_over_cellDoubleClicked(int row, int column)
{
    qDebug()<<__func__;
    //首先 拿到双击那行的文件的名字

    MyTableWidgetItem* item0=(MyTableWidgetItem*)ui->tb_info->item(row,0);
    //判断是不是文件夹  是文件夹，可以跳转（是文件 考虑打开文件TODO）
    if(item0->m_info.type!="file"){
        //是文件夹 路径拼接
        QString dir=ui->lb_curPath->text()+item0->m_info.name+"/";
        //设置路径 lb_curPath->text
        ui->lb_curPath->setText(dir);
        //发送信号-》更新当前的目录-》刷新文件列表
        //Q_EMIT SIG_changeDir(dir);

    }
}


void MainDialog::on_tb_info_customContextMenuRequested(const QPoint &pos)
{
    qDebug()<<__func__;    //弹出菜单
    m_menuFileInfo.exec(QCursor::pos());
}
void MainDialog::slot_downloadFile(bool flag)
{
     qDebug()<<__func__;
     //发信号  下载文件    下载文件夹
     //文件夹 /文件的图标
     int Rows=ui->tb_info->rowCount();
     QString dir=ui->lb_curPath->text();//获取目录/id
     for(int i=0;i<Rows;i++){
         //看选中的文件
         MyTableWidgetItem*item0=(MyTableWidgetItem*)ui->tb_info->item(i,0);
         if(item0->checkState()==Qt::Checked){
             //TODO 列表中有这个文件，不能下载，过滤
            //获取类型，文件/文件夹
            if(item0->m_info.type=="file"){//发信号，下载文件/下载文件夹
                 Q_EMIT SIG_downloadFile(item0->m_info.fileid,dir);
            }else{
                Q_EMIT SIG_downloadFolder(item0->m_info.fileid,dir);
            }
         }
     }

}
void MainDialog::slot_insertDownloadFile(FileInfo &info)
{
    //表格插入信息
    //qDebug()<<__func__;
    //列：文件名  大小  时间  速率  进度  按钮
    //1.新增一行 获取当前行+1 设置行数

//    int rows=ui->tb_info_up->rowCount()+1;
//    ui->tb_info_up->setRowCount(rows);//改错：获取当前行号后从0开始，所以变量row应该是你设置的行号-1
    int rows=ui->tb_info_loadown->rowCount();
    ui->tb_info_loadown->setRowCount(rows+1);
    //2.设置每一行的控件（添加对象）
    MyTableWidgetItem * item0=new MyTableWidgetItem;
    item0->slot_setInfo(info);
    ui->tb_info_loadown->setItem(rows,0,item0);


    QTableWidgetItem * item1=new QTableWidgetItem(FileInfo::getsize(info.size));
    ui->tb_info_loadown->setItem(rows,1,item1);

    QTableWidgetItem * item2=new QTableWidgetItem(info.time);
    ui->tb_info_loadown->setItem(rows,2,item2);
    //大小，传kb，先写字节数
    QTableWidgetItem * item3=new QTableWidgetItem("0KB/s");
    ui->tb_info_loadown->setItem(rows,3,item3);

    //进度条
    //如果创建局部变量，则函数结束之后变量被回收，所以new创建指针变量，在堆区创建
    QProgressBar *progress=new QProgressBar;
    //为什么不用比例：分母是否为0
    progress->setMaximum(info.size);
    ui->tb_info_loadown->setCellWidget(rows,4,progress);

    //ui->tb_info_up->setItem(rows,5,item5);
    QPushButton * button=new QPushButton;
    if(info.isPause==0){
        button->setText("暂停");
    }else{
        button->setText("开始");
    }
    ui->tb_info_loadown->setCellWidget(rows,5,button);
}
void MainDialog::slot_shareFile(bool flag)
{
     qDebug()<<__func__;
     //申请数组
     QVector<int> array;
     int count=ui->tb_info->rowCount();
     //遍历所有项
     for(int i=0;i<count;i++){
         MyTableWidgetItem*item0=(MyTableWidgetItem*)ui->tb_info->item(i,0);
        //看是否选中
         if(item0->checkState()==Qt::Checked){
            //添加到数组里面
             array.push_back(item0->m_info.fileid);
        //发送信号
        }
     }
     Q_EMIT SIG_shareFile(array,ui->lb_curPath->text());
}
void MainDialog::slot_deleteAllShareInfo()
{
    qDebug()<<__func__;
    //ui->table_file->clear();//删文字 行数不变
    int rows=ui->tb_info_shared->rowCount();
    for(int i=rows-1;i>=0;i--){
        ui->tb_info_shared->removeRow(i);
    }
    qDebug()<<"删除完毕?";
}
void MainDialog::slot_insertShareFileInfo(QString name, int size, QString time, int shareLink)
{
    //列：文件名  大小  时间  速率  进度  按钮
    //1.新增一行 获取当前行+1 设置行数

    int rows=ui->tb_info_shared->rowCount();
    ui->tb_info_shared->setRowCount(rows+1);
    //2.设置每一行的控件（添加对象）
    QTableWidgetItem * item0=new QTableWidgetItem(name);
    ui->tb_info_shared->setItem(rows,0,item0);

    QTableWidgetItem * item1=new QTableWidgetItem(FileInfo::getsize(size));
    ui->tb_info_shared->setItem(rows,1,item1);

    QTableWidgetItem * item2=new QTableWidgetItem(time);
    ui->tb_info_shared->setItem(rows,2,item2);
    //大小，传kb，先写字节数
    QTableWidgetItem * item3=new QTableWidgetItem(QString::number(shareLink));
    ui->tb_info_shared->setItem(rows,3,item3);



}
void MainDialog::slot_getShare(bool flag)
{
    qDebug()<<__func__;
    //弹窗 输入分享码
    QString txt=QInputDialog::getText(this,"获取分享","输入分享码");
    //过滤
    int code=txt.toInt();
    if(txt.length()!=9||code<100000000||code>999999999){
        QMessageBox::about(this,"提示","分享码非法");
        return;
    }
    //发送信号 什么目录下面 添加什么分享码的文件
    Q_EMIT SIG_getShareByLink(code,ui->lb_curPath->text());

}
void MainDialog::slot_deleteFile(bool flag)
{
    qDebug()<<__func__;
    //看被选中的项
    QVector<int> array;
    int count=ui->tb_info->rowCount();
    //遍历所有项
    for(int i=0;i<count;i++){
        MyTableWidgetItem*item0=(MyTableWidgetItem*)ui->tb_info->item(i,0);
       //看是否选中
        if(item0->checkState()==Qt::Checked){
           //添加到数组里面
            array.push_back(item0->m_info.fileid);
       //发送信号
       }
    }
    //添加到数组

    //信号
    Q_EMIT SIG_deleteFile(array,ui->lb_curPath->text());
}

void MainDialog::on_pb_pre_clicked()
{
    //获取目录
    QString path=ui->lb_curPath->text();
    //判断 "/"结束
    if(path=="/")return;

    //首先找到最右边的"/",从他左边开始，向右寻找，找"/"
    //  /03/  ->  /03
    //left：取多少个长度
    path=path.left(path.lastIndexOf("/"));//    /03

    //新的目录就是找到的"/"以及左边的所有字符
    path=path.left(path.lastIndexOf("/")+1);//    /
    qDebug()<<path;
    ui->lb_curPath->setText(path);
    //跳转路径
    Q_EMIT SIG_changeDir(path);
}

void MainDialog::slot_uploadPause(bool flag)
{
    qDebug()<<__func__;
    int Rows=ui->tb_info_up->rowCount();
    //;遍历表单
    //遍历所有项
    for(int i=0;i<Rows;i++){
        MyTableWidgetItem*item0=(MyTableWidgetItem*)ui->tb_info_up->item(i,0);
       //看是否选中
        if(item0->checkState()==Qt::Checked){
           QPushButton*button=(QPushButton*)ui->tb_info_up->cellWidget(i,5);
            if(button->text()=="暂停"){
                //发送信号，设置文件信息结构体暂停标志位
                button->setText("开始");
                Q_EMIT SIG_setUploadPause(item0->m_info.timestamp,1);
            }

       }
    }
}
void MainDialog::slot_uploadResume(bool flag)
{
    qDebug()<<__func__;
    int Rows=ui->tb_info_up->rowCount();
    //;遍历表单
    //遍历所有项
    for(int i=0;i<Rows;i++){
        MyTableWidgetItem*item0=(MyTableWidgetItem*)ui->tb_info_up->item(i,0);
       //看是否选中
        if(item0->checkState()==Qt::Checked){
           QPushButton*button=(QPushButton*)ui->tb_info_up->cellWidget(i,5);
            if(button->text()=="开始"){
                //发送信号，设置文件信息结构体暂停标志位
                button->setText("暂停");
            }
              Q_EMIT SIG_setUploadPause(item0->m_info.timestamp,0);
       }
    }
}
void MainDialog::slot_downloadPause(bool flag)
{
    qDebug()<<__func__;
    int Rows=ui->tb_info_loadown->rowCount();
    //;遍历表单
    //遍历所有项
    for(int i=0;i<Rows;i++){
        MyTableWidgetItem*item0=(MyTableWidgetItem*)ui->tb_info_loadown->item(i,0);
       //看是否选中
        if(item0->checkState()==Qt::Checked){
           QPushButton*button=(QPushButton*)ui->tb_info_loadown->cellWidget(i,5);
            if(button->text()=="暂停"){
                //发送信号，设置文件信息结构体暂停标志位
                button->setText("开始");
                Q_EMIT SIG_setDownloadPause(item0->m_info.timestamp,1);
            }

       }
    }
}



void MainDialog::slot_downloadResume(bool flag)
{
    qDebug()<<__func__;
    int Rows=ui->tb_info_loadown->rowCount();
    //;遍历表单
    //遍历所有项
    for(int i=0;i<Rows;i++){
        MyTableWidgetItem*item0=(MyTableWidgetItem*)ui->tb_info_loadown->item(i,0);
       //看是否选中
        if(item0->checkState()==Qt::Checked){
           QPushButton*button=(QPushButton*)ui->tb_info_loadown->cellWidget(i,5);
            if(button->text()=="开始"){
                //发送信号，设置文件信息结构体暂停标志位
                button->setText("暂停");
                Q_EMIT SIG_setDownloadPause(item0->m_info.timestamp,0);
            }

       }
    }
}

void MainDialog::on_tb_info_loadown_cellDoubleClicked(int row, int column)
{
        //查看当前状态，切换打勾/未打勾
        MyTableWidgetItem*item0=(MyTableWidgetItem*)ui->tb_info_loadown->item(row,0);
        if(item0->checkState()==Qt::Checked){
            item0->setCheckState(Qt::Unchecked);
        }else{
            item0->setCheckState(Qt::Checked);
        }
}


void MainDialog::on_tb_info_loadown_customContextMenuRequested(const QPoint &pos)
{
    qDebug()<<__func__;    //弹出菜单
    m_menudownload.exec(QCursor::pos());
}



void MainDialog::on_tb_info_up_cellDoubleClicked(int row, int column)
{
    qDebug()<<__func__;    //弹出菜单
    m_menuupload.exec(QCursor::pos());
}

void MainDialog::slot_updateDownloadFileProgress(int timestamp, int pos)
{
    //遍历所有项 第0列
    qDebug()<<__func__;
    int row=ui->tb_info_loadown->rowCount();//
    for(int i=0;i<row;++i){
        //取到所有文件信息的时间戳，看是否一致
        MyTableWidgetItem * item0=(MyTableWidgetItem *)ui->tb_info_loadown->item(i,0);
        if(item0->m_info.timestamp==timestamp){
            //一致，更新进度
            QProgressBar * item4=(QProgressBar *)ui->tb_info_loadown->cellWidget(i,4);
            item0->m_info.pos=pos;
            item4->setValue(pos);
            //看是否结束
            if(item4->value()>=item4->maximum()){
                //是 删除这一项 添加到完成
                slot_deleteDownloadFileByRow(i);
                slot_insertDownloadComplete(item0->m_info);
                return;
            }
        }
    }
}
void MainDialog::slot_deleteDownloadFileByRow(int row)
{
    qDebug()<<__func__;
    ui->tb_info_loadown->removeRow(row);

}
