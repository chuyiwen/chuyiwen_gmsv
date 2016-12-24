#include "version.h"
#include "autil.h"
#include "lssproto_serv.h"
#include "common.h"	// for StoneAge
#include "log.h"	// for StoneAge
#include "char.h"
#include <stdio.h>
#include <time.h>
#include <net.h>


void lssproto_SetServerLogFiles( char *r , char *w )
{
    lssproto_strcpysafe( lssproto_writelogfilename , w , sizeof(lssproto_writelogfilename ));
    lssproto_strcpysafe( lssproto_readlogfilename , r , sizeof(lssproto_readlogfilename ));
}
int lssproto_InitServer(	int (*writefunc)(int,char*,int) ,	int worksiz )
{
    int i;
    if( (void*)writefunc == NULL) {
        lssproto.write_func = lssproto_default_write_wrap;
    } else {
        lssproto.write_func = writefunc;
    }
    lssproto_AllocateCommonWork(worksiz);
    lssproto_stringwrapper = (char**)calloc( 1,sizeof(char*) * MAXLSRPCARGS);
    if(lssproto_stringwrapper ==NULL)
        return -1;
    memset( lssproto_stringwrapper , 0, sizeof(char*)*MAXLSRPCARGS);
    for(i=0;i<MAXLSRPCARGS;i++) {
        lssproto_stringwrapper[i] = (char*)calloc( 1, worksiz );
        if( lssproto_stringwrapper[i] == NULL) {
            for(i=0;i<MAXLSRPCARGS;i++) {
                free( lssproto_stringwrapper[i]);
                return -1;
            }
        }
    }
    lssproto.ret_work = (char*)calloc( 1,sizeof( worksiz ));
    if( lssproto.ret_work == NULL ) {
        return -1;
    }
    return 0;
}

void lssproto_CleanupServer( void )
{
    int i;
    free( lssproto.work );
    free( lssproto.arraywork);
    free( lssproto.escapework );
    free( lssproto.val_str);
    free( lssproto.token_list );
    for(i=0;i<MAXLSRPCARGS;i++) {
        free( lssproto_stringwrapper[i]);
    }
    free( lssproto_stringwrapper );
    free( lssproto.ret_work );
}

#define DME() print("<DME(%d)%d:%d>",fd,__LINE__,func)
extern int cliretfunc;
extern char playcdkey[16];
extern char *errraw;
extern char *MesgSlice[SLICE_MAX];

int lssproto_ServerDispatchMessage(int fd, char *encoded)
{
	//print("\ntest=%s,state=%d\n",encoded,CONNECT_getState(fd));
	if(strlen(encoded)>getBigBao() && CONNECT_getState(fd) == NOTLOGIN){
		unsigned long tmpip;
    	int a,b,c,d;
    	char ip[32];
    	tmpip=CONNECT_get_userip(fd);
    	a=(tmpip % 0x100); tmpip=tmpip / 0x100;
   		b=(tmpip % 0x100); tmpip=tmpip / 0x100;
    	c=(tmpip % 0x100); tmpip=tmpip / 0x100;
    	d=(tmpip % 0x100);
    	sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
    	if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
#ifdef _CAX_OFF_IPTABLES    		
    		if(strcmp(ip,"0.0.0.0")!=0){
    			char systemstr[128];
					sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
					system(systemstr);
					//print("\nIP=%s\n",ip);
					logBigBao(ip,strlen(encoded));
				}
#endif				
				CONNECT_endOne_debug(fd);
				return -1;
    	}
	}
	
	int func,fieldcount;
	char raw[1024 * 64];
#ifdef _SERVER_DEF
    util_DiscardMessage();
#endif

	if(strlen(encoded)>getBigBao2()){
		CONNECT_endOne_debug(fd);
		return -1;
	}

	memset(raw, 0, sizeof(raw));
	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());
	//printf("\nKey1=%s,KEY2=$s\n",PersonalKey,getRunningKey());

	util_DecodeMessage(raw,encoded);
	//print("\nraw=%s\n",raw);
	if(strlen(raw)<1){
		//DME();
		return -1;
	}
	
	errraw = raw;

	if( !util_SplitMessage(raw,SEPARATOR) ){
		print("\nDME1:package=%s\n",raw);
				//DME();
		return -1;
	}
	
	if (!util_GetFunctionFromSlice(&func, &fieldcount)) {
		//logHack(fd,HACK_GETFUNCFAIL);
				//DME();
		return -1;
	}
	//printf("\n客户端接口=%d\n",func);
#ifdef _SERVER_DEF
if (func != 71 && func != 79 && func != 87 && func != 77 && func != 81 && func != 73 && func != 75)//登录前封包 全部可执行，其他封包检查是否已经有玩家在线！
		{
			if( !CHAR_CHECKINDEX( CONNECT_getCharaindex(fd) ) )//并没有登入人物，仅仅是一个连接！
			{
        close(fd);//断开玩家之间的连接
				return -1;//你要做的事!
			}
		}
