#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();
    int currentTimestampInt = static_cast<int>(currentTimestamp / 1000); // 将毫秒转换为秒
     qDebug() << "currentMSecsSinceEpoch:" << currentTimestamp;
    qDebug() << "Current timestamp in seconds since epoch:" << currentTimestampInt;

    return a.exec();
}
