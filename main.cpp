#include<ckernel.h>
#include <QApplication>
#include "test.h"
int main01(int argc, char *argv[])
{
//    Myclass cla;
//    cla.fun1();
    QApplication a(argc, argv);
    Ckernel* kernle=Ckernel::GetInstance();
    return a.exec();
}
