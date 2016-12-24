#include "version.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "common.h"
#include "util.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "npc_exchangeman.h"
#include "net.h"
#include "char.h"
#include "object.h"
#include "readmap.h"
#include "addressbook.h"
#include "handletime.h"
#include "configfile.h"
#include "event.h"
#include "pet.h"
#include "battle.h"
#include "battle_command.h"
#include "magic.h"
#include "petmail.h"
#include "item_gen.h"
#include "pet_skill.h"
#include "log.h"  //add this because the second had it
#include "map_deal.h" // CoolFish: 2001/4/18
#include "trade.h" // CoolFish: Trade 2001/4/18
#include "family.h" // CoolFish: Family 2001/5/24
#include "item_event.h" // shan: blackmarket
#include "npc_petshop.h"
#include "enemy.h"
#include "char_base.h"
#ifdef _PROFESSION_SKILL			// WON ADD 人物职业技能
#include "profession_skill.h"
#endif
#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
#include "chatroom.h"
#endif
#ifdef _ALLBLUES_LUA
#include "mylua/function.h"
#endif
BOOL checkStringErr( char * );

#ifdef _AVOIEDamageCharName
int
dbKeyHasDamageChar( char* key)
{
	if( strstr(key , "'") != NULL  )
	{
		printf("test . key damage\n");
		return 1;
	}
	return 0;
}
#endif

// shan add
extern struct FM_PKFLOOR fmpkflnum[FAMILY_FMPKFLOOR];
extern char question[64];
extern char answer[64];
extern int questionstarttime;
extern int questionman;
#ifdef _UNTEXT_TALK
extern int textcnt;
#endif
extern int sameipnum;
extern int player_online;
extern int autopkcnt;
char mmcdkey[10][32]={"空","空","空","空","空","空","空","空","空","空"};
char mmcdkeybuf[10][64]={"-1.-1.-1","-1.-1.-1","-1.-1.-1","-1.-1.-1","-1.-1.-1","-1.-1.-1","-1.-1.-1","-1.-1.-1","-1.-1.-1","-1.-1.-1",};
//static unsigned long cdkeycount=0;
//static unsigned long cdkeymaxnum=100000;
//static char** playcdkey=NULL;
//static unsigned long* cdkeylogintime=0;
//static int* cdkeylogincount=0;
/* -----------------------------------------------------------------------
 * ----------------------------------------------------------------------*/
static int Callfromcli_Util_getTargetCharaindex( int fd, int toindex)
{
	int	to_charaindex = -1;
    int fd_charaindex = CONNECT_getCharaindex( fd );

	/* 愤坌愤褥 */
	if( toindex == 0 ) {
		to_charaindex = fd_charaindex;
	}
	else if( toindex > 0 && toindex < 6 ) {
		to_charaindex = CHAR_getCharPet( fd_charaindex, toindex-1);
		if( !CHAR_CHECKINDEX( to_charaindex)) {
			to_charaindex = -1;
		}
	}
	else if( toindex > 5 && toindex < 11 ) {
		to_charaindex = CHAR_getPartyIndex( fd_charaindex, toindex - 6);
	}
	return to_charaindex;
}

#ifdef _MO_LOGINCHECK
#define MX (z>>5^y<<2) + (y>>3^z<<4)^(sum^y) + (k[p&3^e]^z);
long btea(long* v, long n, long* k) {
    unsigned long z=v[n-1], y=v[0], sum=0, e, DELTA=0x9e3779b9;
    long p, q ;
    if (n > 1) {          /* Coding Part */
      q = 6 + 52/n;
      while (q-- > 0) {
        sum += DELTA;
        e = (sum >> 2) & 3;
        for (p=0; p<n-1; p++) y = v[p+1], z = v[p] += MX;
        y = v[0];
        z = v[n-1] += MX;
      }
      return 0 ;
    } else if (n < -1) {  /* Decoding Part */
      n = -n;
      q = 6 + 52/n;
      sum = q*DELTA ;
      while (sum != 0) {
        e = (sum >> 2) & 3;
        for (p=n-1; p>0; p--) z = v[p-1], y = v[p] -= MX;
        z = v[n-1];
        y = v[0] -= MX;
        sum -= DELTA;
      }
      return 0;
    }
    return 1;
}
#endif
int check_jqm(char *jqm){
	int i;
	int len = strlen(jqm);
	for(i=0;i<len;i++){
		if ( (jqm[i]>47&&jqm[i]<58) || (jqm[i]>64 &&jqm[i]<91) || (jqm[i]>98 &&jqm[i]<123))
    {
      continue;
    }
    return -1;
  }
  return 1;
}

//int cdkeyinit(void){
//	playcdkey = (char**)calloc(cdkeymaxnum, sizeof(char*));
//	int tempi;
//	for(tempi=0;tempi<cdkeymaxnum;tempi++){
//		playcdkey[tempi] = (char*)calloc(12, sizeof(char));
//		playcdkey[tempi][0] = 0;
//	}
//	if(cdkeylogintime=0) cdkeylogintime = malloc(sizeof(unsigned long)*cdkeymaxnum);
//	if(cdkeylogincount=0) cdkeylogincount = malloc(sizeof(int)*cdkeymaxnum);
//	
//	return 1;
//}

/*----------------------------------------
 * 弁仿奶失件玄互夫弘奶件允月 匹手丢乒伉卞卺户月分仃卅及匹民尼永弁反卅中
 ----------------------------------------*/
void lssproto_ClientLogin_recv( int fd,char* cdkey, char* passwd )
{
    /*  2褐卞仇木互裟壬木月及反中中    */
    {//ttom avoid the restore 2001/01/09
			if(CONNECT_isNOTLOGIN(fd)==FALSE){
				print("\n the Client had  Logined fd=%d",fd);
				return;
			}
    }
    
    
    
    //print( "CliLogin cdkey=%s\n" , cdkey );
    /* connect卞戊疋□允月 */
    CONNECT_setCdkey( fd, cdkey );
    CONNECT_setPasswd( fd, passwd );
    //CONNECT_setCtype( fd, CLI );
    
/*    if(getLoginJqmType()==1){   //开启机器码后封包无机器码不允许登录
    	//CONNECT_endOne_debug(fd);
    	lssproto_ClientLogin_send( fd , "您的客户端不完整，请联系客服。" );
	  	return;
    }*/

    unsigned long tmpip;
    int a,b,c,d,i;
    char ip[32];
    tmpip=CONNECT_get_userip(fd);
    a=(tmpip % 0x100); tmpip=tmpip / 0x100;
    b=(tmpip % 0x100); tmpip=tmpip / 0x100;
    c=(tmpip % 0x100); tmpip=tmpip / 0x100;
    d=(tmpip % 0x100);
    sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
    //print( "\n登陆账号=%s 密码=%s 来自=%s\n",cdkey,passwd,ip);
   {
	int res;

	if (strlen(cdkey)==0 || strlen(passwd)==0 || strlen(ip)==0){
	  //printf("登陆信息有错误！\n");
	  lssproto_ClientLogin_send( fd , "no" );
	  CONNECT_endOne_debug(fd);
	  return;
	}
	if (sasql_chehk_lock(cdkey)){
		//printf("该账号%s禁止登陆!\n",cdkey);
	  lssproto_ClientLogin_send( fd , "no" );
	  CONNECT_endOne_debug(fd);
	  return;
	}
	if (sasql_chehk_lock(ip)){
		//printf("该IP%s禁止登陆!\n",ip);
	  lssproto_ClientLogin_send( fd , "no" );
	  CONNECT_endOne_debug(fd);
	  return;
	}
	if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
		if ( getSameIpNum()>0 && sasql_onlinenum(cdkey,ip) >= getSameIpNum() ){
			//printf("同一IP%s禁止同时登陆%d次!\n",ip,sameipnum);
	  	lssproto_ClientLogin_send( fd , "no" );
	 		CONNECT_endOne_debug(fd);
	  	return;
		}
	}
	res=sasql_query(cdkey,passwd);
	if (res==3){
		if(getNoCdkeyPlayer()>0 && fd-player_online>=getNoCdkeyPlayer() && getNoCdkeyMode()!=0){
			if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
				if(getNoCdkeyType()==0){
					//if(sasql_query_online_ip(ip)==0 && sasql_check_lockip(ip,getFengType())<1){
					if(sasql_query_online_ip(ip)==0){
						//if(sasql_add_lockip(ip,getFengType(),getServernumber(),2)==1){
#ifdef _CAX_OFF_IPTABLES						
						if(strcmp(ip,"0.0.0.0")!=0){
							char systemstr[128];
							sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
							system(systemstr);
							logWrongCdkey(ip);
						}
#endif						
						CONNECT_endOne_debug(fd);
						return;
						//}
					}
				}else{
#ifdef _CAX_OFF_IPTABLES					
					if(strcmp(ip,"0.0.0.0")!=0){
						char systemstr[128];
						sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
						system(systemstr);
						logWrongCdkey(ip);
					}
#endif					
					CONNECT_endOne_debug(fd);	
					return;
				}
			}
		}
		if(!sasql_register(cdkey,passwd))
		{//账号没有注册
			lssproto_ClientLogin_send( fd , "no" );
//			if(getSaType()==8018)
//	  		CONNECT_endOne_debug(fd);
	  	return;
		}
	}else if (res!=1){
		if(getNoCdkeyPlayer()>0 && fd-player_online>=getNoCdkeyPlayer() && getNoCdkeyMode()==2){
			if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
				if(getNoCdkeyType()==0){
					//if(sasql_query_online_ip(ip)==0 && sasql_check_lockip(ip,getFengType())<1){
					if(sasql_query_online_ip(ip)==0){
						//if(sasql_add_lockip(ip,getFengType(),getServernumber(),2)==1){
#ifdef _CAX_OFF_IPTABLES						
						if(strcmp(ip,"0.0.0.0")!=0){
							char systemstr[128];
							sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
							system(systemstr);
							logWrongCdkey(ip);
						}
#endif						
						CONNECT_endOne_debug(fd);
						return;
						//}
					}
				}else{
#ifdef _CAX_OFF_IPTABLES					
					if(strcmp(ip,"0.0.0.0")!=0){
						char systemstr[128];
						sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
						system(systemstr);
						logWrongCdkey(ip);
					}
#endif					
					CONNECT_endOne_debug(fd);
					return;
				}
			}
		}
		lssproto_ClientLogin_send( fd , "no" );;
//		if(getSaType()==8018)
//	  	CONNECT_endOne_debug(fd);
	  return;
	}
	if(sasql_cdkey_online(cdkey)>0){
		saacproto_ACKick_send(acfd,cdkey,-1,1);
		lssproto_ClientLogin_send( fd , "no" );
		sasql_online(cdkey,NULL,NULL,NULL,NULL,0);
	  CONNECT_endOne_debug(fd);
	  return;
	}
	CONNECT_setCtype( fd, CLI );
	lssproto_ClientLogin_send( fd , "ok" );
	sasql_online(cdkey,NULL,ip,NULL,NULL,4);
	}

}
void lssproto_ClientLoginJqm_recv( int fd,char* cdkey, char* passwd, char* jqm 
#ifdef _CAX_NEWMAC_LOGIN
,char* mac 
#endif
#ifdef _MO_LOGINCHECK
,char* szdata
#endif
 )
{
    /*  2褐卞仇木互裟壬木月及反中中    */
    {//ttom avoid the restore 2001/01/09
			if(CONNECT_isNOTLOGIN(fd)==FALSE){
				print("\n the Client had  Logined fd=%d",fd);
				return;
			}
    }
//		print("\n fd: %d",fd);
//		print("\n cdkey: %s",cdkey);
//		print("\n password: %s",passwd);
//		print("\n jqm: %s",jqm);
 //     print("\n 1-mac: %s",mac);

    char* newjqm[128];
    int logintype = -1;
    char buftest[128];
    memset(newjqm,0,sizeof(newjqm));
    memset(buftest,0,sizeof(buftest));
    strcpy(newjqm,jqm);
    if(strstr(jqm,"|")!=NULL){
    	if(getStringFromIndexWithDelim(jqm,"|", 1, buftest, sizeof(buftest)) == FALSE)
				strcpy(newjqm,jqm);
			else
				strcpy(newjqm,buftest);
			if(getStringFromIndexWithDelim(jqm,"|", 2, buftest, sizeof(buftest)) == FALSE)
				logintype = -1;
			else
				logintype = atoi(buftest);
    }
    jqm = newjqm;
    
 //   print("\n logintype : %d",logintype);
 //   print("\n jqm 2 : %s",jqm);
    
    /*  if(check_jqm(jqm)==-1){
    	lssproto_ClientLogin_send( fd , "登录错误，请重新登录。" );
    	return;
    }*/
    
    //print( "CliLogin cdkey=%s\n" , cdkey );
    /* connect卞戊疋□允月 */
    CONNECT_setCdkey( fd, cdkey );
    CONNECT_setPasswd( fd, passwd );
    CONNECT_setPcid(fd, jqm);
#ifdef _CAX_NEWMAC_LOGIN   
    CONNECT_setMAC(fd, mac);
#endif   

 if (sasql_check_lockmac(mac)>0||sasql_check_lockpcid(jqm)>0){
		lssproto_ClientLogin_send( fd , "连接超时，请联系GM。" );
		CONNECT_endOne_debug(fd);
    }

#ifdef _MO_LOGINCHECK
	long n = XXTEA_ROUND;
	long k[4] = {XXTEA_KEY_1,XXTEA_KEY_2,XXTEA_KEY_3,XXTEA_KEY_4};
	char data[1024] = "";
	int datalen = 0;
	int j = 0;
	char demac[20];
	char detime[20];
	long recvtime;
	//密文转换明文
	//printf("Hex data: %s\n", szdata);
	datalen = strlen(szdata);

	for(j = 0; j < datalen/2; j++){
		int temp;
		sscanf(&szdata[2 * j], "%02X", &temp);
		data[j] = (char)temp;
    }
	data[datalen/2] = '\0';
	//printf("Bin data: \n %s\n", data);
	btea((long*)data, -n, k);   // 解密数据
	//printf( "Decrypt Data:\n %s\n", data);

	sscanf(data,"%[^=]=%[^\n]",&demac,&detime);
	//printf("\n demac=%s,mac=%s\n",demac, mac);
	if(strstr(demac,mac)==NULL){
			lssproto_ClientLogin_send( fd , "登录验证错误，请联系GM。" );
			CONNECT_endOne_debug(fd);
			return;
	}
	recvtime=atoi(detime);
	//获取系统时间
	time_t timep;
	struct tm *p;
	timep = time (NULL);
	p = localtime ( &timep );
	if(abs(timep-recvtime)>86400){
		lssproto_ClientLogin_send( fd , "连接超时，请联系GM。" );
		CONNECT_endOne_debug(fd);
		return;
	}
#endif


    //CONNECT_setCtype( fd, CLI );
    if(getLoginType()>0){
    	if(logintype == -1){
    		lssproto_ClientLogin_send( fd , "请更新客户端后登录本线路。" );
    		return;
    	}else if(logintype!=0 && logintype!=1){
    		lssproto_ClientLogin_send( fd , "此线路只支持无外挂或者STW外挂登录。" );
    		return;
    	}
    }

    unsigned long tmpip;
    int a,b,c,d,i;
    char ip[32];
    tmpip=CONNECT_get_userip(fd);
    a=(tmpip % 0x100); tmpip=tmpip / 0x100;
    b=(tmpip % 0x100); tmpip=tmpip / 0x100;
    c=(tmpip % 0x100); tmpip=tmpip / 0x100;
    d=(tmpip % 0x100);
    sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
    //print( "\n登陆账号=%s 密码=%s 来自=%s\n",cdkey,passwd,ip);
   {
	int res;

	if (strlen(cdkey)==0 || strlen(passwd)==0 || strlen(ip)==0){
	  //printf("登陆信息有错误！\n");
	  lssproto_ClientLogin_send( fd , "no" );
	  CONNECT_endOne_debug(fd);
	  return;
	}
	if (sasql_chehk_lock(jqm)){
		//printf("该账号%s禁止登陆!\n",cdkey);
	  lssproto_ClientLogin_send( fd , "由于您的严重违规行为，您已被例如黑名单，无法登陆本游戏。" );
	  //CONNECT_endOne_debug(fd);
	  return;
	}
	if (sasql_chehk_lock(ip)){
		//printf("该IP%s禁止登陆!\n",ip);
	  lssproto_ClientLogin_send( fd , "no" );
	  CONNECT_endOne_debug(fd);
	  return;
	}
	
		if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
			if ( getSameIpNum()>0 && sasql_onlinenum(cdkey,ip) >= getSameIpNum() ){
				//printf("同一IP%s禁止同时登陆%d次!\n",ip,sameipnum);
	  		//lssproto_ClientLogin_send( fd , "no" );
	  		//char tmpbuf[128];
	  		//sprintf(tmpbuf,"您的登录数已达上限（%d个ID），无法进入游戏。",getSameIpNum());
	  		lssproto_ClientLogin_send( fd , "您的登录数已达上限，无法进入游戏。" );
	 			//CONNECT_endOne_debug(fd);
	  		return;
			}
		}
		if( getSameJqmNum()>0 && sasql_onlinejqmnum(jqm,1) >= getSameJqmNum() ){
			//lssproto_ClientLogin_send( fd , "no" );
	 		//CONNECT_endOne_debug(fd);
	 		//char tmpbuf[128];
	 		//sprintf(tmpbuf,"您在该线的登录数已达到上限（%d个ID），请尝试登陆其他线路。",getSameJqmNum());
	 		lssproto_ClientLogin_send( fd , "您在该线的登录数已达到上限，请尝试登陆其他线路。" );
	 		//printf("同一IP%s禁止同时登陆%d次!\n");
	  	return;
		}
		if( getSameJqmAllNum()>0 && sasql_onlinejqmnum(jqm,2) >= getSameJqmAllNum() ){
			//lssproto_ClientLogin_send( fd , "no" );
	 		//CONNECT_endOne_debug(fd);
	 		//char tmpbuf[128];
	 		//sprintf(tmpbuf,"您的登录数已达上限（%d个ID），无法进入游戏。",getSameJqmAllNum());
	 		lssproto_ClientLogin_send( fd , "您的登录数已达上限，无法进入游戏。" );
	 		//printf("您的登录数已达上限，无法进入游戏\n");
	  	return;
		}
	res=sasql_query(cdkey,passwd);
	//    print("\n res 2 : %d",res);
	if (res==3){
		if(getNoCdkeyPlayer()>0 && fd-player_online>=getNoCdkeyPlayer() && getNoCdkeyMode()!=0){
			if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
				if(getNoCdkeyType()==0){
					//if(sasql_query_online_ip(ip)==0 && sasql_check_lockip(ip,getFengType())<1){
					if(sasql_query_online_ip(ip)==0){
						//if(sasql_add_lockip(ip,getFengType(),getServernumber(),2)==1){
#ifdef _CAX_OFF_IPTABLES						
						if(strcmp(ip,"0.0.0.0")!=0){
							char systemstr[128];
							sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
							system(systemstr);
							logWrongCdkey(ip);
						}
#endif						
						CONNECT_endOne_debug(fd);
						return;
						//}
					}
				}else{
#ifdef _CAX_OFF_IPTABLES					
					if(strcmp(ip,"0.0.0.0")!=0){
						char systemstr[128];
						sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
						system(systemstr);
						logWrongCdkey(ip);
					}
#endif					
					CONNECT_endOne_debug(fd);	
					return;
				}
			}
		}
		if(!sasql_register(cdkey,passwd))
		{//账号没有注册
			//lssproto_ClientLogin_send( fd , "no" );
			lssproto_ClientLogin_send( fd , "您的账号不存在，如未注册，请到官方网站注册。" );
//			if(getSaType()==8018)
//	  		CONNECT_endOne_debug(fd);
	  	return;
		}
	}else if (res!=1){
		if(getNoCdkeyPlayer()>0 && fd-player_online>=getNoCdkeyPlayer() && getNoCdkeyMode()==2){
			if(strcmp(ip,getNoAttIp(0))!=0 && strcmp(ip,getNoAttIp(1))!=0 && strcmp(ip,getNoAttIp(2))!=0 && strcmp(ip,getNoAttIp(3))!=0 && strcmp(ip,getNoAttIp(4))!=0){
				if(getNoCdkeyType()==0){
					//if(sasql_query_online_ip(ip)==0 && sasql_check_lockip(ip,getFengType())<1){
					if(sasql_query_online_ip(ip)==0){
						//if(sasql_add_lockip(ip,getFengType(),getServernumber(),2)==1){
#ifdef _CAX_OFF_IPTABLES						
						if(strcmp(ip,"0.0.0.0")!=0){
							char systemstr[128];
							sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
							system(systemstr);
							logWrongCdkey(ip);
						}
#endif						
						CONNECT_endOne_debug(fd);
						return;
						//}
					}
				}else{
#ifdef _CAX_OFF_IPTABLES					
					if(strcmp(ip,"0.0.0.0")!=0){
						char systemstr[128];
						sprintf(systemstr,"iptables -I INPUT -s %s -j DROP",ip);
						system(systemstr);
						logWrongCdkey(ip);
					}
#endif					
					CONNECT_endOne_debug(fd);
					return;
				}
			}
		}
		//密码错误
		//lssproto_ClientLogin_send( fd , "no" );
		lssproto_ClientLogin_send( fd , "您的密码错误，请仔细确认后登陆。" );
//		if(getSaType()==8018)
//	  	CONNECT_endOne_debug(fd);
	  return;
	}
	if(sasql_cdkey_online(cdkey)>0){
		saacproto_ACKick_send(acfd,cdkey,-1,1);
		lssproto_ClientLogin_send( fd , "no" );
		sasql_online(cdkey,NULL,NULL,NULL,NULL,0);
	  CONNECT_endOne_debug(fd);
	  return;
	}
	CONNECT_setCtype( fd, CLI );
	char* oldpcid = NULL;
	oldpcid = sasql_query_jqm(cdkey);
#ifdef _CAX_NEWMAC_LOGIN	
	char* oldmac = NULL;
	oldmac = sasql_query_mac(cdkey);
#endif


/*	if(strlen(oldpcid)<10){
		CONNECT_setOldPcid(fd,jqm);
	}else{
		CONNECT_setOldPcid(fd,oldpcid);
	}*/
	
	lssproto_ClientLogin_send( fd , "ok" );
	
	//print("\n jqm: %s",jqm);
	sasql_online_jqm(cdkey,jqm,ip,logintype);
#ifdef _CAX_NEWMAC_LOGIN	
	sasql_online_mac(cdkey,mac,ip,logintype);
#endif	
	}

}

