#pragma once
#include<string.h>

#define _DEF_UDP_PORT (12345)
#define _DEF_BUFFER_SIZE (4096)
#define _DEF_NAME_SIZE (60)
#define _DEF_CONTENT_SIZE (1024)

//协议头 从10000开始
#define _DEF_PROTOCAL_BASE (10000)
//UDP协议头
//上线
#define _DEF_PROTOCAL_ONLINE_RQ (_DEF_PROTOCAL_BASE+1)
#define _DEF_PROTOCAL_ONLINE_RS (_DEF_PROTOCAL_BASE+2)
//下线
#define _DEF_PROTOCAL_OFFLINE_RQ (_DEF_PROTOCAL_BASE+3)
//聊天
#define _DEF_PROTOCAL_CHAT_RQ (_DEF_PROTOCAL_BASE+4)
//传文件
#define _DEF_PROTOCAL_FILEINFO_RQ (_DEF_PROTOCAL_BASE+5)

//UDP协议
//请求结构体
//上线通知：自己的IP 我的主机名
//上线回复：自己的IP 我的主机名
struct STRU_ONLINE {
    STRU_ONLINE():nType(_DEF_PROTOCAL_ONLINE_RQ){
        memset(szName, 0, _DEF_NAME_SIZE);
    }
    int nType;
    //long ip;
    char szName[_DEF_NAME_SIZE];
};

//下线通知：自己的IP
struct STRU_OFFLINE_RQ {
    STRU_OFFLINE_RQ() :nType(_DEF_PROTOCAL_OFFLINE_RQ) {

    }
    int nType;
};

//聊天请求：聊天内容、自己的IP
struct STRU_CHAT_RQ {
    STRU_CHAT_RQ():nType(_DEF_PROTOCAL_CHAT_RQ){
        memset(szContent, 0, _DEF_CONTENT_SIZE);
    }
    int nType;
    char szContent[_DEF_CONTENT_SIZE];
};
