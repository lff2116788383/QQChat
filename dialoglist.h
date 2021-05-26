#ifndef DIALOGLIST_H
#define DIALOGLIST_H

#include <QWidget>
#include <QUdpSocket>
#include <QColorDialog>
namespace Ui {
class DialogList;
}

enum MsgType {Msg,UsrEnter,UsrLeft};

class DialogList : public QWidget
{
    Q_OBJECT

public:
    explicit DialogList(QWidget *parent ,QString usrname);
    ~DialogList();
signals:
    void widgetClose();
protected:
   void closeEvent(QCloseEvent *);

public:
   void sndMsg(MsgType type); //广播UDP消息
   void usrEnter(QString username);//处理新用户加入
   void usrLeft(QString usrname,QString time); //处理用户离开
   QString getUsr(); //获取用户名
   QString getMsg(); //获取聊天信息
private:
   QUdpSocket * udpSocket; //udp套接字
   qint16 port; //端口
   QString uName; //用户名
   QColor color;
  void ReceiveMessage();   //接受UDP消息

private:
    Ui::DialogList *ui;
};

#endif // DIALOGLIST_H
