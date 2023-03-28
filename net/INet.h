#pragma once
#include<WinSock2.h>
#include<iostream>
#include<set>
using namespace std;

//#pragma comment(lib,"Ws2_32.lib")

class INetmediator;//声明有一个INetmediator的类，不需要包含头文件
class INet {
public:
	INet() {};
	virtual ~INet() {};//使用时，父类指针指向子类对象，需要使用虚析构
	//初始化网络
	virtual bool InitNet() = 0;
	//关闭网络
	virtual void UninitNet() = 0;
	//发送数据
	virtual bool SendData(long lSendIP, char* buf,int nlen) = 0;
    //IP地址转换，long类型
    static string getIpString(long ip){
        SOCKADDR_IN sockaddr;
        sockaddr.sin_addr.S_un.S_addr=ip;
        return inet_ntoa(sockaddr.sin_addr);
    }
    //获取本地IP地址列表
    static set<long> getValidIpList(){
        //1.获取主机名字
        char hostName[100]="";
        if(SOCKET_ERROR!=gethostname(hostName,100)){
            //2.通过主机名字获取IP地址列表
            int i=0;
            set<long>setIp;
            struct hostent* remoteHost=gethostbyname(hostName);
            while(remoteHost->h_addr_list[i]!=0){
                setIp.insert(*(u_long*)remoteHost->h_addr_list[i++]);
            }
            return setIp;
        }else{
            return set<long>();
        }
    }
protected:
	//接收数据
	virtual bool RecvData() = 0;
	INetmediator* m_pMediator;
};
