#include "dialoglist.h"
#include "ui_dialoglist.h"
#include <QMessageBox>
#include <QDateTime>
#include <QColorDialog>
#include <QFileDialog>
extern QString globalid;
extern QString globalname;
//#pragma execution_character_set("utf-8")
DialogList::DialogList(QWidget *parent,QString usrname)
    : QWidget(parent)
    , ui(new Ui::DialogList)
//DialogList::DialogList(QWidget *parent ,QString usrname)
//    :QWidget(parent),
//    ui(new Ui::DialogList)
{
    ui->setupUi(this);
    //setWindowIcon(QIcon(":/images/qq.png"));
    usrname=globalname;
    this->uName = usrname; //获取用户名
    udpSocket = new QUdpSocket(this);
    port = 23333;
    udpSocket->bind(port,QUdpSocket::ShareAddress |QUdpSocket::ReuseAddressHint);  //采用ShareAddress模式(即允许其它的服务连接到相同的地址和端口，特别是用在多客户端监听同一个服务器端口等时特别有效)，和ReuseAddressHint模式(重新连接服务器)
    connect(udpSocket,&QUdpSocket::readyRead,this,&DialogList::ReceiveMessage); //监听信号
    sndMsg(UsrEnter);//有新用户加入

    connect(ui->sendBtn,&QPushButton::clicked,this,[=](){
        sndMsg(Msg);
    });

    connect(ui->exitBtn,&QPushButton::clicked,this,[=]()
    {
        this->close();
    });

    //字体设置
    connect(ui->fontCbx,&QFontComboBox::currentFontChanged,this,[=](const QFont &f){
           ui->msgTxtEdit->setCurrentFont(f);
           ui->msgTxtEdit->setFocus();
       });

    //字号设置
    void (QComboBox:: * cbxSingal)(const QString &text) = &QComboBox::currentIndexChanged;
    connect(ui->sizeCbx,cbxSingal,this,[=](const QString &text){
        ui->msgTxtEdit->setFontPointSize(text.toDouble());
        ui->msgTxtEdit->setFocus();
    });

    //加粗
    connect(ui->boldTBtn,&QToolButton::clicked,this,[=](bool checked){
        if(checked)
        {
            ui->msgTxtEdit->setFontWeight(QFont::Bold);
        }
        else
        {
            ui->msgTxtEdit->setFontWeight(QFont::Normal);
        }
        ui->msgTxtEdit->setFocus();
    });

    //倾斜
    connect(ui->italicTBtn,&QToolButton::clicked,this,[=](bool checked){

            ui->msgTxtEdit->setFontItalic(checked);

            ui->msgTxtEdit->setFocus();
        });

    //下划线
    connect(ui->underlineTBtn,&QToolButton::clicked,this,[=](bool checked){

          ui->msgTxtEdit->setFontUnderline(checked);

          ui->msgTxtEdit->setFocus();
      });

    //设置文本颜色
    connect(ui->colorTBtn,&QToolButton::clicked,this,[=](){
          color = QColorDialog::getColor(color,this); //color对象可以在widget.h中定义私有成员
           if(color.isValid())
           {
               ui->msgTxtEdit->setTextColor(color);
               ui->msgTxtEdit->setFocus();
           }
       });

    //保存聊天记录
    connect(ui->saveTBtn,&QToolButton::clicked,this,[=](){
                if(ui->msgBrowser->document()->isEmpty())
                {
                    QMessageBox::warning(this,"警告","聊天记录为空，无法保存！",QMessageBox::Ok);
                }
                else
                {
                    QString fName = QFileDialog::getSaveFileName(this,"保存聊天记录","聊天记录","(*.txt)");
                    if(!fName.isEmpty())
                    {
                        //保存名称不为空再做保存操作
                        QFile file(fName);
                        file.open(QIODevice::WriteOnly | QFile::Text);

                        QTextStream stream(&file);
                        stream << ui->msgBrowser->toPlainText();
                        file.close();
                    }
                }
            });

    //清空聊天记录
    connect(ui->clearTBtn,&QToolButton::clicked,[=](){
           ui->msgBrowser->clear();
   });
}

