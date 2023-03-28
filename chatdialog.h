#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QWidget>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QWidget
{
    Q_OBJECT

signals:

    void SIG_SendChatMsg(QString ip,QString content);

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void setInfo(QString ip);
    void slot_setChatMsg(QString content);


private slots:
    void on_pb_send_clicked();

private:
    Ui::ChatDialog *ui;
    QString m_ip;
};

#endif // CHATDIALOG_H
