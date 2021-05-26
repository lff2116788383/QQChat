#include "register.h"
#include "login.h"
#include "ui_register.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
extern QSqlDatabase db;
Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
     setWindowTitle("注册界面");
     setWindowIcon(QIcon(":/images/qq.png"));

    connect(ui->backbtn,&QPushButton::clicked,[=](){
         //关闭数据库连接
         QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
         Login* l = new Login;
         l->show();

         this->close();
     });
     connect(ui->okbtn,&QPushButton::clicked,[=](){
         QString users_id = ui->users_id->text();
         QString users_password1 = ui->users_password1->text();
         QString users_name = ui->users_name->text();
         QString users_password2 = ui->users_password2->text();
         if(ui->users_name==NULL||ui->users_id==NULL||users_password1==NULL)
         {
             qDebug()<<"输入的昵称，账号或密码为空！";
         }
         else
         {
             if(users_password2==NULL||users_password1!=users_password2)
             {
                 qDebug()<<"请再次确认密码！";
             }
             else
             {
                 isRegister=false;
                 QSqlQuery result = db.exec(" select * from users");
                 while(result.next()){
                     qDebug()<<"查询成功";
                     qDebug()<<"users_id:"<<result.value("users_id").toString();
                     qDebug()<<"users_name:"<<result.value("users_name").toString();
                     qDebug()<<"users_password:"<<result.value("users_password").toString();
                     if(users_id==result.value("users_id").toString())
                     {
                         isRegister=true;
                         qDebug()<<"此账号已注册！";
                         QMessageBox::information(this,"消息","此账号已注册！");
                         break;
                     }
                 }
                 if(!isRegister)
                 {
                 //创建QSqlTableModel实例
                     QSqlTableModel model;

                     //设置所需要操作的表格
                     model.setTable("users");

                     //获取系统时间
                     //QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

                     //获取一个空的record,但是与数据库表有一样的字段空间？不然插入会失败
                     //帮助文档：It returns an empty record, having only the field names.
                     QSqlRecord record = model.record();

                     //设置各字段键值
                     record.setValue("users_id", users_id);
                     record.setValue("users_name", users_name);
                     record.setValue("users_password", users_password1);
                     //插入，-1代表插入到行尾
                     bool result_successed = model.insertRecord(-1, record);

                     if(result_successed){

                         qDebug()<<"注册成功！";
                         QMessageBox::information(this,"消息","注册成功！");

                     }else {
                         qDebug()<<"注册失败！";
                         QMessageBox::information(this,"消息","注册失败！");
                     }
                 }

             }

          }



     });
}

Register::~Register()
{
    delete ui;
}