#endif	
	cliretfunc=func;
	CONNECT_getCdkey( fd, playcdkey, sizeof( playcdkey ));
	

	if (func==LSSPROTO_W_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char direction[1024 * 64];

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_destring(fd,4, direction);
		util_deint(fd,5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			////logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%s",x,y,direction);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_W_recv(fd, x, y, direction);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_W2_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char direction[1024 * 64];

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_destring(fd,4, direction);
		util_deint(fd,5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			////logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%s",x,y,direction);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_W2_recv(fd, x, y, direction);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_EV_RECV) {
		int checksum=0, checksumrecv;
		int event;
		int seqno;
		int x;
		int y;
		int dir;

		checksum += util_deint(fd,2, &event);
		checksum += util_deint(fd,3, &seqno);
		checksum += util_deint(fd,4, &x);
		checksum += util_deint(fd,5, &y);
		checksum += util_deint(fd,6, &dir);
		util_deint(fd,7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d|%d|%d",event,seqno,x,y,dir);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_EV_recv(fd, event, seqno, x, y, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DU_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		util_deint(fd,4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d",x,y);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_DU_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_EO_RECV) {
		int checksum=0, checksumrecv;
		int dummy;

		checksum += util_deint(fd,2, &dummy);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",dummy);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_EO_recv(fd, dummy);
		util_DiscardMessage();
		return 0;
	}
     
	if (func==LSSPROTO_BU_RECV) {
		int checksum=0, checksumrecv;
		int dummy;

		checksum += util_deint(fd,2, &dummy);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",dummy);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_BU_recv(fd, dummy);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_JB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		util_deint(fd,4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d",x,y);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_JB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_LB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		util_deint(fd,4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d",x,y);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_LB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_B_RECV) {
		int checksum=0, checksumrecv;
		char command[1024 * 64];

		checksum += util_destring(fd,2, command);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}		
		sprintf(errraw,"%s",command);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_B_recv(fd, command);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SKD_RECV) {
		int checksum=0, checksumrecv;
		int dir;
		int index;

		checksum += util_deint(fd,2, &dir);
		checksum += util_deint(fd,3, &index);
		util_deint(fd,4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d",dir,index);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_SKD_recv(fd, dir, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ID_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int haveitemindex;
		int toindex;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_deint(fd,4, &haveitemindex);
		checksum += util_deint(fd,5, &toindex);
		util_deint(fd,6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d|%d",x,y,haveitemindex,toindex);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_ID_recv(fd, x, y, haveitemindex, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PI_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int dir;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_deint(fd,4, &dir);
		util_deint(fd,5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d",x,y,dir);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_PI_recv(fd, x, y, dir);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DI_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int itemindex;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_deint(fd,4, &itemindex);
		util_deint(fd,5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d",x,y,itemindex);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_DI_recv(fd, x, y, itemindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DG_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int amount;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_deint(fd,4, &amount);
		util_deint(fd,5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d",x,y,amount);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_DG_recv(fd, x, y, amount);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DP_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int petindex;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_deint(fd,4, &petindex);
		util_deint(fd,5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d",x,y,petindex);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_DP_recv(fd, x, y, petindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MI_RECV) {
		int checksum=0, checksumrecv;
		int fromindex;
		int toindex;

		checksum += util_deint(fd,2, &fromindex);
		checksum += util_deint(fd,3, &toindex);
		util_deint(fd,4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d",fromindex,toindex);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_MI_recv(fd, fromindex, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MSG_RECV) {
		int checksum=0, checksumrecv;
		int index;
		char message[1024 * 64];
		int color;

		checksum += util_deint(fd,2, &index);
		checksum += util_destring(fd,3, message);
		checksum += util_deint(fd,4, &color);
		util_deint(fd,5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%s|%d",index,message,color);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_MSG_recv(fd, index, message, color);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PMSG_RECV) {
		int checksum=0, checksumrecv;
		int index;
		int petindex;
		int itemindex;
		char message[1024 * 64];
		int color;

		checksum += util_deint(fd,2, &index);
		checksum += util_deint(fd,3, &petindex);
		checksum += util_deint(fd,4, &itemindex);
		checksum += util_destring(fd,5, message);
		checksum += util_deint(fd,6, &color);
		util_deint(fd,7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d|%s|%d",index,petindex,itemindex,message,color);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_PMSG_recv(fd, index, petindex, itemindex, message, color);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_AB_RECV) {
		int checksum=0, checksumrecv;
		util_deint(fd,2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s","空");
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_AB_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DAB_RECV) {
		int checksum=0, checksumrecv;
		int index;

		checksum += util_deint(fd,2, &index);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",index);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_DAB_recv(fd, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_AAB_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		util_deint(fd,4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d",x,y);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_AAB_recv(fd, x, y);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_L_RECV) {
		int checksum=0, checksumrecv;
		int dir;

		checksum += util_deint(fd,2, &dir);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",dir);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_L_recv(fd, dir);
		util_DiscardMessage();
		return 0;
	}

#ifdef	_CAX_TALK_LJIU	
	if (func==LSSPROTO_TK_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char message[1024 * 64];
		int color;
		int area;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_destring(fd,4, message);
		checksum += util_deint(fd,5, &color);
		checksum += util_deint(fd,6, &area);
		util_deint(fd,7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
				
		sprintf(errraw,"%d|%d|%s|%d|%d",x,y,message,color,area);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
    if(strcmp( message , "P|" ) == 0)
		{
			lssproto_TK_recv(fd, x, y, "P|hi", color, area);
		}else{
			lssproto_TK_recv(fd, x, y, message, color, area);
		}
		util_DiscardMessage();
		return 0;
	}
#else	
	if (func==LSSPROTO_TK_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		char message[1024 * 64];
		int color;
		int area;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_destring(fd,4, message);
		checksum += util_deint(fd,5, &color);
		checksum += util_deint(fd,6, &area);
		util_deint(fd,7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%s|%d|%d",x,y,message,color,area);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_TK_recv(fd, x, y, message, color, area);
		util_DiscardMessage();
		return 0;
	}
#endif

	if (func==LSSPROTO_M_RECV) {
		int checksum=0, checksumrecv;
		int fl;
		int x1;
		int y1;
		int x2;
		int y2;

		checksum += util_deint(fd,2, &fl);
		checksum += util_deint(fd,3, &x1);
		checksum += util_deint(fd,4, &y1);
		checksum += util_deint(fd,5, &x2);
		checksum += util_deint(fd,6, &y2);
		util_deint(fd,7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d|%d|%d",fl,x1,y1,x2,y2);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_M_recv(fd, fl, x1, y1, x2, y2);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_C_RECV) {
		int checksum=0, checksumrecv;
		int index;

		checksum += util_deint(fd,2, &index);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",index);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_C_recv(fd, index);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_S_RECV) {
		int checksum=0, checksumrecv;
		char category[1024 * 64];

		checksum += util_destring(fd,2, category);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
#ifdef _FIX_LSSP_S_BUG
		if (is_digit(category)<0) return -1;
#endif
		sprintf(errraw,"%s",category);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_S_recv(fd, category);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FS_RECV) {
		int checksum=0, checksumrecv;
		int flg;

		checksum += util_deint(fd,2, &flg);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",flg);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_FS_recv(fd, flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_HL_RECV) {
		int checksum=0, checksumrecv;
		int flg;

		checksum += util_deint(fd,2, &flg);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",flg);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_HL_recv(fd, flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PR_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int request;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_deint(fd,4, &request);
		util_deint(fd,5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d",x,y,request);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_PR_recv(fd, x, y, request);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_KS_RECV) {
		int checksum=0, checksumrecv;
		int petarray;
		checksum += util_deint(fd,2, &petarray);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",petarray);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_KS_recv(fd, petarray);
		util_DiscardMessage();
		return 0;
	}

#ifdef _STANDBYPET
	if (func==LSSPROTO_SPET_RECV) {
		int checksum=0, checksumrecv;
		int standbypet;
		checksum += util_deint(fd,2, &standbypet);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",standbypet);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_SPET_recv(fd, standbypet);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _RIGHTCLICK
	if (func==LSSPROTO_RCLICK_RECV) {
		int checksum=0, checksumrecv;
		int type;
		char data[1024];
		checksum += util_deint(fd,2, &type);
		checksum += util_destring(fd,3, data);
		util_deint(fd,4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%s",type,data);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_RCLICK_recv(fd, type, data);
		util_DiscardMessage();
		return 0;
	}
#endif

	if (func==LSSPROTO_AC_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int actionno;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_deint(fd,4, &actionno);
		util_deint(fd,5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d",x,y,actionno);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_AC_recv(fd, x, y, actionno);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_MU_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int array;
		int toindex;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_deint(fd,4, &array);
		checksum += util_deint(fd,5, &toindex);
		util_deint(fd,6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d|%d",x,y,array,toindex);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_MU_recv(fd, x, y, array, toindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PS_RECV) {
		int checksum=0, checksumrecv;
		int havepetindex;
		int havepetskill;
		int toindex;
		char data[1024 * 64];
		
		checksum += util_deint(fd,2, &havepetindex);
		checksum += util_deint(fd,3, &havepetskill);
		checksum += util_deint(fd,4, &toindex);
		checksum += util_destring(fd,5, data);
		util_deint(fd,6, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);

					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d|%s",havepetindex,havepetskill,toindex,data);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_PS_recv(fd, havepetindex, havepetskill, toindex, data);
		
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ST_RECV) {
		int checksum=0, checksumrecv;
		int titleindex;

		checksum += util_deint(fd,2, &titleindex);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",titleindex);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_ST_recv(fd, titleindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_DT_RECV) {
		int checksum=0, checksumrecv;
		int titleindex;

		checksum += util_deint(fd,2, &titleindex);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",titleindex);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_DT_recv(fd, titleindex);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FT_RECV) {
		int checksum=0, checksumrecv;
		char data[1024 * 64];

		checksum += util_destring(fd,2, data);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s",data);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_FT_recv(fd, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SKUP_RECV) {
		int checksum=0, checksumrecv;
		int skillid;

		checksum += util_deint(fd,2, &skillid);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",skillid);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_SKUP_recv(fd, skillid);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_KN_RECV) {
		int checksum=0, checksumrecv;
		int havepetindex;
		char data[1024 * 64];

		checksum += util_deint(fd,2, &havepetindex);
		checksum += util_destring(fd,3, data);
		util_deint(fd,4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%s",havepetindex,data);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_KN_recv(fd, havepetindex, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_WN_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int seqno;
		int objindex;
		int select;
		char data[1024 * 64];

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_deint(fd,4, &seqno);
		checksum += util_deint(fd,5, &objindex);
		checksum += util_deint(fd,6, &select);
		checksum += util_destring(fd,7, data);
		
		util_deint(fd,8, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d|%d|%d|%d|%s",CHAR_getInt(CONNECT_getCharaindex(fd),CHAR_FLOOR),x,y,seqno,objindex,select,data);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_WN_recv(fd, x, y, seqno, objindex, select, data);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_SP_RECV) {
		int checksum=0, checksumrecv;
		int x;
		int y;
		int dir;

		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_deint(fd,4, &dir);
		util_deint(fd,5, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d",x,y,dir);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_SP_recv(fd, x, y, dir);
		util_DiscardMessage();
		return 0;
	}
#ifdef _CAX_NEWMAC_LOGIN
if (func==LSSPROTO_CLIENTLOGIN_RECV) {
		int checksum=0, checksumrecv;
		char cdkey[CDKEYLEN];
		char passwd[PASSWDLEN];
		char cpuid[1024];
		char mac[1024];
#ifdef _MO_LOGINCHECK
		char szdata[1024];
#endif
		strcpy(PersonalKey, getDefaultKey(fd));

		checksum += util_destring(fd,2, cdkey);
		checksum += util_destring(fd,3, passwd);
		checksum += util_destring(fd,4, cpuid);
		checksum += util_destring(fd,5, mac);
#ifdef _MO_LOGINCHECK
		checksum += util_destring(fd,6, szdata);
		util_deint(fd,7, &checksumrecv);
#else
		util_deint(fd,6, &checksumrecv);
#endif

		if (checksum!=checksumrecv) {
#ifndef _MO_LOGINCHECK
				checksum = 0;
				checksumrecv = 0;
				checksum += util_destring(fd,2, cdkey);
				checksum += util_destring(fd,3, passwd);
				util_deint(fd,4, &checksumrecv);
				if (checksum!=checksumrecv) {
					util_DiscardMessage();
					return -1;
				}
				sprintf(errraw,"%s|%s",cdkey,passwd);
				logFengBao(fd,playcdkey,func,errraw);
		//		print("\n 1cdkey=%s,passwd=%s\n",cdkey, passwd);
				lssproto_ClientLogin_recv(fd, cdkey, passwd);
				util_DiscardMessage();
				return 0;
#else
				util_DiscardMessage();
				return -1;
#endif
			}	else{
				sprintf(errraw,"%s|%s|%s|%s|%s",cdkey,passwd,cpuid,mac,szdata);
			//	logFengBao(fd,playcdkey,func,errraw);
#ifndef _MO_LOGINCHECK
				if(strlen(cpuid)<20)
					lssproto_ClientLogin_recv(fd, cdkey, passwd);
				else
				//	print("\n 2cdkey=%s,passwd=%s,cpuid=%s mac=%s\n",cdkey, passwd,cpuid,mac);
					lssproto_ClientLoginJqm_recv(fd, cdkey, passwd,cpuid,mac);
#else
				//printf("\n 2cdkey=%s,passwd=%s,cpuid=%s mac=%s\n szdata=%s\n",cdkey, passwd,cpuid,mac,szdata);
				if(strlen(cpuid)<40){
					util_DiscardMessage();
					return -1;
				}
				if(strlen(mac)>13){
					util_DiscardMessage();
					return -1;
				}
				if(strlen(szdata)!=(XXTEA_ROUND*8)){
					util_DiscardMessage();
					return -1;
				}
				lssproto_ClientLoginJqm_recv(fd, cdkey, passwd,cpuid,mac,szdata);
#endif
				util_DiscardMessage();
				return 0;
			}
		return 0;
	}
#else
	if (func==LSSPROTO_CLIENTLOGIN_RECV) {
		int checksum=0, checksumrecv;
		char cdkey[CDKEYLEN];
		char passwd[PASSWDLEN];
		char cpuid[128];
		
		strcpy(PersonalKey, getDefaultKey(fd));

		checksum += util_destring(fd,2, cdkey);
		checksum += util_destring(fd,3, passwd);
		checksum += util_destring(fd,4, cpuid);
		util_deint(fd,5, &checksumrecv);
		
			if (checksum!=checksumrecv) {
				checksum = 0;
				checksumrecv = 0;
				checksum += util_destring(fd,2, cdkey);
				checksum += util_destring(fd,3, passwd);
				util_deint(fd,4, &checksumrecv);
				if (checksum!=checksumrecv) {
					util_DiscardMessage();
					return -1;
				}
				sprintf(errraw,"%s|%s",cdkey,passwd);
				logFengBao(fd,playcdkey,func,errraw);
				lssproto_ClientLogin_recv(fd, cdkey, passwd);
				util_DiscardMessage();
				return 0;
			}else{
				sprintf(errraw,"%s|%s",cdkey,passwd);
				logFengBao(fd,playcdkey,func,errraw);
				if(strlen(cpuid)<20)
					lssproto_ClientLogin_recv(fd, cdkey, passwd);
				else
					lssproto_ClientLoginJqm_recv(fd, cdkey, passwd,cpuid);
				//print("\ncdkey=%s,passwd=%s,cpuid=%s\n",cdkey, passwd,cpuid);
				util_DiscardMessage();
				return 0;
			}
		return 0;
	}
#endif
/*
if (func==LSSPROTO_CLIENTLOGIN_RECV) {
		int checksum=0, checksumrecv;
		char cdkey[CDKEYLEN];
		char passwd[PASSWDLEN];
		char cpuid[128];
		char GHid[128];
		char GHic[128];
		char GHsp[128];
		
		strcpy(PersonalKey, getDefaultKey(fd));

		checksum += util_destring(fd,2, cdkey);
		checksum += util_destring(fd,3, passwd);
		checksum += util_destring(fd,4, cpuid);
		checksum += util_destring(fd,5, GHid);
		checksum += util_destring(fd,6, GHic);
		checksum += util_destring(fd,7, GHsp);
		util_deint(fd,8, &checksumrecv);
		
			if (checksum!=checksumrecv) {
				checksum = 0;
				checksumrecv = 0;
				checksum += util_destring(fd,2, cdkey);
				checksum += util_destring(fd,3, passwd);
				util_deint(fd,4, &checksumrecv);
				if (checksum!=checksumrecv) {
					util_DiscardMessage();
					return -1;
				}
				sprintf(errraw,"%s|%s",cdkey,passwd);
				logFengBao(fd,playcdkey,func,errraw);
				lssproto_ClientLogin_recv(fd, cdkey, passwd);
				util_DiscardMessage();
				return 0;
			}else{
				sprintf(errraw,"%s|%s",cdkey,passwd);
				logFengBao(fd,playcdkey,func,errraw);
				if(strlen(cpuid)<20)
					lssproto_ClientLogin_recv(fd, cdkey, passwd);
				else
					lssproto_ClientLoginJqm_recv(fd, cdkey, passwd,cpuid,GHid,GHic,GHsp);
				//print("\ncdkey=%s,passwd=%s,cpuid=%s\n",cdkey, passwd,cpuid);
				util_DiscardMessage();
				return 0;
			}
//		if (checksum!=checksumrecv) {
//			util_DiscardMessage();
//			
//			//logHack(fd, HACK_CHECKSUMERROR);
//					//DME();
//		return -1;
//		}
//		sprintf(errraw,"%s|%s",cdkey,passwd);
//		logFengBao(fd,playcdkey,func,errraw);
//		//print("\nfunc=%d,raw=%s\n",func,errraw);
//		lssproto_ClientLogin_recv(fd, cdkey, passwd);
//		util_DiscardMessage();
		return 0;
	}
*/
	if (func==LSSPROTO_CREATENEWCHAR_RECV) {
		int checksum=0, checksumrecv;
		int dataplacenum;
#ifdef _FIX_LONGCHARNAME_ATTACK
		char charname[1024];
#else
		char charname[CHARNAMELEN];
#endif
		int imgno;
		int faceimgno;
		int vital;
		int str;
		int tgh;
		int dex;
		int earth;
		int water;
		int fire;
		int wind;
		int hometown;

		checksum += util_deint(fd,2, &dataplacenum);
		checksum += util_destring(fd,3, charname);
		checksum += util_deint(fd,4, &imgno);
		checksum += util_deint(fd,5, &faceimgno);
		checksum += util_deint(fd,6, &vital);
		checksum += util_deint(fd,7, &str);
		checksum += util_deint(fd,8, &tgh);
		checksum += util_deint(fd,9, &dex);
		checksum += util_deint(fd,10, &earth);
		checksum += util_deint(fd,11, &water);
		checksum += util_deint(fd,12, &fire);
		checksum += util_deint(fd,13, &wind);
		checksum += util_deint(fd,14, &hometown);
		util_deint(fd,15, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
#ifdef _FIX_LONGCHARNAME_ATTACK
		if(strlen(charname)>CHARNAMELEN){
			util_DiscardMessage();
			return -1;
		}
#endif
		sprintf(errraw,"%d|%s|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d",dataplacenum,charname,imgno,faceimgno,vital,str,tgh,dex,earth,water,fire,wind,hometown);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_CreateNewChar_recv(fd, dataplacenum, charname, imgno, faceimgno, vital, str, tgh, dex, earth, water, fire, wind, hometown);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARDELETE_RECV) {
		int checksum=0, checksumrecv;
#ifdef _FIX_LONGCHARNAME_ATTACK
		char charname[1024];
#else
		char charname[CHARNAMELEN];
#endif
		char safepass[32];

		checksum += util_destring(fd,2, charname);
		checksum += util_destring(fd,3, safepass);
		util_deint(fd,4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
#ifdef _FIX_LONGCHARNAME_ATTACK
		if(strlen(charname)>CHARNAMELEN){
			util_DiscardMessage();
			return -1;
		}
#endif
		sprintf(errraw,"%s|%s",charname,safepass);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_CharDelete_recv(fd, charname,safepass);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLOGIN_RECV) {
		int checksum=0, checksumrecv;
#ifdef _FIX_LONGCHARNAME_ATTACK
		char charname[1024];
#else
		char charname[CHARNAMELEN];
#endif
		checksum += util_destring(fd,2, charname);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
#ifdef _FIX_LONGCHARNAME_ATTACK
		if(strlen(charname)>CHARNAMELEN){
			util_DiscardMessage();
			return -1;
		}
#endif
		sprintf(errraw,"%s",charname);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_CharLogin_recv(fd, charname);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLIST_RECV) {
		int checksum=0, checksumrecv;
		util_deint(fd,2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s","空");
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_CharList_recv( fd);

		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_CHARLOGOUT_RECV) {
		int checksum=0, checksumrecv;
		int Flg=1;
#ifdef _CHAR_NEWLOGOUT
		checksum += util_deint(fd,2, &Flg);
		util_deint(fd,3, &checksumrecv);
#else
		util_deint(fd,2, &checksumrecv);
#endif
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",Flg);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_CharLogout_recv(fd, Flg);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PROCGET_RECV) {
		int checksum=0, checksumrecv;
		
		strcpy( PersonalKey, getDefaultKey(fd));

		util_deint(fd,2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s","空");
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_ProcGet_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_PLAYERNUMGET_RECV) {
		int checksum=0, checksumrecv;
		util_deint(fd,2, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s","空");
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_PlayerNumGet_recv(fd);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_ECHO_RECV) {
		int checksum=0, checksumrecv;
		char test[1024 * 64];
		memset(test, 0, sizeof(test));
		checksum += util_destring(fd,2, test);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s",test);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
//		if(strlen(test)>10){
//			unsigned long tmpip;
//    	int a,b,c,d;
//    	char ip[32];
//    	tmpip=CONNECT_get_userip(fd);
//    	a=(tmpip % 0x100); tmpip=tmpip / 0x100;
//   		b=(tmpip % 0x100); tmpip=tmpip / 0x100;
//    	c=(tmpip % 0x100); tmpip=tmpip / 0x100;
//    	d=(tmpip % 0x100);
//    	sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
//    	if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
//    		char systemstr[128];
//				sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
//				CONNECT_endOne_debug(fd);
//				system(systemstr);
//				print("\nIP=%s,test=%s\n",ip,test);
//				return -1;
//    	}
//		}
		lssproto_Echo_recv(fd, test);
		util_DiscardMessage();
		return 0;
	}


	if (func==LSSPROTO_TD_RECV) {
		int checksum=0, checksumrecv;
		char message[1024 * 64];

		checksum += util_destring(fd,2, message);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s",message);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_TD_recv(fd, message);
		util_DiscardMessage();
		return 0;
	}

	if (func==LSSPROTO_FM_RECV) {
		int checksum=0, checksumrecv;
		char message[1024 * 64];

		checksum += util_destring(fd,2, message);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s",message);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_FM_recv(fd, message);
		util_DiscardMessage();
		return 0;
	}
    
	if (func==LSSPROTO_PETST_RECV) {
		int checksum=0, checksumrecv;
		int nPet;
		int sPet;		

		checksum += util_deint(fd,2, &nPet);
		checksum += util_deint(fd,3, &sPet);		
		util_deint(fd,4, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}	
		sprintf(errraw,"%d|%d",nPet,sPet);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_PETST_recv(fd, nPet, sPet);		
		util_DiscardMessage();
		return 0;
	}

#ifdef _CHECK_GAMESPEED
	if (func==LSSPROTO_CS_RECV) {
	    char buffer[2];
	    buffer[0] = '\0';
	  sprintf(errraw,"%s","空");
	  logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_CS_recv( fd );
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _TEAM_KICKPARTY
	if ( func == LSSPROTO_KTEAM_RECV ) {
		int checksum = 0, checksumrecv;
		int sindex;
		checksum += util_deint(fd, 2, &sindex);
		util_deint(fd, 3, &checksumrecv);
		if(checksum!=checksumrecv){
			util_DiscardMessage();
			logHack( fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d",sindex);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_KTEAM_recv( fd, sindex);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _MIND_ICON
	if(func==LSSPROTO_MA_RECV){
		int checksum = 0, checksumrecv;
		int nMind;
		int x, y;
		
		checksum += util_deint(fd, 2, &nMind);
		checksum += util_deint(fd, 3, &x);
		checksum += util_deint(fd, 4, &y);
		util_deint(fd, 5, &checksumrecv);
		if(checksum!=checksumrecv){
			util_DiscardMessage();
			logHack( fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}		
		sprintf(errraw,"%d|%d|%d",x,y,nMind);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_MA_recv(fd, x, y, nMind);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
	if (func==LSSPROTO_CHATROOM_RECV) {
		int checksum=0, checksumrecv;
		char test[1024 * 64];

		checksum += util_destring(fd,2, test);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s",test);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_CHATROOM_recv(fd, test);
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _NEWREQUESTPROTOCOL			// (不可开) Syu ADD 新增Protocol要求细项
	if ( func==LSSPROTO_RESIST_RECV ) {
	    char buffer[2];
	    buffer[0] = '\0';
	  sprintf(errraw,"%s","空");
	  logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_RESIST_recv( fd );
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
		if(func==LSSPROTO_BATTLESKILL_RECV){
				int checksum = 0, checksumrecv;
				int iNum;
				
				checksum += util_deint(fd, 2, &iNum);
				util_deint(fd, 3, &checksumrecv);
				if(checksum!=checksumrecv){
					util_DiscardMessage();
					logHack( fd, HACK_CHECKSUMERROR);
							//DME();
		return -1;
				}		
				sprintf(errraw,"%d",iNum);
				logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
				lssproto_BATTLESKILL_recv(fd, iNum);
				util_DiscardMessage();
				return 0;
		}
#endif
#ifdef _STREET_VENDOR
	if(func == LSSPROTO_STREET_VENDOR_RECV){
		int checksum = 0,checksumrecv;
		char message[1024 * 64];

		checksum += util_destring(fd,2,message);
		util_deint(fd,3,&checksumrecv);
		if(checksum != checksumrecv){
			util_DiscardMessage();
			logHack(fd,HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s",message);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_STREET_VENDOR_recv(fd,message);
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _JOBDAILY
	if(func == LSSPROTO_JOBDAILY_RECV){
		int checksum = 0,checksumrecv;
		char buffer[1024 * 32];

		checksum += util_destring(fd,2,buffer);
		util_deint(fd,3,&checksumrecv);
		if(checksum != checksumrecv){
			util_DiscardMessage();
			logHack(fd,HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s",buffer);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_JOBDAILY_recv(fd,buffer);
		util_DiscardMessage();
		return 0;

	}
#endif
#ifdef _TEACHER_SYSTEM
	if(func == LSSPROTO_TEACHER_SYSTEM_RECV){
		int checksum = 0,checksumrecv;
		char message[1024 * 64];

		checksum += util_destring(fd,2,message);
		util_deint(fd,3,&checksumrecv);
		if(checksum != checksumrecv){
			util_DiscardMessage();
			logHack(fd,HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%s",message);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_TEACHER_SYSTEM_recv(fd,message);
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _ADD_STATUS_2
	if(func == LSSPROTO_S2_RECV){
		return 0;
	}
#endif

#ifdef _PET_ITEM
	if (func==LSSPROTO_PET_ITEM_RECV) {
		int checksum=0, checksumrecv;
		int petindex;
		int x,y;
		int i,j,k,l,m,n;
		int fromindex;
		int toindex;
		
		checksum += util_deint(fd,2, &x);
		checksum += util_deint(fd,3, &y);
		checksum += util_deint(fd,4, &petindex);
		checksum += util_deint(fd,5, &fromindex);
		checksum += util_deint(fd,6, &toindex);
		util_deint(fd,7, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
			//logHack(fd, HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d|%d|%d",x,y,petindex,fromindex,toindex);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_PETITEM_recv( fd, x, y, petindex, fromindex, toindex );
		util_DiscardMessage();
		return 0;
	}
#endif

#ifdef _ONLINE_SHOP
	if(func == LSSPROTO_SHOP_RECV){
		int checksum = 0,checksumrecv;
		int shopflag,piece;

		checksum += util_deint(fd,2,&shopflag);
		checksum += util_deint(fd,3,&piece);
		util_deint(fd,4,&checksumrecv);
		if(checksum != checksumrecv){
			util_DiscardMessage();
			logHack(fd,HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d",shopflag,piece);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_SHOP_recv(fd,shopflag,piece);
		util_DiscardMessage();
		return 0;
	}
	
	if(func == LSSPROTO_SHOPBUY_RECV){
		int checksum = 0,checksumrecv;
		int shopflag,piece,shopno,buynum;

		checksum += util_deint(fd,2,&shopflag);
		checksum += util_deint(fd,3,&piece);
		checksum += util_deint(fd,4,&shopno);
		checksum += util_deint(fd,5,&buynum);
		util_deint(fd,6,&checksumrecv);
		if(checksum != checksumrecv){
			util_DiscardMessage();
			logHack(fd,HACK_CHECKSUMERROR);
					//DME();
		return -1;
		}
		sprintf(errraw,"%d|%d|%d|%d",shopflag,piece,shopno,buynum);
		logFengBao(fd,playcdkey,func,errraw);
		//print("\nfunc=%d,raw=%s\n",func,errraw);
		lssproto_SHOPBUY_recv(fd,shopflag,piece,shopno,buynum);
		util_DiscardMessage();
		return 0;
	}
#endif
#ifdef _CAX_ESC_FENGBAO
	if (func==LSSPROTO_ESC_RECV) {
		int escindex,checksumrecv;
		int checksum = 0;
		checksum += util_deint(fd,2, &escindex);
		util_deint(fd,3, &checksumrecv);
		if (checksum!=checksumrecv) {
			util_DiscardMessage();
		//	DME(); 
			return -1;
		}
		sprintf(errraw,"%d",escindex);
		logFengBao(fd,playcdkey,func,errraw);
		FreeSamenu("ESC_Recv",escindex,CONNECT_getCharaindex(fd));
		util_DiscardMessage();
		return 0;
	}	
#endif	
	printf("\n无法找到客户端接口=%d\n",func);

	util_DiscardMessage();
	logHack(fd,HACK_NOTDISPATCHED);
			//DME();
		return -1;
}

/*
   servertoclient XYD( int x, int y, int dir );
*/
void lssproto_XYD_send(int fd,int x,int y,int dir)
{
	if(fd<0) return;
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, x);
	checksum += util_mkint(fd,buffer, y);
	checksum += util_mkint(fd,buffer, dir);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_XYD_SEND, buffer);
}

void lssproto_EV_send(int fd,int seqno,int result)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, seqno);
	checksum += util_mkint(fd,buffer, result);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EV_SEND, buffer);
}

void lssproto_EN_send(int fd,int result,int field)
{
	char buffer[1024 * 64];
	int checksum=0;

	//print(" EN_send ");

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, result);
	checksum += util_mkint(fd,buffer, field);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EN_SEND, buffer);
}


void lssproto_RS_send(int fd,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RS_SEND, buffer);
}


void lssproto_RD_send(int fd,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RD_SEND, buffer);
}

void lssproto_B_send(int fd,char* command)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());
	checksum += util_mkstring(fd,buffer, command);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_B_SEND, buffer);
}

void lssproto_I_send(int fd,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_I_SEND, buffer);
}

void lssproto_SI_send(int fd,int fromindex,int toindex)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());
	checksum += util_mkint(fd,buffer, fromindex);
	checksum += util_mkint(fd,buffer, toindex);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SI_SEND, buffer);
}

void lssproto_MSG_send(int fd,int aindex,char* text,int color)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, aindex);
	checksum += util_mkstring(fd,buffer, text);
	checksum += util_mkint(fd,buffer, color);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_MSG_SEND, buffer);
}


void lssproto_PME_send(int fd,int objindex,int graphicsno,int x,int y,int dir,int flg,int no,char* cdata)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, objindex);
	checksum += util_mkint(fd,buffer, graphicsno);
	checksum += util_mkint(fd,buffer, x);
	checksum += util_mkint(fd,buffer, y);
	checksum += util_mkint(fd,buffer, dir);
	checksum += util_mkint(fd,buffer, flg);
	checksum += util_mkint(fd,buffer, no);
	checksum += util_mkstring(fd,buffer, cdata);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PME_SEND, buffer);
}


void lssproto_AB_send(int fd,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_AB_SEND, buffer);
}


void lssproto_ABI_send(int fd,int num,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, num);
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_ABI_SEND, buffer);
}

void lssproto_TK_send(int fd,int index,char* message,int color)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, index);
	checksum += util_mkstring(fd,buffer, message);
	checksum += util_mkint(fd,buffer, color);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_TK_SEND, buffer);
}

void lssproto_MC_send(int fd,int fl,int x1,int y1,int x2,int y2,int tilesum,int objsum,int eventsum,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, fl);
	checksum += util_mkint(fd,buffer, x1);
	checksum += util_mkint(fd,buffer, y1);
	checksum += util_mkint(fd,buffer, x2);
	checksum += util_mkint(fd,buffer, y2);
	checksum += util_mkint(fd,buffer, tilesum);
	checksum += util_mkint(fd,buffer, objsum);
	checksum += util_mkint(fd,buffer, eventsum);
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_MC_SEND, buffer);
}


void lssproto_M_send(int fd,int fl,int x1,int y1,int x2,int y2,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, fl);
	checksum += util_mkint(fd,buffer, x1);
	checksum += util_mkint(fd,buffer, y1);
	checksum += util_mkint(fd,buffer, x2);
	checksum += util_mkint(fd,buffer, y2);
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_M_SEND, buffer);
}

void lssproto_C_send(int fd,char* data)
{
#ifdef	_WAN_FIX
	if(getVsFlg()==2){
		int charaindex = -1;
		charaindex = CONNECT_getCharaindex( fd );
		if(CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER){
			char sendmsg[20];
			sprintf(sendmsg,"%d|%d",CHAR_getInt(charaindex,CHAR_X),CHAR_getInt(charaindex,CHAR_Y));
			if(getStayEncount(fd)==1 && strstr(data,sendmsg)==NULL && CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE)
				return;
		}
	}else if(getVsFlg()==1){
		int charaindex = -1;
		charaindex = CONNECT_getCharaindex( fd );
		if(CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER){
			char sendmsg[20];
			sprintf(sendmsg,"%d|%d",CHAR_getInt(charaindex,CHAR_X),CHAR_getInt(charaindex,CHAR_Y));
			if(getStayEncount(fd)==1 && strstr(data,sendmsg)==NULL && CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER)
				return;
			if(CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT){
				int partyindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1);
				if(CHAR_CHECKINDEX(partyindex)){
					int partyfd = getfdFromCharaIndex( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1));
				 	if(getStayEncount(partyfd)==1)
				 		return;
				}
			}
		}
	}else if(getVsFlg()==3){
		int charaindex = -1;
		charaindex = CONNECT_getCharaindex( fd );
		if(CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER){
			char sendmsg[20];
			sprintf(sendmsg,"%d|%d",CHAR_getInt(charaindex,CHAR_X),CHAR_getInt(charaindex,CHAR_Y));
			if(CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT){
				int partyindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1);
				if(CHAR_CHECKINDEX(partyindex)){
					int partyfd = getfdFromCharaIndex( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1));
				 	if(getStayEncount(partyfd)==1)
				 		return;
				}
			}
		}
	}
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_C_SEND, buffer);
}

void lssproto_CA_send(int fd,char* data)
{
#ifdef	_WAN_FIX
	if(getVsFlg()==2){
		int charaindex = -1;
		charaindex = CONNECT_getCharaindex( fd );
		if(CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER){
			char sendmsg[20];
			sprintf(sendmsg,"%d|%d",CHAR_getInt(charaindex,CHAR_X),CHAR_getInt(charaindex,CHAR_Y));
			if(getStayEncount(fd)==1 && strstr(data,sendmsg)==NULL && CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_NONE)
				return;
		}
	}else if(getVsFlg()==1){
		int charaindex = -1;
		charaindex = CONNECT_getCharaindex( fd );
		if(CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER){
			char sendmsg[20];
			sprintf(sendmsg,"%d|%d",CHAR_getInt(charaindex,CHAR_X),CHAR_getInt(charaindex,CHAR_Y));
			if(getStayEncount(fd)==1 && strstr(data,sendmsg)==NULL && CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER)
				return;
			if(CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT){
				int partyindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1);
				if(CHAR_CHECKINDEX(partyindex)){
					int partyfd = getfdFromCharaIndex( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1));
				 	if(getStayEncount(partyfd)==1)
				 		return;
				}
			}
		}
	}else if(getVsFlg()==3){
		int charaindex = -1;
		charaindex = CONNECT_getCharaindex( fd );
		if(CHAR_getInt( charaindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER){
			char sendmsg[20];
			sprintf(sendmsg,"%d|%d",CHAR_getInt(charaindex,CHAR_X),CHAR_getInt(charaindex,CHAR_Y));
			if(CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_CLIENT){
				int partyindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1);
				if(CHAR_CHECKINDEX(partyindex)){
					int partyfd = getfdFromCharaIndex( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1));
				 	if(getStayEncount(partyfd)==1)
				 		return;
				}
			}
		}
	}
#endif
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CA_SEND, buffer);
}

void lssproto_CD_send(int fd,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CD_SEND, buffer);
}

void lssproto_R_send(int fd,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_R_SEND, buffer);
}

void lssproto_S_send(int fd,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_S_SEND, buffer);
}

