#include<QJsonObject>
#include<QJsonDocument>
#include<QDebug>
using namespace std;
int main(){

        QJsonObject jsonObject;
        QJsonValue jsonValue = jsonObject.value("type");
        if (!jsonValue.isNull())
        {
            QString type = jsonValue.toString();
            // ...
        }
    QJsonDocument jsonDocument(jsonObject);

}