void lssproto_CreateNewChar_recv( int fd,int dataplacenum,char* charname,
								  int imgno,int faceimgno,
								  int vital,int str,int tgh,int dex,
								  int earth,int water,int fire,int wind,
								  int hometown )
{
    char cdkey[CDKEYLEN];

    if( CONNECT_isCLI( fd ) == FALSE )return;

    if( CONNECT_isNOTLOGIN(fd) == FALSE ){
        lssproto_CreateNewChar_send( fd, FAILED, "Not NOTLOGIN State\n" );
        return;
    }
    if( strlen( charname ) == 0 ){
        lssproto_CreateNewChar_send(fd,FAILED, "名字过短，无法建立人物\n");
        return;
#ifdef _AVOIEDamageCharName
		}else if ( dbKeyHasDamageChar(charname)){
			lssproto_CreateNewChar_send(fd,FAILED, "名字禁止使用\n");
			return;
#endif
    }else if( strlen(charname) >= 32 ){
        lssproto_CreateNewChar_send(fd,FAILED, "名字过长，无法建立人物。\n");
        return;
    // Nuke start 0711: Avoid naming as WAEI
    }else if (strstr(charname,"华义")
// WON ADD
			 || strstr(charname,"gm")   || strstr(charname,"GM")
			 || strstr(charname,"Gm")   || strstr(charname,"gM")
			 || strstr(charname,"ｇｍ") || strstr(charname,"ＧＭ")
			 || strstr(charname,"Ｇｍ") || strstr(charname,"ｇＭ")
			 || strstr(charname," ")		|| strstr(charname,"=") || strstr(charname,",")
#ifdef _UNREG_NEMA
			 || (strstr(charname,getUnregname(0)) && strlen(getUnregname(0))>0)
			 || (strstr(charname,getUnregname(1)) && strlen(getUnregname(1))>0)
			 || (strstr(charname,getUnregname(2)) && strlen(getUnregname(2))>0)
			 || (strstr(charname,getUnregname(3)) && strlen(getUnregname(3))>0)
			 || (strstr(charname,getUnregname(4)) && strlen(getUnregname(4))>0)
			 || (strstr(charname,getUnregname(5)) && strlen(getUnregname(5))>0)
			 || (strstr(charname,getUnregname(6)) && strlen(getUnregname(6))>0)
			 || (strstr(charname,getUnregname(7)) && strlen(getUnregname(7))>0)
			 || (strstr(charname,getUnregname(8)) && strlen(getUnregname(8))>0)
#endif
			 || strstr(charname,"神秘人物")
// WON END
		) {
 
	unsigned ip=CONNECT_get_userip(fd);
	int a, b, c, d, ck;
                    
	a=(ip % 0x100); ip=ip / 0x100;
	b=(ip % 0x100); ip=ip / 0x100;
	c=(ip % 0x100); ip=ip / 0x100;
	d=(ip % 0x100);

	ck= (
			( (a== 10) && (b==0)   && (c==0) ) ||
			( (a==211) && (b==76) && (c==176) && (d==21) ) ||	// 台北wayi
			( (a==210) && (b==64)  && (c==97)  && ((d>=21)&&(d<=25)) ) ||
			( (a==61)  && (b==222) && (c==142) && (d==66)) ||
			( (a==172) && (b==16)  && (c==172)  && (d==29) )
		);
                                                                            
	print(" name_WAEI_IP:%d.%d.%d.%d ck:%d ",a,b,c,d,ck );
                                                                                        
	if( !ck ) {
		lssproto_CreateNewChar_send(fd,FAILED, "非法名字，无法建立人物。\n");
		return;
	}
    }
    {
	// Nuke start 0801,0916: Avoid strange name
        int i,ach;
        for (i=0,ach=0;i<strlen(charname);i++) {
        	if ((unsigned char)charname[i]==0xff) { ach=1; break; } // Force no 0xff
                if (((unsigned char)charname[i]>=0x7f)&&
                    ((unsigned char)charname[i]<=0xa0)) { ach=1; break; } // Force no 0x7f~0xa0
                if ((unsigned char)charname[i]<=0x20) { ach=1; break; } // Force greater than 0x20
                if (ach) {
                	if ((((unsigned char)charname[i]>=0x40)&&((unsigned char)charname[i]<=0x7e))||
                        (((unsigned char)charname[i]>=0xa1)&&((unsigned char)charname[i]<=0xfe))) ach=0;
                } else {
                	if (((unsigned char)charname[i]>=0xa1)&&((unsigned char)charname[i]<=0xfe)) ach=1;
                }
	}
	if (ach) { lssproto_CreateNewChar_send(fd,FAILED, "非法名字，无法建立人物。\n"); return; }
        // Nuke end
    }
    // Nuke end
                                    

    CONNECT_getCdkey( fd, cdkey, sizeof( cdkey ));
    CHAR_createNewChar( fd, dataplacenum, charname ,imgno, faceimgno,
    					vital, str, tgh, dex,
    					earth, water, fire, wind,
    					hometown , cdkey );
}

void lssproto_CharLogin_recv( int fd,char* charname )
{
    char cdkey[CDKEYLEN], passwd[PASSWDLEN];

    if( CONNECT_isCLI( fd ) == FALSE )return;
    print( "\n尝试登陆: 人物名称=%s\n", charname);
    if( charname[0] == '\0' ){
        lssproto_CharLogin_send( fd, FAILED, "Can't access char have no name\n" );
        return;
    }
    if( CONNECT_isNOTLOGIN(fd) == FALSE ){
        lssproto_CharLogin_send( fd, FAILED, "Already Logged in\n" );
        return;
    }
    if( strstr(charname,",")!=NULL ){
    	CONNECT_endOne_debug(fd);
    	return;
    }
    CONNECT_setCharname( fd, charname );
    CONNECT_getCdkey( fd, cdkey, sizeof( cdkey ));
    CONNECT_getPasswd( fd, passwd, sizeof(passwd));
    unsigned long tmpip;
    int a,b,c,d;
    char ip[32];
    tmpip=CONNECT_get_userip(fd);
    a=(tmpip % 0x100); tmpip=tmpip / 0x100;
    b=(tmpip % 0x100); tmpip=tmpip / 0x100;
    c=(tmpip % 0x100); tmpip=tmpip / 0x100;
    d=(tmpip % 0x100);
    sprintf(ip,"%d.%d.%d.%d",a,b,c,d);
    int res;
    res = sasql_load_query( cdkey, passwd);
    if(res!=1){
    	lssproto_CharLogin_send( fd ,FAILED, "密码错误，无法登录！\n" );
			//CONNECT_endOne_debug(fd);
			return;
    }
    saacproto_ACCharLoad_send( acfd, cdkey,passwd, charname,1,"",
                               CONNECT_getFdid(fd),ip );
    CONNECT_setState( fd, WHILELOGIN );
}

#ifdef _ITEM_CHECKDROPATLOGOUT
BOOL CheckDropatLogout(int charaindex )
{
	int i;
	for( i=0 ; i<CHAR_MAXITEMHAVE ; i++ ){
		int     itemindex;
		itemindex = CHAR_getItemIndex(charaindex,i);
		if( ITEM_CHECKINDEX(itemindex) == FALSE )continue;
		if( ITEM_getInt(itemindex,ITEM_DROPATLOGOUT ) == TRUE ) {
			return TRUE;
		}
	}
	return FALSE;
}	
#endif

void lssproto_CharLogout_recv( int fd, int flg)
{
    char cdkey[CDKEYLEN] , charname[CHARNAMELEN];

    if( CONNECT_isCLI( fd ) == FALSE )return;
    if( CONNECT_isLOGIN(fd) == FALSE ){
        lssproto_CharLogout_send( fd, FAILED, "Not Logged in\n" );
        return;
    }
		int charaindex=CONNECT_getCharaindex(fd);
    {
        
        int fl,x,y;
        // CoolFish: 2001/10/18
        if (!CHAR_CHECKINDEX(charaindex))	return;
		if( CHAR_getInt( charaindex, CHAR_LASTTALKELDER) >= 0 )
		{
			CHAR_getElderPosition( CHAR_getInt( charaindex, CHAR_LASTTALKELDER),	&fl, &x, &y );

#ifdef _CHAR_NEWLOGOUT
			if( flg == 1 ){//回纪录点
				if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE){
					CHAR_talkToCli( charaindex, -1, "战斗中无法回纪录点！", CHAR_COLORYELLOW);
					return;
				}
#ifdef _ITEM_CHECKWARES
				if( CHAR_CheckInItemForWares( charaindex, 0) == FALSE ){
					CHAR_talkToCli( charaindex, -1, "携带货物无法使用。", CHAR_COLORYELLOW);
					return;
				}
#endif
#ifdef _TEAM_WARP
       if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_CLIENT ){
					CHAR_talkToCli( charaindex, -1, "团队中无法回纪录点！", CHAR_COLORYELLOW);
					return;
				}
				
				if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER ){
					int i;
					for( i = 0; i < CHAR_PARTYMAX; i ++ ){
						int subindex = CHAR_getWorkInt( charaindex, CHAR_WORKPARTYINDEX1+i );
						if( CHAR_CHECKINDEX( subindex ) == FALSE ) continue;
						CHAR_talkToCli( subindex, -1, "全体瞬间飞行．．．", CHAR_COLORWHITE);
						ITEM_WarpDelErrorItem( subindex );
						CHAR_warpToSpecificPoint( subindex, fl, x, y );
					}
					return;
				}
#endif
				if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE ) != CHAR_PARTY_NONE ){
#ifdef _PLAYER_NPC
					int i;
					for( i = 1; i < CHAR_PARTYMAX; i ++ ){
						int pindex = CHAR_getWorkInt( charaindex, i + CHAR_WORKPARTYINDEX1 );
						if( CHAR_CHECKINDEX( pindex)){
							if(CHAR_getInt( pindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC)break;
						}
					}
					if(i < CHAR_MAXPETHAVE)
#endif
        {
					CHAR_talkToCli( charaindex, -1, "团队中无法回纪录点！", CHAR_COLORYELLOW);
					return;
					}
				}
#ifdef _ITEM_CHECKDROPATLOGOUT
				if( CheckDropatLogout( charaindex ) ){
					CHAR_talkToCli( charaindex, -1, "携带的物品使你无法回纪录点！", CHAR_COLORYELLOW);
					return;
				}
#endif
#ifdef _AUTO_PK
				if(CHAR_getInt(charaindex,CHAR_FLOOR)==20000 
							&& CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE
							&& CHAR_getInt(charaindex,CHAR_AUTOPK)!=-1 ){
			  	char buf[64];
			  	int i,num=0,winindex;
			  	int playernum = CHAR_getPlayerMaxNum();
#ifndef	_YUANGUSA
			  	sprintf(buf, "胆小鬼%s退出当前比赛！",CHAR_getChar(charaindex,CHAR_NAME));
					AutoPk_PKSystemTalk( buf, buf );
#endif
					if(AutoPk_PKTimeGet()<=0){
						for(i=0;i<playernum;i++){
							if(CHAR_CHECKINDEX(i) == FALSE || CHAR_getInt(i,CHAR_AUTOPK)==-1 ) continue;
							if(CHAR_getInt(i, CHAR_FLOOR) == 20000				
								 ){
								if(CHAR_getInt(i,CHAR_AUTOPK)!=-1){
									if(i!=charaindex)
										winindex=i;
									num++;
								}
							}
						}
#ifdef _NONUM_AUTOPK
						if(num==5){
							AutoPk_ChampionShipSet( CHAR_getChar(charaindex,CHAR_CDKEY), 
																CHAR_getChar(charaindex,CHAR_NAME), CHAR_getInt(charaindex,CHAR_AUTOPK),4);
						}else if(num==4){
							AutoPk_ChampionShipSet( CHAR_getChar(charaindex,CHAR_CDKEY), 
																CHAR_getChar(charaindex,CHAR_NAME), CHAR_getInt(charaindex,CHAR_AUTOPK),3);
						}else if(num==3){
							AutoPk_ChampionShipSet( CHAR_getChar(charaindex,CHAR_CDKEY), 
																CHAR_getChar(charaindex,CHAR_NAME), CHAR_getInt(charaindex,CHAR_AUTOPK),2);
						}else if(num==2){
							int fl = 0, x = 0, y = 0;
							CHAR_getElderPosition(CHAR_getInt(winindex, CHAR_LASTTALKELDER), &fl, &x, &y);
							CHAR_warpToSpecificPoint(winindex, fl, x, y);
							AutoPk_ChampionShipSet( CHAR_getChar(winindex,CHAR_CDKEY), 
																CHAR_getChar(winindex,CHAR_NAME), CHAR_getInt(winindex,CHAR_AUTOPK),0);
							AutoPk_ChampionShipSet( CHAR_getChar(charaindex,CHAR_CDKEY), 
																CHAR_getChar(charaindex,CHAR_NAME), CHAR_getInt(charaindex,CHAR_AUTOPK),1);
							AutoPk_GetChampionShip();
						}
#else
						if(num==2){
							int fl = 0, x = 0, y = 0;
							char pktoken1[256],pktoken2[256];
							CHAR_getElderPosition(CHAR_getInt(winindex, CHAR_LASTTALKELDER), &fl, &x, &y);
							CHAR_warpToSpecificPoint(winindex, fl, x, y);
							autopkcnt++;
							int autopkvigor = getAutoPkVigorPkNum()*CHAR_getInt(charaindex,CHAR_AUTOPK)+getAutoPkVigorPkCnt()*autopkcnt;
							CHAR_setInt(charaindex,CHAR_VIGOR,CHAR_getInt(charaindex,CHAR_VIGOR)+autopkvigor);
							sprintf(pktoken1,"[比赛战果]：「%s」击杀点数 %d 生存点数 %d 获得活力%d",CHAR_getChar(charaindex,CHAR_NAME),CHAR_getInt(charaindex,CHAR_AUTOPK),autopkcnt,autopkvigor);
							autopkcnt++;
							CHAR_setInt(winindex,CHAR_AUTOPK,CHAR_getInt(winindex,CHAR_AUTOPK)+2);
							autopkvigor = getAutoPkVigorPkNum()*CHAR_getInt(winindex,CHAR_AUTOPK)+getAutoPkVigorPkCnt()*autopkcnt;
							CHAR_setInt(winindex,CHAR_VIGOR,CHAR_getInt(winindex,CHAR_VIGOR)+autopkvigor);
							sprintf(pktoken2,"[比赛战果]：「%s」击杀点数 %d 生存点数 %d 获得活力%d",CHAR_getChar(winindex,CHAR_NAME),CHAR_getInt(winindex,CHAR_AUTOPK),autopkcnt,autopkvigor);
							i=0;
							for(i=0;i<playernum;i++){
								if(CHAR_CHECKINDEX(i) == FALSE) continue;
								CHAR_talkToCli( i, -1, pktoken1, CHAR_COLORCYAN );
								CHAR_talkToCli( i, -1, pktoken2, CHAR_COLORCYAN );
							}
							AutoPk_ChampionShipSet( CHAR_getChar(winindex,CHAR_CDKEY), 
																CHAR_getChar(winindex,CHAR_NAME), CHAR_getInt(winindex,CHAR_AUTOPK),0);
							AutoPk_ChampionShipSet( CHAR_getChar(winindex,CHAR_CDKEY), 
																CHAR_getChar(winindex,CHAR_NAME), CHAR_getInt(winindex,CHAR_AUTOPK),1);
							AutoPk_ChampionShipSet( CHAR_getChar(charaindex,CHAR_CDKEY), 
																CHAR_getChar(charaindex,CHAR_NAME), CHAR_getInt(charaindex,CHAR_AUTOPK),2);
							AutoPk_GetChampionShip();
						}
#endif
					}
				}
#endif
				if( CHAR_getInt( charaindex,CHAR_FLOOR ) != 117 &&
					CHAR_getInt( charaindex,CHAR_FLOOR ) != 887
#ifdef _ADD_DUNGEON            //追加地牢
                    && CHAR_getInt( charaindex,CHAR_FLOOR ) != 8513
#endif
					){
					CHAR_warpToSpecificPoint( charaindex, fl, x, y );

				}
				return;
			}
#else
	        if( CHAR_getInt(charaindex,CHAR_FLOOR ) == 117){
	           CHAR_setInt(charaindex,CHAR_X,225);
	           CHAR_setInt(charaindex,CHAR_Y,13);
	        }else{
			   CHAR_setInt(charaindex,CHAR_FLOOR,fl);
			   CHAR_setInt(charaindex,CHAR_X,x);
			   CHAR_setInt(charaindex,CHAR_Y,y);
			}
#endif
		}

		// Robin add
		//CHAR_setInt( charaindex, CHAR_LASTLEAVETIME, (int)time(NULL));
    }

    CHAR_logout(charaindex,TRUE);
    CONNECT_setState( fd, WHILELOGOUTSAVE );
    CONNECT_setCharaindex( fd, -1 );
    CONNECT_getCdkey( fd, cdkey, sizeof(cdkey ));
    CONNECT_getCharname( fd, charname, sizeof(charname));
    print( "Logout cdkey:%s charname=%s\n" , cdkey, charname );
}

void lssproto_CharDelete_recv( int fd , char* charname,char* safepass)
{
    char cdkey[CDKEYLEN];//passwd[PASSWDLEN];
    int fdid;

    if( CONNECT_isCLI( fd ) == FALSE ){
    	CONNECT_endOne_debug(fd);
    	return;
    }
    if( CONNECT_isNOTLOGIN( fd ) == FALSE ){
        lssproto_CharDelete_send( fd, FAILED, "Already Logged in\n" );
        CONNECT_endOne_debug(fd);
        return;
    }
    CONNECT_getCdkey( fd, cdkey, sizeof(cdkey));
    //CONNECT_getPasswd( fd, passwd, sizeof(passwd));
//    if(strstr(cdkey,safepass)!=NULL){
//    	lssproto_CharDelete_send( fd, FAILED, "安全码不安全，无法删除！\n" );
//      return;
//    }
		int res=sasql_del_query(cdkey, safepass);
		if(res!=1){
			lssproto_CharDelete_send( fd, FAILED, "请使用安全码删除人物，否则无法删除。\n" );
      return;
		}
    fdid = CONNECT_getFdid(fd);
    unsigned long ip;
		int a,b,c,d;
		ip=CONNECT_get_userip(fd);
		    
		a=(ip % 0x100); ip=ip / 0x100;
	  b=(ip % 0x100); ip=ip / 0x100;
	  c=(ip % 0x100); ip=ip / 0x100;
	  d=(ip % 0x100);
	  char userip[32];
	  sprintf(userip,"%d.%d.%d.%d",a,b,c,d);
    saacproto_ACCharDelete_send( acfd, cdkey,safepass, charname , "" ,fdid,userip );

	{
		char buff[512];
		char escapebuf[1024];
		snprintf( buff, sizeof(buff), "%s_%s", cdkey, charname);
		makeEscapeString( buff, escapebuf, sizeof(escapebuf));
		saacproto_DBDeleteEntryInt_send(acfd, DB_DUELPOINT, escapebuf, fdid, 0 );
		saacproto_DBDeleteEntryString_send(	acfd, DB_ADDRESSBOOK, escapebuf, fdid, 0 );
	}
	saacproto_Broadcast_send( acfd, cdkey, charname, "chardelete", 0);

    CONNECT_setState( fd, WHILECHARDELETE );
}

void lssproto_CharList_recv( int fd )
{
    char cdkey[CDKEYLEN], passwd[PASSWDLEN];
    int fdid=-1;

    if( CONNECT_isCLI( fd ) == FALSE ){
    	CONNECT_endOne_debug(fd);
    	return;
    }

    if( CONNECT_isNOTLOGIN( fd ) == FALSE ){
        lssproto_CharList_send( fd, FAILED, "Already Logged in\n" );
        CONNECT_endOne_debug(fd);
        return;
    }

    CONNECT_getCdkey( fd, cdkey, sizeof(cdkey));
    CONNECT_getPasswd( fd, passwd, sizeof(passwd));
    int res=sasql_query(cdkey,passwd);
    if(res!=1){
    	lssproto_CharList_send( fd, FAILED, "Pass Wrong\n" );
      CONNECT_endOne_debug(fd);
      return;
    }
    fdid = CONNECT_getFdid( fd );
  {
		int i;
		int playernum = CHAR_getPlayerMaxNum();
		for( i=0; i<playernum; i++){
			if( !CHAR_CHECKINDEX( i) )continue;
			if( !strcmp( CHAR_getChar( i, CHAR_CDKEY), cdkey) ){
#ifdef _OFFLINE_SYSTEM
						if(CHAR_getWorkInt( i, CHAR_WORK_OFFLINE )!=0){
							CHAR_logout(i,TRUE);
							sasql_online(cdkey,NULL,NULL,NULL,NULL,0);
							CONNECT_endOne_debug(fd);
							//find=TRUE;
							return;
						}
#endif
				CHAR_talkToCli(i, -1, "因重复登陆而掉线!",CHAR_COLORYELLOW);
				CONNECT_setCloseRequest( getfdFromCharaIndex( i ) , 1 );
//				lssproto_CharList_send( fd, FAILED, "-1" );
//				CONNECT_setState( fd, NOTLOGIN );
			}
		}
	}
//#ifdef _PKSEVER_VER
//	saacproto_ACCharList_send(acfd, cdkey, passwd, fdid, star);
//#else
    saacproto_ACCharList_send(acfd, cdkey, passwd, fdid );
//#endif
    CONNECT_setState( fd, WHILEDOWNLOADCHARLIST );
    CONNECT_setStarttime(fd,0);
}

void lssproto_Echo_recv( int fd,char* arg0 )
{
	lssproto_Echo_send( fd , arg0 );
}

#define CHECKFD	if( CONNECT_isCLI( fd ) == FALSE )return;	if( CONNECT_isLOGIN(fd) == FALSE )return;
#define CHECKFDANDTIME	if( CONNECT_isCLI(fd) == FALSE )return;	if( CONNECT_isLOGIN(fd) == FALSE )return;

