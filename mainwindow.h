#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool searchresult;
    QString friend_id;
    QString friend_name;
    QString users_id;
    bool isAdd;
    QVector<bool>isShow;// 代表是否打开窗口的标识，false未打开，true打开
    bool isshow;


    void showFriendlist();
    void addFriendlist(QString,QString);

private slots:
    //void receivedata(QString data);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
