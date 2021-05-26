#include "login.h"
#include "widget.h"
#include "mainwindow.h"
#include "register.h"
#include "ui_login.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
QSqlDatabase db;
QString globalid;
QString globalname; //声明一个全局变量用于窗体间传值
Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowTitle("登录界面");
    setWindowIcon(QIcon(":/images/qq.png"));
    searchresult=false;

    //连接数据库
        db =  QSqlDatabase::addDatabase("QODBC");
        db.setHostName("127.0.0.1");
        db.setPort(3306);
        db.setDatabaseName("2");
        db.setUserName("root");
        db.setPassword("12178319qq");
        bool ok = db.open();
        if (ok){
            QMessageBox::information(this, "消息", "数据库连接成功！");
        }
        else {
            QMessageBox::information(this, "消息", "数据库连接失败！");
            qDebug()<<"error open database because"<<db.lastError().text();
        }
        /*
        //查询数据库中所有表的名称
        QStringList tables = db.tables();
        foreach(QString table, tables)
            qDebug()<<table;


        //ODBC查询数据
        QSqlQuery result = db.exec(" select * from users");
        while(result.next()){
            qDebug()<<"查询成功";
            qDebug()<<"user_id:"<<result.value("user_id").toInt();
            qDebug()<<"name:"<<result.value("name").toString();
            qDebug()<<"password:"<<result.value("password").toString();
            qDebug()<<"score:"<<result.value("score").toInt()<<endl;
        }
        //ODBC插入数据
        result.clear();
        bool result_successed = result.exec(
                    "insert into users(name,password,score) values ( 'joe2y','123456',0)");

        if(result_successed){
            qDebug()<<"插入成功！";
        }else {
            qDebug()<<"插入失败！";
        }
    //ODBC更新数据
    result.clear();
    bool result_successed2 = result.exec("update users set score=10 where user_id=2");

    if(result_successed2){
        qDebug()<<"更新成功！"<<endl;
    }else{
        qDebug()<<"更新失败！"<<endl;
    }
     */
        connect(ui->register_2,&QPushButton::clicked,[=](){
            Register* r = new Register;
            r->show();
            this->close();

        });
        connect(ui->login,&QPushButton::clicked,[=](){
            if(ui->id->text()==NULL||ui->password->text()==NULL)
            {
                qDebug()<<"账号或密码为空！";
                QMessageBox::information(this,"消息","账号或密码为空！");
            }
            else
            {
                QSqlQuery result = db.exec(" select * from users");
                while(result.next()){
                    qDebug()<<"查询成功";
                    qDebug()<<"users_id:"<<result.value("users_id").toString();
                    qDebug()<<"users_name:"<<result.value("users_name").toString();
                    qDebug()<<"users_password:"<<result.value("users_password").toString();

                    //数据正确，将查询到的id和name赋值给两个全局变量，退出查询
                    if(ui->id->text()==result.value("users_id").toString()&&
                            ui->password->text()==result.value("users_password").toString())
                    {
                       globalid=result.value("users_id").toString();
                       globalname=result.value("users_name").toString();

                       searchresult=true;
                        break;
                    }

                }
                if(searchresult)
                {
                    qDebug()<<"账号密码正确！";

                    MainWindow* m=new MainWindow;

                    m->show();

                    //关闭数据库连接
                    //QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
                    this->close();

                }
                else
                {
                    qDebug()<<"账号或密码错误！";
                    QMessageBox::information(this,"消息","账号或密码错误！");
                }

            }
        });



}

Login::~Login()
{
    delete ui;
}