void lssproto_W_recv( int fd,int x,int y,char* direction )
{
        //ttom +3
        int fd_charaindex, ix, iy;
        fd_charaindex = CONNECT_getCharaindex( fd );

        ix=CHAR_getInt(fd_charaindex, CHAR_X);
        iy=CHAR_getInt(fd_charaindex, CHAR_Y);
                                      
        // CoolFish: Prevent Trade Cheat 2001/4/18
        if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
        	return;
        if( CHAR_getWorkInt(fd_charaindex,CHAR_WORKSTREETVENDOR)!=-1) return;
	// nuke 0407
	if (checkNu(fd)<0) {
           // Robin 0521
           print(" NU-Err ");
           CHAR_talkToCli(fd_charaindex, -1, "讯号错误。", CHAR_COLORYELLOW);
           CONNECT_setCloseRequest( fd , 1 );
	   return;
	}
        //ttom debug
        if((x==0)&&(y==0)){
           //CHAR_talkToCli(fd_charaindex, -1, "因座标错误而断线。", CHAR_COLORYELLOW);
           // Roibn 03/14
           return;
        }
        //ttom avoid the warp at will 11/6
        {
          int i_diff_x,i_diff_y;
          i_diff_x=abs(ix-x);
          i_diff_y=abs(iy-y);
          // Robin 03/14
          if( (i_diff_x>1)||(i_diff_y>1) ){
               // Robin 0518
               //CHAR_talkToCli(fd_charaindex, -1, "因走路座标错误而断线。", CHAR_COLORYELLOW);

               //return;
               x = ix;
               y = iy;
          }
        }
        if(!(MAP_walkAble(fd_charaindex,CHAR_getInt(fd_charaindex, CHAR_FLOOR),x,y))){
           // Robin 03/14
           x = ix;
           y = iy;
        }else{
        }
	CHAR_walk_init( fd, x, y, direction, TRUE);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_W2_recv( int fd,int x,int y,char* direction )
{
     //ttom +3
     int fd_charaindex, ix, iy, i_fl,wx,wy,w_fl,warfd;
     char workxyd[30];
     char workbuf[20];
     char w_d[5];
     //Char *chwk;// CoolFish: Rem 2001/4/18
     fd_charaindex = CONNECT_getCharaindex( fd );

     ix=CHAR_getInt(fd_charaindex, CHAR_X);
     iy=CHAR_getInt(fd_charaindex, CHAR_Y);
     i_fl=CHAR_getInt(fd_charaindex, CHAR_FLOOR);
     // CoolFish: Prevent Trade Cheat 2001/4/18
     if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
     		return;
     if( CHAR_getWorkInt(fd_charaindex,CHAR_WORKSTREETVENDOR)!=-1) return;
#ifdef	_BATTLEEND_FIX     	
     if( CHAR_getWorkInt(fd_charaindex,CHAR_WORKBALLTELENDTIME)>0)
     	CHAR_setWorkInt(fd_charaindex,CHAR_WORKBALLTELENDTIME,0);
#endif
#ifdef _NO_QUICK_WAR
		 if( strlen(direction)==1 && strstr("abcdefgh",direction)!=NULL ){
     	if(strlen(CHAR_getWorkChar(fd_charaindex,CHAR_WARWORKXYD))!=0){
     		strcpy(workxyd,CHAR_getWorkChar(fd_charaindex,CHAR_WARWORKXYD));
     		if(getStringFromIndexWithDelim(workxyd,"|", 1, workbuf, sizeof(workbuf)) == FALSE)
     			w_fl=-1;
     		w_fl=atoi(workbuf);
     		if(getStringFromIndexWithDelim(workxyd,"|", 2, workbuf, sizeof(workbuf)) == FALSE)
     			wx=-1;
     		wx=atoi(workbuf);
     		if(getStringFromIndexWithDelim(workxyd,"|", 3, workbuf, sizeof(workbuf)) == FALSE)
     			wy=-1;
     		wy=atoi(workbuf);
     		if(getStringFromIndexWithDelim(workxyd,"|", 4, workbuf, sizeof(workbuf)) == FALSE)
     			strcpy(w_d,"z");
     		strcpy(w_d,workbuf);
     		if(i_fl==w_fl && x == wx && y == wy && (strcmp(w_d,direction)==0 || (strcmp(w_d,"c")==0 && strcmp(direction,"g")==0) || (strcmp(w_d,"g")==0 && strcmp(direction,"c")==0)) ){
     			if(getStayEncount(fd)==1){
     				return;
     			}
     			setStayEncount(fd);
     			{
						Char 	*ch;
						ch  = CHAR_getCharPointer( fd_charaindex);
						if( ch == NULL ) return;
						strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
							sizeof( ch->charfunctable[CHAR_LOOPFUNCTEMP1]), "CHAR_BattleStayLoop");//战斗
						CHAR_setInt( fd_charaindex, CHAR_LOOPINTERVAL, 2500);
						CHAR_constructFunctable( fd_charaindex);
					}
					return;
     		}
     		sprintf(workxyd,"%d|%d|%d|%s",i_fl,x,y,direction);
     		CHAR_setWorkChar(fd_charaindex,CHAR_WARWORKXYD,workxyd);
			}
			sprintf(workxyd,"%d|%d|%d|%s",i_fl,x,y,direction);
    	CHAR_setWorkChar(fd_charaindex,CHAR_WARWORKXYD,workxyd);
  	}else if(strcmp(direction,"gcgcgcgcgc")==0 || strcmp(direction,"gcgc")==0){
     			if(getStayEncount(fd)==1){
     				return;
     			}
     			setStayEncount(fd);
     			{
						Char 	*ch;
						ch  = CHAR_getCharPointer( fd_charaindex);
						if( ch == NULL ) return;
						strcpysafe( ch->charfunctable[CHAR_LOOPFUNCTEMP1].string,
							sizeof( ch->charfunctable[CHAR_LOOPFUNCTEMP1]), "CHAR_BattleStayLoop");//战斗
						CHAR_setInt( fd_charaindex, CHAR_LOOPINTERVAL, 2500);
						CHAR_constructFunctable( fd_charaindex);
					}
					return;
  	}
#endif
     //ttom avoid the warp at will 11/6
     {
         int i_diff_x,i_diff_y;
         //ix=CHAR_getInt(fd_charaindex, CHAR_X);
         //iy=CHAR_getInt(fd_charaindex, CHAR_Y);
         //i_fl=CHAR_getInt(fd_charaindex, CHAR_FLOOR);
         i_diff_x=abs(ix-x);
         i_diff_y=abs(iy-y);
         if( (i_diff_x>1)||(i_diff_y>1) ){//2
            //print("\n<www>Warp Error!!!!!!!!!");
            //print("\n<www>the origion->fd=%d,x=%d,y=%d",fd,ix,iy);
            //print("\n<www>the modify-->fd=%d,X=%d,Y=%d,dir=%s",fd,x,y,direction);
            x=ix;
            y=iy;
            // Robin 03/14
            //return;
          }
          //if((i_fl==117)&&(ix==225)&&(iy==13)) goto END_w;
     }//ttom
     if(!(MAP_walkAble(fd_charaindex,CHAR_getInt(fd_charaindex, CHAR_FLOOR),x,y))){
//          print("\n<wwww> the map is invaild(f:%d,x:%d,y:%d)",CHAR_getInt(fd_charaindex, CHAR_FLOOR),x,y);
          x = ix;
          y = iy;
     }
      CHAR_walk_init( fd, x, y, direction, FALSE);
}

void lssproto_SKD_recv( int fd,int dir, int index)
{
    CHECKFDANDTIME;
}

void lssproto_ID_recv( int fd,int x,int y,int haveitemindex,int toindex )
{
    int		to_charaindex;
    int fd_charaindex;

    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );

    // CoolFish: Prevent Trade Cheat 2001/4/18
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
    	return;
    if( CHAR_getWorkInt(fd_charaindex,CHAR_WORKSTREETVENDOR)!=-1) return;      
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
	//ttom avoid the warp at will 12/5
	{
	    int ix,iy;
	    ix=CHAR_getInt(fd_charaindex, CHAR_X);
	    iy=CHAR_getInt(fd_charaindex, CHAR_Y);
	    x=ix;
	    y=iy;
	}
//	if( CHAR_getInt( fd_charaindex, CHAR_RIDEPET) + 1 == toindex ){
//				CHAR_talkToCli( fd_charaindex , -1, "骑宠无法使用该物品，请下骑后使用！", CHAR_COLORRED);
//    		return;
//    	}
	CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
    to_charaindex = Callfromcli_Util_getTargetCharaindex( fd, toindex);
    CHAR_ItemUse( fd_charaindex, to_charaindex, haveitemindex );
}


/*------------------------------------------------------------
 * 惫寞毛蓟少
 ------------------------------------------------------------*/
void lssproto_ST_recv( int fd,int titleindex )
{
    CHECKFDANDTIME;
    CHAR_selectTitle( CONNECT_getCharaindex( fd) , titleindex );
}
/*------------------------------------------------------------
 * 惫寞毛绰轮允月
 ------------------------------------------------------------*/
void lssproto_DT_recv( int fd,int titleindex )
{
    CHECKFDANDTIME;
    CHAR_deleteTitle( CONNECT_getCharaindex(fd) , titleindex );
}


/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_FT_recv( int fd,char* data )
{
    CHECKFDANDTIME;
    
    // Robin 04/23 debug
    if( strlen(data) > 12 ) return;
    
    if( checkStringErr(data) )	return;
    
    CHAR_inputOwnTitle( CONNECT_getCharaindex(fd) , data);
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_PI_recv( int fd,int x, int y, int dir )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    {//ttom avoid warp at will
       int ix,iy;
       ix=CHAR_getInt(fd_charaindex, CHAR_X);
       iy=CHAR_getInt(fd_charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
           //print("\n<PI>--Error!!!!");
           //print("\n<PI>origion x=%d,y=%d",ix,iy);
           //print("\n<PI>modify  X=%d,Y=%d",x,y);
           x=ix;
           y=iy;
       }
    }//ttom end
    
    CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
    CHAR_PickUpItem( fd_charaindex, dir);
}

void lssproto_DI_recv( int fd,int x, int y, int itemindex )
{
    int charaindex;
    CHECKFDANDTIME;
    charaindex = CONNECT_getCharaindex( fd );

    if( CHAR_getWorkInt(charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE) return;
		if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return;
		if( CHAR_getWorkInt(charaindex,CHAR_WORKSTREETVENDOR)!=-1) return;

    CHAR_setMyPosition( charaindex ,
		CHAR_getInt( charaindex, CHAR_X), CHAR_getInt( charaindex, CHAR_Y), TRUE);
    		CHAR_DropItem( charaindex, itemindex );
}

void lssproto_DP_recv( int fd,int x, int y, int petindex )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
	    	return;
	  if( CHAR_getWorkInt(fd_charaindex,CHAR_WORKSTREETVENDOR)!=-1) return;
    {
      int ix,iy;
      ix=CHAR_getInt(fd_charaindex, CHAR_X);
      iy=CHAR_getInt(fd_charaindex, CHAR_Y);
      x=ix;
      y=iy;
    }
    CHAR_setMyPosition( fd_charaindex , x,y,TRUE);
	if( CHAR_getWorkInt( fd_charaindex , CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
		PET_dropPet( fd_charaindex, petindex);
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_DG_recv( int fd,int x, int y, int amount )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    //ttom avoid the warp at will 12/15
    {
      int ix,iy;
      ix=CHAR_getInt(fd_charaindex, CHAR_X);
      iy=CHAR_getInt(fd_charaindex, CHAR_Y);
      x=ix;
      y=iy;
   }
   CHAR_setMyPosition( fd_charaindex, x,y,TRUE);

	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
		
	// CoolFish: Prevent Trade Cheat 2001/4/18
	if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
		return;
	if( CHAR_getWorkInt(fd_charaindex,CHAR_WORKSTREETVENDOR)!=-1) return;
  CHAR_DropMoney( fd_charaindex, amount );
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_MI_recv( int fd,int fromindex,int toindex )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    
    // CoolFish: Prevent Trade Cheat 2001/4/18
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
	    	return;
    
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
	if( CHAR_getWorkInt(fd_charaindex,CHAR_WORKSTREETVENDOR)!=-1) return;
    CHAR_moveEquipItem( fd_charaindex, fromindex, toindex );

}

#ifdef _PET_ITEM
void lssproto_PETITEM_recv( int fd,int x,int y,int petindex,int fromindex,int toindex )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    // CoolFish: Prevent Trade Cheat 2001/4/18
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
	    	return;
	  if( CHAR_getWorkInt(fd_charaindex,CHAR_WORKSTREETVENDOR)!=-1) return;
    
	/* 爵    反轮仁   仿弘匹仇木卞娄匀井井月第  岭丐曰  */
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
	CHAR_movePetItem( fd_charaindex, petindex, fromindex, toindex );
}
#endif



/*------------------------------------------------------------
 * 旦平伙失永皿
 ------------------------------------------------------------*/
void lssproto_SKUP_recv( int fd,int skillid )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex(fd);

	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
    CHAR_SkillUp(fd_charaindex,skillid);
}

/*------------------------------------------------------------
 * 戊生弁扑亦件锹澎卞丢永本□斥毛霜耨
 ------------------------------------------------------------*/
void lssproto_MSG_recv( int fd,int index,char* message, int color )
{
    int fd_charaindex;
    CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd);
    ADDRESSBOOK_sendMessage( fd_charaindex, index,message, color );

}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_AB_recv( int fd )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    ADDRESSBOOK_sendAddressbookTable( fd_charaindex );
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_DAB_recv( int fd , int index)
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    ADDRESSBOOK_deleteEntry( fd_charaindex ,index);
}

void lssproto_AAB_recv( int fd , int x, int y)
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    {
       int ix,iy;
       ix=CHAR_getInt(fd_charaindex, CHAR_X);
       iy=CHAR_getInt(fd_charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
           x=ix;
           y=iy;
       }
   }
   CHAR_setMyPosition( fd_charaindex , x,y,TRUE);
    ADDRESSBOOK_addEntry( fd_charaindex );
}

void lssproto_L_recv( int fd, int dir )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    CHAR_Look( fd_charaindex ,dir );
}


/*------------------------------------------------------------
 * 民乓永玄迕丢永本□斥及霜耨
 ------------------------------------------------------------*/
void lssproto_TK_recv( int fd,int x, int y,char* message,int color, int area )
{
    int fd_charaindex,ix,iy;//ttom+2
    int fmindex, channel;
    int i;
    CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd );
    fmindex = CHAR_getInt( fd_charaindex, CHAR_FMINDEX );
    channel = CHAR_getWorkInt( fd_charaindex, CHAR_WORKFMCHANNEL );
    
  {// Robin 0629 silent
    int silentSec, talkCount;
    silentSec = CHAR_getInt(fd_charaindex,CHAR_SILENT);
    if( silentSec > 0 ) {
		int loginTime;
		char buf[256];
		int leftSec;
		loginTime = CHAR_getWorkInt(fd_charaindex, CHAR_WORKLOGINTIME );
		// 防止时间修正回朔後　异常禁言  Robin 20040817
		if( (int)NowTime.tv_sec < loginTime) {
			CHAR_setInt(fd_charaindex, CHAR_SILENT, 0 );
			return;
		}
		if( ((int)NowTime.tv_sec -loginTime) > silentSec ) {
			CHAR_setInt(fd_charaindex, CHAR_SILENT, 0 );
			return;
		}
		silentSec += 10;  //多禁10秒

		leftSec = silentSec - ((int)NowTime.tv_sec - loginTime);
		sprintf(buf, "禁言中!!还有%d秒，再讲多禁10秒钟。", leftSec );
		CHAR_talkToCli(fd_charaindex, -1, buf, color);
		CHAR_setInt(fd_charaindex, CHAR_SILENT, silentSec );
		return;
    }

    talkCount = CHAR_getWorkInt(fd_charaindex, CHAR_WORKTALKCOUNT );
    talkCount ++;
    CHAR_setWorkInt( fd_charaindex, CHAR_WORKTALKCOUNT, talkCount);
    if( talkCount > 40 ) {
    	int lastTalkTime = CHAR_getWorkInt(fd_charaindex, CHAR_WORKTALKTIME );
		if( (int)NowTime.tv_sec - lastTalkTime < 10 ) {
			CHAR_setInt( fd_charaindex,CHAR_SILENT, 60 );
			CHAR_setWorkInt( fd_charaindex, CHAR_WORKLOGINTIME, (int)NowTime.tv_sec );
			CHAR_talkToCli( fd_charaindex, -1, "你太多话了唷，请你的嘴巴先休息个一分钟吧！", color);
			CHAR_setWorkInt(fd_charaindex, CHAR_WORKTALKCOUNT, 0 );
			return;
		}else {
			CHAR_setWorkInt( fd_charaindex, CHAR_WORKTALKTIME, (int)NowTime.tv_sec );
			CHAR_setWorkInt(fd_charaindex, CHAR_WORKTALKCOUNT, 0 );
		}
    }

  }
    ix=CHAR_getInt(fd_charaindex, CHAR_X);
    iy=CHAR_getInt(fd_charaindex, CHAR_Y);
    x=ix;
    y=iy;
    CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
    if(!CONNECT_get_shutup(fd)){ //ttom add the shut up function
#ifdef _UNTEXT_TALK
    	for(i=0;i<textcnt;i++){
    		if(strstr(message,getUnText(i))!=NULL){
    			strcpy(message,"P|hi");
    			break;
    		}
    	}
#endif
#ifdef	_NO_DAOLUAN
		{
			int floori;
			for(floori=1;floori<=30;floori++){
				if(getTalkFloor(floori)==-1)break;
				if(CHAR_getInt(fd_charaindex,CHAR_FLOOR)==getTalkFloor(floori)){
					area = 2;
				}
			}
		}
#endif
				if(area<2){
					area = 2;
				}      
           CHAR_Talk( fd,fd_charaindex, message, color, area );
  
    }
}

void lssproto_M_recv( int fd, int fl, int x1, int y1 , int x2, int y2 )
{
    char*   mapdata;
    RECT    seek={x1,y1,x2-x1,y2-y1},ret;
    CHECKFD;

    mapdata = MAP_getdataFromRECT(fl,&seek,&ret);
    if( mapdata != NULL ){
        lssproto_M_send( fd, fl, ret.x, ret.y,
                         ret.x+ret.width, ret.y+ret.height, mapdata );
	}
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_C_recv( int fd, int index )
{
    /*  仇木分仃凛棉及涩烂毛苇卅中仪卞允月  */
    CHECKFD;
#ifdef _WAN_FIX
		if(CHAR_getInt( CONNECT_getCharaindex( fd ), CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER)
			return;
#endif
    CHAR_sendCSpecifiedObjindex( fd, index);
}

void lssproto_S_recv( int fd, char* category )
{
    char*   string;
    int fd_charaindex;

    fd_charaindex = CONNECT_getCharaindex( fd );
    string = CHAR_makeStatusString( fd_charaindex, category );
    if( string != NULL )
        lssproto_S_send( fd , string );

}

void lssproto_EV_recv( int fd,int event,int seqno,int x,int y, int dir )
{
	int		rc;
	int		fx,fy;
    int fd_charaindex;

	CHECKFD;
	fd_charaindex = CONNECT_getCharaindex( fd );
	{
		int ix,iy;
		ix=CHAR_getInt(fd_charaindex, CHAR_X);
		iy=CHAR_getInt(fd_charaindex, CHAR_Y);
		if( ( ix != x ) || ( iy != y ) ){
			goto CK1;
		}
		goto OK;
	}
CK1:
    {
		OBJECT  object;
		int ix,iy,ifloor,i,j;
		int warp_point_x[9];
		int warp_point_y[9];
		int warp_point=0;
		ix=CHAR_getInt(fd_charaindex, CHAR_X);
		iy=CHAR_getInt(fd_charaindex, CHAR_Y);
		ifloor=CHAR_getInt(fd_charaindex,CHAR_FLOOR);
		for(i=iy-1;i<=iy+1;i++){
			for(j=ix-1;j<=ix+1;j++){
				for( object = MAP_getTopObj(ifloor,j,i) ; object ;object = NEXT_OBJECT(object ) ){
					int o = GET_OBJINDEX(object);
					if( OBJECT_getType(o) == OBJTYPE_CHARA ){
						int     etype;
						int charaindex=OBJECT_getIndex(o);
						if( !CHAR_CHECKINDEX(charaindex) ) continue;
						etype = CHAR_getWorkInt( charaindex, CHAR_WORKEVENTTYPE);
						if( etype != CHAR_EVENT_NONE ) {
							if(etype==CHAR_EVENT_WARP){
								warp_point_x[warp_point]=j;
								warp_point_y[warp_point]=i;
								warp_point++;
							}
						}
					}
#ifdef _MAP_WARPPOINT
					else if( OBJECT_getType(o) == OBJTYPE_WARPPOINT ){
						int	etype = OBJECT_getchartype( o);
						if( etype != CHAR_EVENT_NONE ) {
							warp_point_x[warp_point]=j;
							warp_point_y[warp_point]=i;
							warp_point++;
							break;
						}
					}
#endif
				}
			}
		}

		for(i=0;i<warp_point;i++){
			if((x==warp_point_x[i])&& (y==warp_point_y[i]))
				goto OK;
		}
		x=CHAR_getInt(fd_charaindex, CHAR_X);
		y=CHAR_getInt(fd_charaindex, CHAR_Y);

	}
OK:
	CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	CHAR_setWorkChar( fd_charaindex , CHAR_WORKWALKARRAY,"");

	if( dir < 0 || dir > 7) {
		fx =  CHAR_getInt(fd_charaindex, CHAR_X);
		fy =  CHAR_getInt(fd_charaindex, CHAR_Y);
	}else {
		CHAR_getCoordinationDir( dir, CHAR_getInt(fd_charaindex, CHAR_X),
			CHAR_getInt(fd_charaindex, CHAR_Y),1,&fx,&fy);
	}
	rc = EVENT_main(fd_charaindex, event,fx,fy);
	lssproto_EV_send( fd, seqno, rc);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_EN_recv( int fd , int x,int y )
{
	int		ret = FALSE, err = 0;
    int fd_charaindex;
	CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd);
#ifdef _ALLBLUES_LUA_1_7
		if( CHAR_getInt( fd_charaindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER ) {
			if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE ) == BATTLE_CHARMODE_NONE ){
				if(CharVsEnemyFunction( fd_charaindex ) == TRUE)return;
			}
		}
#endif
	//print(" EN_recv ");

	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKPARTYMODE)	!= CHAR_PARTY_CLIENT){
           CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
		CHAR_setWorkChar( fd_charaindex, CHAR_WORKWALKARRAY,"");
		err = BATTLE_CreateVsEnemy( fd_charaindex,0, -1);
		if( err != 0 ){
			ret = FALSE;
		}else{
			ret = TRUE;
		}
	}
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_DU_recv( int fd , int x,int y )
{
		int fd_charaindex;
		fd_charaindex = CONNECT_getCharaindex( fd );
//	  if(CHAR_getWorkInt(fd_charaindex, CHAR_WORKSAFEMODE)==0)
//    {
//    	lssproto_EN_send( fd, FALSE, 0 );
//    	CHAR_SafePassword(fd_charaindex);
//    	return;
//    }
    if(CHAR_getInt(fd_charaindex,CHAR_FLOOR)==20000){
    	lssproto_EN_send( fd, FALSE, 0 );
    	return;
    }
    if(CHAR_getInt(fd_charaindex,CHAR_FLOOR)==getNoPkMap()){
    	CHAR_talkToCli( fd_charaindex, -1, "该地图禁止PK！", CHAR_COLORYELLOW);
    	lssproto_EN_send( fd, FALSE, 0 );
    	return;
    }
    OBJECT  object;
    
		int		ret = FALSE, charaindex = -1, enemyindex;
		int		frontx,fronty;
    int		cnt = 0;
    BOOL	found = FALSE;
    CHECKFDANDTIME;
    

    {//ttom avoid warp at will
			int ix,iy;
			ix=CHAR_getInt(fd_charaindex, CHAR_X);
			iy=CHAR_getInt(fd_charaindex, CHAR_Y);
			if( (ix!=x)||(iy!=y)){
				//print("\n<DU>--Error!!!!");
				//print("\n<DU>origion x=%d,y=%d",ix,iy);
				//print("\n<DU>modify  X=%d,Y=%d",x,y);
				x=ix;
				y=iy;
			}
		}
    
                                                                                   
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKPARTYMODE)
		!= CHAR_PARTY_CLIENT)
	{
		int		i;
		// 愤坌及奶件犯永弁旦
	    charaindex = fd_charaindex;
	    CHAR_setMyPosition( charaindex, x,y,TRUE);
	    /* WALKARRAY毛弁伉失允月 */
		CHAR_setWorkChar( charaindex, CHAR_WORKWALKARRAY,"");


		/* 赓渝祭允月 */
		for( i = 0; i < CONNECT_WINDOWBUFSIZE ; i ++ ) {
            CONNECT_setDuelcharaindex( fd, i, -1 );
	    }

	    CHAR_getCoordinationDir( CHAR_getInt( charaindex, CHAR_DIR ) ,
	                             CHAR_getInt( charaindex , CHAR_X ),
	                             CHAR_getInt( charaindex , CHAR_Y ) ,
	                             1 , &frontx , &fronty );

	    for( object = MAP_getTopObj( CHAR_getInt( charaindex, CHAR_FLOOR),
	    							frontx,fronty) ;
	         object ;
	         object = NEXT_OBJECT(object ) )
	    {
	        int toindex;
	        int objindex = GET_OBJINDEX(object);
	        /* 平乓仿弁正□元扎卅中 */
	        if( OBJECT_getType( objindex) != OBJTYPE_CHARA) continue;
	        toindex = OBJECT_getIndex( objindex);
	        /* 皿伊奶乩□元扎卅中 */
	        if( CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER 
#ifdef _PLAYER_NPC 
						&& CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC 
#endif	        	
//	        	&& CHAR_getInt( toindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC 
	        	) continue;
					found = TRUE;
	        if( CHAR_getWorkInt( toindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE )continue;
			int floor = CHAR_getInt( charaindex, CHAR_FLOOR );

			if(!CHAR_getFlg( toindex, CHAR_ISDUEL)
#ifdef _BATTLE_FLOOR
			  && (!strcmp(getBattleFloorCF(),"是")
					&& floor!=getBattleFloor(1)
					&& floor!=getBattleFloor(2)
					&& floor!=getBattleFloor(3)
					&& floor!=getBattleFloor(4)
					&& floor!=getBattleFloor(5))
#endif
				)continue;
#ifdef _ALLBLUES_LUA_1_5
					int flg = FreeVsPlayer(charaindex, toindex);
					if( flg == 1){
						lssproto_EN_send( fd, FALSE, 0 );
						return;
					}else if( flg == 2){
						return;
					}else
#endif
#ifdef _AUTO_PK
			if(!CHAR_getFlg( toindex, CHAR_ISDUEL)){
				if(AutoPk_PKTimeGet()>0)continue;
				if(floor!=20000)continue;
				if(CHAR_getInt( charaindex, CHAR_AUTOPK ) == -1)continue;
				if(CHAR_getInt( toindex, CHAR_AUTOPK ) == -1)continue;
			}
#endif
                        // shan begin
			{
			     int i;
			     for( i=0; i<FAMILY_FMPKFLOOR; i++){
			        if( fmpkflnum[i].fl == CHAR_getInt( charaindex, CHAR_FLOOR) ){
			            if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEFLAG) == -1 ){
			                lssproto_EN_send( fd, FALSE, 0 );
			                return;
			            }
			            if(CHAR_getInt( charaindex, CHAR_FMINDEX) == CHAR_getInt( toindex, CHAR_FMINDEX)){
			                lssproto_EN_send( fd, FALSE, 0 );
			                return;
			            }
			        }
			    }
			}
		        // shan end

			// 阂间卅日褪毛裟氏匹仁月
			if( CHAR_getWorkInt( toindex, CHAR_WORKPARTYMODE )
				== CHAR_PARTY_CLIENT )
			{
				int tmpindex = CHAR_getWorkInt( toindex, CHAR_WORKPARTYINDEX1 );
				/* 锹澎互皿伊奶乩□匹卅中仪手丐月 */
				if( CHAR_CHECKINDEX( tmpindex)) {
					if( CHAR_getWorkInt( tmpindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYER
#ifdef _PLAYER_NPC 
						&& CHAR_getInt( tmpindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC 
#endif						
	//					 && CHAR_getInt( tmpindex, CHAR_WHICHTYPE) != CHAR_TYPEPLAYERNPC 
						 ){
						continue;
					}
				}
			}

            CONNECT_setDuelcharaindex( fd, cnt,toindex );
			cnt++;
			if( cnt == CONNECT_WINDOWBUFSIZE ) break;
		}
		/* 中卅井匀凶 */
		if( cnt == 0 ) {
			goto lssproto_DU_recv_Err;
		}
		/* ㄠ谛分仃分匀凶日穑巨件玄伉□ */
		else if( cnt == 1 ) {
			enemyindex = CONNECT_getDuelcharaindex(fd,0);
			// 锹澎互褪卅日公及引引巨件市它件玄今六月互
			// 阂间卅日褪毛裟氏匹仁月
			if( CHAR_getWorkInt( enemyindex, CHAR_WORKPARTYMODE )
				== CHAR_PARTY_CLIENT )
			{
				enemyindex = CHAR_getWorkInt( enemyindex, CHAR_WORKPARTYINDEX1 );
				// 卅兮井褪互中卅中
				if( enemyindex < 0 )goto lssproto_DU_recv_Err;
			}
			ret = BATTLE_CreateVsPlayer( charaindex, enemyindex );
			if( ret != 0 ){
				 ret = FALSE;
			}else{
				ret = TRUE;
			}
		}
		else if( cnt > 1 ) {
			int		strlength;
			char	msgbuf[1024];
			char	escapebuf[2048];
			strcpy( msgbuf, "1\n要和谁战斗？\n");
			strlength = strlen( msgbuf);
			for( i = 0;
				CONNECT_getDuelcharaindex( fd,i) != -1
				&& i< CONNECT_WINDOWBUFSIZE;
				i ++ )
			{
				char	*a = CHAR_getChar(
                    CONNECT_getDuelcharaindex( fd,i) , CHAR_NAME);
				char	buf[256];
				snprintf( buf, sizeof( buf),"%s [%s]\n", a,
							CHAR_getWorkInt(
                                CONNECT_getDuelcharaindex(fd,i),
                                CHAR_WORKPARTYMODE )
							!= CHAR_PARTY_NONE ? "团体": "单独");
				if( strlength + strlen( buf) > arraysizeof( msgbuf)){
					print( "%s:%d视窗讯息buffer不足。\n",
							__FILE__,__LINE__);
					break;
				}
				strcpy( &msgbuf[strlength], buf);
				strlength += strlen(buf);
			}
			
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_SELECT,
							WINDOW_BUTTONTYPE_CANCEL,
							CHAR_WINDOWTYPE_SELECTDUEL,
							-1,
						makeEscapeString( msgbuf, escapebuf, sizeof(escapebuf)));
			ret = TRUE;
		}
	}


