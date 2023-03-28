#ifndef CKERNEL_H
#define CKERNEL_H

#include"mainwindow.h"
#include"INetMediator.h"
#include <QObject>
#include<map>
#include"chatdialog.h"
using namespace std;

class CKernel : public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel();

signals:
public slots:
    //处理数据槽函数
    void slot_ReadyData(long lSendIP,char* buf, int nlen);
    //处理上线请求的函数
    void slot_onlineRq(long lSendIP,char* buf, int nlen);
    //处理上线回复的函数
    void slot_onlineRs(long lSendIP,char* buf, int nlen);
    //处理下线请求的函数
    void slot_offlineRq(long lSendIP,char* buf, int nlen);
    //处理聊天请求的函数
    void slot_chatRq(long lSendIP,char* buf, int nlen);
    //处理双击用户所在行
    void slot_userClient(QString ip);
    //处理发送聊天请求
    void slot_SendChatMsg(QString ip,QString content);
    //处理关闭窗口的槽函数
    void slot_close();
private:
    MainWindow* m_mainWnd;
    INetmediator* m_pUdp;
    map<long,ChatDialog*>m_mapIpToChat;
};

#endif // CKERNEL_H
