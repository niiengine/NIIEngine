/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef SERVINFO_H_
#define SERVINFO_H_

#include "util.h"
#include "imconn.h"
#include "ConfigFileReader.h"

#define MAX_RECONNECT_CNT	64
#define MIN_RECONNECT_CNT	4

typedef struct 
{
	string		server_ip;
	Nui16	server_port;
	uint32_t	idle_cnt;
	uint32_t	reconnect_cnt;
	SocketIO * serv_conn;
} serv_info_t;

template <class T> 
void serv_init(serv_info_t * server_list, uint32_t server_count)
{
	for (uint32_t i = 0; i < server_count; i++) 
    {
		T * pConn = new T();
		pConn->connect(server_list[i].server_ip.c_str(), server_list[i].server_port, i);
		server_list[i].serv_conn = pConn;
		server_list[i].idle_cnt = 0;
		server_list[i].reconnect_cnt = MIN_RECONNECT_CNT / 2;
	}
}

template <class T> 
void serv_check_reconnect(serv_info_t * server_list, uint32_t server_count)
{
	T * pConn;
	for (uint32_t i = 0; i < server_count; i++) 
    {
		pConn = (T*)server_list[i].serv_conn;
		if (!pConn) {
			server_list[i].idle_cnt++;
			if (server_list[i].idle_cnt >= server_list[i].reconnect_cnt) 
            {
				pConn = new T();
				pConn->Connect(server_list[i].server_ip.c_str(), server_list[i].server_port, i);
				server_list[i].serv_conn = pConn;
			}
		}
	}
}

template <class T>
void serv_reset(serv_info_t * server_list, uint32_t server_count, uint32_t serv_idx)
{
	if (serv_idx >= server_count) 
    {
		return;
	}

	server_list[serv_idx].serv_conn = NULL;
	server_list[serv_idx].idle_cnt = 0;
	server_list[serv_idx].reconnect_cnt *= 2;
	if (server_list[serv_idx].reconnect_cnt > MAX_RECONNECT_CNT) 
    {
		server_list[serv_idx].reconnect_cnt = MIN_RECONNECT_CNT;
	}
}

serv_info_t * read_server_config(ConfigFile * file, const char * server_ip_format,
    const char * server_port_format, uint32_t & cnt);


#endif /* SERVINFO_H_ */