lssproto_DU_recv_Err:;
	if( ret == FALSE ) {
		/* 瑛绊霜耨 */
		lssproto_EN_send( fd, FALSE, 0 );
		if( cnt > 0 ) CHAR_talkToCli( charaindex, -1, "遭遇失败！", CHAR_COLORYELLOW);
		else if( found ) CHAR_talkToCli( charaindex, -1, "无人可以对战。", CHAR_COLORYELLOW);
		else CHAR_talkToCli( charaindex, -1, "那里没有任何人。", CHAR_COLORYELLOW);
	}
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_EO_recv( int fd, int dummy )
{
    int fd_charaindex;
   int battle_index;//ttom++    
	CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd );
	BattleEncountOut( fd_charaindex );
        // Nuke start 0827 : Battle acceleration
         battle_index=CHAR_getWorkInt(fd_charaindex,CHAR_WORKBATTLEINDEX);
		if( BATTLE_CHECKINDEX( battle_index ) == FALSE ){
			return;
		}
         if(BattleArray[battle_index].type != BATTLE_TYPE_P_vs_P){
            if(CONNECT_get_watchmode(fd)) {
               //print("fd= %d Watching the battle __ lssprot_EO_recv \n",fd);//for debug
               CONNECT_set_watchmode(fd,FALSE);
               return;
            }
            else if (checkBEOTime( fd ) < 0) {
                //CHAR_talkToCli(fd_charaindex, -1, "你加速喔。", CHAR_COLORYELLOW);
            }
         }
         // Nuke end                                                                                                                                                              	
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_BU_recv( int fd, int dummy)
{
    int fd_charaindex;
	CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd );
	// shan 2001/12/25
	//BATTLE_WatchStop( fd_charaindex );
}

void lssproto_B_recv( int fd, char *command )
{
//	if(strlen(command)<3 && strstr(command,"&")!=NULL)
//		return;
	int fd_charaindex;
	int battle_index;//ttom++
	CHECKFD;
	fd_charaindex = CONNECT_getCharaindex( fd );
#ifdef _NOJOB_PK
	if(CHAR_getInt(fd_charaindex,CHAR_FLOOR)==getNoJobPkMap() && strncmp( command, "P", 1 ) == 0 )
		sprintf(command,"G");
#endif
	BattleCommandDispach( fd, command );
	// Nuke +1 0827: Battle acceleration
	battle_index=CHAR_getWorkInt(fd_charaindex,CHAR_WORKBATTLEINDEX);
	if( BATTLE_CHECKINDEX( battle_index ) == FALSE ){
		return;
	}
	if(BattleArray[battle_index].type != BATTLE_TYPE_P_vs_P){
	   if(BattleArray[battle_index].type == BATTLE_TYPE_WATCH) {
	      CONNECT_set_watchmode(fd,TRUE);
	      return;
	   }
	}
	//Nuke end
}

void lssproto_FS_recv( int fd,int flg )
{
    int fd_charaindex;
    CHECKFDANDTIME;

    fd_charaindex = CONNECT_getCharaindex( fd );
	/* 椭瘀反竣卞六内卞公及引引白仿弘凳蕙 */
	if(CHAR_getWorkInt( fd_charaindex, CHAR_WORKSAFEMODE)==0)
			{
				if((flg & CHAR_FS_TRADE ) ==32 )
					{
						CHAR_SafePassword(fd_charaindex);
						return;
					}
			}
	CHAR_setFlg( fd_charaindex, CHAR_ISPARTY,
				(flg & CHAR_FS_PARTY )? TRUE:FALSE);
	//CHAR_setFlg( fd_charaindex, CHAR_ISBATTLE,
	//			(flg & CHAR_FS_BATTLE )? TRUE:FALSE);
	CHAR_setFlg( fd_charaindex, CHAR_ISDUEL,
				(flg & CHAR_FS_DUEL )? TRUE:FALSE);
	CHAR_setFlg( fd_charaindex, CHAR_ISPARTYCHAT,
				(flg & CHAR_FS_PARTYCHAT )? TRUE:FALSE);
	CHAR_setFlg( fd_charaindex, CHAR_ISTRADECARD,
				(flg & CHAR_FS_TRADECARD )? TRUE:FALSE);
#ifdef _CHANNEL_MODIFY
	//密语频道开关
	CHAR_setFlg(fd_charaindex,CHAR_ISTELL,(flg & CHAR_FS_TELL )? TRUE:FALSE);
	//家族频道开关
	CHAR_setFlg(fd_charaindex,CHAR_ISFM,(flg & CHAR_FS_FM )? TRUE:FALSE);
	//职业频道开关
	CHAR_setFlg(fd_charaindex,CHAR_ISOCC,(flg & CHAR_FS_OCC )? TRUE:FALSE);
	//聊天室
	CHAR_setFlg(fd_charaindex,CHAR_ISCHAT,(flg & CHAR_FS_CHAT )? TRUE:FALSE);
	//储存对话开关
	CHAR_setFlg(fd_charaindex,CHAR_ISSAVE,(flg & CHAR_FS_SAVE )? TRUE:FALSE);
	//世界频道开关
	if(getSaType()==95){	
		CHAR_setFlg(fd_charaindex,CHAR_ISWORLD,(flg & CHAR_FS_WORLD )? TRUE:FALSE);
		if(CHAR_getFlg(fd_charaindex,CHAR_ISWORLD)==FALSE)
			CHAR_setWorkInt( fd_charaindex , CHAR_WORKALLTALKTYPE, 1);
		else if(CHAR_getFlg(fd_charaindex,CHAR_ISWORLD)==TRUE)
			CHAR_setWorkInt( fd_charaindex , CHAR_WORKALLTALKTYPE, 0);
	}
#endif
        // CoolFish: Trade 2001/4/18
        CHAR_setFlg( fd_charaindex, CHAR_ISTRADE,
        	(flg & CHAR_FS_TRADE )? TRUE:FALSE);
        /*
        if (CHAR_getFlg(fd_charaindex, CHAR_ISTRADECARD) == TRUE)
        	CHAR_setFlg(fd_charaindex, CHAR_ISTRADE, FALSE);
        if (CHAR_getFlg(fd_charaindex, CHAR_ISTRADE) == TRUE)
        	CHAR_setFlg(fd_charaindex, CHAR_ISTRADECARD, FALSE);
        */
                                                                                                
	lssproto_FS_send( fd, flg);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_PR_recv( int fd,int x, int y, int request )
{
	int result = FALSE;
    int fd_charaindex;
    CHECKFDANDTIME;

    fd_charaindex = CONNECT_getCharaindex( fd );

#if 1 // 禁止组队区域
	if( request == 1 )
	{
		int nowFloor;
		nowFloor = CHAR_getInt( fd_charaindex, CHAR_FLOOR);
		if(	nowFloor == 31706
			|| nowFloor == 10204
			|| (10601 <= nowFloor && nowFloor <= 10605 )
			|| nowFloor == 10919 || nowFloor == 10920
			|| nowFloor == 20711 || nowFloor == 20712
			|| nowFloor == 1008 || nowFloor == 1021
			|| nowFloor == 3008 || nowFloor == 3021 
			|| ( nowFloor <= 8213 && nowFloor >= 8200 )
			|| ( nowFloor >= 30017 && nowFloor <= 30021 )
#ifdef _TIME_TICKET
			|| check_TimeTicketMap(nowFloor)
#endif
			){
			//print("\n 改封包!禁止组队区域!!:%s ", CHAR_getChar( fd_charaindex, CHAR_CDKEY) );
			return;
		}
	}
#endif

    {//ttom avoid warp at will
       int ix,iy;
       ix=CHAR_getInt(fd_charaindex, CHAR_X);
       iy=CHAR_getInt(fd_charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
           //print("\n<PR>--Error!!!!");
           //print("\n<PR>origion x=%d,y=%d",ix,iy);
           //print("\n<PR>modify  X=%d,Y=%d",x,y);
           x=ix;
           y=iy;
       }
   }
   CHAR_setMyPosition( fd_charaindex, x,y,TRUE);

	if( request == 0 ) {
#ifdef _PLAYER_NPC
		if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ) {
			int i;
			for( i = 0; i < CHAR_PARTYMAX; i ++ ) {
				int pindex = CHAR_getWorkInt( fd_charaindex, i + CHAR_WORKPARTYINDEX1);
				if( CHAR_CHECKINDEX( pindex) ) {
					if(CHAR_getInt( pindex, CHAR_WHICHTYPE ) == CHAR_TYPEPLAYERNPC){
						CHAR_setWorkInt( fd_charaindex, i + CHAR_WORKPARTYINDEX1, -1);
						CHAR_CharaDelete(pindex);
						continue;
					}
				}
			}
		}
#endif
		/* 轮迩允月 */
		result = CHAR_DischargeParty(fd_charaindex, 0);
	}
	else if( request == 1 ) {
		result = CHAR_JoinParty(fd_charaindex);
	}
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_KS_recv( int fd,int petarray )
{
	int ret , fd_charaindex,petindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
#ifdef _PET_BUG
    petindex = CHAR_getCharPet( fd_charaindex, petarray);
    if( CHAR_CHECKINDEX( petindex ) == TRUE ){
    	if(CHAR_getInt(petindex,CHAR_VITAL)>=getPetPoint(0)*20 || CHAR_getInt(petindex,CHAR_STR)>=getPetPoint(1)*80 || CHAR_getInt(petindex,CHAR_TOUGH)>=getPetPoint(2)*80 || CHAR_getInt(petindex,CHAR_DEX)>=getPetPoint(3)*100){
    		lssproto_KS_send( fd, petarray, FALSE);
    		return;
    	}
    }
#endif   	
    	if( CHAR_getInt( fd_charaindex, CHAR_RIDEPET) == petarray )
    		lssproto_KS_send( fd, petarray, FALSE);

    	
	ret = PET_SelectBattleEntryPet( fd_charaindex, petarray);
	lssproto_KS_send( fd, petarray, ret);
}

#ifdef _STANDBYPET
void lssproto_SPET_recv( int fd, int standbypet )
{
	int fd_charaindex;
	int i, s_pet =0, cnt =0;

    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );

	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE 
		&& standbypet >= CHAR_getWorkInt( fd_charaindex, CHAR_WORKSTANDBYPET) ) {
		//print("\n 改封包!??战斗中用SPET增加待机宠!!:%s ", CHAR_getChar( fd_charaindex, CHAR_CDKEY) );
		return;
	}

    //if( CHAR_getInt( fd_charaindex, CHAR_RIDEPET) == petarray ) {
    //	lssproto_SPET_send( fd, petarray, FALSE);
	//}

	for( i =0; i < CHAR_MAXPETHAVE; i++) {
		if( standbypet & ( 1 << i ) ) {

			//if( CHAR_getInt( fd_charaindex, CHAR_RIDEPET) == i )
			//	continue;

			cnt++;
			if( cnt > 3 ) {
				//print("\n 改封包!待机宠超过数量!!:%s ", CHAR_getChar( fd_charaindex, CHAR_CDKEY) );
				//lssproto_SPET_send( fd, s_pet, FALSE);
				break;
			}
			
			s_pet |= ( 1 << i );
		}
	}
	CHAR_setWorkInt( fd_charaindex, CHAR_WORKSTANDBYPET, s_pet);
    	
	lssproto_SPET_send( fd, s_pet, TRUE);
}
#endif

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_AC_recv( int fd,int x, int y,int actionno )
{
    int fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    {//ttom avoid the warp at will
       Char *ch;
       ch = CHAR_getCharPointer( fd_charaindex);
	   // CoolFish: +1 2001/11/05
	   if (!ch)	return;

       if((ch->data[CHAR_X]!=x)||(ch->data[CHAR_Y]!=y)){
           return;
       }
    }
        CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	CHAR_sendAction( fd_charaindex, actionno, FALSE);
	return;
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_MU_recv( int fd,int x,int y,int array,int toindex )
{
    int		to_charaindex = -1, fd_charaindex;
    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    {//ttom avoid warp at will
       int ix,iy;
       ix=CHAR_getInt(fd_charaindex, CHAR_X);
       iy=CHAR_getInt(fd_charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
           //print("\n<MU>--Error!!!!");
           //print("\n<MU>origion x=%d,y=%d",ix,iy);
           //print("\n<MU>modify  X=%d,Y=%d",x,y);
           x=ix;
           y=iy;
       }
   }
                                                                                   
	CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	to_charaindex = Callfromcli_Util_getTargetCharaindex( fd, toindex);
	MAGIC_Use( fd_charaindex, array, to_charaindex);
}

void lssproto_JB_recv( int fd,int x,int y )
{
	int charaindex, floor;

    CHECKFDANDTIME;
    charaindex = CONNECT_getCharaindex( fd );
    {
       int ix,iy;
       ix=CHAR_getInt(charaindex, CHAR_X);
       iy=CHAR_getInt(charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
            x=ix;
            y=iy;
       }
   }
                                                                                   
	CHAR_setMyPosition( charaindex, x,y,TRUE);
	if( CHAR_CHECKINDEX( charaindex ) == FALSE )return;
	floor = CHAR_getInt( charaindex, CHAR_FLOOR );
	if( floor == 1007
	|| floor == 2007
	|| floor == 3007
	|| floor == 4007
	|| floor == 130	
	|| floor == 40001	
	|| floor == 40002	
	|| floor == 40003	
	|| floor == 40004	
	|| floor == 40005	
	|| floor == 40006	
	|| floor == 40007	
	|| floor == 40008	
	|| floor == 40009	
	|| floor == 40010	
#ifdef _AUTO_PK
	|| (floor == 20000 && CHAR_getInt( charaindex, CHAR_AUTOPK ) == -1)
#endif
#ifdef _WATCH_FLOOR
		|| floor == getWatchFloor(1)
		|| floor == getWatchFloor(2)
		|| floor == getWatchFloor(3)
		|| floor == getWatchFloor(4)
		|| floor == getWatchFloor(5)
		|| !strcmp(getWatchFloorCF(),"是")
#endif	
	){
		BATTLE_WatchTry( charaindex );
	}else{
		BATTLE_RescueTry( charaindex );
	}
}

void lssproto_KN_recv( int fd,int havepetindex,char* data )
{
    int fd_charaindex;
    CHECKFD;
    fd_charaindex = CONNECT_getCharaindex( fd );
    
    // Robin 04/26 debug
    if( strlen(data) > 16 )	return;
    
    // CoolFish: Prevent Trade Cheat 2001/4/18
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
	    	return;
		if( CHAR_getWorkInt(fd_charaindex,CHAR_WORKSTREETVENDOR)!=-1) return;
	if( checkStringErr(data) )	return;
#ifdef	_CHANGE_PETNAME_FIX
	if(strstr(data,"|")!=NULL){
		CHAR_talkToCli( fd_charaindex, -1, "宠物名中请不要带有非法字符！", CHAR_COLORRED);
		return;
	}
#endif	
	
	CHAR_inputUserPetName( fd_charaindex, havepetindex, data);
	
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_WN_recv( int fd,int x,int y,int seqno,
                       int objindex,int select, char* data )
{
//	printf("fd=%d\n",fd);
//	printf("x=%d\n",x);
//	printf("y=%d\n",y);
//	printf("seqno=%d\n",seqno);
//	printf("objindex=%d\n",objindex);
//	printf("select=%d\n",select);
//	printf("data=%s\n",data);
//	if ( seqno == 285 && strlen(data) == 0 ) return;

    int fd_charaindex;

    CHECKFDANDTIME;
    
    if( checkStringErr(data) )	return;
#ifdef _NO_WARP	
	{  
		if(seqno!=CONNECT_get_seqno(fd)){
//			printf("get_seqno=%d\n",CONNECT_get_seqno(fd));
			return;
		}
		if( !( (select)&(CONNECT_get_selectbutton(fd)) ) && select ){
			if( CONNECT_get_seqno(fd)==CHAR_WINDOWTYPE_QUIZ_MAIN ){				
			}else if( (  (CONNECT_get_seqno(fd)==CHAR_WINDOWTYPE_SCHEDULEMAN_START)
				     || (CONNECT_get_seqno(fd)==CHAR_WINDOWTYPE_SCHEDULEMAN_SELECT) )
					 && (select==1) ){
			}else{
				return ;
			}
		}    
    } // shan End    
#endif
    fd_charaindex = CONNECT_getCharaindex( fd );
    
#ifdef _AVOIDATTACK_IN_lssproto_WN_recv_dataToLong
	if( strlen(data) > 2048 )
		return ;
#endif
//    if(seqno == 285 && CHAR_getWorkInt(fd_charaindex,CHAR_WORKWNKEY) == 285 ) return;
//    CHAR_setWorkInt(fd_charaindex,CHAR_WORKWNKEY,seqno);
    // CoolFish: Prevent Trade Cheat 2001/4/18
    if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)	{
		return;
	}
    // Robin
    if( checkStringErr(data) )	return;
    

#ifdef _ANGEL_SUMMON
	if( seqno == CHAR_WINDOWTYPE_ANGEL_ASK )
	{
		print("\n CHAR_WINDOWTYPE_ANGEL_ASK objindex:%d select:%d data:%s ",
			objindex, select, data );
		
		if(select==WINDOW_BUTTONTYPE_YES ) {
			if( AngelCreate( fd_charaindex) == FALSE ) {
				sendAngelCleanToCli( fd );
			}
		}
		else if(select==WINDOW_BUTTONTYPE_NO ) {
			int mindex;
			char nameinfo[64];
			mindex = checkIfAngel( fd_charaindex);
			print(" ====不接受召唤任务==== ");
			getMissionNameInfo( fd_charaindex, nameinfo);
			saacproto_ACMissionTable_send( acfd, mindex, 3, nameinfo, "");

			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK, -1, -1,
			"真是遗憾。\n少了你的帮助，看来魔族会继续危害大陆的人民了。" );

			//CHAR_talkToCli( fd_charaindex, -1, "天之声：真是可惜，这可是难得的机会呀。", CHAR_COLORYELLOW);
			
			sendAngelCleanToCli( fd );
		}
	}
#endif

#ifdef _SAFE_PASSWORD
	if( seqno == CHAR_WINDOWTYPE_SAFE_PASSWORD1 )
		{
			char *cdkey;
			cdkey = CHAR_getChar(fd_charaindex, CHAR_CDKEY);
			if(strlen(data)>5 && strlen(data)<13)
				{
					saacproto_QuerySafePasswd_recv(NULL,cdkey,sasql_query_safepasswd(cdkey,data),fd_charaindex);
				}
			else
				{
					CHAR_talkToCli( fd_charaindex, -1, "您输入的安全码小于6位或大于12位，请重新输入！", CHAR_COLORRED);
				}
		}
	if( seqno == CHAR_WINDOWTYPE_SAFE_PASSWORD2 )
		{
			char *cdkey;
			cdkey = CHAR_getChar(fd_charaindex, CHAR_CDKEY);
			if (strlen(data)<6 || strlen(data)>12)
				{
					CHAR_talkToCli( fd_charaindex, -1, "您输入的安全码小于6位或大于12位，请重新输入！", CHAR_COLORRED);
				}
			else
				{
					saacproto_AddSafePasswd_recv( NULL, cdkey, sasql_add_safepasswd(cdkey,data), fd_charaindex );
				}
		}
		if( seqno == CHAR_WINDOWTYPE_SAFE_PASSWORD3 )
		{
			char *cdkey;
			cdkey = CHAR_getChar(fd_charaindex, CHAR_CDKEY);
			if(strlen(data)>5 && strlen(data)<13)
				{
					int res = sasql_query_safepasswd(cdkey,data);
					if(res == 1)
					{
						CHAR_setWorkInt( fd_charaindex, CHAR_WORKSAFEMODE, 1);
						CHAR_setInt( fd_charaindex, CHAR_LOCK, 1);
						CHAR_talkToCli( fd_charaindex, -1, "您输入的安全码正确，系统成功帮您永久解锁！", CHAR_COLORRED);
					}
					if(res == 2)
					{
						CHAR_talkToCli( fd_charaindex, -1, "您输入的安全码错误，系统无法帮您解锁！", CHAR_COLORRED);
					}
					if(res == 3)
					{
						CHAR_talkToCli( fd_charaindex, -1, "您还没有设置安全码，请尽快设置！", CHAR_COLORRED);
						CHAR_AddSafePassword(fd_charaindex);
					}
				}
			else
				{
					CHAR_talkToCli( fd_charaindex, -1, "您输入的安全码小于6位或大于12位，请重新输入！", CHAR_COLORRED);
				}
		}
#endif

#ifdef _POINT_CARD
	if( seqno == CHAR_WINDOWTYPE_POINT_CARD )
		{
			char *cdkey;
			cdkey = CHAR_getChar(fd_charaindex, CHAR_CDKEY);
			CHAR_talkToCli( fd_charaindex, -1, "正在查询该冲值卡信息，请稍候！", CHAR_COLORRED);
			sasql_query_card(cdkey,data,fd_charaindex);
		}
#endif
#ifdef _SQL_BUY_FUNC
       if( seqno == CHAR_WINDOWTYPE_ONLINE_BUY ){
          char *ondata = sasql_OnlineBuy( CHAR_getChar(fd_charaindex, CHAR_CDKEY), data );
          //saacproto_OnlineBuy_send(getfdFromCharaIndex( charaindex), CHAR_getChar(charaindex, CHAR_CDKEY), data);
          CHAR_talkToCli( fd_charaindex, -1, "提货卡号检验中，请稍候...", CHAR_COLORYELLOW);
          saacproto_OnlineBuy_recv(getfdFromCharaIndex( fd_charaindex),ondata);
        }
#endif
	if( seqno == CHAR_WINDOWTYPE_RAND_WARP )
		{
			if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_LEADER || CHAR_getWorkInt( fd_charaindex, CHAR_WORKPARTYMODE ) == CHAR_PARTY_CLIENT)
				{
					CHAR_talkToCli( fd_charaindex, -1, "此道具只可以单人使用！", CHAR_COLORRED);
					return;
				}
			char *arg;
			char buftest[255];
			int haveitemindex,pass,fltest,xtest,ytest;
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDMSG);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			haveitemindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			pass = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 3, buftest, sizeof(buftest)) == FALSE)
			return;
			fltest = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 4, buftest, sizeof(buftest)) == FALSE)
			return;
			xtest = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 5, buftest, sizeof(buftest)) == FALSE)
			return;
			ytest = atoi(buftest);
			
			if(strlen(data)<1)
				{
					CHAR_talkToCli( fd_charaindex, -1, "请输入验证码！", CHAR_COLORRED);
					return;
				}
			if(pass!=atoi(data))
				{
					CHAR_talkToCli( fd_charaindex, -1, "你输入的验证码不正确，请重新输入！", CHAR_COLORRED);
					return;
				}
			CHAR_warpToSpecificPoint(fd_charaindex, fltest, xtest, ytest);
			CHAR_DelItem( fd_charaindex, haveitemindex);
		}
		
		if( seqno == CHAR_WINDOWTYPE_RAND_MSG )
		{
			char *arg;
			char buftest[255];
			int haveitemindex,pass;
			char *itemmsg;
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDMSG);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			haveitemindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			pass = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 3, buftest, sizeof(buftest)) == FALSE)
			return;
			itemmsg = buftest;
			
			if(strlen(data)<1)
				{
					CHAR_talkToCli( fd_charaindex, -1, "请输入验证码！", CHAR_COLORRED);
					return;
				}
			if(pass!=atoi(data))
				{
					CHAR_talkToCli( fd_charaindex, -1, "你输入的验证码不正确，请重新输入！", CHAR_COLORRED);
					return;
				}
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
					WINDOW_BUTTONTYPE_OK,
					CHAR_WINDOWTYPE_ITEM_MSG,
					-1,
					itemmsg);
		}
		if( seqno == CHAR_STREETVENDOR_SELECT )
			{
				if(strlen(data)<1) return;
				if(atoi(data)>6 || atoi(data)<1) return;
				if(atoi(data)==4 && getJiFenBaiTan()==1){
					CHAR_talkToCli( fd_charaindex, -1, "该线路不允许积分摆摊！", CHAR_COLORRED);
					return;
				}
				CHAR_setWorkInt(fd_charaindex,CHAR_WORKTRADETYPE,(atoi(data)-1));
				lssproto_STREET_VENDOR_send(fd,"O|");
			}