DialogList::~DialogList()
{
    delete ui;
}

void DialogList::closeEvent(QCloseEvent *e)
{
    emit this->widgetClose();
    sndMsg(UsrLeft);
    udpSocket->close();
    udpSocket->destroyed();
    //DialogList::closeEvent(e);
}



void DialogList::sndMsg(MsgType type)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out << type << getUsr();   //将消息类型 和 用户名 放入到流中
    switch (type) {
    case Msg:
        if(ui->msgTxtEdit->toPlainText() == "")
        {
            QMessageBox::warning(0,"警告","发送内容不能为空",QMessageBox::Ok);
            return;
        }
        out  <<getMsg();  //发送的是聊天信息    发送格式   消息类型 + 用户名   + 发送内容
        break;
    case UsrEnter:        //发送的是新用户进入  发送格式   消息类型 + 用户名
        break;
    case UsrLeft:         // 发送的是用户离开  发送格式    消息类型 + 用户名
        break;
    default:
        break;
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast,port);
}

void DialogList::ReceiveMessage()
{

    QByteArray datagram;
    datagram.resize(udpSocket->pendingDatagramSize());
    udpSocket->readDatagram(datagram.data(),datagram.size());
    QDataStream in (&datagram,QIODevice::ReadOnly);
    int msgType;
    in >> msgType; //用户类型获取

    QString usrName,msg;  //用户名、信息
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    switch (msgType){
    case Msg:
        in >> usrName  >>msg;
        ui->msgBrowser->setTextColor(Qt::blue);
        ui->msgBrowser->setCurrentFont(QFont("Times New Roman",12));
        ui->msgBrowser->append("[ " + usrName + " ]" + time);
        ui->msgBrowser->append(msg);
        break;
    case UsrEnter:
        in >> usrName ;
        usrEnter(usrName);
        break;
    case UsrLeft:
        in >> usrName;
        usrLeft(usrName,time);
        break;
    default:
        break;
    }
}

//获取用户名
QString DialogList::getUsr()
{
    return uName;
}

//获取聊天信息
QString DialogList::getMsg()
{

    QString msg = ui->msgTxtEdit->toHtml();
    ui->msgTxtEdit->clear();
    ui->msgTxtEdit->setFocus();
    return msg;
}

//处理新用户加入
void  DialogList::usrEnter(QString usrname)
{
    bool isEmpty = ui->usrTblWidget->findItems(usrname,Qt::MatchExactly).isEmpty();
    if(isEmpty)
    {
        QTableWidgetItem *usr = new QTableWidgetItem(usrname);

        ui->usrTblWidget->insertRow(0);
        ui->usrTblWidget->setItem(0,0,usr);
        ui->msgBrowser->setTextColor(Qt::gray);
        ui->msgBrowser->setCurrentFont(QFont("Times New Roman",10));
        ui->msgBrowser->append(tr("%1 在线！").arg(usrname));
        ui->usrNumLbl->setText(tr("在线人数：%1").arg(ui->usrTblWidget->rowCount()));

        //已经在线的各个端点也要告知新加入的端点他们自己的信息，若不这样做，在新端点用户列表中就无法显示其他已经在线的用户
        sndMsg(UsrEnter);
    }
}

//用户离开
void  DialogList::usrLeft(QString usrname,QString time)
{
    int rowNum = ui->usrTblWidget->findItems(usrname, Qt::MatchExactly).first()->row();
    ui->usrTblWidget->removeRow(rowNum);
    ui->msgBrowser->setTextColor(Qt::gray);
    ui->msgBrowser->setCurrentFont(QFont("Times New Roman", 10));
    ui->msgBrowser->append(QString("%1 于 %2 离开！").arg(usrname).arg(time));
    ui->usrNumLbl->setText(QString("在线人数：%1").arg(ui->usrTblWidget->rowCount()));
}

