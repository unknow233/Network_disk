#ifndef CKERNEL_H
#define CKERNEL_H
#include"maindialog.h"
#include<QDebug>
#include <QObject>
//单例模式
class Ckernel : public QObject
{
    Q_OBJECT
public:
   static Ckernel* GetInstance(){
           static Ckernel m_kernel;
        return &m_kernel;
    }

private:
    explicit Ckernel(QObject *parent = nullptr);
    ~Ckernel();
   void loadIniFile();
signals:

private:

    //所有操作都由kernel完成, maindialog作为kernel的成员
    MainDialog* m_maindialog;
    QString m_ip;
    QString m_port;
private slots:
    void slot_close();

};

#endif // CKERNEL_H