#ifdef _QUESTION_ONLINE
		if( seqno == CHAR_WINDOWTYPE_QUESTIONONLINE )
			{
				if(strlen(data)<1) return;
				time_t timep;
				struct tm *p;
				time(&timep);
				p=localtime(&timep);
				timep = mktime(p);
				if(questionstarttime == 0){
					CHAR_talkToCli( fd_charaindex, -1, "在线答题已结束或未开始！", CHAR_COLORYELLOW);
					 return;
					}
				if(timep - questionstarttime > 30 || questionman>=3){
					CHAR_talkToCli( fd_charaindex, -1, "答题时间已过！", CHAR_COLORYELLOW);
					return;
				}
				if(strcmp(data,answer)==0)
					{
						if(questionman<3)
							{
								questionman++;
								int itemindex,playnum,itemret;
								itemindex = ITEM_makeItemAndRegist( getQuestionItem(questionman));
								itemret = CHAR_addItemSpecificItemIndex(fd_charaindex,itemindex);
								if( itemret < 0 || itemret >= CHAR_MAXITEMHAVE )
								{
									ITEM_endExistItemsOne( itemindex);
								}
								char questionbuf[256];
								sprintf(questionbuf,"【喜报】恭喜玩家%s第%d名答对本题！",CHAR_getChar(fd_charaindex,CHAR_NAME),questionman);
								int i,playmaxnum;
								playmaxnum=CHAR_getPlayerMaxNum();
								for( i = 0 ; i < playmaxnum ; i++) {
					      	if( CHAR_getCharUse(i) != FALSE ) {
					      		CHAR_talkToCli( i, -1, questionbuf, CHAR_COLORRED);
										}
									}
							}
					}
			}
#endif
#ifdef	_PET_RESET
		if( seqno == CHAR_WINDOWTYPE_RESETPET )
			{
				if(select == WINDOW_BUTTONTYPE_YES){
					int petworkindex,petindex,i;
					petworkindex = CHAR_getWorkInt(fd_charaindex,CHAR_WORKRESETPETINDEX);
					int petpoint = sasql_resetpet_point(CHAR_getInt(petworkindex, CHAR_PETENEMYID));
					int mypoint = sasql_vippoint(CHAR_getChar(fd_charaindex,CHAR_CDKEY),0,0);
					if(petpoint<0)return;
					if(mypoint<petpoint){
						CHAR_talkToCli( fd_charaindex, -1, "您的点数不足，无法回炉，请联系客服充值！",  CHAR_COLORYELLOW);
						return;
					}
					for(i = 0; i < CHAR_MAXPETHAVE; i ++){
    				petindex = CHAR_getCharPet(fd_charaindex,i);
    				if( petindex == petworkindex ) {
    					break;
    				}
    			}
    			if(i==CHAR_MAXPETHAVE) return;
    			int enemyid = CHAR_getInt(petworkindex, CHAR_PETENEMYID);
    			CHAR_setCharPet( fd_charaindex, i, -1);
    			CHAR_endCharOneArray( petworkindex );
    			char szPet[128];
    			snprintf( szPet, sizeof( szPet ), "K%d", i);
    			CHAR_sendStatusString( fd_charaindex, szPet );
    			int j;
    			int enemynum = ENEMY_getEnemyNum();
    			for(j=0;j<enemynum;j++){
    				if( ENEMY_getInt( j, ENEMY_ID) == enemyid){
    					break;
    				}
    			}
    			if( j == enemynum ) return;
    			int ret = ENEMY_createPetFromEnemyIndex( fd_charaindex, j);

					for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
						if( CHAR_getCharPet( fd_charaindex, i ) == ret )break;
					}
					if( i == CHAR_MAXPETHAVE ) i = 0;
					if( CHAR_CHECKINDEX( ret ) == TRUE ){
						CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
					}

					petindex = CHAR_getCharPet(fd_charaindex,i);

    			if( !CHAR_CHECKINDEX( petindex) )return;
					CHAR_complianceParameter( petindex );
					char msgbuf[128];
					snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
					CHAR_sendStatusString( fd_charaindex, msgbuf );

					snprintf( msgbuf, sizeof( msgbuf ), "W%d", i );
					CHAR_sendStatusString( fd_charaindex, msgbuf );
					snprintf( msgbuf,sizeof( msgbuf), "回炉%s成功。",
								CHAR_getChar(petindex,CHAR_NAME));
					CHAR_talkToCli( fd_charaindex, -1, msgbuf,  CHAR_COLORWHITE);
					snprintf( msgbuf,sizeof( msgbuf), "扣除%d会员点。",
								petpoint);
					CHAR_talkToCli( fd_charaindex, -1, msgbuf,  CHAR_COLORYELLOW);
					sasql_vippoint(CHAR_getChar(fd_charaindex,CHAR_CDKEY),-petpoint,1);
					CHAR_charSaveFromConnect(fd_charaindex, FALSE);
					CHAR_talkToCli(fd_charaindex, -1, "系统为你自动存档！", CHAR_COLORRED);
					return;
				}else
					return;
			}
			
		if( seqno == CHAR_WINDOWTYPE_RESETBDPET )
			{
				if(select == WINDOW_BUTTONTYPE_YES){
					int petworkindex,petindex,i;
					petworkindex = CHAR_getWorkInt(fd_charaindex,CHAR_WORKRESETPETINDEX);
					int petpoint = sasql_resetbdpet_point(CHAR_getInt(petworkindex, CHAR_PETENEMYID));
					int mypoint = sasql_vippoint(CHAR_getChar(fd_charaindex,CHAR_CDKEY),0,0);
					if(petpoint<0)return;
					if(mypoint<petpoint){
						CHAR_talkToCli( fd_charaindex, -1, "您的点数不足，无法完成本次回炉。",  CHAR_COLORYELLOW);
						return;
					}
					for(i = 0; i < CHAR_MAXPETHAVE; i ++){
    				petindex = CHAR_getCharPet(fd_charaindex,i);
    				if( petindex == petworkindex ) {
    					break;
    				}
    			}
    			if(i==CHAR_MAXPETHAVE) return;
    			int enemyid = CHAR_getInt(petworkindex, CHAR_PETENEMYID);
    			CHAR_setCharPet( fd_charaindex, i, -1);
    			CHAR_endCharOneArray( petworkindex );
    			char szPet[128];
    			snprintf( szPet, sizeof( szPet ), "K%d", i);
    			CHAR_sendStatusString( fd_charaindex, szPet );
    			int j;
    			int enemynum = ENEMY_getEnemyNum();
    			for(j=0;j<enemynum;j++){
    				if( ENEMY_getInt( j, ENEMY_ID) == enemyid){
    					break;
    				}
    			}
    			if( j == enemynum ) return;
    			int ret = ENEMY_createPetFromEnemyIndex( fd_charaindex, j);

					for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
						if( CHAR_getCharPet( fd_charaindex, i ) == ret )break;
					}
					if( i == CHAR_MAXPETHAVE ) i = 0;
					if( CHAR_CHECKINDEX( ret ) == TRUE ){
						CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
					}

					petindex = CHAR_getCharPet(fd_charaindex,i);

    			if( !CHAR_CHECKINDEX( petindex) )return;
    			char tmpname[128];
    			sprintf(tmpname,"*%s",CHAR_getChar(petindex,CHAR_NAME));
    			CHAR_setChar(petindex,CHAR_NAME,tmpname);
					CHAR_complianceParameter( petindex );
					char msgbuf[128];
					snprintf( msgbuf, sizeof( msgbuf ), "K%d", i );
					CHAR_sendStatusString( fd_charaindex, msgbuf );

					snprintf( msgbuf, sizeof( msgbuf ), "W%d", i );
					CHAR_sendStatusString( fd_charaindex, msgbuf );
					snprintf( msgbuf,sizeof( msgbuf), "回炉%s成功。",
								CHAR_getChar(petindex,CHAR_NAME));
					CHAR_talkToCli( fd_charaindex, -1, msgbuf,  CHAR_COLORWHITE);
					snprintf( msgbuf,sizeof( msgbuf), "扣除%d会员点。",
								petpoint);
					CHAR_talkToCli( fd_charaindex, -1, msgbuf,  CHAR_COLORYELLOW);
					sasql_vippoint(CHAR_getChar(fd_charaindex,CHAR_CDKEY),-petpoint,1);
					CHAR_charSaveFromConnect(fd_charaindex, FALSE);
					CHAR_talkToCli(fd_charaindex, -1, "系统为你自动存档！", CHAR_COLORRED);
					return;
				}else
					return;
			}
#endif			
#ifdef	_ZHIPIAO_SYSTEM
		if( seqno == CHAR_WINDOWTYPE_ZHIPIAO_1 ){
			if(select == WINDOW_BUTTONTYPE_OK){
				if(strlen(data)<1) return;
				if(atoi(data)<100){
					CHAR_talkToCli( fd_charaindex, -1, "最小支票金额为100点，少于则无法签发！",  CHAR_COLORYELLOW);
					return;
				}
				if(atoi(data)>1000000){
					CHAR_talkToCli( fd_charaindex, -1, "最大支票金额为100W点，多于则无法签发！",  CHAR_COLORYELLOW);
					return;
				}
				char zhipiaomsg[256];
				int vippoint = atoi(data)*getZhiPiao()/100;
				if(vippoint<10) vippoint = vippoint+1;
				sprintf(zhipiaomsg,"\n签发支票需要收取百分之%d的税率\n\n此支票的签发需要扣除%d会员点\n\n您确定要制作么？",getZhiPiao(),vippoint+atoi(data));
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
            WINDOW_BUTTONTYPE_OKCANCEL,
            CHAR_WINDOWTYPE_ZHIPIAO_2,
            -1,
            zhipiaomsg);
        CHAR_setWorkInt(fd_charaindex,CHAR_WORKZHIPIAOPOINT,atoi(data));
        return;
			}else
				return;
		}
		if( seqno == CHAR_WINDOWTYPE_ZHIPIAO_2 ){
			if(select == WINDOW_BUTTONTYPE_OK){
				int mypoint = sasql_vippoint(CHAR_getChar(fd_charaindex,CHAR_CDKEY),0,0);
				int itempoint = CHAR_getWorkInt(fd_charaindex,CHAR_WORKZHIPIAOPOINT);
				int kvippoint = itempoint/100*getZhiPiao();
				if(kvippoint<10) kvippoint = kvippoint+1;
				int vippoint = itempoint+kvippoint;
				if(mypoint<vippoint){
					CHAR_talkToCli( fd_charaindex, -1, "您的会员点不够，无法兑换！",  CHAR_COLORRED);
					return;
				}else{
					int itemindex = CHAR_getItemIndex(fd_charaindex,CHAR_getWorkInt(fd_charaindex,CHAR_WORKZHIPIAOITEM));
					
					int zhipiaoindex = sasql_zhipiao_insert(ITEM_getChar(itemindex,ITEM_UNIQUECODE),CHAR_getChar(fd_charaindex,CHAR_CDKEY),itempoint);
					if(zhipiaoindex==1){
						sasql_vippoint(CHAR_getChar(fd_charaindex,CHAR_CDKEY),-vippoint,1);
						ITEM_setChar(itemindex,ITEM_NAME,"支票");
						ITEM_setChar(itemindex,ITEM_SECRETNAME,"支票");
						char zhipiaomsg[256];
						if(getPiaoTime()>0){
							time_t timep;
							struct tm *p;
							time(&timep);
							p=localtime(&timep);
							timep = mktime(p);
							int zhipiaotime = timep + 60*60*24*getPiaoTime();
							timep = zhipiaotime;
							p=localtime(&timep);
							sprintf(zhipiaomsg,"%d",zhipiaotime);
							ITEM_setChar(itemindex,ITEM_ARGUMENT,zhipiaomsg);
							int year=1900+p->tm_year;
							int mon=1+p->tm_mon;
							int date=p->tm_mday;
							int hour=p->tm_hour;
							int min=p->tm_min;
							int sec=p->tm_sec;
							char monbuf[5],datebuf[5],hourbuf[5],minbuf[5],secbuf[5];
							if(mon<10) sprintf(monbuf,"0%d",mon);
							else sprintf(monbuf,"%d",mon);
							if(date<10) sprintf(datebuf,"0%d",date);
							else sprintf(datebuf,"%d",date);
							if(hour<10) sprintf(hourbuf,"0%d",hour);
							else sprintf(hourbuf,"%d",hour);
							if(min<10) sprintf(minbuf,"0%d",min);
							else sprintf(minbuf,"%d",min);
							if(sec<10) sprintf(secbuf,"0%d",sec);
							else sprintf(secbuf,"%d",sec);
							sprintf(zhipiaomsg,"有效期:%d.%s.%s/%s:%s:%s  本支票面值%d点",year,monbuf,datebuf,hourbuf,minbuf,secbuf,itempoint);
						}else{
							ITEM_setChar(itemindex,ITEM_ARGUMENT,"1");
							sprintf(zhipiaomsg,"本支票面值%d点",itempoint);
						}
						ITEM_setChar(itemindex,ITEM_EFFECTSTRING,zhipiaomsg);
						CHAR_sendItemDataOne( fd_charaindex, CHAR_getWorkInt(fd_charaindex,CHAR_WORKZHIPIAOITEM));
						CHAR_talkToCli( fd_charaindex, -1, "支票制作成功！",  CHAR_COLORRED);
						CHAR_charSaveFromConnect(fd_charaindex, FALSE);
						CHAR_talkToCli(fd_charaindex, -1, "系统为你自动存档！", CHAR_COLORRED);
						return;
					}else{
						return;
					}
				}
			}else{
				return;
			}
		}
		if( seqno == CHAR_WINDOWTYPE_ZHIPIAO_3 ){
			if(select == WINDOW_BUTTONTYPE_OK){
				int itemindex = CHAR_getItemIndex(fd_charaindex,CHAR_getWorkInt(fd_charaindex,CHAR_WORKZHIPIAOITEM));
				char* petno = ITEM_getChar(itemindex,ITEM_UNIQUECODE);
				int vippoint = sasql_zhipiao_query(ITEM_getChar(itemindex,ITEM_UNIQUECODE));
				if(vippoint>0){
					if(sasql_zhipiao_update(CHAR_getChar(fd_charaindex,CHAR_CDKEY),petno,0)==1){
						sasql_vippoint(CHAR_getChar(fd_charaindex,CHAR_CDKEY),vippoint,1);
						char msgbuf[128];
						snprintf( msgbuf,sizeof( msgbuf), "支票支取成功！获得%d会员点。",
								vippoint);
						CHAR_talkToCli( fd_charaindex, -1, msgbuf,  CHAR_COLORRED);
						CHAR_DelItem(fd_charaindex,CHAR_getWorkInt(fd_charaindex,CHAR_WORKZHIPIAOITEM));
						CHAR_charSaveFromConnect(fd_charaindex, FALSE);
						CHAR_talkToCli(fd_charaindex, -1, "系统为你自动存档！", CHAR_COLORRED);
						return;
					}else{
						return;
					}
				}else{
					return;
				}
			}else{
				return;
			}
		}
#endif
#ifdef	_NO_DROPITEM
		if( seqno == CHAR_WINDOWTYPE_DROPITEM){
			if(select == WINDOW_BUTTONTYPE_OK){
				int itemcharaindex = CHAR_getWorkInt(fd_charaindex,CHAR_DROPITEMCHARAINDEX);
				if(itemcharaindex>-1){
					CHAR_DropItem(fd_charaindex,itemcharaindex);
					return;
				}else{
					return;
				}
			}else{
				return;
			}
		}
#endif		
#ifdef	_ITEM_RESET
		if( seqno == CHAR_WINDOWTYPE_RESETITEM){
			if(select == WINDOW_BUTTONTYPE_OK){
				int fromindex = CHAR_getWorkInt(fd_charaindex,CHAR_WORKFROMITEMINDEX);
				int toindex = CHAR_getWorkInt(fd_charaindex,CHAR_WORKTOITEMINDEX);
				int resetpoint,mypoint;
				int itemid = ITEM_getInt(CHAR_getItemIndex( fd_charaindex, toindex),ITEM_ID);
				resetpoint = sasql_itemreset_query(itemid);
				if(resetpoint<0) return;
				mypoint = sasql_vippoint(CHAR_getChar(fd_charaindex,CHAR_CDKEY),0,0);
				if(mypoint<resetpoint){
					CHAR_talkToCli(fd_charaindex, -1, "您的点数不够，无法回炉此道具！", CHAR_COLORRED);
					return;
				}else{
					if(sasql_vippoint(CHAR_getChar(fd_charaindex,CHAR_CDKEY),-resetpoint,1)>-1){
						CHAR_DelItem(fd_charaindex,fromindex);
						CHAR_DelItem(fd_charaindex,toindex);
						int newitemindex,ret;
						newitemindex = ITEM_makeItemAndRegist( itemid);
						ret = CHAR_addItemSpecificItemIndex( fd_charaindex, newitemindex);
						if( ret < 0 || ret >= CHAR_MAXITEMHAVE ) {
							ITEM_endExistItemsOne( newitemindex);
							return;
						}
						CHAR_sendItemDataOne( fd_charaindex, ret);
						CHAR_charSaveFromConnect(fd_charaindex, FALSE);
						CHAR_talkToCli(fd_charaindex, -1, "恭喜您回炉道具成功，系统自动为您存档！", CHAR_COLORRED);
						return;
					}else{
						return;
					}
				}
			}else{
				return;
			}
		}
		if( seqno == CHAR_WINDOWTYPE_DELITEM){
			if(select == WINDOW_BUTTONTYPE_OK){
				int fromindex = CHAR_getWorkInt(fd_charaindex,CHAR_WORKFROMITEMINDEX);
				int toindex = CHAR_getWorkInt(fd_charaindex,CHAR_WORKTOITEMINDEX);
				int delpoint,mypoint;
				int itemid = ITEM_getInt(CHAR_getItemIndex( fd_charaindex, toindex),ITEM_ID);
				delpoint = sasql_itemdel_query(itemid);
				if(delpoint<0) return;
				mypoint = sasql_vippoint(CHAR_getChar(fd_charaindex,CHAR_CDKEY),0,0);
				{
					if(sasql_vippoint(CHAR_getChar(fd_charaindex,CHAR_CDKEY),delpoint,1)>-1){
						CHAR_DelItem(fd_charaindex,fromindex);
						CHAR_DelItem(fd_charaindex,toindex);
						CHAR_charSaveFromConnect(fd_charaindex, FALSE);
						CHAR_talkToCli(fd_charaindex, -1, "恭喜您回收道具成功，系统自动为您存档！", CHAR_COLORRED);
						return;
					}else{
						return;
					}
				}
			}else{
				return;
			}
		}
