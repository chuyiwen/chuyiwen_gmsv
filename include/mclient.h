#ifndef __MCLIENT_H__
#define __MCLIENT_H__

#define MSPERSIONALKEY "20020729"


#define MPROTO_QUE_SEND			1001
#define MPROTO_QUE_RECV			1002

#define MPROTO_NOTALK_RECV		1004
#define MPROTO_GMTALK_SEND		1005
#define MPROTO_GMTALK_RECV		1006
#define MPROTO_HELLO_SEND		1007
#define MPROTO_HELLO_RECV		1008
#define MPROTO_WAEIKICK_SEND	1009
#define MPROTO_WAEIKICK_RECV	1010

#define MPROTO_JAIL_RECV		1012
#define MPROTO_MESSAGE_RECV		1014

#define MPROTO_TYPE_SEND		1016

#ifdef _GSERVER_RUNTIME //����GSERVERִ�ж���ʱ���MSERVER
#define MPROTO_RECALL_GSERVER_RUNTIME       1026
#endif

int mproto_ClientDispatchMessage(int fd,char* data);

void mproto_Que_Recv(int fd,char* uid,int userfdid,char* ans);
void mproto_Que_Send( int fd, int uid, int index, char* question);

void mproto_NoTalk_Recv(int fd,char* uid,int userfdid,int nTime);
void mproto_GMTalk_Recv(int fd,char* uid,int ntime,char* data);
void mproto_WaeiKick_Recv(int fd,char* uid,int userfdid);
void mproto_Jail_Recv(int fd,char* uid,int userfdid);

void mproto_Message_Recv(int fd,char* uid,int userfdid,char* ans);

int connectmServer(char* hostname,unsigned short port);
void mproto_Type_Send( int fd, int playernum, int itemuse);

#ifdef _RECAL_ASK_PLAYER			// WON Ҫ����������
void mproto_RECALL_ASK_PLAYER_Recv(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date, int backup_flag);
void mproto_RECALL_BACKUP_PLAYER_Recv(int fd, char *uid, int userfdid, int GmCliId, char *id, int char_num, int date, char *char_data, int backup_flag);
#endif

#ifdef _GSERVER_RUNTIME
void gserver_runtime();
#endif

#endif
