#include<ckernel.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Ckernel* kernle=Ckernel::GetInstance();
    return a.exec();
}