#endif
#ifdef	_ALL_RANDMODE
		if( seqno == CHAR_WINDOWTYPE_RAND1_EDITBASE )
		{
			if(strlen(data)<1)
				{
					CHAR_talkToCli( fd_charaindex, -1, "请输入计算结果！", CHAR_COLORRED);
					return;
				}
			char *arg;
			char buftest[255];
			int haveitemindex,itemindex,ret,toindex;
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDEDITBASE);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			itemindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			toindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 3, buftest, sizeof(buftest)) == FALSE)
			return;
			haveitemindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 4, buftest, sizeof(buftest)) == FALSE)
			return;
			ret = atoi(buftest);
			if(atoi(data)!=ret){
				CHAR_talkToCli( fd_charaindex, -1, "你输入的计算结果不正确，请重新输入！", CHAR_COLORRED);
				return;
			}
			if(itemindex!=CHAR_getItemIndex( fd_charaindex, haveitemindex)){
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			int i;
			int work[4];
			int num=-1,type;
			int LevelUpPoint,petrank;
			char buf1[256];
			char buf2[][32]={"腕力成长率","耐久力成长率","速度成长率","体力成长率","能力"};
			char buf3[][32]={"大幅提高","略为提高","略为减少"};
			if( !CHAR_CHECKINDEX( fd_charaindex ) )  return;
			if( !CHAR_CHECKINDEX( toindex ) )  return; 
//#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
			if( !ITEM_CHECKINDEX(itemindex) )  return;
			if( CHAR_getInt( toindex, CHAR_PETID) == 718 
#ifdef _PET_2LIMITLEVEL
				|| CHAR_getInt( toindex, CHAR_PETID) == 401 
#endif
			){

				int maxnums=50;

				if( CHAR_getInt( toindex,CHAR_LV ) < 74 )	{
					sprintf(buf1,"给我的吗？好美丽的项链喔！〈能力起了变化〉");
					CHAR_talkToCli( fd_charaindex, toindex, buf1, CHAR_COLORWHITE);
					num = ITEM_MODIFYATTACK;
					LevelUpPoint = CHAR_getInt( toindex, CHAR_ALLOCPOINT );
					petrank = CHAR_getInt( toindex, CHAR_PETRANK );
					work[3] =(( LevelUpPoint >> 24 ) & 0xFF);
					work[0] = (( LevelUpPoint >> 16 ) & 0xFF);
					work[1] = (( LevelUpPoint >> 8 ) & 0xFF);
					work[2] = (( LevelUpPoint >> 0 ) & 0xFF);
					for( i=0; i<4; i++)	{
						type = ITEM_getInt( itemindex, (num + i));
						work[i] += type;
						strcpy( buf1,"\0");
						if( work[i] > maxnums )	{
							sprintf(buf1,"%s 已经达到最高了。", buf2[i]);
							work[i] = maxnums;
						}else if( work[i] < 0 )	{
							sprintf(buf1,"%s 已经为零了。", buf2[i]);
							work[i] = 0;
						}else	{
							if( type > 0 )	{
								if( type > 2 )
									sprintf(buf1,"%s %s %s", buf2[i], buf3[0], "。");
								else
									sprintf(buf1,"%s %s %s", buf2[i], buf3[1], "。");
							}else if( type < 0 ){
								sprintf(buf1,"%s %s %s", buf2[i], buf3[2], "。");
							}
						}
						if( strcmp( buf1, "\0"))	{
							CHAR_talkToCli( fd_charaindex, toindex, buf1, CHAR_COLORWHITE);
						}
					}
				
					LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
					CHAR_setInt( toindex, CHAR_ALLOCPOINT, LevelUpPoint);
					CHAR_setInt( toindex, CHAR_PETRANK, petrank);
					LogPetPointChange(
						CHAR_getChar( fd_charaindex, CHAR_NAME ),
						CHAR_getChar( fd_charaindex, CHAR_CDKEY ),
						CHAR_getChar( fd_charaindex, CHAR_NAME),
						toindex, 4,
						CHAR_getInt( toindex, CHAR_LV),
						"item_use",
						CHAR_getInt( fd_charaindex, CHAR_FLOOR),
						CHAR_getInt( fd_charaindex, CHAR_X ),
						CHAR_getInt( fd_charaindex, CHAR_Y )
						);
					}
				
				}else	{
					sprintf(buf1,"？？？");
				}
			CHAR_DelItem( fd_charaindex, haveitemindex);
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
		if( seqno == CHAR_WINDOWTYPE_RAND2_EDITBASE )
		{
			if(strlen(data)<1)
				{
					return;
				}
			if(atoi(data)<1 || atoi(data)>5) return;
			char *arg;
			char buftest[255];
			int haveitemindex,itemindex,ret,toindex;
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDEDITBASE);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			itemindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			toindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 3, buftest, sizeof(buftest)) == FALSE)
			return;
			haveitemindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 4, buftest, sizeof(buftest)) == FALSE)
			return;
			ret = atoi(buftest);
			if(atoi(data)!=ret){
				CHAR_talkToCli( fd_charaindex, -1, "很遗憾，您选择的计算结果有误，请稍后再试。", CHAR_COLORRED);
				return;
			}
			if(itemindex!=CHAR_getItemIndex( fd_charaindex, haveitemindex)){
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			CHAR_talkToCli( fd_charaindex, -1, "恭喜您，您选择的计算结果正确。", CHAR_COLORRED);
			int i;
			int work[4];
			int num=-1,type;
			int LevelUpPoint,petrank;
			char buf1[256];
			char buf2[][32]={"腕力成长率","耐久力成长率","速度成长率","体力成长率","能力"};
			char buf3[][32]={"大幅提高","略为提高","略为减少"};
			if( !CHAR_CHECKINDEX( fd_charaindex ) )  return;
			if( !CHAR_CHECKINDEX( toindex ) )  return; 
//#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
			if( !ITEM_CHECKINDEX(itemindex) )  return;
			if( CHAR_getInt( toindex, CHAR_PETID) == 718 
#ifdef _PET_2LIMITLEVEL
				|| CHAR_getInt( toindex, CHAR_PETID) == 401 
#endif
			){

				int maxnums=50;

				if( CHAR_getInt( toindex,CHAR_LV ) < 74 )	{
					sprintf(buf1,"给我的吗？好美丽的项链喔！〈能力起了变化〉");
					CHAR_talkToCli( fd_charaindex, toindex, buf1, CHAR_COLORWHITE);
					num = ITEM_MODIFYATTACK;
					LevelUpPoint = CHAR_getInt( toindex, CHAR_ALLOCPOINT );
					petrank = CHAR_getInt( toindex, CHAR_PETRANK );
					work[3] =(( LevelUpPoint >> 24 ) & 0xFF);
					work[0] = (( LevelUpPoint >> 16 ) & 0xFF);
					work[1] = (( LevelUpPoint >> 8 ) & 0xFF);
					work[2] = (( LevelUpPoint >> 0 ) & 0xFF);
					for( i=0; i<4; i++)	{
						type = ITEM_getInt( itemindex, (num + i));
						work[i] += type;
						strcpy( buf1,"\0");
						if( work[i] > maxnums )	{
							sprintf(buf1,"%s 已经达到最高了。", buf2[i]);
							work[i] = maxnums;
						}else if( work[i] < 0 )	{
							sprintf(buf1,"%s 已经为零了。", buf2[i]);
							work[i] = 0;
						}else	{
							if( type > 0 )	{
								if( type > 2 )
									sprintf(buf1,"%s %s %s", buf2[i], buf3[0], "。");
								else
									sprintf(buf1,"%s %s %s", buf2[i], buf3[1], "。");
							}else if( type < 0 ){
								sprintf(buf1,"%s %s %s", buf2[i], buf3[2], "。");
							}
						}
						if( strcmp( buf1, "\0"))	{
							CHAR_talkToCli( fd_charaindex, toindex, buf1, CHAR_COLORWHITE);
						}
					}
				
					LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
					CHAR_setInt( toindex, CHAR_ALLOCPOINT, LevelUpPoint);
					CHAR_setInt( toindex, CHAR_PETRANK, petrank);
					LogPetPointChange(
						CHAR_getChar( fd_charaindex, CHAR_NAME ),
						CHAR_getChar( fd_charaindex, CHAR_CDKEY ),
						CHAR_getChar( fd_charaindex, CHAR_NAME),
						toindex, 4,
						CHAR_getInt( toindex, CHAR_LV),
						"item_use",
						CHAR_getInt( fd_charaindex, CHAR_FLOOR),
						CHAR_getInt( fd_charaindex, CHAR_X ),
						CHAR_getInt( fd_charaindex, CHAR_Y )
						);
					}
				
				}else	{
					sprintf(buf1,"？？？");
				}
			CHAR_DelItem( fd_charaindex, haveitemindex);
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
		if( seqno == CHAR_WINDOWTYPE_RAND3_EDITBASE )
		{
			if(strlen(data)<1)
				{
					CHAR_talkToCli( fd_charaindex, -1, "请输入结果！", CHAR_COLORRED);
					return;
				}
			char *arg;
			char buftest[255];
			int haveitemindex,itemindex,toindex;
			char ret[255];
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDEDITBASE);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			itemindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			toindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 3, buftest, sizeof(buftest)) == FALSE)
			return;
			haveitemindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 4, buftest, sizeof(buftest)) == FALSE)
			return;
			strcpy(ret,buftest);
			if(strcmp(ret,data)!=0){
				CHAR_talkToCli( fd_charaindex, -1, "你输入的结果不正确，请重新输入！", CHAR_COLORRED);
				return;
			}
			if(itemindex!=CHAR_getItemIndex( fd_charaindex, haveitemindex)){
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			int i;
			int work[4];
			int num=-1,type;
			int LevelUpPoint,petrank;
			char buf1[256];
			char buf2[][32]={"腕力成长率","耐久力成长率","速度成长率","体力成长率","能力"};
			char buf3[][32]={"大幅提高","略为提高","略为减少"};
			if( !CHAR_CHECKINDEX( fd_charaindex ) )  return;
			if( !CHAR_CHECKINDEX( toindex ) )  return; 
//#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
			if( !ITEM_CHECKINDEX(itemindex) )  return;
			if( CHAR_getInt( toindex, CHAR_PETID) == 718 
#ifdef _PET_2LIMITLEVEL
				|| CHAR_getInt( toindex, CHAR_PETID) == 401 
#endif
			){

				int maxnums=50;

				if( CHAR_getInt( toindex,CHAR_LV ) < 74 )	{
					sprintf(buf1,"给我的吗？好美丽的项链喔！〈能力起了变化〉");
					CHAR_talkToCli( fd_charaindex, toindex, buf1, CHAR_COLORWHITE);
					num = ITEM_MODIFYATTACK;
					LevelUpPoint = CHAR_getInt( toindex, CHAR_ALLOCPOINT );
					petrank = CHAR_getInt( toindex, CHAR_PETRANK );
					work[3] =(( LevelUpPoint >> 24 ) & 0xFF);
					work[0] = (( LevelUpPoint >> 16 ) & 0xFF);
					work[1] = (( LevelUpPoint >> 8 ) & 0xFF);
					work[2] = (( LevelUpPoint >> 0 ) & 0xFF);
					for( i=0; i<4; i++)	{
						type = ITEM_getInt( itemindex, (num + i));
						work[i] += type;
						strcpy( buf1,"\0");
						if( work[i] > maxnums )	{
							sprintf(buf1,"%s 已经达到最高了。", buf2[i]);
							work[i] = maxnums;
						}else if( work[i] < 0 )	{
							sprintf(buf1,"%s 已经为零了。", buf2[i]);
							work[i] = 0;
						}else	{
							if( type > 0 )	{
								if( type > 2 )
									sprintf(buf1,"%s %s %s", buf2[i], buf3[0], "。");
								else
									sprintf(buf1,"%s %s %s", buf2[i], buf3[1], "。");
							}else if( type < 0 ){
								sprintf(buf1,"%s %s %s", buf2[i], buf3[2], "。");
							}
						}
						if( strcmp( buf1, "\0"))	{
							CHAR_talkToCli( fd_charaindex, toindex, buf1, CHAR_COLORWHITE);
						}
					}
				
					LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
					CHAR_setInt( toindex, CHAR_ALLOCPOINT, LevelUpPoint);
					CHAR_setInt( toindex, CHAR_PETRANK, petrank);
					LogPetPointChange(
						CHAR_getChar( fd_charaindex, CHAR_NAME ),
						CHAR_getChar( fd_charaindex, CHAR_CDKEY ),
						CHAR_getChar( fd_charaindex, CHAR_NAME),
						toindex, 4,
						CHAR_getInt( toindex, CHAR_LV),
						"item_use",
						CHAR_getInt( fd_charaindex, CHAR_FLOOR),
						CHAR_getInt( fd_charaindex, CHAR_X ),
						CHAR_getInt( fd_charaindex, CHAR_Y )
						);
					}
				
				}else	{
					sprintf(buf1,"？？？");
				}
			CHAR_DelItem( fd_charaindex, haveitemindex);
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
		if( seqno == CHAR_WINDOWTYPE_RAND4_EDITBASE )
		{
			if(strlen(data)<1)
				{
					return;
				}
			//if(atoi(data)<getRandMin() || atoi(data)>getRandMax()) return;
			if(CHAR_getWorkInt(fd_charaindex,CHAR_WORKLUANPCINT)!=0) return;
			CHAR_setWorkInt(fd_charaindex,CHAR_WORKLUANPCINT,1);
			int cdkeynum;
			for(cdkeynum=9;cdkeynum>0;cdkeynum--){
				strcpy(mmcdkey[cdkeynum],mmcdkey[cdkeynum-1]);
				strcpy(mmcdkeybuf[cdkeynum],mmcdkeybuf[cdkeynum-1]);
			}
			strcpy(mmcdkey[0],CHAR_getChar(fd_charaindex,CHAR_CDKEY));
			sprintf(mmcdkeybuf[0],"%d.%d.%d",CHAR_getInt(fd_charaindex,CHAR_FLOOR),CHAR_getInt(fd_charaindex,CHAR_X),CHAR_getInt(fd_charaindex,CHAR_Y));
			time_t timep;
			struct tm *p;
			time(&timep);
			p=localtime(&timep);
			timep = mktime(p);
			CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,timep);
			char *arg;
			char buftest[255];
			int haveitemindex,itemindex,ret,toindex;
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDEDITBASE);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			itemindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			toindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 3, buftest, sizeof(buftest)) == FALSE)
			return;
			haveitemindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 4, buftest, sizeof(buftest)) == FALSE)
			return;
			ret = atoi(buftest);
			if(atoi(data)!=ret){
				if(ret>=1 && ret<=5){
					if(atoi(data)>=10000){
						sasql_cdkey_lock(CHAR_getChar(fd_charaindex,CHAR_CDKEY),15);
						CONNECT_setCloseRequest( fd , 1 );
						return;
					}
				}else{
					if(atoi(data)>=1 && atoi(data)<=5){
						//sasql_cdkey_lock(CHAR_getChar(fd_charaindex,CHAR_CDKEY),14);
						CONNECT_setCloseRequest( fd , 1 );
						return;
					}
				}
				CHAR_talkToCli( fd_charaindex, -1, "很遗憾，您输入的结果有误，请稍后再试。", CHAR_COLORRED);
				return;
			}
			if(itemindex!=CHAR_getItemIndex( fd_charaindex, haveitemindex)){
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			CHAR_talkToCli( fd_charaindex, -1, "恭喜您，您输入的结果正确。", CHAR_COLORRED);
			int i;
			int work[4];
			int num=-1,type;
			int LevelUpPoint,petrank;
			char buf1[256];
			char buf2[][32]={"腕力成长率","耐久力成长率","速度成长率","体力成长率","能力"};
			char buf3[][32]={"大幅提高","略为提高","略为减少"};
			if( !CHAR_CHECKINDEX( fd_charaindex ) )  return;
			if( !CHAR_CHECKINDEX( toindex ) )  return; 
//#define RAND(x,y)   ((x-1)+1+ (int)( (double)(y-(x-1))*rand()/(RAND_MAX+1.0)) )
			if( !ITEM_CHECKINDEX(itemindex) )  return;
			if( CHAR_getInt( toindex, CHAR_PETID) == 718 
#ifdef _PET_2LIMITLEVEL
				|| CHAR_getInt( toindex, CHAR_PETID) == 401 
#endif
			){

				int maxnums=50;

				if( CHAR_getInt( toindex,CHAR_LV ) < 74 )	{
					sprintf(buf1,"给我的吗？好美丽的项链喔！〈能力起了变化〉");
					CHAR_talkToCli( fd_charaindex, toindex, buf1, CHAR_COLORWHITE);
					num = ITEM_MODIFYATTACK;
					LevelUpPoint = CHAR_getInt( toindex, CHAR_ALLOCPOINT );
					petrank = CHAR_getInt( toindex, CHAR_PETRANK );
					work[3] =(( LevelUpPoint >> 24 ) & 0xFF);
					work[0] = (( LevelUpPoint >> 16 ) & 0xFF);
					work[1] = (( LevelUpPoint >> 8 ) & 0xFF);
					work[2] = (( LevelUpPoint >> 0 ) & 0xFF);
					for( i=0; i<4; i++)	{
						type = ITEM_getInt( itemindex, (num + i));
						work[i] += type;
						strcpy( buf1,"\0");
						if( work[i] > maxnums )	{
							sprintf(buf1,"%s 已经达到最高了。", buf2[i]);
							work[i] = maxnums;
						}else if( work[i] < 0 )	{
							sprintf(buf1,"%s 已经为零了。", buf2[i]);
							work[i] = 0;
						}else	{
							if( type > 0 )	{
								if( type > 2 )
									sprintf(buf1,"%s %s %s", buf2[i], buf3[0], "。");
								else
									sprintf(buf1,"%s %s %s", buf2[i], buf3[1], "。");
							}else if( type < 0 ){
								sprintf(buf1,"%s %s %s", buf2[i], buf3[2], "。");
							}
						}
						if( strcmp( buf1, "\0"))	{
							CHAR_talkToCli( fd_charaindex, toindex, buf1, CHAR_COLORWHITE);
						}
					}
				
					LevelUpPoint = ( work[3]<< 24) + ( work[0]<< 16) + ( work[1]<< 8) + ( work[2]<< 0);
					CHAR_setInt( toindex, CHAR_ALLOCPOINT, LevelUpPoint);
					CHAR_setInt( toindex, CHAR_PETRANK, petrank);
					LogPetPointChange(
						CHAR_getChar( fd_charaindex, CHAR_NAME ),
						CHAR_getChar( fd_charaindex, CHAR_CDKEY ),
						CHAR_getChar( fd_charaindex, CHAR_NAME),
						toindex, 4,
						CHAR_getInt( toindex, CHAR_LV),
						"item_use",
						CHAR_getInt( fd_charaindex, CHAR_FLOOR),
						CHAR_getInt( fd_charaindex, CHAR_X ),
						CHAR_getInt( fd_charaindex, CHAR_Y )
						);
					}
				
				}else	{
					sprintf(buf1,"？？？");
				}
			CHAR_DelItem( fd_charaindex, haveitemindex);
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
#endif
#ifdef	_MM_NO_JIAOBEN
		if( seqno == CHAR_WINDOWTYPE_RAND1_DROPMM )
		{
			if(strlen(data)<1)
				{
					CHAR_talkToCli( fd_charaindex, -1, "请输入计算结果！", CHAR_COLORRED);
					return;
				}
			char *arg;
			char buftest[255];
			int havepetindex,ret;
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKDROPMM);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			havepetindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			ret = atoi(buftest);
			if(atoi(data)!=ret){
				CHAR_talkToCli( fd_charaindex, -1, "你输入的计算结果不正确，请重新输入！", CHAR_COLORRED);
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			int petindex;
			petindex = CHAR_getCharPet(fd_charaindex,havepetindex);
    	if( !CHAR_CHECKINDEX( petindex )) return;
    	if(CHAR_getInt(petindex,CHAR_PETID)!=718 && CHAR_getInt(petindex,CHAR_PETID)!=401) return;
    	PET_dropPetFLXY(fd_charaindex, havepetindex, -1,-1,-1);
    	//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
		if( seqno == CHAR_WINDOWTYPE_RAND2_DROPMM )
		{
			if(strlen(data)<1)
				{
					return;
				}
			if(atoi(data)<1 || atoi(data)>5) return;
			char *arg;
			char buftest[255];
			int havepetindex,ret;
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKDROPMM);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			havepetindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			ret = atoi(buftest);
			if(atoi(data)!=ret){
				CHAR_talkToCli( fd_charaindex, -1, "很遗憾，您选择的计算结果有误，请稍后再试。", CHAR_COLORRED);
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			CHAR_talkToCli( fd_charaindex, -1, "恭喜您，您选择的计算结果正确。", CHAR_COLORRED);
			int petindex;
			petindex = CHAR_getCharPet(fd_charaindex,havepetindex);
    	if( !CHAR_CHECKINDEX( petindex )) return;
    	if(CHAR_getInt(petindex,CHAR_PETID)!=718 && CHAR_getInt(petindex,CHAR_PETID)!=401) return;
    	PET_dropPetFLXY(fd_charaindex, havepetindex, -1,-1,-1);
    	//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
		if( seqno == CHAR_WINDOWTYPE_RAND3_DROPMM )
		{
			if(strlen(data)<1)
				{
					CHAR_talkToCli( fd_charaindex, -1, "请输入结果！", CHAR_COLORRED);
					return;
				}
			char *arg;
			char buftest[255];
			int havepetindex;
			char ret[255];
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKDROPMM);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			havepetindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			strcpy(ret,buftest);
			if(strcmp(ret,data)!=0){
				CHAR_talkToCli( fd_charaindex, -1, "你输入的结果不正确，请重新输入！", CHAR_COLORRED);
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			int petindex;
			petindex = CHAR_getCharPet(fd_charaindex,havepetindex);
    	if( !CHAR_CHECKINDEX( petindex )) return;
    	if(CHAR_getInt(petindex,CHAR_PETID)!=718 && CHAR_getInt(petindex,CHAR_PETID)!=401) return;
    	PET_dropPetFLXY(fd_charaindex, havepetindex, -1,-1,-1);
    	//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
		if( seqno == CHAR_WINDOWTYPE_RAND4_DROPMM )
		{
			if(strlen(data)<1)
				{
					return;
				}
			//if(atoi(data)<getRandMin() || atoi(data)>getRandMax()) return;
			if(CHAR_getWorkInt(fd_charaindex,CHAR_WORKLUANPCINT)!=0) return;
			CHAR_setWorkInt(fd_charaindex,CHAR_WORKLUANPCINT,1);
			int cdkeynum;
			for(cdkeynum=9;cdkeynum>0;cdkeynum--){
				strcpy(mmcdkey[cdkeynum],mmcdkey[cdkeynum-1]);
				strcpy(mmcdkeybuf[cdkeynum],mmcdkeybuf[cdkeynum-1]);
			}
			strcpy(mmcdkey[0],CHAR_getChar(fd_charaindex,CHAR_CDKEY));
			sprintf(mmcdkeybuf[0],"%d.%d.%d",CHAR_getInt(fd_charaindex,CHAR_FLOOR),CHAR_getInt(fd_charaindex,CHAR_X),CHAR_getInt(fd_charaindex,CHAR_Y));
			time_t timep;
			struct tm *p;
			time(&timep);
			p=localtime(&timep);
			timep = mktime(p);
			CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,timep);
			char *arg;
			char buftest[255];
			int havepetindex,ret;
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKDROPMM);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			havepetindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			ret = atoi(buftest);
			if(atoi(data)!=ret){
				if(ret>=1 && ret<=5){
					if(atoi(data)>=10000){
						sasql_cdkey_lock(CHAR_getChar(fd_charaindex,CHAR_CDKEY),15);
						CONNECT_setCloseRequest( fd , 1 );
						return;
					}
				}else{
					if(atoi(data)>=1 && atoi(data)<=5){
						//sasql_cdkey_lock(CHAR_getChar(fd_charaindex,CHAR_CDKEY),14);
						CONNECT_setCloseRequest( fd , 1 );
						return;
					}
				}
				CHAR_talkToCli( fd_charaindex, -1, "很遗憾，您输入的结果有误，请稍后再试。", CHAR_COLORRED);
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			CHAR_talkToCli( fd_charaindex, -1, "恭喜您，您输入的结果正确。", CHAR_COLORRED);
			int petindex;
			petindex = CHAR_getCharPet(fd_charaindex,havepetindex);
    	if( !CHAR_CHECKINDEX( petindex )) return;
    	if(CHAR_getInt(petindex,CHAR_PETID)!=718 && CHAR_getInt(petindex,CHAR_PETID)!=401) return;
    	PET_dropPetFLXY(fd_charaindex, havepetindex, -1,-1,-1);
    	//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
		if( seqno == CHAR_WINDOWTYPE_RAND1_FLLOWMM )
		{
			if(strlen(data)<1)
				{
					CHAR_talkToCli( fd_charaindex, -1, "请输入计算结果！", CHAR_COLORRED);
					return;
				}
			char *arg;
			char buftest[255];
			int havepetindex,ret;
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKDROPMM);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			havepetindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			ret = atoi(buftest);
			if(atoi(data)!=ret){
				CHAR_talkToCli( fd_charaindex, -1, "你输入的计算结果不正确，请重新输入！", CHAR_COLORRED);
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			int petindex;
			petindex = CHAR_getCharPet(fd_charaindex,havepetindex);
    	if( !CHAR_CHECKINDEX( petindex )) return;
    	if(CHAR_getInt(petindex,CHAR_PETID)!=718 && CHAR_getInt(petindex,CHAR_PETID)!=401) return;
			if( !PET_dropPetFollow( fd_charaindex, havepetindex, -1, -1, -1 ) ) {
				CHAR_talkToCli( fd_charaindex, -1, "宠物跟随失败！", CHAR_COLORWHITE );
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
		if( seqno == CHAR_WINDOWTYPE_RAND2_FLLOWMM )
		{
			if(strlen(data)<1)
				{
					return;
				}
			if(atoi(data)<1 || atoi(data)>5) return;
			char *arg;
			char buftest[255];
			int havepetindex,ret;
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKDROPMM);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			havepetindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			ret = atoi(buftest);
			if(atoi(data)!=ret){
				CHAR_talkToCli( fd_charaindex, -1, "很遗憾，您选择的计算结果有误，请稍后再试。", CHAR_COLORRED);
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			CHAR_talkToCli( fd_charaindex, -1, "恭喜您，您选择的计算结果正确。", CHAR_COLORRED);
			int petindex;
			petindex = CHAR_getCharPet(fd_charaindex,havepetindex);
    	if( !CHAR_CHECKINDEX( petindex )) return;
    	if(CHAR_getInt(petindex,CHAR_PETID)!=718 && CHAR_getInt(petindex,CHAR_PETID)!=401) return;
			if( !PET_dropPetFollow( fd_charaindex, havepetindex, -1, -1, -1 ) ) {
				CHAR_talkToCli( fd_charaindex, -1, "宠物跟随失败！", CHAR_COLORWHITE );
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
		if( seqno == CHAR_WINDOWTYPE_RAND3_FLLOWMM )
		{
			if(strlen(data)<1)
				{
					CHAR_talkToCli( fd_charaindex, -1, "请输入结果！", CHAR_COLORRED);
					return;
				}
			char *arg;
			char buftest[255];
			int havepetindex;
			char ret[255];
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKDROPMM);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			havepetindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			strcpy(ret,buftest);
			if(strcmp(ret,data)!=0){
				CHAR_talkToCli( fd_charaindex, -1, "你输入的结果不正确，请重新输入！", CHAR_COLORRED);
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			int petindex;
			petindex = CHAR_getCharPet(fd_charaindex,havepetindex);
    	if( !CHAR_CHECKINDEX( petindex )) return;
    	if(CHAR_getInt(petindex,CHAR_PETID)!=718 && CHAR_getInt(petindex,CHAR_PETID)!=401) return;
			if( !PET_dropPetFollow( fd_charaindex, havepetindex, -1, -1, -1 ) ) {
				CHAR_talkToCli( fd_charaindex, -1, "宠物跟随失败！", CHAR_COLORWHITE );
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
		if( seqno == CHAR_WINDOWTYPE_RAND4_FLLOWMM )
		{
			if(strlen(data)<1)
				{
					return;
				}
			//if(atoi(data)<getRandMin() || atoi(data)>getRandMax()) return;
			if(CHAR_getWorkInt(fd_charaindex,CHAR_WORKLUANPCINT)!=0) return;
			CHAR_setWorkInt(fd_charaindex,CHAR_WORKLUANPCINT,1);
			int cdkeynum;
			for(cdkeynum=9;cdkeynum>0;cdkeynum--){
				strcpy(mmcdkey[cdkeynum],mmcdkey[cdkeynum-1]);
				strcpy(mmcdkeybuf[cdkeynum],mmcdkeybuf[cdkeynum-1]);
			}
			strcpy(mmcdkey[0],CHAR_getChar(fd_charaindex,CHAR_CDKEY));
			sprintf(mmcdkeybuf[0],"%d.%d.%d",CHAR_getInt(fd_charaindex,CHAR_FLOOR),CHAR_getInt(fd_charaindex,CHAR_X),CHAR_getInt(fd_charaindex,CHAR_Y));
			time_t timep;
			struct tm *p;
			time(&timep);
			p=localtime(&timep);
			timep = mktime(p);
			CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,timep);
			char *arg;
			char buftest[255];
			int havepetindex,ret;
			arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKDROPMM);
			if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
			return;
			havepetindex = atoi(buftest);
			if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
			return;
			ret = atoi(buftest);
			if(atoi(data)!=ret){
				if(ret>=1 && ret<=5){
					if(atoi(data)>=10000){
						sasql_cdkey_lock(CHAR_getChar(fd_charaindex,CHAR_CDKEY),15);
						CONNECT_setCloseRequest( fd , 1 );
						return;
					}
				}else{
					if(atoi(data)>=1 && atoi(data)<=5){
						//sasql_cdkey_lock(CHAR_getChar(fd_charaindex,CHAR_CDKEY),14);
						CONNECT_setCloseRequest( fd , 1 );
						return;
					}
				}
				CHAR_talkToCli( fd_charaindex, -1, "很遗憾，您输入的结果有误，请稍后再试。", CHAR_COLORRED);
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
			CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
			CHAR_talkToCli( fd_charaindex, -1, "恭喜您，您输入的结果正确。", CHAR_COLORRED);
			int petindex;
			petindex = CHAR_getCharPet(fd_charaindex,havepetindex);
    	if( !CHAR_CHECKINDEX( petindex )) return;
    	if(CHAR_getInt(petindex,CHAR_PETID)!=718 && CHAR_getInt(petindex,CHAR_PETID)!=401) return;
			if( !PET_dropPetFollow( fd_charaindex, havepetindex, -1, -1, -1 ) ) {
				CHAR_talkToCli( fd_charaindex, -1, "宠物跟随失败！", CHAR_COLORWHITE );
				return;
			}
			//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
			return;
		}
#endif
#ifdef	_DP_ZHIPIAO_ITEM
		if( seqno == CHAR_WINDOWTYPE_DPZHIPIAO_1 )
		{
			if(strlen(data)<1)
				{
					CHAR_talkToCli( fd_charaindex, -1, "请输入计算结果！", CHAR_COLORRED);
					return;
				}
			if(atoi(data)!=CHAR_getWorkInt(fd_charaindex,CHAR_WORKDPZHIPIAORAND)){
				CHAR_talkToCli( fd_charaindex, -1, "你输入的计算结果不正确，请重新输入！", CHAR_COLORRED);
				return;
			}
			char buf[255];
			lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT, 
            WINDOW_BUTTONTYPE_OKCANCEL,
            CHAR_WINDOWTYPE_DPZHIPIAO_2,
            -1,
            makeEscapeString( "请输入你想要兑换的DP点数：\n", buf, sizeof(buf)));
		}
		if( seqno == CHAR_WINDOWTYPE_DPZHIPIAO_2 ){
			if(select == WINDOW_BUTTONTYPE_OK){
				if(strlen(data)<1) return;
				if(atoi(data)<100){
					CHAR_talkToCli( fd_charaindex, -1, "最小支票金额为100DP，少于则无法签发！",  CHAR_COLORYELLOW);
					return;
				}
				if(atoi(data)>10000000){
					CHAR_talkToCli( fd_charaindex, -1, "最大支票金额为1000WDP，多于则无法签发！",  CHAR_COLORYELLOW);
					return;
				}
				char zhipiaomsg[256];
				int dppoint = atoi(data)*CHAR_getWorkInt(fd_charaindex,CHAR_WORKDPZHIPIAOSHUI)/100;
				if(dppoint<10) dppoint = dppoint+1;
				sprintf(zhipiaomsg,"\n签发支票需要收取百分之%d的税率\n\n此支票的签发需要扣除%dDP\n\n您确定要制作么？",CHAR_getWorkInt(fd_charaindex,CHAR_WORKDPZHIPIAOSHUI),dppoint+atoi(data));
				lssproto_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE, 
            WINDOW_BUTTONTYPE_OKCANCEL,
            CHAR_WINDOWTYPE_DPZHIPIAO_3,
            -1,
            zhipiaomsg);
        CHAR_setWorkInt(fd_charaindex,CHAR_WORKDPZHIPIAOPOINT,atoi(data));
        return;
			}else
				return;
		}
		if( seqno == CHAR_WINDOWTYPE_DPZHIPIAO_3 ){
			if(select == WINDOW_BUTTONTYPE_OK){
				int mypoint = CHAR_getInt(fd_charaindex,CHAR_DUELPOINT);
				int itempoint = CHAR_getWorkInt(fd_charaindex,CHAR_WORKDPZHIPIAOPOINT);
				int kdppoint = itempoint/100*CHAR_getWorkInt(fd_charaindex,CHAR_WORKDPZHIPIAOSHUI);
				if(kdppoint<10) kdppoint =kdppoint+1;
				int dppoint = itempoint+kdppoint;
				if(mypoint<dppoint){
					CHAR_talkToCli( fd_charaindex, -1, "您的DP不够，无法兑换！",  CHAR_COLORRED);
					return;
				}else{
					int itemindex = CHAR_getItemIndex(fd_charaindex,CHAR_getWorkInt(fd_charaindex,CHAR_WORKDPZHIPIAOITEM));
					if( ITEM_CHECKINDEX(itemindex) == FALSE )return;
					CHAR_setInt(fd_charaindex,CHAR_DUELPOINT,mypoint-dppoint);
					ITEM_setChar(itemindex,ITEM_NAME,"DP支票");
					ITEM_setChar(itemindex,ITEM_SECRETNAME,"DP支票");
					char zhipiaomsg[256];
					sprintf(zhipiaomsg,"本DP支票面值%dDP",itempoint);
					ITEM_setChar(itemindex,ITEM_EFFECTSTRING,zhipiaomsg);
					sprintf(zhipiaomsg,"1|%d",itempoint);
					ITEM_setChar(itemindex,ITEM_ARGUMENT,zhipiaomsg);
					CHAR_sendItemDataOne( fd_charaindex, CHAR_getWorkInt(fd_charaindex,CHAR_WORKDPZHIPIAOITEM));
					CHAR_talkToCli( fd_charaindex, -1, "DP支票制作成功！",  CHAR_COLORRED);
					sprintf(zhipiaomsg,"扣除%dDP",dppoint);
					CHAR_talkToCli(fd_charaindex, -1, zhipiaomsg, CHAR_COLORYELLOW);
					CHAR_send_P_StatusString( fd_charaindex ,CHAR_P_STRING_DUELPOINT);
					CHAR_send_DpDBUpdate_AddressBook( fd_charaindex, TRUE );
					CHAR_charSaveFromConnect(fd_charaindex, FALSE);
					CHAR_talkToCli(fd_charaindex, -1, "系统为你自动存档！", CHAR_COLORRED);
					return;
				}
			}else{
				return;
			}
		}
#endif
#ifdef	_NPC_RAND_MSG	
		if( seqno == CHAR_WINDOWTYPE_NPC_RAND1_MSG ){
			if(select == WINDOW_BUTTONTYPE_OK){
				if(strlen(data)<1)
				{
					CHAR_talkToCli( fd_charaindex, -1, "请输入计算结果！", CHAR_COLORRED);
					return;
				}
				char *arg;
				char buftest[255];
				int npcindex,ret;
				char *szmsg;
				arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDMSG);
				if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
					return;
				ret = atoi(buftest);
				if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
					return;
				npcindex = atoi(buftest);
				if(getStringFromIndexWithDelim(arg,"|", 3, buftest, sizeof(buftest)) == FALSE)
					return;
				szmsg = buftest;
				if(atoi(data)!=ret){
					CHAR_talkToCli( fd_charaindex, -1, "你输入的计算结果不正确，请重新输入！", CHAR_COLORRED);
					return;
				}
				//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
				CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
				NPC_TypeCheck( npcindex, fd_charaindex, szmsg);
				//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
				return;
			}else
				return;
		}
		if( seqno == CHAR_WINDOWTYPE_NPC_RAND2_MSG ){
				if(strlen(data)<1)
				{
					return;
				}
				if(atoi(data)<1 || atoi(data)>5) return;
				char *arg;
				char buftest[255];
				int npcindex,ret;
				char *szmsg;
				arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDMSG);
				if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
					return;
				ret = atoi(buftest);
				if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
					return;
				npcindex = atoi(buftest);
				if(getStringFromIndexWithDelim(arg,"|", 3, buftest, sizeof(buftest)) == FALSE)
					return;
				szmsg = buftest;
				if(atoi(data)!=ret){
					CHAR_talkToCli( fd_charaindex, -1, "很遗憾，您选择的计算结果有误，请稍后再试。", CHAR_COLORRED);
					return;
				}
				//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
				CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
				CHAR_talkToCli( fd_charaindex, -1, "恭喜您，您选择的计算结果正确。", CHAR_COLORRED);
				NPC_TypeCheck( npcindex, fd_charaindex, szmsg);
				//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
				return;
		}
		if( seqno == CHAR_WINDOWTYPE_NPC_RAND3_MSG ){
			if(select == WINDOW_BUTTONTYPE_OK){
				if(strlen(data)<1)
				{
					CHAR_talkToCli( fd_charaindex, -1, "请输入结果！", CHAR_COLORRED);
					return;
				}
				char *arg;
				char buftest[255];
				int npcindex;
				char ret[255];
				char *szmsg;
				arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDMSG);
				if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE)
					return;
				strcpy(ret,buftest);
				if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE)
					return;
				npcindex = atoi(buftest);
				if(getStringFromIndexWithDelim(arg,"|", 3, buftest, sizeof(buftest)) == FALSE)
					return;
				szmsg = buftest;
				if(strcmp(ret,data)!=0){
					CHAR_talkToCli( fd_charaindex, -1, "你输入的结果不正确，请重新输入！", CHAR_COLORRED);
					return;
				}
				//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
				CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
				NPC_TypeCheck( npcindex, fd_charaindex, szmsg);
				//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
				return;
			}else
				return;
		}
		if( seqno == CHAR_WINDOWTYPE_NPC_RAND4_MSG ){
				if(strlen(data)<1)
				{
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				//if(atoi(data)<getRandMin() || atoi(data)>getRandMax()) return;
				if(CHAR_getWorkInt(fd_charaindex,CHAR_WORKLUANPCINT)!=0){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				CHAR_setWorkInt(fd_charaindex,CHAR_WORKLUANPCINT,1);
				int cdkeynum;
				for(cdkeynum=9;cdkeynum>0;cdkeynum--){
					strcpy(mmcdkey[cdkeynum],mmcdkey[cdkeynum-1]);
					strcpy(mmcdkeybuf[cdkeynum],mmcdkeybuf[cdkeynum-1]);
				}
				strcpy(mmcdkey[0],CHAR_getChar(fd_charaindex,CHAR_CDKEY));
				sprintf(mmcdkeybuf[0],"%d.%d.%d",CHAR_getInt(fd_charaindex,CHAR_FLOOR),CHAR_getInt(fd_charaindex,CHAR_X),CHAR_getInt(fd_charaindex,CHAR_Y));
				time_t timep;
				struct tm *p;
				time(&timep);
				p=localtime(&timep);
				timep = mktime(p);
				CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,timep);
				char *arg;
				char buftest[255];
				int npcindex,ret;
				char *szmsg;
				arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDMSG);
				if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				ret = atoi(buftest);
				if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				npcindex = atoi(buftest);
				if(getStringFromIndexWithDelim(arg,"|", 3, buftest, sizeof(buftest)) == FALSE){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				szmsg = buftest;
				if(atoi(data)!=ret){
					if(ret>=1 && ret<=5){
						if(atoi(data)>=10000){
							sasql_cdkey_lock(CHAR_getChar(fd_charaindex,CHAR_CDKEY),15);
							CONNECT_setCloseRequest( fd , 1 );
							return;
						}
					}else{
						if(atoi(data)>=1 && atoi(data)<=5){
							//sasql_cdkey_lock(CHAR_getChar(fd_charaindex,CHAR_CDKEY),14);
							CONNECT_setCloseRequest( fd , 1 );
							return;
						}
					}
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					CHAR_talkToCli( fd_charaindex, -1, "很遗憾，您输入的结果有误，请稍后再试。", CHAR_COLORRED);
					return;
				}
				//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
				CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
				CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
				CHAR_talkToCli( fd_charaindex, -1, "恭喜您，您输入的结果正确。", CHAR_COLORRED);
				NPC_TypeCheck( npcindex, fd_charaindex, szmsg);
				//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
				return;
		}

		if( seqno == CHAR_WINDOWTYPE_NPCENEMY_RAND_MSG ){
				if(strlen(data)<1)
				{
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				//if(atoi(data)<getRandMin() || atoi(data)>getRandMax()) return;
				if(CHAR_getWorkInt(fd_charaindex,CHAR_WORKLUANPCINT)!=0){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				CHAR_setWorkInt(fd_charaindex,CHAR_WORKLUANPCINT,1);
				int cdkeynum;
				for(cdkeynum=9;cdkeynum>0;cdkeynum--){
					strcpy(mmcdkey[cdkeynum],mmcdkey[cdkeynum-1]);
					strcpy(mmcdkeybuf[cdkeynum],mmcdkeybuf[cdkeynum-1]);
				}
				strcpy(mmcdkey[0],CHAR_getChar(fd_charaindex,CHAR_CDKEY));
				sprintf(mmcdkeybuf[0],"%d.%d.%d",CHAR_getInt(fd_charaindex,CHAR_FLOOR),CHAR_getInt(fd_charaindex,CHAR_X),CHAR_getInt(fd_charaindex,CHAR_Y));
				time_t timep;
				struct tm *p;
				time(&timep);
				p=localtime(&timep);
				timep = mktime(p);
				CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,timep);
				char *arg;
				char buftest[255];
				int npcindex,ret;
				arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDMSG);
				if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				ret = atoi(buftest);
				if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				npcindex = atoi(buftest);
				if(atoi(data)!=ret){
					if(ret>=1 && ret<=5){
						if(atoi(data)>=10000){
							sasql_cdkey_lock(CHAR_getChar(fd_charaindex,CHAR_CDKEY),15);
							CONNECT_setCloseRequest( fd , 1 );
							return;
						}
					}else{
						if(atoi(data)>=1 && atoi(data)<=5){
							CONNECT_setCloseRequest( fd , 1 );
							return;
						}
					}
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					CHAR_talkToCli( fd_charaindex, -1, "很遗憾，您输入的结果有误，请稍后再试。", CHAR_COLORRED);
					return;
				}
				//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,0);
				CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
				CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
				CHAR_talkToCli( fd_charaindex, -1, "恭喜您，您输入的结果正确。", CHAR_COLORRED);
				NPC_NPCEnemy_Encount( npcindex, fd_charaindex, 1);
				//CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDTYPE,0);
				return;
		}

		if( seqno == CHAR_WINDOWTYPE_LUANPC_RANDMSG ){
				if(strlen(data)<1)
				{
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				//if(atoi(data)<getRandMin() || atoi(data)>getRandMax()) return;
				if(CHAR_getWorkInt(fd_charaindex,CHAR_WORKLUANPCINT)!=0){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				CHAR_setWorkInt(fd_charaindex,CHAR_WORKLUANPCINT,1);
				int cdkeynum;
				for(cdkeynum=9;cdkeynum>0;cdkeynum--){
					strcpy(mmcdkey[cdkeynum],mmcdkey[cdkeynum-1]);
					strcpy(mmcdkeybuf[cdkeynum],mmcdkeybuf[cdkeynum-1]);
				}
				strcpy(mmcdkey[0],CHAR_getChar(fd_charaindex,CHAR_CDKEY));
				sprintf(mmcdkeybuf[0],"%d.%d.%d",CHAR_getInt(fd_charaindex,CHAR_FLOOR),CHAR_getInt(fd_charaindex,CHAR_X),CHAR_getInt(fd_charaindex,CHAR_Y));
				time_t timep;
				struct tm *p;
				time(&timep);
				p=localtime(&timep);
				timep = mktime(p);
				CHAR_setWorkInt(fd_charaindex,CHAR_WORKRANDEDITBASETIME,timep);
				char *arg;
				char buftest[255];
				int npcindex,talkindex,ret,flag;
				arg = CHAR_getWorkChar(fd_charaindex,CHAR_WORKRANDMSG);
				if(getStringFromIndexWithDelim(arg,"|", 1, buftest, sizeof(buftest)) == FALSE){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				ret = atoi(buftest);
				if(getStringFromIndexWithDelim(arg,"|", 2, buftest, sizeof(buftest)) == FALSE){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				npcindex = atoi(buftest);
				if(getStringFromIndexWithDelim(arg,"|", 3, buftest, sizeof(buftest)) == FALSE){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				talkindex = atoi(buftest);
				if(getStringFromIndexWithDelim(arg,"|", 4, buftest, sizeof(buftest)) == FALSE){
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					return;
				}
				flag = atoi(buftest);
				if(atoi(data)!=ret){
					if(ret>=1 && ret<=5){
						if(atoi(data)>=10000){
							sasql_cdkey_lock(CHAR_getChar(fd_charaindex,CHAR_CDKEY),15);
							CONNECT_setCloseRequest( fd , 1 );
							return;
						}
					}else{
						if(atoi(data)>=1 && atoi(data)<=5){
							//sasql_cdkey_lock(CHAR_getChar(fd_charaindex,CHAR_CDKEY),14);
							CONNECT_setCloseRequest( fd , 1 );
							return;
						}
					}
					CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,"");
					CHAR_talkToCli( fd_charaindex, -1, "很遗憾，您输入的结果有误，请稍后再试。", CHAR_COLORRED);
					return;
				}
				sprintf(buftest,"%d|%d|%d|%d",ret,npcindex,talkindex,1);
				CHAR_setWorkChar(fd_charaindex,CHAR_WORKRANDMSG,buftest);
				CHAR_setChar(fd_charaindex,CHAR_RANDTYPE,"");
				CHAR_talkToCli( fd_charaindex, -1, "恭喜您，您输入的结果正确。", CHAR_COLORRED);
				NPC_Lua_TalkedCallBack(npcindex,fd_charaindex,"hi",0,-1);
				return;
		}
#endif
    {//ttom avoid the warp at will
       Char *ch;
       ch = CHAR_getCharPointer( fd_charaindex);
	   // CoolFish: +1 2001/11/05
	   if (!ch)	return;
       if((ch->data[CHAR_X]!=x)||(ch->data[CHAR_Y]!=y)){
           // Robin 04/20 test
           return;
           x=ch->data[CHAR_X];
           y=ch->data[CHAR_Y];
       }
     //ttom avoid WN at will
     if(seqno==CHAR_WINDOWTYPE_NPCENEMY_START){
     	//print("12121212\n");
//      Char *ch;
        OBJECT  object;
        int ix,iy,ifloor,i,j;
        int     whichtype= -1;
        int enemy=0;
        int enemy_index;
        if(!CHECKOBJECT(objindex)){
            goto  FIRST;
        }
        enemy_index=OBJECT_getIndex(objindex);//ttom 11/15/2000
        ix    =ch->data[CHAR_X];
        iy    =ch->data[CHAR_Y];
        ifloor=ch->data[CHAR_FLOOR];
        for(i=iy-1;i<=iy+1;i++){
            for(j=ix-1;j<=ix+1;j++){
             for( object = MAP_getTopObj(ifloor,j,i) ; object ;
                  object = NEXT_OBJECT(object ) ){
                  int objindex = GET_OBJINDEX(object);
                  switch( OBJECT_getType(objindex)  ){
                          case OBJTYPE_CHARA:
                               whichtype =  CHAR_getInt( OBJECT_getIndex( objindex), CHAR_WHICHTYPE);
                               if( whichtype == CHAR_TYPENPCENEMY){
                                   int i_ene_temp;
                                   i_ene_temp=OBJECT_getIndex( objindex);
                                   //print("\n<WN>--enetemp=%d",i_ene_temp);
                                   if(i_ene_temp== enemy_index){
                                       goto START_WN;
                                   }else{
                                       enemy=0;
                                   }
                               }
                               break;
                          case OBJTYPE_ITEM:
                               break;
                          case OBJTYPE_GOLD:
                               break;
                          default:
                           break;
                  }
             }
            }
        }
        if(enemy==0){
FIRST:
		
           lssproto_EN_send( fd, FALSE, 0 );
           CHAR_talkToCli(fd_charaindex, -1, "事件错误。", CHAR_COLORYELLOW);
           goto END_WN;
        }
     }
     }
     //ttom end
START_WN:
        CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE) == BATTLE_CHARMODE_NONE){
	         if(seqno==CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN){
		         if(!CONNECT_get_first_warp(fd)){
		             select=1;
	        	 }
		     }
	         CHAR_processWindow( fd_charaindex, seqno,
							select, objindex, makeStringFromEscaped(data));
							
//#ifdef _NO_WARP
//			 if (CONNECT_get_seqno(fd)==CHAR_WINDOWTYPE_WINDOWWARPMAN_MAIN)
//             {
//				 CONNECT_set_seqno(fd,-1);
//				 CONNECT_set_selectbutton(fd,1);
//			 }
//#endif
	}
//ttom+1
END_WN:
	return;
          //CONNECT_set_pass(fd,TRUE);//ttom
}

/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_HL_recv( int fd,int flg )
{
	char	msgbuf[128];
	int		i, fd_charaindex;
    CHECKFD;

    fd_charaindex = CONNECT_getCharaindex( fd );
	if( CHAR_getWorkInt( fd_charaindex, CHAR_WORKBATTLEMODE)
		== BATTLE_CHARMODE_NONE)
	{
		return;
	}
#ifdef _LOCKHELP_OK				// (不可开) Syu ADD 锁定不可加入战斗
	if((CHAR_getInt(fd_charaindex,CHAR_FLOOR) >= 8200 && CHAR_getInt(fd_charaindex,CHAR_FLOOR) <= 8213) ||
		 (CHAR_getInt(fd_charaindex,CHAR_FLOOR) >= 30017 && CHAR_getInt(fd_charaindex,CHAR_FLOOR) <= 30021)
		 || CHAR_getInt(fd_charaindex,CHAR_FLOOR) == 20000
		){
		return ; 
	}
#endif
#ifdef	_NPCENEMY_1
	if(CHAR_getWorkInt(fd_charaindex,CHAR_WORKBATTLEHELP)==1) return;
#endif		
	if( flg == TRUE ) {
		BattleArray[CHAR_getWorkInt( fd_charaindex,
			CHAR_WORKBATTLEINDEX)].Side[
			CHAR_getWorkInt( fd_charaindex,
			CHAR_WORKBATTLESIDE)].flg |= BSIDE_FLG_HELP_OK;

		snprintf( msgbuf, sizeof( msgbuf),
				  "%s 在求救！",
				  CHAR_getChar( fd_charaindex, CHAR_NAME));
	}
	else {
		BattleArray[CHAR_getWorkInt( fd_charaindex,
			CHAR_WORKBATTLEINDEX)].Side[
			CHAR_getWorkInt( fd_charaindex,
			CHAR_WORKBATTLESIDE)].flg &= ~BSIDE_FLG_HELP_OK;

		snprintf( msgbuf, sizeof( msgbuf),
				  "%s 决定拒绝帮助。",
				  CHAR_getChar( fd_charaindex, CHAR_NAME));
	}

	for( i = 0; i < 5; i ++ ) {
		int toindex = BattleArray[CHAR_getWorkInt(
					    fd_charaindex, CHAR_WORKBATTLEINDEX)].Side[
						CHAR_getWorkInt( fd_charaindex,
						CHAR_WORKBATTLESIDE)].Entry[i].charaindex;
		if( CHAR_CHECKINDEX( toindex)) {
			int tofd = getfdFromCharaIndex( toindex );
			if( tofd != -1 ) {
				lssproto_HL_send( tofd, flg);
			}
			/* 丢永本□斥霜耨 */
			CHAR_talkToCli( toindex, -1, msgbuf, CHAR_COLORYELLOW);
			CHAR_sendBattleEffect( toindex, ON);
		}
	}
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_ProcGet_recv( int fd )
{
	outputNetProcLog( fd, 1);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_PlayerNumGet_recv( int fd )
{
	int		i;
	int		clicnt  =0;
	int		playercnt = 0;
    for( i = 0; i < ConnectLen; i ++ ) {
        if( CONNECT_getUse_debug(i,1017) ){
            if( CONNECT_getCtype(i) == CLI) {
            	clicnt ++;
            	if( CONNECT_getCharaindex(i) >= 0 ) playercnt++;
            }
		}
	}
	lssproto_PlayerNumGet_send( fd, clicnt, playercnt);
}


/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_LB_recv( int fd,int x,int y )
{
    int fd_charaindex;

    CHECKFDANDTIME;
    fd_charaindex = CONNECT_getCharaindex( fd );
    {//ttom avoid warp at will
       int ix,iy;
       ix=CHAR_getInt(fd_charaindex, CHAR_X);
       iy=CHAR_getInt(fd_charaindex, CHAR_Y);
       if( (ix!=x)||(iy!=y)){
         //print("\n<LB>--Error!!!!");
         //print("\n<LB>origion x=%d,y=%d",ix,iy);
         //print("\n<LB>modify  X=%d,Y=%d",x,y);
         x=ix;
         y=iy;
       }
    }   
    CHAR_setMyPosition( fd_charaindex, x,y,TRUE);
	/* 杀梁匹五月井民尼永弁仄化杀梁允月 */
	BATTLE_WatchTry( fd_charaindex );
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_Shutdown_recv( int fd,char* passwd,int min )
{
	char	buff[32];
	if( strcmp( passwd, "hogehoge") == 0 ) {
	    int     i;
	    int     playernum = CHAR_getPlayerMaxNum();
		snprintf( buff, sizeof( buff),"%s公告。",getGameserverID());
	    for( i = 0 ; i < playernum ; i++) {
	        if( CHAR_getCharUse(i) != FALSE ) {
				CHAR_talkToCli( i, -1, buff, CHAR_COLORYELLOW);
			}
		}
		SERVSTATE_setLimittime(min);
		SERVSTATE_setShutdown( NowTime.tv_sec );
		SERVSTATE_setDsptime( 0 );
	}
}
void lssproto_PMSG_recv( int fd,int index,int petindex,int itemindex,
						char* message,int color )
{

	// CoolFish: Prevent Trade Cheat 2001/4/18
	int fd_charaindex;
	fd_charaindex = CONNECT_getCharaindex(fd);
	if (CHAR_getWorkInt(fd_charaindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE)
	    	return;
	if( CHAR_getWorkInt(fd_charaindex,CHAR_WORKSTREETVENDOR)!=-1) return;
	PETMAIL_sendPetMail( CONNECT_getCharaindex( fd ),
							index, petindex, itemindex, message, color);

}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_PS_recv( int fd, int havepetindex, int havepetskill, int toindex, char* data )
{
    int to_charaindex = Callfromcli_Util_getTargetCharaindex( fd, toindex);
	int charaindex = CONNECT_getCharaindex( fd );
	int	petindex;
	BOOL	ret;
	petindex = CHAR_getCharPet( charaindex, havepetindex);
	if( !CHAR_CHECKINDEX( petindex)) return;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE && strlen(data)>0) return;
	ret = PETSKILL_Use( petindex, havepetskill, to_charaindex, data );
	lssproto_PS_send( fd, ret, havepetindex, havepetskill, toindex);
}
/*------------------------------------------------------------
 ------------------------------------------------------------*/
void lssproto_SP_recv( int fd,int x,int y, int dir  )
{
    int fd_charaindex;

    fd_charaindex = CONNECT_getCharaindex( fd );
    {//ttom avoid the warp at will
       int i_x,i_y;
       i_x=CHAR_getInt(fd_charaindex, CHAR_X);
       i_y=CHAR_getInt(fd_charaindex, CHAR_Y);
                         
       if((i_x!=x)||(i_y!=y)){
           x=i_x;
           y=i_y;
       }
    }//ttom
                                                                                       
    CHAR_setMyPosition_main( fd_charaindex, x,y,dir,TRUE);

}

/*------------------------------------------------------------
 * CoolFish: Trade Command 2001/4/18
 ------------------------------------------------------------*/
void lssproto_TD_recv( int fd, char* message )
{
      int fd_charaindex;
      CHECKFDANDTIME;
          
      fd_charaindex = CONNECT_getCharaindex( fd );
//      print(" MAP_TRADEPICKUP_check0 ");

      CHAR_Trade(fd, fd_charaindex, message);
}

/*------------------------------------------------------------
 * CoolFish: Family Command 2001/5/24
 ------------------------------------------------------------*/
void lssproto_FM_recv( int fd, char* message )
{
      int fd_charaindex;
      struct timeval recvtime;
      CHECKFDANDTIME;
      
      // add code by shan
      CONNECT_getLastrecvtime( fd, &recvtime);
      if( time_diff( NowTime, recvtime) < 0.5 ){
          return;
      }
      CONNECT_setLastrecvtime(fd, &NowTime);

      fd_charaindex = CONNECT_getCharaindex( fd );
      
      if( checkStringErr( message ) )	return;
      
      CHAR_Family(fd, fd_charaindex, message);
      
}

// shan 2002/01/10
void lssproto_PETST_recv( int fd,  int nPet, int sPet )
{
	if(nPet<0 || nPet>4) return;
      int charaindex;
	  int i, nums=0;
      CHECKFDANDTIME;
      
      charaindex = CONNECT_getCharaindex( fd );
	  if (!CHAR_CHECKINDEX( charaindex ) )	return;

	  if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE)
		!= BATTLE_CHARMODE_NONE) return ;
	  
	  for( i=0; i<5; i++)	{
		if( CHAR_getWorkInt( charaindex, CHAR_WORK_PET0_STAT+i) == TRUE )
			nums++;
	  }
	  if( nums <= 3 )
		CHAR_setWorkInt( charaindex, CHAR_WORK_PET0_STAT+nPet, sPet);
//		if( nums ==3 && CHAR_getWorkInt(charaindex,CHAR_WORK_PET0_STAT+nPet)==0){
//			CHAR_setWorkInt( charaindex, CHAR_WORK_PET0_STAT+nPet, 1);
//			lssproto_SPET_recv(fd,nPet+1);
//		}
}

#ifdef _MIND_ICON
void lssproto_MA_recv(int fd, int x, int y, int nMind)
{
	int charaindex;
    CHECKFDANDTIME;

	charaindex = CONNECT_getCharaindex( fd);
	if(!CHAR_CHECKINDEX( charaindex)) return;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return ;

	{
       int i_x, i_y;
       i_x = CHAR_getInt( charaindex, CHAR_X);
       i_y = CHAR_getInt( charaindex, CHAR_Y);

       if((i_x!=x)||(i_y!=y)){
           x = i_x;
           y = i_y;
       }
    }

	//print("\nshan------------------>mind action->%d x->%d y->%d", nMind, x, y);
	CHAR_setMyPosition( charaindex, x, y, TRUE);
	CHAR_setWorkInt( charaindex, CHAR_MIND_NUM, nMind);
	CHAR_sendMindEffect( charaindex, CHAR_getWorkInt( charaindex, CHAR_MIND_NUM));
	if(CHAR_getWorkInt( charaindex, CHAR_MIND_NUM) != 101290 &&
	   CHAR_getWorkInt( charaindex, CHAR_MIND_NUM) != 101294   &&
	   CHAR_getWorkInt( charaindex, CHAR_MIND_NUM) != 101288 )
	CHAR_setWorkInt( charaindex, CHAR_MIND_NUM, 0);
	//print("\nshan------------------>end");

	return;
}
#endif
BOOL checkStringErr( char *checkstring )
{
        int i,ach;
        for (i=0,ach=0;i<strlen(checkstring);i++) {
        	if ((unsigned char)checkstring[i]==0xff) { ach=1; break; } // Force no 0xff
                if ((unsigned char)checkstring[i]==0x80) { ach=1; break; } // Force no 0x80
                if ((unsigned char)checkstring[i]==0x7f) { ach=1; break; } // Force no 0x7f
                if ((unsigned char)checkstring[i]<=0x20) { ach=1; break; } // Force greater than 0x20
                if (ach) {
                	if ((((unsigned char)checkstring[i]>=0x40)&&((unsigned char)checkstring[i]<=0x7e))||
                        (((unsigned char)checkstring[i]>=0xa1)&&((unsigned char)checkstring[i]<=0xfe))) ach=0;
                } else {
                	if (((unsigned char)checkstring[i]>=0xa1)&&((unsigned char)checkstring[i]<=0xfe)) ach=1;
                }
	}
	if (ach)
	{
		print(" StringDog! ");
		return	TRUE;
	}
	
	return FALSE;
	
}

#ifdef _TEAM_KICKPARTY
void lssproto_KTEAM_recv( int fd, int si)
{
	int charaindex=-1, pindex;
	if( si < 0 || si > 5 ) return;
	charaindex = CONNECT_getCharaindex( fd );
	if( !CHAR_CHECKINDEX( charaindex) ) return;

	if( CHAR_getWorkInt( charaindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_LEADER ) return;

	pindex = CHAR_getWorkInt( charaindex, si + CHAR_WORKPARTYINDEX1);
	if( !CHAR_CHECKINDEX( pindex) ) return;
	if( CHAR_getWorkInt( pindex, CHAR_WORKPARTYMODE) == CHAR_PARTY_LEADER ) return;

	if( CHAR_DischargeParty( pindex, 0) == FALSE ){
		CHAR_talkToCli( charaindex, -1, "踢除失败！", CHAR_COLORYELLOW);
	}else{
		char buf1[256];
		sprintf( buf1, "队长[%s]将你踢除！", CHAR_getUseName( charaindex ));
		CHAR_talkToCli( pindex, -1, buf1, CHAR_COLORYELLOW);
		sprintf( buf1, "将[%s]踢除出团队！", CHAR_getUseName( pindex ));
		CHAR_talkToCli( charaindex, -1, buf1, CHAR_COLORYELLOW);
	}
}
#endif

#ifdef _CHATROOMPROTOCOL			// (不可开) Syu ADD 聊天室频道
void lssproto_CHATROOM_recv (int fd , char *data)
{
	ChatRoom_recvall ( fd , data ) ;
}
#endif

#ifdef _NEWREQUESTPROTOCOL			// (不可开) Syu ADD 新增Protocol要求细项
void lssproto_RESIST_recv (int fd )
{
	int charindex = -1 ;

	char token[256];
	charindex = CONNECT_getCharaindex( fd );
	if( !CHAR_CHECKINDEX( charindex) ) return;
	sprintf ( token , "%d|%d|%d|%d|%d|%d|%d|%d" ,
		CHAR_getInt( charindex, CHAR_EARTH_RESIST ) ,
		CHAR_getInt( charindex, CHAR_WATER_RESIST ) ,
		CHAR_getInt( charindex, CHAR_FIRE_RESIST ) ,
		CHAR_getInt( charindex, CHAR_WIND_RESIST ) ,
		CHAR_getInt( charindex, CHAR_EARTH_EXP ) ,
		CHAR_getInt( charindex, CHAR_WATER_EXP ) ,
		CHAR_getInt( charindex, CHAR_FIRE_EXP ) ,
		CHAR_getInt( charindex, CHAR_WIND_EXP )
		);
	lssproto_RESIST_send ( fd , token ) ;
}
#endif

#ifdef _OUTOFBATTLESKILL			// (不可开) Syu ADD 非战斗时技能Protocol
void lssproto_BATTLESKILL_recv (int fd, int iNum)
{
	int charaindex = CONNECT_getCharaindex( fd );
#ifndef _PROSKILL_OPTIMUM
	int skillindex=-1,char_pskill=-1,profession_skill=-1;
#endif

	if( !CHAR_CHECKINDEX( charaindex) ) return;
	if( CHAR_getWorkInt( charaindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE ) return;

#ifndef _PROSKILL_OPTIMUM	// Robin fix cancel 此处略过职业检查, 改在 PROFESSION_SKILL_Use 中检查
	// 人物的职业
	char_pskill = CHAR_getInt( charaindex, PROFESSION_CLASS );
	
	skillindex = PROFESSION_SKILL_GetArray( charaindex, iNum);
	int Pskillid = skillindex;
	// 技能的职业
	profession_skill = PROFESSION_SKILL_getInt( Pskillid, PROFESSION_SKILL_PROFESSION_CLASS);

	if( (char_pskill > 0) && (char_pskill == profession_skill) ){
#else
	if( 1 ){
#endif
		if( PROFESSION_SKILL_Use( charaindex, iNum, 0, NULL ) != 1 ){
			if(CHAR_getWorkInt( charaindex, CHAR_ENCOUNT_NUM)-(int)time(NULL)>0){
//				print("\n 玩家(%s)使用技能错误!", CHAR_getUseName( charaindex ) );
#ifdef _PROSKILL_ERR_KICK
				CONNECT_setCloseRequest( fd , 1 );
#endif
			}
		}
	}

}

#endif

#ifdef _STREET_VENDOR
void lssproto_STREET_VENDOR_recv(int fd,char *message)
{
	int charaindex = CONNECT_getCharaindex(fd);
	if(!CHAR_CHECKINDEX(charaindex)) return;
	if(CHAR_getWorkInt(charaindex,CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE) return;
	if(CHAR_getWorkInt(charaindex,CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE){
		CHAR_talkToCli(charaindex,-1,"组队状态下不能交易",CHAR_COLORYELLOW);
		return;
	}
	if (CHAR_getWorkInt( charaindex, CHAR_WORKSAFEMODE)==0)
    {
    	CHAR_SafePassword(charaindex);
    	return;
    }
	CHAR_sendStreetVendor(charaindex,message);
}
#endif

#ifdef _RIGHTCLICK
void lssproto_RCLICK_recv(int fd, int type, char* data)
{
	print("\n RCLICK_recv( type=%d data=%s) ", type, data);
}
#endif

#ifdef _JOBDAILY
void lssproto_JOBDAILY_recv(int fd,char *data)
{
	int charaindex = CONNECT_getCharaindex(fd);
	if(!CHAR_CHECKINDEX(charaindex)) return;

	CHAR_JobDaily(charaindex,data);
}
#endif

#ifdef _TEACHER_SYSTEM
void lssproto_TEACHER_SYSTEM_recv(int fd,char *data)
{
	int charaindex = CONNECT_getCharaindex(fd);

	if(!CHAR_CHECKINDEX(charaindex)) return;
	CHAR_Teacher_system(charaindex,data);
}
#endif

#ifdef _ONLINE_SHOP
void lssproto_SHOP_recv(int fd,int shopflag,int piece)
{
	int charaindex = CONNECT_getCharaindex(fd);

	if(!CHAR_CHECKINDEX(charaindex)) return;
	char *mycdkey=CHAR_getChar(charaindex,CHAR_CDKEY);
	//saacproto_ACShop_send(fd,mycdkey,charaindex,shopflag,piece);
	char dataname[20];
	char res[2048];
	int rownum=0,pieces=0,shopnum=0,startrow,vippoint=0;
	sprintf(dataname,"OnlineShop%d",shopflag);
	vippoint=sasql_vippoint(mycdkey,0,0);
	rownum=sasql_chehk_shop(dataname);
	if(rownum>0)
		{
			pieces=rownum/8+1;
			shopnum=rownum-8*(piece-1);
			if(shopnum>8) shopnum=8;
			startrow=8*(piece-1);
			char* shopres;
			shopres=sasql_query_shop(dataname,startrow,shopnum);
			lssproto_SHOP_send(fd,shopnum,vippoint,piece,pieces,shopflag,shopres);
		}
}

void lssproto_SHOPBUY_recv(int fd,int shopflag,int piece,int shopno,int buynum)
{
	int charaindex = CONNECT_getCharaindex(fd);
	int i,petindex;
	if(!CHAR_CHECKINDEX(charaindex)) return;
	if (CHAR_getWorkInt( charaindex, CHAR_WORKSAFEMODE)==0)
    {
    	CHAR_SafePassword(charaindex);
    	return;
    }
   if(shopflag==1)
   	{
   		for( i = 0 ;i < CHAR_MAXPETHAVE ; i++) {
				petindex = CHAR_getCharPet( charaindex, i);
				if( petindex == -1  )
			break;
			}

  		if( i+buynum > CHAR_MAXPETHAVE )      {
				CHAR_talkToCli( charaindex, -1, "宠物已满！！",  CHAR_COLORYELLOW);
				return;
			}
   	}
   	else
   		{
   				if(buynum>CHAR_findSurplusItemBox(charaindex)){
					CHAR_talkToCli( charaindex, -1, "物品栏位不足。", CHAR_COLORYELLOW);

					return;
				}
			}
	char *mycdkey=CHAR_getChar(charaindex,CHAR_CDKEY);
	char dataname[20];
	int rownum=0,pieces=0,shopnum=0,startrow,vippoint=0;
	sprintf(dataname,"OnlineShop%d",shopflag);
	vippoint=sasql_vippoint(mycdkey,0,0);
	rownum=sasql_chehk_shop(dataname);
	if(rownum>0)
		{
			pieces=rownum/8+1;
			shopnum=rownum-8*(piece-1);
			if(shopnum>8) shopnum=8;
			startrow=8*(piece-1);
			char* shopres;
			shopres=sasql_query_shop(dataname,startrow,shopnum);
			int pvippoint = - sasql_buy_shop(dataname,startrow,shopnum,shopno) * buynum;
			vippoint=sasql_vippoint(mycdkey,pvippoint,1);
			//saacproto_ACShopBuy_send(fd, charaindex,0,vippoint,piece,pieces,shopflag,shopres,shopno,buynum);
		}
	//saacproto_ACShopBuy_send(fd,mycdkey,charaindex,shopflag,piece,shopno,buynum);
}
#endif
