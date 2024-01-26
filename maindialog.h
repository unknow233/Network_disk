#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include<QMenu>
QT_BEGIN_NAMESPACE
namespace Ui { class MainDialog; }
QT_END_NAMESPACE

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void closeEvent(QCloseEvent *) override;
signals:
    void SIG_mainClose();
    void SIG_UpFile(QString path, QString dir);
private slots:
    void on_pb_file_clicked();

    void on_pb_share_clicked();

    void on_pb_translate_clicked();

    void on_pb_favorite_clicked();

    void on_pb_create_clicked();

    void solt_PushButtonaddFolder();
    void solt_PushButtonupFolder();
    void solt_PushButtonupFile();
private:
    Ui::MainDialog *ui;
    QMenu* m_pAddMenu;
};
#endif // MAINDIALOG_H