void lssproto_D_send(int fd,int category,int dx,int dy,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, category);
	checksum += util_mkint(fd,buffer, dx);
	checksum += util_mkint(fd,buffer, dy);
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_D_SEND, buffer);
}

void lssproto_FS_send(int fd,int flg)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, flg);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_FS_SEND, buffer);
}

void lssproto_HL_send(int fd,int flg)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, flg);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_HL_SEND, buffer);
}

void lssproto_PR_send(int fd,int request,int result)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, request);
	checksum += util_mkint(fd,buffer, result);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PR_SEND, buffer);
}

#ifdef _PETS_SELECTCON
void lssproto_PETS_send(int fd,int petarray,int result)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, petarray);
	checksum += util_mkint(fd,buffer, result);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PETST_SEND, buffer);
}
#endif

void lssproto_KS_send(int fd,int petarray,int result)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, petarray);
	checksum += util_mkint(fd,buffer, result);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_KS_SEND, buffer);
}

void lssproto_SPET_send(int fd, int standbypet, int result)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, standbypet);
	checksum += util_mkint(fd,buffer, result);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SPET_SEND, buffer);
}

void lssproto_PS_send(int fd,int result,int havepetindex,int havepetskill,int toindex)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, result);
	checksum += util_mkint(fd,buffer, havepetindex);
	checksum += util_mkint(fd,buffer, havepetskill);
	checksum += util_mkint(fd,buffer, toindex);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PS_SEND, buffer);
}

