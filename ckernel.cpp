#include "ckernel.h"
#include"UDPmediator.h"
#include"INet.h"
#include"PackDef.h"
#include<QMessageBox>
#include<QDebug>
#include<QHostAddress>


CKernel::CKernel(QObject *parent) : QObject(parent)
{
    //创建主窗口对象并显示
    m_mainWnd=new MainWindow;
    m_mainWnd->show();
    //绑定用户双击某一行的信号和槽
    connect(m_mainWnd,SIGNAL(SIG_userClient(QString)),
            this,SLOT(slot_userClient(QString)));
    //绑定关闭窗口的信号和槽
    connect(m_mainWnd,SIGNAL(SIG_close()),
            this,SLOT(slot_close()));


    //创建中介者类的对象
    m_pUdp=new UDPmediator;
    //绑定接收数据的信号和槽：写在信号所在对象初始化的后面
    connect(m_pUdp,SIGNAL(SIG_ReadyData(long,char*,int)),this,SLOT(slot_ReadyData(long,char*,int)));
    //打开网络
    if(!m_pUdp->OpenNet()){
        QMessageBox::about(m_mainWnd,"提示","网络错误");
        exit(0);
    }
    //发上线广播
    STRU_ONLINE rq;
    gethostname(rq.szName,_DEF_NAME_SIZE);//获取主机名字
    m_pUdp->SendData(INADDR_BROADCAST,(char*)&rq,sizeof(rq));
}

CKernel::~CKernel()
{
    qDebug()<<__func__;
    //回收聊天窗口
    for(auto ite=m_mapIpToChat.begin();ite!=m_mapIpToChat.end();){
        ChatDialog* chat=ite->second;
        chat->hide();
        delete chat;
        chat=NULL;
        ite=m_mapIpToChat.erase(ite);
    }
    //回收主窗口对象
    if(m_mainWnd){
        m_mainWnd->hide();
        delete m_mainWnd;
        m_mainWnd=NULL;
    }

    //回收中介者对象
    if(m_pUdp){
        m_pUdp->CloseNet();
        delete m_pUdp;
        m_pUdp-NULL;
    }
}

//处理接收到的数据
void CKernel::slot_ReadyData(long lSendIP, char *buf, int nlen)
{
    qDebug()<<__func__<<":"<<buf<<endl;//函数名
    /*QMessageBox::about(m_mainWnd,
                       QString("来自[%1]的信息").arg(QHostAddress(htonl(lSendIP)).toString()),
                       buf);*/
    //1.取出协议头
    int nType=*(int*)buf;//取前四个字节的数据
    //2.根据协议头判断是什么请求
    switch(nType){
    case _DEF_PROTOCAL_ONLINE_RQ:
        slot_onlineRq(lSendIP,buf, nlen);
        break;
    case _DEF_PROTOCAL_ONLINE_RS:
        slot_onlineRs(lSendIP,buf, nlen);
        break;
    case _DEF_PROTOCAL_OFFLINE_RQ:
        slot_offlineRq(lSendIP,buf, nlen);
        break;
    case _DEF_PROTOCAL_CHAT_RQ:
        slot_chatRq(lSendIP,buf, nlen);
        break;
    default:break;
    }
    delete[]buf;
}

