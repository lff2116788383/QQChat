#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialoglist.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
extern QSqlDatabase db;
extern QString globalid;
extern QString globalname; //声明外部变量
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("QQ:%1(%2)").arg(globalname).arg(globalid));
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setWindowIcon(QIcon(":/images/qq.png"));


    ui->name->setText(globalname);
    ui->addbtn->hide();
    ui->searchlabel->hide();


    //搜索按钮事件
    connect(ui->searchbtn,&QToolButton::clicked,this,[=](){
        if(ui->searchEdit->text()==NULL)
        {
            qDebug()<<"搜索框为空!";
        }
        else
        {

             searchresult=false;


            QSqlQuery result = db.exec(" select * from users");
            while(result.next())
            {
                qDebug()<<"查询成功";
                qDebug()<<"user_id:"<<result.value("users_id").toString();
                qDebug()<<"users_name:"<<result.value("users_name").toString();
                qDebug()<<"users_password:"<<result.value("users_password").toString();
               //数据正确，退出查询
                if(ui->searchEdit->text()==result.value("users_id").toString()||ui->searchEdit->text()==result.value("users_name").toString())
                {
                    friend_id = result.value("users_id").toString();
                    friend_name =result.value("users_name").toString();

                    searchresult=true;
                    break;
                }

            }
            if(searchresult)
            {
                qDebug()<<"搜索到了";

                ui->addbtn->show();

                ui->searchlabel->setText(QString("%1(%2)").arg(friend_name).arg(friend_id));
                ui->searchlabel->show();


            }
            else
            {
                qDebug()<<"搜索失败！";
                QMessageBox::information(this,"消息","搜索失败！");
            }
        }

    });

    //添加好友按钮事件
    connect(ui->addbtn,&QPushButton::clicked,[=](){
        if(globalid!=friend_id||globalname!=friend_name)
        {

            //QString friend_id=ui->searchEdit->text();

            //创建QSqlTableModel实例
            QSqlTableModel model;

            //设置所需要操作的表格
            model.setTable("friend");


            //获取一个空的record,但是与数据库表有一样的字段空间？不然插入会失败
            //帮助文档：It returns an empty record, having only the field names.
            QSqlRecord record = model.record();

            //设置各字段键值
            record.setValue("users_id", globalid);
            record.setValue("users_name", globalname);
            record.setValue("friend_id", friend_id);
            record.setValue("friend_name", friend_name);


             isAdd=false;
            QSqlQuery result = db.exec(" select * from friend");
            while(result.next())
            {
                qDebug()<<"查询成功";
                qDebug()<<"user_id:"<<result.value("users_id").toString();
                qDebug()<<"friend_id:"<<result.value("friend_id").toString();
                qDebug()<<"friend_name:"<<result.value("friend_name").toString();

                if(globalid==result.value("users_id").toString()

                        &&friend_id==result.value("friend_id").toString()
                        &&friend_name==result.value("friend_name").toString())
                {
                    isAdd=true;
                    qDebug()<<"好友已添加！";
                    QMessageBox::information(this,"消息","好友已添加！");

                    break;

                }

            }
            if(!isAdd)
            {

                //插入，-1代表插入到行尾
                bool result_successed = model.insertRecord(-1, record);

                if(result_successed){
                    qDebug()<<"添加好友成功！";
                    QMessageBox::information(this,"消息","添加好友成功！");
                    addFriendlist(friend_name,friend_id);
                }else {
                    qDebug()<<"添加好友失败！";
                }
                ui->addbtn->hide();
                ui->searchlabel->hide();
            }
            else
            {
                //QMessageBox::information(this,"消息","好友已添加！");
            }
        }
        else
        {
            QMessageBox::information(this,"消息","无法添加！");
        }
    });

    showFriendlist();



}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::showFriendlist()
{

    //创建好友列表
    QList<QString>nameList;
    QList<QString>idList;
    int friend_num=0;
//    nameList << "水票奇缘" << "忆梦如澜" <<"北京出版人"<<"Cherry"<<"淡然"
//             <<"娇娇girl"<<"落水无痕"<<"青墨暖暖"<<"无语";


    QStringList iconNameList; //图标资源列表
    iconNameList << "spqy"<< "ymrl" <<"qq" <<"Cherry"<< "dr"
                 <<"jj"<<"lswh"<<"qmnn"<<"wy";
    QSqlQuery result = db.exec(" select * from friend");

    while(result.next())
    {
        qDebug()<<"查询成功";
        qDebug()<<"user_id:"<<result.value("users_id").toString();
        qDebug()<<"friend_id:"<<result.value("friend_id").toString();
        qDebug()<<"friend_name:"<<result.value("friend_name").toString();
        if(globalid==result.value("users_id").toString())
        {
            nameList.push_back(result.value("friend_name").toString());
            idList.push_back(result.value("friend_id").toString());
            friend_num++;
        }
    }

    QVector<QToolButton *>vToolBtn; //声明存放QtoolButton的容器 vToolBtn
    for(int i = 0 ;i <  friend_num ; i ++)
    {
        isShow.push_back(false);
        QToolButton * btn = new QToolButton;  //创建新按钮
        btn->setText(QString("%1(%2)").arg(nameList[i]).arg(idList[i]));    //设置按钮名称
        //QString str = QString(":/images/%1.png").arg(iconNameList.at(i));
        QString str = QString(":/images/qq.png");

        btn->setIcon(QPixmap(str));   //设置图片
        btn->setIconSize(QPixmap(str).size()); //设置图片大小
        btn->setAutoRaise(true);  //设置图片透明效果
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);//设置按钮风格，同时显示文字和图标

        ui->vLayout->addWidget(btn); //将按钮添加到布局中

        vToolBtn.push_back(btn); //将9个按钮放入到布局中
    }

    //添加点击事件
    for(int i = 0 ; i < vToolBtn.size();i++)
    {
        connect(vToolBtn[i],&QToolButton::clicked,this,[=]()
        {
            if(isShow[i])
            {
                QMessageBox::information(this,"警告",QString("用户%1窗口已弹出").arg(vToolBtn[i]->text()));
                return;
            }
            isShow[i] = true;
            //qDebug() <<i;
            //此时，widget的构造函数已经修改，（见4.2步骤）创建widget窗口时，参数1 ：0代表以顶层方式弹出
            //参数2：代表vToolBtn[i]->text()代表告诉聊天窗口 人物的姓名
            DialogList*dialog = new DialogList(0,vToolBtn[i]->text());
            dialog->setWindowTitle(vToolBtn[i]->text());
            dialog ->setWindowIcon(vToolBtn[i]->icon());
            dialog ->show();

            connect(dialog,&DialogList::widgetClose,this,[=](){
                isShow[i] = false;
            });

        });
    }

}
void MainWindow::addFriendlist(QString name,QString id)
{

    QStringList iconNameList; //图标资源列表
    iconNameList << "spqy"<< "ymrl" <<"qq" <<"Cherry"<< "dr"
                 <<"jj"<<"lswh"<<"qmnn"<<"wy";

    QToolButton * btn = new QToolButton;  //创建新按钮
    btn->setText(QString("%1(%2)").arg(name).arg(id));     //设置按钮名称
    //QString str = QString(":/images/%1.png").arg(iconNameList.at(3));
    QString str = QString(":/images/qq.png");
    btn->setIcon(QPixmap(str));   //设置图片
    btn->setIconSize(QPixmap(str).size()); //设置图片大小
    btn->setAutoRaise(true);  //设置图片透明效果
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);//设置按钮风格，同时显示文字和图标

    ui->vLayout->addWidget(btn); //将按钮添加到布局中
    isshow = true;
    connect(btn,&QToolButton::clicked,this,[=](){
        DialogList*dialog = new DialogList(0,btn->text());
        dialog->setWindowTitle(btn->text());
        dialog ->setWindowIcon(btn->icon());
        dialog ->show();

        connect(dialog,&DialogList::widgetClose,this,[=](){
           isshow = false;
        });

    });


}