void lssproto_SKUP_send(int fd,int point)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, point);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SKUP_SEND, buffer);
}
void lssproto_WN_send(int fd,int windowtype,int buttontype,int seqno,int objindex,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;
#ifdef _NO_WARP
	// shan hanjj add Begin
    CONNECT_set_seqno(fd,seqno);
    CONNECT_set_selectbutton(fd,buttontype);    
	// shan End
#endif

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, windowtype);
	checksum += util_mkint(fd,buffer, buttontype);
	checksum += util_mkint(fd,buffer, seqno);
	checksum += util_mkint(fd,buffer, objindex);
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_WN_SEND, buffer);
	
}

void lssproto_EF_send(int fd,int effect,int level,char* option)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, effect);
	checksum += util_mkint(fd,buffer, level);
	checksum += util_mkstring(fd,buffer, option);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_EF_SEND, buffer);
}
void lssproto_SE_send(int fd,int x,int y,int senumber,int sw)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, x);
	checksum += util_mkint(fd,buffer, y);
	checksum += util_mkint(fd,buffer, senumber);
	checksum += util_mkint(fd,buffer, sw);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SE_SEND, buffer);
}

void lssproto_ClientLogin_send(int fd,char* result)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());
	checksum += util_mkstring(fd,buffer, result);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CLIENTLOGIN_SEND, buffer);
}