//处理上线请求的函数
void CKernel::slot_onlineRq(long lSendIP,char* buf, int nlen){
    qDebug()<<__func__;
    //1.拆包
    STRU_ONLINE *rq=(STRU_ONLINE*)buf;

    //2.添加该用户到自己的好友列表
    //QString = char*
    //string = char*
    m_mainWnd->slot_addFriend(INet::getIpString(lSendIP).c_str(),QString::fromStdString(rq->szName));

    if(m_mapIpToChat.count(lSendIP)==0){
        //3.创建与该用户的聊天窗口，但是先不显示
        ChatDialog* chat=new ChatDialog;
        chat->setInfo(INet::getIpString(lSendIP).c_str());
        connect(chat,SIGNAL(SIG_SendChatMsg(QString, QString)),
                this,SLOT(slot_SendChatMsg(QString, QString)));

        //4.在map中保存聊天窗口
        m_mapIpToChat[lSendIP]=chat;
    }

    //5.判断是不是自己，如果是自己就不回复
    set<long>setIp=INet::getValidIpList();
    if(setIp.count(lSendIP)>0){
        return;
    }

    //6.给对方发上线回复包
    STRU_ONLINE rs;
    rs.nType=_DEF_PROTOCAL_ONLINE_RS;
    gethostname(rs.szName,sizeof(rs.szName));
    m_pUdp->SendData(lSendIP,(char*)&rs,sizeof(rs));
}
//处理上线回复的函数
void CKernel::slot_onlineRs(long lSendIP,char* buf, int nlen){
    qDebug()<<__func__;
    //1.拆包
    STRU_ONLINE *rs=(STRU_ONLINE*)buf;
    //2.添加该用户到自己的好友列表
    m_mainWnd->slot_addFriend(INet::getIpString(lSendIP).c_str(),QString::fromStdString(rs->szName));
    if(m_mapIpToChat.count(lSendIP)==0){
        //3.创建与该用户的聊天窗口，但是先不显示
        ChatDialog* chat=new ChatDialog;
        chat->setInfo(INet::getIpString(lSendIP).c_str());
        connect(chat,SIGNAL(SIG_SendChatMsg(QString, QString)),
                this,SLOT(slot_SendChatMsg(QString, QString)));

        //4.在map中保存聊天窗口
        m_mapIpToChat[lSendIP]=chat;
    }
}
//处理下线请求的函数
void CKernel::slot_offlineRq(long lSendIP,char* buf, int nlen){
    qDebug()<<__func__;
    //1.从好友列表中删除
    m_mainWnd->slot_deleteFriend(INet::getIpString(lSendIP).c_str());
    //2.回收聊天窗口，并从map中删除节点
    auto ite=m_mapIpToChat.find(lSendIP);
    if(ite!=m_mapIpToChat.end()){
        ChatDialog *chat=ite->second;
        chat->hide();
        delete chat;
        chat=NULL;
        ite=m_mapIpToChat.erase(ite);
    }
}
//处理聊天请求的函数
void CKernel::slot_chatRq(long lSendIP,char* buf, int nlen){
    qDebug()<<__func__;
    //1.拆包
    STRU_CHAT_RQ* rq=(STRU_CHAT_RQ*)buf;
    //2.打开聊天窗口
    if(m_mapIpToChat.count(lSendIP)>0){
        //3.取出窗口并显示
        ChatDialog* chat=m_mapIpToChat[lSendIP];
        chat->showNormal();
        //4.设置聊天内容
        chat->slot_setChatMsg(rq->szContent);
    }
}

//处理双击用户所在行
void CKernel::slot_userClient(QString ip)
{
    qDebug()<<__func__;
    //1.转换ip
    long lip=inet_addr(ip.toStdString().c_str());
    //2.判断窗口是否存在
    if(m_mapIpToChat.count(lip)>0){
        //取出窗口并显示
        ChatDialog* chat=m_mapIpToChat[lip];
        chat->showNormal();
    }
}

//处理发送聊天请求
void CKernel::slot_SendChatMsg(QString ip, QString content)
{
    qDebug()<<__func__;
    //1.打包
    STRU_CHAT_RQ rq;
    strcpy(rq.szContent,content.toStdString().c_str());
    //2.发送数据
    m_pUdp->SendData(inet_addr(ip.toStdString().c_str()),(char*)&rq,sizeof(rq));
}

//关闭窗口的槽函数
void CKernel::slot_close()
{
    qDebug()<<__func__;
    //1.发送下线请求
    STRU_OFFLINE_RQ rq;
    m_pUdp->SendData(INADDR_BROADCAST,(char*)&rq,sizeof(rq));

    //2.回收聊天窗口
    for(auto ite=m_mapIpToChat.begin();ite!=m_mapIpToChat.end();){
        ChatDialog* chat=ite->second;
        chat->hide();
        delete chat;
        chat=NULL;
        ite=m_mapIpToChat.erase(ite);
    }
    //3.回收主窗口
    if(m_mainWnd){
        m_mainWnd->hide();
        delete m_mainWnd;
        m_mainWnd=NULL;
    }

    //4.关闭网络
    if(m_pUdp){
        m_pUdp->CloseNet();
        delete m_pUdp;
        m_pUdp-NULL;
    }
    exit(0);
}
