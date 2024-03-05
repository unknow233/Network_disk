#ifndef MYTABLEWIDGETITEM_H
#define MYTABLEWIDGETITEM_H

#include <QTableWidgetItem>
#include"common.h"
class MainDialog;
class MyTableWidgetItem : public QTableWidgetItem//public QObject,
{
   // Q_OBJECT 类中不用信号与槽机制，不加
public:
    MyTableWidgetItem();

public slots:
    void slot_setInfo(FileInfo&info);//传引用不用拷贝生成副本
private:
    FileInfo m_info;
    friend class MainDialog;

};

#endif // MYTABLEWIDGETITEM_H