void lssproto_CreateNewChar_send(int fd,char* result,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, result);
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CREATENEWCHAR_SEND, buffer);
}

void lssproto_CharDelete_send(int fd,char* result,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, result);
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARDELETE_SEND, buffer);
}

void lssproto_CharLogin_send(int fd,char* result,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, result);
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLOGIN_SEND, buffer);
}

void lssproto_CharList_send(int fd,char* result,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, result);
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLIST_SEND, buffer);
}

void lssproto_CharLogout_send(int fd,char* result,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, result);
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHARLOGOUT_SEND, buffer);
}

void lssproto_ProcGet_send(int fd,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");
	
	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PROCGET_SEND, buffer);
}

void lssproto_PlayerNumGet_send(int fd,int logincount,int player)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, logincount);
	checksum += util_mkint(fd,buffer, player);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_PLAYERNUMGET_SEND, buffer);
}

void lssproto_Echo_send(int fd,char* test)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, test);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_ECHO_SEND, buffer);
}

// CoolFish: Trade 2001/4/18

void lssproto_TD_send(int fd, int index, char* message)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, message);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_TD_SEND, buffer);
}
#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
void lssproto_CHATROOM_send ( int fd , char* message )
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, message);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_CHATROOM_SEND, buffer);
}
#endif
#ifdef _NEWREQUESTPROTOCOL			// (不可开) Syu ADD 新增Protocol要求细项
void lssproto_RESIST_send ( int fd , char* message )
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, message);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RESIST_SEND, buffer);
}
#endif
#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
void lssproto_BATTLESKILL_send ( int fd , char* message )
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, message);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_BATTLESKILL_SEND, buffer);
}
#endif
void lssproto_NU_send(int fd, int nu)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, nu);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_NU_SEND, buffer);
}


