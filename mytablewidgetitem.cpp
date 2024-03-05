#include "mytablewidgetitem.h"

MyTableWidgetItem::MyTableWidgetItem()
{

}

void MyTableWidgetItem::slot_setInfo(FileInfo &info)
{
    //作为第一列出现 文件名
    m_info=info;

    this->setText(info.name);

    //设置图标
    if(info.type=="file"){
        this->setIcon(QIcon(":/images/file.png"));
    }else{//文件夹
        this->setIcon(QIcon(":/images/folder.png"));
    }

    //勾选    未打勾
    this->setCheckState(Qt::Unchecked);
}
