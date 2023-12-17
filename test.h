#ifndef TEST_H
#define TEST_H



#include<QDebug>
#include<QRegularExpression>
class Myclass;

//类
class Myclass{
public:
    Myclass(){

    }
    void fun1(){

        QRegularExpression reg("^1{3}$");
        if(reg.isValid()){
            qDebug()<<"test";
        }
    }

public:
};

#endif // TEST_H