void lssproto_FM_send(int fd, char* message)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, message);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_FM_SEND, buffer);
}


void lssproto_WO_send(int fd,int effect)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, effect);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_WO_SEND, buffer);
}
#ifdef _ITEM_CRACKER
void lssproto_IC_send(int fd, int x, int y)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, x);
	checksum += util_mkint(fd,buffer, y);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_IC_SEND, buffer);
}
#endif
#ifdef _MAGIC_NOCAST             // 精灵:沉默
void lssproto_NC_send(int fd,int flg)
{
	if(fd<0) return;
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, flg);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_NC_SEND, buffer);
}
#endif

#ifdef _STREET_VENDOR
void lssproto_STREET_VENDOR_send(int fd,char *message)
{
	char buffer[1024 * 64];
	int checksum = 0;

	strcpy(buffer,"");

	CONNECT_getCdkey(fd,PersonalKey,4096);
	strcat(PersonalKey,getRunningKey());

	checksum += util_mkstring(fd,buffer,message);
	util_mkint(fd,buffer,checksum);
	util_SendMesg(fd,LSSPROTO_STREET_VENDOR_SEND,buffer);
}
#endif

#ifdef _RIGHTCLICK
void lssproto_RCLICK_send(int fd, int type, char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	print("\n RCLICK_send( type=%d data=%s) ", type, data );

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, type);
	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_RCLICK_SEND, buffer);
}
#endif

