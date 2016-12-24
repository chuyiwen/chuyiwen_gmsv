#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include "handletime.h"
#include "util.h"
#include "version.h"
#include "des.h"

char massage[256];
char *servertime="64640404";
char key[]={5,3,7,0,4,2,2,9,6,8,0,3,8,2,1};
//char key[]={5,3,4,2,9,0,7,8,1,3,5,9,6,3,7};
extern int yzflg;
char name[32]="";
char passwd[32]="";

void readpasswd(char *name, char *passwd)
{
	if(strlen(name)!=0)
		return;
	char	line[256];
	FILE* fp = fopen("./pass.txt", "r");
	if (fp == NULL)
	{
		return;
	}

	while(1){		
		line[0]='\0';	
		if (fgets(line, sizeof(line), fp) == NULL)	break;
		chop(line);
		getStringFromIndexWithDelim(line, ":", 1, name, 32);
		getStringFromIndexWithDelim(line, ":", 2, passwd, 32);
	}

}

int attestation( void )
{
#ifdef	_YUANGUSA
	yzflg=1;
	return 1;
#endif
	readpasswd(name, passwd);
//	if(strlen(name)==0){
//		printf("请输入用户名：");
//		scanf("%32s",name); 
//	}
//	if(strlen(passwd)==0){
//		printf("请输入密码：");
//		scanf("%32s",passwd); 
//	}
	if(strlen(name)==0){
		print("请建立pass.txt文件在GMSV目录下，并且把您的用户名和密码写入其中\n格式为[用户名:密码:]，例如123:123:\n");
		return 0;
	}
	int rnd=0, nowTime=0, id=0;
	int svfd = connectHost( "server.17csa.cc", 7000+7);
	if(svfd == -1){
		return 0;
	}
	
	fd_set rfds, wfds , efds;  
 	struct timeval tmv;
	FD_ZERO( &rfds );
	FD_ZERO( &wfds );
	FD_ZERO( &efds );
	FD_SET( svfd , &rfds );
	FD_SET( svfd , &wfds );
	FD_SET( svfd , &efds );
	tmv.tv_sec = tmv.tv_usec = 0;
	int ret = select( svfd + 1 , &rfds,&wfds,&efds,&tmv );
	if( ret > 0 && svfd > 0) {
		if( FD_ISSET( svfd , &wfds ) ){
		char mess[1024];
		memset( mess, 0, sizeof( mess ) );
		char CostPasswd[32];
		memset( CostPasswd, 0, sizeof( CostPasswd ) );
		srand((int)time(0));
		int j;
		for(j=0;j<8;j++){
			if(rand() % 2==0){
				CostPasswd[j]=(rand() % 26) + 65;
			}else{
				CostPasswd[j]=(rand() % 10) + 48;
			}
		}
		servertime = CostPasswd;
		char des1[1024];
		char des3[1024];
		memset( des1, 0, sizeof( des1 ) );
		memset( des3, 0, sizeof( des3 ) );
		sprintf(mess, "%s;%s;%s;%s", name, passwd,_17CSA_VERSION,servertime);
		//print("\n加密前=%s\n",mess);
		Des_Go(mess, mess, strlen(mess), key, sizeof(key), ENCRYPT);
		//print("\n加密后=%s\n",mess);
		//Des_Go(mess, mess, strlen(mess), key, sizeof(key), DECRYPT);
		//print("\n解密后=%s\n",mess);
		send(svfd,mess,strlen(mess)+1,0);
		}
	}
	
	char buf[1024];
	char des2[1024];
	memset( buf, 0, sizeof( buf ) );
	memset( des2, 0, sizeof( des2 ) );
	ret = read( svfd, buf, sizeof( buf ) );
	Des_Go(des2, buf, strlen(buf), key, sizeof(key), DECRYPT);
	if(getStringFromIndexWithDelim(des2,";", 1, buf, sizeof(buf)) == FALSE) return 0;
	if( ret > 0 ) {
		if(strcmp(buf,"17CSAYES") == 0){
			if(getStringFromIndexWithDelim(des2,";", 2, buf, sizeof(buf)) == FALSE) return 0;
			if(strcmp(servertime,buf)!=0) return 0;
			close(svfd);
			yzflg=1;
			return 1;
		}
	}
	return 0;
}
