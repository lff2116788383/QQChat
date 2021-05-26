#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    bool searchresult;

signals:
    //void senddata(QString);

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