#ifdef _JOBDAILY
void lssproto_JOBDAILY_send(int fd,char *data)
{
	char buffer[1024 * 32];
	int checksum = 0;

	strcpy(buffer,"");

	CONNECT_getCdkey(fd,PersonalKey,4096);
	strcat(PersonalKey,getRunningKey());

	checksum += util_mkstring(fd,buffer,data);
	util_mkint(fd,buffer,checksum);
	util_SendMesg(fd,LSSPROTO_JOBDAILY_SEND,buffer);
}
#endif

#ifdef _TEACHER_SYSTEM
void lssproto_TEACHER_SYSTEM_send(int fd,char *data)
{
	char buffer[1024 * 64];
	int checksum = 0;

	strcpy(buffer,"");

	CONNECT_getCdkey(fd,PersonalKey,4096);
	strcat(PersonalKey,getRunningKey());

	checksum += util_mkstring(fd,buffer,data);
	util_mkint(fd,buffer,checksum);
	util_SendMesg(fd,LSSPROTO_TEACHER_SYSTEM_SEND,buffer);
}
#endif

#ifdef _ADD_STATUS_2
void lssproto_S2_send(int fd,char* data)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_S2_SEND, buffer);
}
#endif

#ifdef _ONLINE_SHOP
void lssproto_SHOP_send(int fd,int shopnum,int vippoint,int piece,int pieces,int shopflag,char* res)
{
	char buffer[1024 * 64];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkint(fd,buffer, shopnum);
	checksum += util_mkint(fd,buffer, vippoint);
	checksum += util_mkint(fd,buffer, piece);
	checksum += util_mkint(fd,buffer, pieces);
	checksum += util_mkint(fd,buffer, shopflag);
	checksum += util_mkstring(fd,buffer, res);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_SHOP_SEND, buffer);
}
#endif

void lssproto_Upmap_send(int fd,int formmap,int tomap)
{
	char buffer[1024 * 64];
	int checksum=0;
	fd = getfdFromCharaIndex(fd);
	strcpy(buffer,"");
	checksum+=util_mkint(fd,buffer, formmap);
	checksum+=util_mkint(fd,buffer, tomap);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_UPMAP, buffer);
}

#ifdef _CAX_DENGON_GG
void lssproto_DENGON_send(int fd, char *data, int color, int num)
{
	if( CONNECT_checkfd(fd) == FALSE )return;
#ifdef _DEBUG_SEND_CLI
	printf("[发送]lssproto_DENGON_send-data:%s,%d,%d\n", data, color, num);
#endif
	char buffer[65500];
	int checksum=0;

	strcpy(buffer,"");

	CONNECT_getCdkey( fd, PersonalKey, 4096);
	strcat(PersonalKey, getRunningKey());

	checksum += util_mkstring(fd,buffer, data);
	checksum += util_mkint(fd,buffer, color);
	checksum += util_mkint(fd,buffer, num);
	util_mkint(fd,buffer, checksum);
	util_SendMesg(fd, LSSPROTO_DENGON_SEND, buffer);
}
#endif
