#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_addFriend(QString ip, QString hostname)
{
    //1.总行号+1
    ui->tw_friend->setRowCount(ui->tw_friend->rowCount()+1);
    //2.在最后一行添加两个item
    QTableWidgetItem* item1=new QTableWidgetItem(ip);
    ui->tw_friend->setItem(ui->tw_friend->rowCount()-1,0,item1);
    QTableWidgetItem* item2=new QTableWidgetItem(hostname);
    ui->tw_friend->setItem(ui->tw_friend->rowCount()-1,1,item2);
}

void MainWindow::slot_deleteFriend(QString ip)
{
    for(int i=0;i<ui->tw_friend->rowCount();i++){
        if(ip==ui->tw_friend->item(i,0)->text()){
            ui->tw_friend->removeRow(i);
            break;
        }
    }
    this->update();
}

#include<QDebug>


void MainWindow::on_tw_friend_cellDoubleClicked(int row, int column)
{
    qDebug()<<QString("%1,%2").arg(row).arg(column);
    //获取点击哪一行的第0列的数据，也就是IP地址
    QString ip=ui->tw_friend->item(row,0)->text();
    Q_EMIT SIG_userClient(ip);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(QMessageBox::question(this,"提示","是否关闭")==QMessageBox::Yes){
        Q_EMIT SIG_close();
    }else {
        //忽略事件
        event->ignore();
    }
}
