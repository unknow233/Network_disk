#include<ckernel.h>
#include <QApplication>
int main(int argc, char *argv[])
{
//    Myclass cla;
//    cla.fun1();
    QApplication a(argc, argv);
    Ckernel* kernle=Ckernel::GetInstance();
    return a.exec();
}
