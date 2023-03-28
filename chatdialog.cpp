#include "chatdialog.h"
#include "ui_chatdialog.h"
#include<QTime>

ChatDialog::ChatDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::setInfo(QString ip)
{
    m_ip=ip;
    //设置窗口标题：与【IP】的聊天
    setWindowTitle(QString("与[%1]的聊天").arg(ip));
}

//在控件上显示收到的聊天内容
void ChatDialog::slot_setChatMsg(QString content)
{
    ui->tb_chat->append(QString("[%1] %2").arg(m_ip).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
}

void ChatDialog::on_pb_send_clicked()
{
    //1.获取输入的内容QString
    QString content=ui->te_chat->toPlainText();
    if(content.isEmpty()){
        return;
    }
    //2.清空te控件
    ui->te_chat->clear();
    //3.显示到上面tb控件
    //append换行追加
    ui->tb_chat->append(QString("[我] %1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
    //4.发送数据到ckernal类
    Q_EMIT SIG_SendChatMsg(m_ip,content);
}
