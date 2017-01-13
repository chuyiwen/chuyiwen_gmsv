#define _SASQL_C_

#include "version.h"

//#include "main.h"
#include "sasql.h"
#include "net.h"
#include "common.h"
#include "char_base.h"
#include "char_data.h"
#include "char.h"
#include "lssproto_serv.h"
#include "item.h"
#include "item_event.h"
#include "buf.h"
#include "object.h"
#include "map_deal.h"
#include "saacproto_cli.h"
#include "readmap.h"
#include "handletime.h"
#include "char_event.h"
#include "npccreate.h"
#include "addressbook.h"
#include "item_event.h"
#include "magic_base.h"
#include "magic.h"
#include "chatmagic.h"
#include "configfile.h"
#include "log.h"
#include "anim_tbl.h"
#include "encount.h"
#include "battle.h"
#include "pet_skill.h"
#include "util.h"
#include "enemy.h"
#include "npcutil.h"
#include "pet.h"
#include "family.h"
#include "defend.h"
#include "correct_bug.h"
#include <mysql/mysql.h>

extern int fmrankindex[20];
extern char fmrankname[20][50];
extern int fmrankpoint[20];

MYSQL mysql;
MYSQL serversql;
MYSQL_RES *mysql_result;
MYSQL_ROW mysql_row;

void easyGetTokenFromString2( char *src,int count,char*output,int len )
{
    int i;
    int counter = 0;

    if( len <= 0 )return;

#define ISSPACETAB( c )   ( (c) == ' ' || (c) == '\t' )

    for(i=0;;i++){
        if( src[i]=='\0'){
            output[0] = '\0';
            return;
        }
        if( i > 0 && ! ISSPACETAB( src[i-1] ) &&
            ! ISSPACETAB(  src[i] ) ){
            continue;
        }

        if( ! ISSPACETAB( src[i]) ){
            counter++;
            if( counter == count){
                int j;
                for(j=0;j<len-1;j++){
                    if( src[i+j] == '\0' ||
                        ISSPACETAB( src[i+j] ) ){
                        break;
                    }
                    output[j]=src[i+j];
                }
                output[j]='\0';
                return;

            }
        }

    }
}

typedef struct tagConfig
{
    char    sql_IP[16]; 

		int     sql_Port; 
		
		char    sql_Port1[16];
		
    char    sql_ID[16];
    
    char    sql_PS[16];

    char    sql_DataBase[16];  

    char    sql_Table[16];    
    
    char    sql_LOCK[16];   
    
    char    sql_NAME[16];  
    
    char    sql_PASS[16];  
}Config;

Config sqlconfig;

int AutoReg = 0;
int openbackground;
int sameipnum = 0;

static int readConfig( char *path )
{
    char buf[255];
    FILE *fp;

    fp = fopen( path , "r" );
    if( fp == NULL ){ return -2; }

    while( fgets( buf , sizeof( buf ) , fp )){
        char command[255];
        char param[255];
        chop(buf);
        
        easyGetTokenFromString2( buf , 1 , command , sizeof( command ));
        easyGetTokenFromString2( buf , 2 , param , sizeof( param ));

        if( strcmp( command , "sql_IP" ) == 0 ){
            strcmp( sqlconfig.sql_IP , param ) ;
            snprintf( sqlconfig.sql_IP , sizeof( sqlconfig.sql_IP) , param );
            printf("\n数据库地址：  %s",sqlconfig.sql_IP);
        } else if( strcmp( command , "sql_Port" ) == 0 ){
        		sqlconfig.sql_Port = atoi( param );
            snprintf( sqlconfig.sql_Port1 , sizeof( sqlconfig.sql_Port1) , param );
				  	printf("\n数据库端口：  %d",sqlconfig.sql_Port);
        } else if( strcmp( command , "sql_ID" ) == 0 ){
        		strcmp( sqlconfig.sql_ID , param ) ;
            snprintf( sqlconfig.sql_ID , sizeof( sqlconfig.sql_ID) , param );
						printf("\n数据库用户：  %s",sqlconfig.sql_ID);
        } else if( strcmp( command , "sql_PS" ) == 0 ){
        		strcmp( sqlconfig.sql_PS , param ) ;
            snprintf( sqlconfig.sql_PS , sizeof( sqlconfig.sql_PS) , param );
						printf("\n数据库密码：  %s",sqlconfig.sql_PS);
        } else if( strcmp( command , "sql_DataBase" ) == 0 ){
        		strcmp( sqlconfig.sql_DataBase , param ) ;
            snprintf( sqlconfig.sql_DataBase , sizeof( sqlconfig.sql_DataBase) , param );
						printf("\n登陆数据库名：%s",sqlconfig.sql_DataBase);
        } else if( strcmp( command , "sql_Table" ) == 0 ){
        		strcmp( sqlconfig.sql_Table , param ) ;
            snprintf( sqlconfig.sql_Table , sizeof( sqlconfig.sql_Table) , param );
				  	printf("\n用户信息表名：  %s",sqlconfig.sql_Table);
				} else if( strcmp( command , "sql_LOCK" ) == 0 ){
        		strcmp( sqlconfig.sql_LOCK , param ) ;
            snprintf( sqlconfig.sql_LOCK , sizeof( sqlconfig.sql_LOCK) , param );
				  	printf("\n用户锁定表名：  %s",sqlconfig.sql_LOCK);
				} else if( strcmp( command , "sql_NAME" ) == 0 ){
        		strcmp( sqlconfig.sql_NAME , param ) ;
            snprintf( sqlconfig.sql_NAME , sizeof( sqlconfig.sql_NAME) , param );
				  	printf("\n账号字段名称：  %s",sqlconfig.sql_NAME);
				} else if( strcmp( command , "sql_PASS" ) == 0 ){
        		strcmp( sqlconfig.sql_PASS , param ) ;
            snprintf( sqlconfig.sql_PASS , sizeof( sqlconfig.sql_PASS) , param );
				  	printf("\n密码字段名称：  %s",sqlconfig.sql_PASS);
				} else if( strcmp( command , "openbackground" ) == 0 ){
        		openbackground = atoi( param );
        		if(openbackground){
        			printf("\n后台功能：    YES");
        		}else{
        			printf("\n后台功能：    NO");
        		}
				} else if( strcmp( command , "AutoReg" ) == 0 ){
        		AutoReg = atoi( param );
        		if(AutoReg){
        			printf("\n开放自动注册：YES");
        		}else{
        			printf("\n开放自动注册：NO");
        		}
    		} else if( strcmp( command , "SameIpNum" ) == 0 ){
    			sameipnum = atoi( param );
    		}
    }
    fclose(fp);
    return 0;
}

BOOL sasql_init( void )
{
		if( mysql_init(&mysql) == NULL & readConfig("sql.cf"))
		{
			  printf("\n数据库初始化失败！");
			  exit(1);
		    return FALSE;
		}
	
	  if( !mysql_real_connect( &mysql,
	          sqlconfig.sql_IP,
	          sqlconfig.sql_ID,//帐号
	          sqlconfig.sql_PS,//密码
	          sqlconfig.sql_DataBase,//选择的资料库
	          sqlconfig.sql_Port,
	          NULL,
	          0 ) )
		{
			printf("\n数据库连接失败！\n");
			return FALSE;
		}
  
  	printf("\n数据库连接成功！\n");
  	return TRUE;
}

int sasql_mysql_query( char *sqlstr )
{
	mysql_query(&mysql,"set names 'gbk'");
  return mysql_query(&mysql,sqlstr);
}

void sasql_close( void )
{
  mysql_close( &mysql );
}


int sasql_query(char *nm, char *pas){
	char sqlstr[256];
	sprintf(sqlstr,"select `%s`,`%s`,`SafePasswd`,`Lock` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_NAME,sqlconfig.sql_PASS,sqlconfig.sql_Table,sqlconfig.sql_NAME,nm);
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	if(mysql_row[2]!=NULL){
	  		if((strcmp(pas,mysql_row[1]) == 0) && (atoi(mysql_row[3])==0 || atoi(mysql_row[3])==getLockType())){
	  		return 1;
		  	}else{
		  		//printf("用户%s密码错误！\n",nm);
		  		return 2;
				}
		  }else
		  	{
		  		if(strcmp(pas,mysql_row[1]) == 0 && atoi(mysql_row[3])==0){
	  			return 1;
		  		}else{
		  			//printf("用户%s密码错误！\n",nm);
		  			return 2;
					}
		  	}
	  }else{
	  	mysql_free_result(mysql_result);
	  	//printf("用户%s未注册！\n",nm);
	  	return 3;
	  }
	}else{
		//printf("\n数据库查找失败！\n");
		return 0;
	}
}

int sasql_load_query(char *nm, char *pas){
	char sqlstr[256];
	sprintf(sqlstr,"select `%s`,`%s` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_NAME,sqlconfig.sql_PASS,sqlconfig.sql_Table,sqlconfig.sql_NAME,nm);
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	if(strcmp(pas,mysql_row[1]) == 0){
	  	return 1;
		  }else{
		  	printf("用户%s密码错误！\n",nm);
		  	return 2;
		  }
	  }else{
	  	mysql_free_result(mysql_result);
	  	printf("用户%s未注册！\n",nm);
	  	return 3;
	  }
	}else{
		printf("\n数据库查找失败！\n");
		return 0;
	}
}

int sasql_del_query(char *nm, char *pas){
	char sqlstr[256];
	sprintf(sqlstr,"select `%s`,`SafePasswd` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_NAME,sqlconfig.sql_Table,sqlconfig.sql_NAME,nm);
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	if(mysql_row[1]!=NULL)
	  		{
	  			if(strcmp(pas,mysql_row[1]) == 0){
	  			return 1;
		  		}else{
		  			return 2;
		  		}
		  	}else{
		  	printf("用户%s密码错误！\n",nm);
		  	return 2;
		  }
	  }else{
	  	mysql_free_result(mysql_result);
	  	printf("用户%s未注册！\n",nm);
	  	return 3;
	  }
	}else{
		printf("\n数据库查找失败！\n");
		return 0;
	}
}



BOOL sasql_online( char *ID, char *NM, char *IP,int fame,int vigor, int flag )
{
	if(openbackground){
		char sqlstr[256];
		if(flag==0)
			sprintf(sqlstr,"update `%s` set `Online`=0,`Offline`=0 where `%s`=BINARY'%s'", sqlconfig.sql_Table, sqlconfig.sql_NAME, ID);
		else if(flag==1)
			{
			if(strstr(NM,",")!=NULL) return FALSE;
			mysql_query(&mysql,"SET NAMES 'gbk'");
			sprintf(sqlstr,"update `%s` set `LoginTime`=NOW(), `IP`='%s', `OnlineName`='%s', `Online`=%d, `Path`='char/0x%x',`Fame`=%d,`Vigor`=%d where `%s`=BINARY'%s'", sqlconfig.sql_Table, IP, NM, getServernumber(), getHash(ID) & 0xff,fame,vigor, sqlconfig.sql_NAME, ID);
		}
		else if(flag==2){
			if(strstr(NM,",")!=NULL) return FALSE;
			sprintf(sqlstr,"update `%s` set `LoginTime`=NOW(), `OnlineName`='%s', `Online`=%d where `%s`=BINARY'%s'", sqlconfig.sql_Table, NM, getServernumber(), sqlconfig.sql_NAME, ID);
		}
		else if(flag==3)
			sprintf(sqlstr,"update `%s` set `Online`=0,`Offline`=0 where `Online`=%d", sqlconfig.sql_Table, getServernumber());
		else if(flag==4)
			sprintf(sqlstr,"update `%s` set `LoginTime`=NOW(),`IP`='%s', `Online`=%d where `%s`=BINARY'%s'", sqlconfig.sql_Table, IP, getServernumber(), sqlconfig.sql_NAME, ID);
		else if(flag==5)
			sprintf(sqlstr,"update `%s` set `Offline`=1 where `%s`=BINARY'%s'", sqlconfig.sql_Table, sqlconfig.sql_NAME, ID);
		if(!mysql_query(&mysql,sqlstr)){
			return TRUE;
		}
	}
	printf("\n更新数据库失败！\n");
	return FALSE;
}

/*
BOOL sasql_online_jqm( char *ID, char *jqm, char *IP, int logintype, char *GHid, char *GHic, char *GHsp )
{
	if(openbackground){
		
		char sqlstr[2048];
		if(strstr(jqm,",")!=NULL || strstr(jqm,"+")!=NULL || strstr(jqm,"?")!=NULL || strstr(jqm,"*")!=NULL || strstr(jqm,"<")!=NULL || strstr(jqm,")")!=NULL || strstr(jqm,"^")!=NULL || strstr(jqm,"`")!=NULL || strstr(jqm,";")!=NULL)
			sprintf(sqlstr,"update `%s` set `LoginTime`=NOW(),`IP`='%s',`PcId`='CUOWU', `Online`=%d,`Type`=%d,`GHid`='%s',`GHic`='%s',`GHsp`='%s' where `%s`=BINARY'%s'", sqlconfig.sql_Table, IP, getServernumber(),logintype,GHid,GHic,GHsp, sqlconfig.sql_NAME, ID);
		else
			sprintf(sqlstr,"update `%s` set `LoginTime`=NOW(),`IP`='%s',`PcId`='%s', `Online`=%d,`Type`=%d,`GHid`='%s',`GHic`='%s',`GHsp`='%s' where `%s`=BINARY'%s'", sqlconfig.sql_Table, IP,jqm, getServernumber(),logintype,GHid,GHic,GHsp, sqlconfig.sql_NAME, ID);
		//print("\n sql: %s",sqlstr);
		if(!mysql_query(&mysql,sqlstr)){
			return TRUE;
		}
	}
	printf("\n更新数据库失败！\n");
	return FALSE;
}
*/

BOOL sasql_online_jqm( char *ID, char *jqm, char *IP,int logintype )
{
	if(openbackground){
		
		char sqlstr[256];
		if(strstr(jqm,",")!=NULL || strstr(jqm,"+")!=NULL || strstr(jqm,"?")!=NULL || strstr(jqm,"*")!=NULL || strstr(jqm,"<")!=NULL || strstr(jqm,")")!=NULL || strstr(jqm,"^")!=NULL || strstr(jqm,"`")!=NULL || strstr(jqm,";")!=NULL)
			sprintf(sqlstr,"update `%s` set `LoginTime`=NOW(),`IP`='%s',`PcId`='CUOWU', `Online`=%d,`Type`=%d where `%s`=BINARY'%s'", sqlconfig.sql_Table, IP, getServernumber(),logintype, sqlconfig.sql_NAME, ID);
		else
			sprintf(sqlstr,"update `%s` set `LoginTime`=NOW(),`IP`='%s',`PcId`='%s', `Online`=%d,`Type`=%d where `%s`=BINARY'%s'", sqlconfig.sql_Table, IP,jqm, getServernumber(),logintype, sqlconfig.sql_NAME, ID);
		//print("\n sql: %s",sqlstr);
		if(!mysql_query(&mysql,sqlstr)){
			return TRUE;
		}
	}
	printf("\n更新数据库失败！\n");
	return FALSE;
}

#ifdef _CAX_NEWMAC_LOGIN
BOOL sasql_online_mac( char *ID, char *mac, char *IP,int logintype )
{
//	if(openbackground){
		char sqlstr[256];
		sprintf(sqlstr,"update `%s` set `LoginTime`=NOW(),`IP`='%s',`MAC`='%s', `Online`=%d where `%s`=BINARY'%s'", sqlconfig.sql_Table, IP, mac , getServernumber(), sqlconfig.sql_NAME, ID);
		if(!sasql_mysql_query(sqlstr)){
			return TRUE;
		}
//	}
	printf("\n更新数据库失败[mac]！\n");
	return FALSE;
}
#endif

BOOL sasql_register(char *id, char *ps)
{
	char sqlstr[256];
	if(AutoReg!=1)return FALSE;
	sprintf(sqlstr,"INSERT INTO `%s` (`%s`,`%s`,`RegTime`,`Path`,`VipPoint`) VALUES (BINARY'%s','%s',NOW(),'char/0x%x',0)",sqlconfig.sql_Table, sqlconfig.sql_NAME, sqlconfig.sql_PASS, id, ps, getHash(id) & 0xff);
	if(!mysql_query(&mysql,sqlstr)){
		printf("\n新用户注册成功！\n");
		return TRUE;
	}
	printf("\n新用户注册失败！\n");
	return FALSE;
}



BOOL sasql_chehk_lock( char *idip )
{
	char sqlstr[256];
	sprintf(sqlstr,"select * from `%s` where `%s`=BINARY'%s'", sqlconfig.sql_LOCK, sqlconfig.sql_NAME, idip);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
		if(num_row>0)
			{
				return TRUE;
			}
		else
			{
				return FALSE;
			}
	}
	return FALSE;
}

BOOL sasql_add_lock( char *idip )
{
	char sqlstr[256];
	sprintf(sqlstr,"INSERT INTO `%s` (`%s`) VALUES (BINARY'%s')", sqlconfig.sql_LOCK, sqlconfig.sql_NAME, idip);
	if(!mysql_query(&mysql,sqlstr)){
			printf("\n添加锁定%s成功！\n",idip);
			return TRUE;
	}
	return FALSE;
}

BOOL sasql_del_lock( char *idip )
{
	char sqlstr[256];
	sprintf(sqlstr,"delete from `%s` where `%s`=BINARY'%s'", sqlconfig.sql_LOCK, sqlconfig.sql_NAME, idip);
	if(!mysql_query(&mysql,sqlstr)){
		printf("\n解除锁定%s成功！\n",idip);
		return TRUE;
	}
	return FALSE;
}

int sasql_vippoint( char *ID, int point, int flag )
{
	char sqlstr[256];
//	print("\NVIPpoint1=%d\n",point);
	if(strstr(ID,"'")!=NULL) return -1;
	sprintf(sqlstr,"select `VipPoint` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,ID);
//	printf("22222222:%s\n",sqlstr);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			if(flag==0){
				return atoi(mysql_row[0]);
			}
			if(flag==520){
				sprintf(sqlstr,"update `%s` set `VipPoint`=%d where `%s`=BINARY'%s'", sqlconfig.sql_Table, point,sqlconfig.sql_NAME, ID);	
			}else{
				point+=atoi(mysql_row[0]);
				sprintf(sqlstr,"update `%s` set `VipPoint`=%d where `%s`=BINARY'%s'", sqlconfig.sql_Table, point,sqlconfig.sql_NAME, ID);	
			}
//			printf("33333333:%s\n",sqlstr);
			if(!mysql_query(&mysql,sqlstr)){
				return point;
			}else{
				return atoi(mysql_row[0]);
			}
		}else{
		mysql_free_result(mysql_result);
		}
	}
	printf("更新会员点数失败！-- %s\n",sqlstr);
	return -1;
}


int sasql_onlinenum( char *cdkey, char *IP )
{
	char sqlstr[256];
	sprintf(sqlstr,"select * from `%s` where `%s`='%s' and `Online`>0 and `IP`='%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,cdkey,IP);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
		if(num_row>0){
			return 0;
		}
	}
	sprintf(sqlstr,"select count(*) from `%s` where `Online`>0 and `Offline`=0 and `IP`='%s'",sqlconfig.sql_Table,IP);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			return atoi(mysql_row[0]);
		}else{
			mysql_free_result(mysql_result);
		}
	}
	return 0;
}

int sasql_onlinejqmnum( char *jqm, int flag )
{
	char sqlstr[256];
//	sprintf(sqlstr,"select * from `%s` where `%s`='%s' and `Online`>0 and `PcId`='%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,cdkey,jqm);
//	if(!mysql_query(&mysql,sqlstr)){
//		int num_row=0;
//		mysql_result=mysql_store_result(&mysql);
//		num_row=mysql_num_rows(mysql_result);
//		mysql_free_result(mysql_result);
//		if(num_row>0){
//			return 0;
//		}
//	}
	if(flag == 1){
		sprintf(sqlstr,"select count(*) from `%s` where `Online`=%d and `Offline`=0 and `PcId`='%s'",sqlconfig.sql_Table,getServernumber(),jqm);
		if(!mysql_query(&mysql,sqlstr)){
			int num_row=0;
			mysql_result=mysql_store_result(&mysql);
			num_row=mysql_num_rows(mysql_result);
			if(num_row>0){
				mysql_row=mysql_fetch_row(mysql_result);
				mysql_free_result(mysql_result);
				return atoi(mysql_row[0]);
			}else{
				mysql_free_result(mysql_result);
			}
		}
	}else{
		sprintf(sqlstr,"select count(*) from `%s` where `Online`>0 and `Offline`=0 and `PcId`='%s'",sqlconfig.sql_Table,jqm);
		if(!mysql_query(&mysql,sqlstr)){
			int num_row=0;
			mysql_result=mysql_store_result(&mysql);
			num_row=mysql_num_rows(mysql_result);
			if(num_row>0){
				mysql_row=mysql_fetch_row(mysql_result);
				mysql_free_result(mysql_result);
				return atoi(mysql_row[0]);
			}else{
				mysql_free_result(mysql_result);
			}
		}
	}
	return 0;
}

#ifdef _CAX_NEWMAC_LOGIN
int sasql_onlinemacnum( char *mac, int flag )
{
	char sqlstr[256];
	if(flag == 1){
		sprintf(sqlstr,"select count(*) from `%s` where `Online`=%d and `MAC`='%s'",sqlconfig.sql_Table,getServernumber(),mac);
		if(!sasql_mysql_query(sqlstr)){
			int num_row=0;
			mysql_result=mysql_store_result(&mysql);
			num_row=mysql_num_rows(mysql_result);
			if(num_row>0){
				mysql_row=mysql_fetch_row(mysql_result);
				mysql_free_result(mysql_result);
				return atoi(mysql_row[0]);
			}else{
				mysql_free_result(mysql_result);
			}
		}
	}else{
		sprintf(sqlstr,"select count(*) from `%s` where `Online`>0  and `MAC`='%s'",sqlconfig.sql_Table,mac);
		if(!sasql_mysql_query(sqlstr)){
			int num_row=0;
			mysql_result=mysql_store_result(&mysql);
			num_row=mysql_num_rows(mysql_result);
			if(num_row>0){
				mysql_row=mysql_fetch_row(mysql_result);
				mysql_free_result(mysql_result);
				return atoi(mysql_row[0]);
			}else{
				mysql_free_result(mysql_result);
			}
		}
	}
	return 0;
}
#endif

int sasql_query_safepasswd(char *id, char *safepasswd){
	char sqlstr[256];
	sprintf(sqlstr,"select `SafePasswd` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,id);
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	if(mysql_row[0]!=NULL)
	  		{
	  			if(strcmp(safepasswd,mysql_row[0]) == 0)
	  				{
	  					if(strstr(id,safepasswd)!=NULL)
	  						return 4;
	  					return 1;
	  				}
	  			else
	  				{
	  					printf("\n安全码输入错误！\n");
	  					return 2;
	  				}
	  		}
	  	else
	  		{
	  			printf("\n用户未填写安全码！\n");
	  			return 3;
	  		}  			
		}else{
			mysql_free_result(mysql_result);
		}
	}
	else{
		printf("\n数据库查找失败！\n");
		return 0;
	}
}

int sasql_add_safepasswd(char *id, char *safepasswd){
	char sqlstr[256];
	if(strstr(safepasswd,",")!=NULL) return 0;
	sprintf(sqlstr,"update `%s` set `SafePasswd`=BINARY'%s' where `%s`=BINARY'%s'",sqlconfig.sql_Table,safepasswd,sqlconfig.sql_NAME,id);
	
	if(!mysql_query(&mysql,sqlstr)){
			printf("用户%s设置安全码成功！\n",id);
			return 1;
	}else{
		printf("\n数据库查找失败！\n");
		return 0;
	}
}

BOOL sasql_add_card(char *pas, int num)
{
	char sqlstr[256];
	sprintf(sqlstr,"INSERT INTO `OnlineCost` (`CostPasswd`,`CostVal`) VALUES (BINARY'%s','%d')", pas, num);
	if(!mysql_query(&mysql,sqlstr)){
		return TRUE;
	}
	return FALSE;
}

void sasql_query_card(char *id, char *cardps,int charaindex){
	char sqlstr[256];
	if(strstr(id,",")!=NULL){
		CHAR_talkToCli( charaindex, -1, "您的账号非法。", CHAR_COLORRED);
		return;
	}
	if(strlen(cardps)!=16){
		CHAR_talkToCli( charaindex, -1, "卡密不存在。", CHAR_COLORRED);
		return;
	}
	if( strstr(cardps,"/")!=NULL || strstr(cardps,"*")!=NULL || strstr(cardps,"+")!=NULL || strstr(cardps,"-")!=NULL || strstr(cardps,";")!=NULL || strstr(cardps,":")!=NULL || strstr(cardps," ")!=NULL || strstr(cardps,"|")!=NULL || strstr(cardps,".")!=NULL || strstr(cardps,",")!=NULL || strstr(cardps,"'")!=NULL || strstr(cardps,"=")!=NULL || strstr(cardps,"`")!=NULL || strstr(cardps,"<")!=NULL || strstr(cardps,">")!=NULL || strstr(cardps,"!")!=NULL || strstr(cardps,"NULL")!=NULL){
		CHAR_talkToCli( charaindex, -1, "卡密非法。", CHAR_COLORRED);
		return;
	}
	sprintf(sqlstr,"select * from `OnlineCost` where `CostPasswd`=BINARY'%s'",cardps);
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  int check,costval;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	check = atoi(mysql_row[4]);
	  	costval = atoi(mysql_row[1]);
	  			if(check == 1)
	  				{
	  					sprintf(sqlstr,"update `OnlineCost` set `cdkey`=BINARY'%s', `CostTime`=NOW(), `check`=0 where `CostPasswd`=BINARY'%s'",id,cardps);
	  					if(!mysql_query(&mysql,sqlstr)){
	  						sprintf(sqlstr,"%d现金冲值成功，您现在的现金帐户金额为：%d",costval,sasql_rmbpoint(id,costval,1));
	  						CHAR_talkToCli( charaindex, -1, sqlstr, CHAR_COLORRED);
							}	
	  				}
	  			else
	  				{
	  					CHAR_talkToCli( charaindex, -1, "该冲值卡已经被使用，请勿重复使用，谢谢！", CHAR_COLORRED);  //冲值卡已经使用
	  					return;
	  				}
	  			}
	  	else
	  		{
	  			mysql_free_result(mysql_result);
	  			CHAR_talkToCli( charaindex, -1, "冲值卡密码填写错误，请重新填写！", CHAR_COLORRED);   //冲值卡填写错误
	  			return;
	  		}  			
	  }
	else{
		printf("\n数据库查找失败！\n");
		return;
	}
}


#ifdef _ONLINE_SHOP
int sasql_chehk_shop( char *dataname )
{
	char sqlstr[256];
	sprintf(sqlstr,"select * from `%s`", dataname);
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
			return num_row;
	}
	return 0;
}

char* sasql_query_shop( char *dataname, int startrow, int shopnum )
{
	char sqlstr[256];
	char shopresult[2048];
	int i;
	sprintf(sqlstr,"select * from `%s` LIMIT %d , %d", dataname, startrow, shopnum);
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	for(i=1;i<=shopnum;i++)
	  	{
	  		mysql_row=mysql_fetch_row(mysql_result);
	  		if(i==1)
	  			{
					sprintf(shopresult,"%s#%s#%s#%s#%s#%s#%s|",mysql_row[0],mysql_row[1],mysql_row[2],mysql_row[3],mysql_row[4],mysql_row[5],mysql_row[6]);
					}
				else
					{
					sprintf(shopresult,"%s%s#%s#%s#%s#%s#%s#%s|",shopresult,mysql_row[0],mysql_row[1],mysql_row[2],mysql_row[3],mysql_row[4],mysql_row[5],mysql_row[6]);
					}
			}
			mysql_free_result(mysql_result);
			return shopresult;
		}else
			{
				mysql_free_result(mysql_result);
				return "err";
			}
	}
	return "err";
}

int sasql_buy_shop( char *dataname, int startrow, int shopnum, int shopno )
{
	char sqlstr[256];
	int i,costnum;
	sprintf(sqlstr,"select * from `%s` where `id`=%d", dataname, shopno);
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  		mysql_row=mysql_fetch_row(mysql_result);
	  		mysql_free_result(mysql_result);
	  		costnum = atoi(mysql_row[5])*atoi(mysql_row[4])/100;
			return costnum;
		}else
			{
				mysql_free_result(mysql_result);
				return 0;
			}
	}
	return 0;
}
#endif

char* sasql_query_question(void)
{
	char sqlstr[256];
	sprintf(sqlstr,"select * from `Question`");
	if(!mysql_query(&mysql,sqlstr))
		{
			int num_row=0;
			mysql_result=mysql_store_result(&mysql);
	  	num_row=mysql_num_rows(mysql_result);
			mysql_free_result(mysql_result);
			if(num_row>0)
				{
					srand((unsigned)time(NULL));
					int questionid;
					questionid = (rand() % num_row)+1;
					sprintf(sqlstr,"select * from `Question` where `id`=%d",questionid);
					if(!mysql_query(&mysql,sqlstr))
						{
							mysql_result=mysql_store_result(&mysql);
	  					num_row=mysql_num_rows(mysql_result);
							if(num_row>0)
								{
									mysql_row=mysql_fetch_row(mysql_result);
									mysql_free_result(mysql_result);
									char question[256];
									sprintf(question,"%s;%s",mysql_row[1],mysql_row[2]);
									return question;
								}
							return "err";
						}
					return "err";
				}
			return "err";
		}
	return "err";
}

#ifdef _ALL_ITEM
int sasql_allnum( int flag, int num, int flag1 )
{
	char sqlstr[256];
	sprintf(sqlstr,"select * from `AllNum` where `id`=%d",flag);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			if(flag1==0){
				return atoi(mysql_row[1]);
			}
			num+=atoi(mysql_row[1]);
			sprintf(sqlstr,"update `AllNum` set `num`=%d where `id`=%d", num,flag);	
			if(!mysql_query(&mysql,sqlstr)){
				return num;
			}
		}
	}
	return num;
}
#endif

BOOL sasql_craete_lock( void )
{

}

BOOL sasql_craete_userinfo( void )
{

}





#ifdef _SQL_BUY_FUNC
char *sasql_OnlineBuy( char *id, char *costpasswd )
{
	char sqlstr[256];
	if(strstr(id,",")!=NULL) return "提货失败，您的账号非法！";
	if(strlen(costpasswd)!=16){
		return "卡密不存在。";
	}
	if( strstr(costpasswd,"/")!=NULL || strstr(costpasswd,"*")!=NULL || strstr(costpasswd,"+")!=NULL || strstr(costpasswd,"-")!=NULL || strstr(costpasswd,";")!=NULL || strstr(costpasswd,":")!=NULL || strstr(costpasswd," ")!=NULL || strstr(costpasswd,"|")!=NULL || strstr(costpasswd,".")!=NULL || strstr(costpasswd,",")!=NULL || strstr(costpasswd,"'")!=NULL || strstr(costpasswd,"=")!=NULL || strstr(costpasswd,"`")!=NULL || strstr(costpasswd,"<")!=NULL || strstr(costpasswd,">")!=NULL || strstr(costpasswd,"!")!=NULL || strstr(costpasswd,"NULL")!=NULL){
		return "卡密非法。";
	}
	static char token[256]="";
	sprintf(sqlstr,"select `CostStr`, `check` from `OnlineBuy` where CostPasswd = BINARY'%s'", costpasswd);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	if(atoi(mysql_row[1]) == 1){
			 	char *coststr = mysql_row[0];
		  	sprintf(sqlstr,"update OnlineBuy set cdkey=BINARY'%s', CostTime=NOW(), `check`=0  where CostPasswd=BINARY'%s'", id, costpasswd);	
				if(!mysql_query(&mysql,sqlstr)){
					printf("提货卡号%s已提取！\n",costpasswd);
				}
				sprintf(token, "%c|%s", costpasswd[0], coststr);
				
		  	return token;
		  }else{
		  	return "该提货卡已使用过，请勿重复使用！";
		  }
		}else{
			return "提货失败，该提货卡密码不正确！";
		}
	}
	return "提货失败，请与本服管理员联系！";
}
void sasql_OnlineBuy_add( char *coststr, int type, int num )
{
	char sqlstr[256];
	char CostPasswd[32];
	int i, j, k=0;
	for(i=0;i<num;i++){
		memset( CostPasswd, 0, sizeof( CostPasswd ) );
		CostPasswd[0]='0' + type;
		for(j=1;j<16;j++){
			if(rand() % 2==0){
				CostPasswd[j]=(rand() % 26) + 65;
			}else{
				CostPasswd[j]=(rand() % 10) + 48;
			}
		}
		sprintf(sqlstr,"INSERT INTO OnlineBuy (CostPasswd, CostStr) VALUES (BINARY'%s', '%s')", CostPasswd, coststr);
		if(!mysql_query(&mysql,sqlstr)){
			mysql_result=mysql_store_result(&mysql);
			mysql_free_result(mysql_result);
			k++;
		}else{
			break;
		}
	}
	if(type <= 0){
		printf("成功添加%d条宠物提货卡\n", k);
	}else if(type == 1){
		printf("成功添加%d条物品提货卡\n", k);
	}else if(type >= 2){
		printf("成功添加%d条石币提货卡\n", k);
	}
	return ;
}
#endif

#ifdef	_PET_RESET
int sasql_resetpet_point(int petno){
	char sqlstr[256];
	sprintf(sqlstr,"select `Point` from `PetReset` where `PetNo`='%d'",petno);
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	if(atoi(mysql_row[0]) >= 0){
	  		return atoi(mysql_row[0]);
		  }else{
		  	return -1;
		  }
	  }else{
	  	mysql_free_result(mysql_result);
	  	return -1;
	  }
	}else{
		return -1;
	}
}

int sasql_resetbdpet_point(int petno){
	char sqlstr[256];
	sprintf(sqlstr,"select `BdPoint` from `PetReset` where `PetNo`='%d'",petno);
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	if(atoi(mysql_row[0]) > 0){
	  		return atoi(mysql_row[0]);
		  }else{
		  	return -1;
		  }
	  }else{
	  	mysql_free_result(mysql_result);
	  	return -1;
	  }
	}else{
		return -1;
	}
}
#endif

#ifdef	_ZHIPIAO_SYSTEM
int sasql_zhipiao_insert(char *petno,char *cdkey,int point){
	char sqlstr[256];
	char ip[32];
	if(strstr(cdkey,",")!=NULL) return -1;
	//print("\npoint=%d\n",point);
	sprintf(sqlstr,"select `IP` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,cdkey);
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	if(atoi(mysql_row[0]) >= 0){
	  		sprintf(ip,mysql_row[0]);
		  }else{
		  	return -1;
		  }
	  }else{
	  	mysql_free_result(mysql_result);
	  	return -1;
	  }
	}
	sprintf(sqlstr,"INSERT INTO ZhiPiao (PiaoNo, Point,Cdkey1,Date1,IP1) VALUES (BINARY'%s', %d,'%s',NOW(),'%s')", petno, point,cdkey,ip);
	if(!mysql_query(&mysql,sqlstr)){
		return 1;
	}else{
		return -1;
	}
}

int sasql_zhipiao_query(char *piaono){
	char sqlstr[256];
	sprintf(sqlstr,"select `Point`,`check` from `ZhiPiao` where `PiaoNo`='%s'",piaono);
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	if(atoi(mysql_row[1]) == 1){
	  		return atoi(mysql_row[0]);
		  }else{
		  	return 0;
		  }
	  }else{
	  	mysql_free_result(mysql_result);
	  	return -1;
	  }
	}else{
		return -1;
	}
}

int sasql_zhipiao_update(char *cdkey,char *piaono,int checktype){
	if(strstr(cdkey,",")!=NULL) return -1;
	char sqlstr[256];
	char ip[32];
	sprintf(sqlstr,"select `IP` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,cdkey);
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	if(atoi(mysql_row[0]) >= 0){
	  		sprintf(ip,mysql_row[0]);
		  }else{
		  	return -1;
		  }
	  }else{
	  	mysql_free_result(mysql_result);
	  	return -1;
	  }
	}
	sprintf(sqlstr,"update ZhiPiao set Cdkey2=BINARY'%s', Date2=NOW(), `IP2`='%s', `check`=%d  where PiaoNo=BINARY'%s'", cdkey,ip ,checktype,piaono);
	if(!mysql_query(&mysql,sqlstr)){
		return 1;
	}else{
		return -1;
	}
}
#endif
#ifdef	_FMRANK_POINT
int sasql_fm_query(void){
	char sqlstr[256];
	mysql_query(&mysql,"SET NAMES 'gbk'");
	sprintf(sqlstr,"select * from `FmRank` ORDER by `Point` desc");
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	int i=0;
	  	while((mysql_row=mysql_fetch_row(mysql_result)) && i<20){
	  		fmrankindex[i]=atoi(mysql_row[0]);
	  		sprintf(fmrankname[i],"%s",mysql_row[1]);
	  		fmrankpoint[i]=atoi(mysql_row[2]);
	  		i++;
		  }
		  mysql_free_result(mysql_result);
		  return i;
	  }else{
	  	mysql_free_result(mysql_result);
	  	return -1;
	  }
	}else{
		return -1;
	}
}

int sasql_fmpoint_query(int fmindex){
	char sqlstr[256];
	mysql_query(&mysql,"SET NAMES 'gbk'");
	sprintf(sqlstr,"select `Point` from `FmRank` where `Index`='%d'",fmindex);
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	return atoi(mysql_row[0]);
	  }else{
	  	mysql_free_result(mysql_result);
	  	return -1;
	  }
	}else{
		return -1;
	}
}

int sasql_fmno_query(int fmindex){
	char sqlstr[256];
	sprintf(sqlstr,"select `Index` from `FmRank` ORDER by `Point` desc");
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	int i=1;
	  	while((mysql_row=mysql_fetch_row(mysql_result))){
	  		if(atoi(mysql_row[0])==fmindex)
	  			return i;
	  		i++;
		  }
		  mysql_free_result(mysql_result);
		  return -1;
	  }else{
	  	mysql_free_result(mysql_result);
	  	return -1;
	  }
	}else{
		return -1;
	}
}

int sasql_fmpoint_add(int fmindex,int point){
	char sqlstr[256];
	sprintf(sqlstr,"update `FmRank` set `Point`=%d where `Index`=%d",point,fmindex);
	
	if(!mysql_query(&mysql,sqlstr)){
		return 1;
	}else{
		return -1;
	}
}

int sasql_fmindex_add(int fmindex,char *fmname,int point){
	char sqlstr[256];
	mysql_query(&mysql,"SET NAMES 'gbk'");
	sprintf(sqlstr,"INSERT INTO `FmRank` (`Index`,`Name`,`Point`) VALUES (%d,'%s',%d)",fmindex,fmname,point);
	
	if(!mysql_query(&mysql,sqlstr)){
		return 1;
	}else{
		return -1;
	}
}

int sasql_fmindex_del(int fmindex){
	char sqlstr[256];
	sprintf(sqlstr,"DELETE FROM `FmRank` where `Index`=%d",fmindex);
	
	if(!mysql_query(&mysql,sqlstr)){
		return 1;
	}else{
		return -1;
	}
}
#endif
#ifdef	_ITEM_RESET
int sasql_itemreset_query(int itemid){
	char sqlstr[256];
	sprintf(sqlstr,"select `ResetPoint` from `ItemReset` where `Id`='%d'",itemid);
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	return atoi(mysql_row[0]);
	  }else{
	  	mysql_free_result(mysql_result);
	  	return -1;
	  }
	}else{
		return -1;
	}
}
int sasql_itemdel_query(int itemid){
	char sqlstr[256];
	sprintf(sqlstr,"select `DelPoint` from `ItemReset` where `Id`='%d'",itemid);
	
	if(!mysql_query(&mysql,sqlstr)){
	  int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	return atoi(mysql_row[0]);
	  }else{
	  	mysql_free_result(mysql_result);
	  	return -1;
	  }
	}else{
		return -1;
	}
}
#endif

int sasql_cdkey_lock(char *cdkey,int locktype){
	char sqlstr[256];
	sprintf(sqlstr,"select * from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME, cdkey);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			sprintf(sqlstr,"update `%s` set `Lock`=%d where `%s`=BINARY'%s'", sqlconfig.sql_Table,locktype,sqlconfig.sql_NAME, cdkey);	
			if(!mysql_query(&mysql,sqlstr)){
				return 1;
			}else{
				return -1;
			}
		}else{
		mysql_free_result(mysql_result);
		}
	}
	return -1;
}

int sasql_ampoint( char *ID, int point, int flag )
{
	char sqlstr[256];
	sprintf(sqlstr,"select `AmPoint` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,ID);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			if(flag==0){
				return atoi(mysql_row[0]);
			}
			point+=atoi(mysql_row[0]);
			sprintf(sqlstr,"update `%s` set `AmPoint`=%d where `%s`=BINARY'%s'", sqlconfig.sql_Table, point,sqlconfig.sql_NAME, ID);	
			if(!mysql_query(&mysql,sqlstr)){
				return point;
			}else{
				return atoi(mysql_row[0]);
			}
		}else{
		mysql_free_result(mysql_result);
		}
	}
	printf("更新%s积分失败！\n",ID);
	return -1;
}

#ifdef	_RMB_SYSTEM
int sasql_rmbpoint( char *ID, int point, int flag )
{
	char sqlstr[256];
	sprintf(sqlstr,"select `RMBPOINT` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,ID);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			if(flag==0){
				return atoi(mysql_row[0]);
			}
			point+=atoi(mysql_row[0]);
			sprintf(sqlstr,"update `%s` set `RMBPOINT`=%d where `%s`=BINARY'%s'", sqlconfig.sql_Table, point,sqlconfig.sql_NAME, ID);	
			if(!mysql_query(&mysql,sqlstr)){
				return point;
			}else{
				return atoi(mysql_row[0]);
			}
		}else{
		mysql_free_result(mysql_result);
		}
	}
	printf("更新%s现金失败！\n",ID);
	return -1;
}
#endif

int sasql_cdkey_online( char *cdkey )
{
	char sqlstr[256];
	sprintf(sqlstr,"select `Online` from `%s` where `%s`='%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,cdkey);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			return atoi(mysql_row[0]);
		}else{
			mysql_free_result(mysql_result);
			return 0;
		}
	}
	return 0;
}

int sasql_check_iponline( char *IP )
{
	char sqlstr[256];
	sprintf(sqlstr,"select * from `%s` where `IP`='%s'",sqlconfig.sql_Table,IP);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
		if(num_row>0){
			return 1;
		}else{
			return 0;
		}
	}
	return -1;
}

#ifdef _EV_NUM
char* sasql_ev_npcmsg( int npcno )
{
	char sqlstr[256];
	sprintf(sqlstr,"select * from `EvNpc` where `No`='%d'",npcno);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			return mysql_row[1];
		}
		mysql_free_result(mysql_result);
	}
	return "err";
}

char* sasql_ev_item( void )
{
	char sqlstr[256];
	sprintf(sqlstr,"select count(*) from `EvItem`");
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			int itemno;
			itemno = RAND(1,atoi(mysql_row[0]));
			sprintf(sqlstr,"select `Id`,`MixNum`,`MaxNum` from `EvItem` where `No`='%d'",itemno);
			if(!mysql_query(&mysql,sqlstr)){
				mysql_result=mysql_store_result(&mysql);
				num_row=mysql_num_rows(mysql_result);
				if(num_row>0){
					mysql_row=mysql_fetch_row(mysql_result);
					mysql_free_result(mysql_result);
					int itemnum = RAND(atoi(mysql_row[1]),atoi(mysql_row[2]));
					sprintf(sqlstr,"%d|%d",atoi(mysql_row[0]),itemnum);
					return sqlstr;
				}
			}
		}
		mysql_free_result(mysql_result);
	}
	return "err";
}

char* sasql_ev_pet( void )
{
	char sqlstr[256];
	sprintf(sqlstr,"select count(*) from `EvPet`");
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			int petno;
			petno = RAND(1,atoi(mysql_row[0]));
			sprintf(sqlstr,"select `PicId`,`Name`,`MixLv`,`MaxLv` from `EvItem` where `No`='%d'",petno);
			if(!mysql_query(&mysql,sqlstr)){
				mysql_result=mysql_store_result(&mysql);
				num_row=mysql_num_rows(mysql_result);
				if(num_row>0){
					mysql_row=mysql_fetch_row(mysql_result);
					mysql_free_result(mysql_result);
					sprintf(sqlstr,"%d|%s|%d|%d",atoi(mysql_row[0]),mysql_row[1],atoi(mysql_row[2]),atoi(mysql_row[3]));
					return sqlstr;
				}
			}
		}
		mysql_free_result(mysql_result);
	}
	return "err";
}
#endif

char* sasql_rand_buf( void )
{
	char sqlstr[256];
	sprintf(sqlstr,"select count(*) from `RandBuf`");
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			int randno;
			randno = RAND(1,atoi(mysql_row[0]));
			sprintf(sqlstr,"select * from `RandBuf` LIMIT %d,%d",randno-1,randno);
			if(!mysql_query(&mysql,sqlstr)){
				mysql_result=mysql_store_result(&mysql);
				num_row=mysql_num_rows(mysql_result);
				if(num_row>0){
					mysql_row=mysql_fetch_row(mysql_result);
					mysql_free_result(mysql_result);
					sprintf(sqlstr,"%s|%s|%s|%s|%s|%s",mysql_row[0],mysql_row[1],mysql_row[2],mysql_row[3],mysql_row[4],mysql_row[5]);
					return sqlstr;
				}
			}
		}
		mysql_free_result(mysql_result);
	}
	return "err";
}

int sasql_check_lockmac( char *mac)
{
	char sqlstr[256];
	sprintf(sqlstr,"select * from `LockMac` where `MAC`='%s'",mac);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
		if(num_row>0){
			return 1;
		}else{
			return 0;
		}
	}
	return -1;
}

int sasql_check_lockpcid( char *pcid)
{
	char sqlstr[256];
	sprintf(sqlstr,"select * from `LockMac` where `PcId`='%s'",pcid);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
		if(num_row>0){
			return 1;
		}else{
			return 0;
		}
	}
	return -1;
}

int sasql_check_lockip( char *IP , int type )
{
	char sqlstr[256];
	sprintf(sqlstr,"select * from `LockIP` where `IP`='%s' and `TYPE`=%d",IP,type);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
		if(num_row>0){
			return 1;
		}else{
			return 0;
		}
	}
	return -1;
}
int sasql_add_lockip(char *IP,int type,int online,int mode){
	char sqlstr[256];
	sprintf(sqlstr,"INSERT INTO `LockIP` (`IP`,`TYPE`,`Online`,`Mode`) VALUES ('%s',%d,%d,%d)",IP,type,online,mode);
	
	if(!mysql_query(&mysql,sqlstr)){
		return 1;
	}else{
		return -1;
	}
}

int sasql_query_online_ip(char *IP){
	char sqlstr[256];
	sprintf(sqlstr,"select * from `OnlineIP` where `IP`='%s'",IP);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
		if(num_row>0){
			return 1;
		}else{
			return 0;
		}
	}
	return -1;
}

int sasql_online_ip(char *cdkey,char *IP){
	char sqlstr[256];
	sprintf(sqlstr,"select * from `OnlineIP` where `Name`='%s'",cdkey);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
		if(num_row>0){
			sprintf(sqlstr,"update `OnlineIP` set `IP`='%s',`LoginTime`=NOW() where `Name`='%s'",IP,cdkey);
			if(!mysql_query(&mysql,sqlstr)){
				return 1;
			}else{
				return 0;
			}
		}else{
			sprintf(sqlstr,"INSERT INTO `OnlineIP` (`Name`,`IP`,`LoginTime`) VALUES ('%s','%s',NOW())",cdkey,IP);
			if(!mysql_query(&mysql,sqlstr)){
				return 1;
			}else{
				return 0;
			}
		}
	}
	return -1;
}

int sasql_offlinenum( char *IP )
{
	char sqlstr[256];
	sprintf(sqlstr,"select count(*) from `%s` where `Online`=%d and `Offline`=1 and `IP`='%s'",sqlconfig.sql_Table,getServernumber(),IP);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			return atoi(mysql_row[0]);
		}else{
			mysql_free_result(mysql_result);
		}
	}
	return 0;
}

int sasql_offlinejqmnum( char *cdkey )
{
	char sqlstr[256];
	char *pcid = NULL;
	sprintf(sqlstr,"select `PcId` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,cdkey);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			pcid = mysql_row[0];
			if(pcid == NULL) return 0;
			sprintf(sqlstr,"select count(*) from `%s` where `Online`=%d and `Offline`>0 and `PcId`='%s'",sqlconfig.sql_Table,getServernumber(),pcid);
			if(!mysql_query(&mysql,sqlstr)){
				num_row=0;
				mysql_result=mysql_store_result(&mysql);
				num_row=mysql_num_rows(mysql_result);
				if(num_row>0){
					mysql_row=mysql_fetch_row(mysql_result);
					mysql_free_result(mysql_result);
					return atoi(mysql_row[0]);
				}
			}
		}else{
			mysql_free_result(mysql_result);
		}
	}
	return 0;
}

char* sasql_ip( char *cdkey )
{
	char sqlstr[256];
	sprintf(sqlstr,"select `IP` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,cdkey);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			return mysql_row[0];
		}else{
			mysql_free_result(mysql_result);
		}
	}
	return "err";
}

char* sasql_query_jqm( char *cdkey )
{
	char sqlstr[256];
	sprintf(sqlstr,"select `PcId` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,cdkey);
	//print("\n sqlstr=%s",sqlstr);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			return mysql_row[0];
		}else{
			mysql_free_result(mysql_result);
		}
	}
	return "err";
}

int sasql_setVipPoint( int charaindex, int point )
{
	char sqlstr[256];
	char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
	sprintf(sqlstr,"update %s set VipPoint=%d where %s=BINARY'%s'", sqlconfig.sql_Table, point, sqlconfig.sql_NAME, id);	
	
	if(!mysql_query(&mysql,sqlstr)){
		//printf("用户%s当前重回币%d！\n",id,point);
		return point;
	}
	return -1;
}

int sasql_getVipPoint( int charaindex )
{
	char sqlstr[256];
	char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
	sprintf(sqlstr,"select VipPoint from `%s` where %s=BINARY'%s'", sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
	//	mysql_free_result(mysql_result);
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	
	  	return atoi(mysql_row[0]);
		}
	}
//	printf("\n数据库查找失败！\n");
	printf("重新连接数据库[3]...");
//	sasql_close();
//	sasql_init();
	printf("完成\n");
	return -1;
}

int sasql_setVipPointForCdkey( char *id, int point )
{
	if(sasql_ckeckStrint(id) == FALSE){
		printf("异常字符的用户名%s\n",id);
	  return -1;
	}
	
	char sqlstr[256];
	sprintf(sqlstr,"update %s set VipPoint=%d where %s=BINARY'%s'", sqlconfig.sql_Table, point, sqlconfig.sql_NAME, id);	
	
	if(!mysql_query(&mysql,sqlstr)){
		//printf("用户%s当前重回币%d！\n",id,point);
		return point;
	}
	return -1;
}


int sasql_getVipPointForCdkey( char *id )
{
	if(sasql_ckeckStrint(id) == FALSE){
		printf("异常字符的用户名%s\n",id);
	  return -1;
	}
	
	char sqlstr[256];
	sprintf(sqlstr,"select VipPoint from `%s` where %s=BINARY'%s'", sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	
	  	return atoi(mysql_row[0]);
		}
	}
//	printf("\n数据库查找失败！\n");
	printf("重新连接数据库[2]...");
//	sasql_close();
//	sasql_init();
	printf("完成\n");
	return -1;
}

BOOL sasql_ckeckStrint( char *str )
{
  int i;
  for(i=0;i<strlen(str);i++){
  	if((str[i] < 'a' || str[i] > 'z') && (str[i] < 'A' || str[i] > 'Z') && (str[i] < '0' || str[i] > '9') && (str[i] != '.')){
  		return FALSE;
  	}
  }
  return TRUE;
}

#ifdef _CARD_PRODUCTION
/**
 * 生成充值卡
 */
void sasql_OnlineCost_add( int cost, int num)
{
	char sqlstr[256];
	char CostPasswd[32];
	memset( CostPasswd, 0, sizeof( CostPasswd ) );
	int i, j, k=0;
	for(i=0;i<num;i++){
		for(j=0;j<16;j++){
			if(rand() % 2==0){
				CostPasswd[j]=(rand() % 26) + 65;
			}else{
				CostPasswd[j]=(rand() % 10) + 48;
			}
		}
		sprintf(sqlstr,"INSERT INTO OnlineCost (CostPasswd, CostVal) VALUES (BINARY'%s', %d)", CostPasswd, cost);
		if(!mysql_query(&mysql,sqlstr)){
			k++;
		}else{
			continue;
		}
	}
#ifdef _DEBUG
	printf("成功添加%d条面值%d的充值卡\n", k, cost);
#endif
	return ;
}
#endif

int sasql_jifenPoint( int charaindex )
{
//printf("进入了数据库\n");
	char sqlstr[256];
	char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
	sprintf(sqlstr,"select AmPoint from `%s` where %s=BINARY'%s'", sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	return atoi(mysql_row[0]);
		}
	}
	printf("更新积分点数失败\n");
	return -1;
}

int sasql_setJfPoint( int charaindex, int point )
{
	char sqlstr[256];
	char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
	sprintf(sqlstr,"update %s set AmPoint=%d where %s=BINARY'%s'", sqlconfig.sql_Table, point, sqlconfig.sql_NAME, id);	
	if(!mysql_query(&mysql,sqlstr)){
//if(!sasql_mysql_query(sqlstr)){
		return point;
	}
	return -1;
}

int sasql_xjPoint( int charaindex )
{
//printf("进入了数据库\n");
	char sqlstr[256];
	char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
	sprintf(sqlstr,"select RMBPOINT from `%s` where %s=BINARY'%s'", sqlconfig.sql_Table, sqlconfig.sql_NAME, id);

	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
	  mysql_result=mysql_store_result(&mysql);
	  num_row=mysql_num_rows(mysql_result);
	  if(num_row>0){
	  	mysql_row=mysql_fetch_row(mysql_result);
	  	mysql_free_result(mysql_result);
	  	return atoi(mysql_row[0]);
		}
	}
	printf("更新RMB点数失败\n");
	return -1;
}

int sasql_setxjPoint( int charaindex, int point )
{
	char sqlstr[256];
	char *id = CHAR_getChar(charaindex, CHAR_CDKEY);
	sprintf(sqlstr,"update %s set RMBPOINT=%d where %s=BINARY'%s'", sqlconfig.sql_Table, point, sqlconfig.sql_NAME, id);	
	if(!mysql_query(&mysql,sqlstr)){
//if(!sasql_mysql_query(sqlstr)){
		return point;
	}
	return -1;
}

struct youx{
	int result;
	int flg;
	int errnono;
	char* errstr;
	int fieldCount;
	int rowCount;
	int rowAt;
}retlua;


#ifdef _CAX_LNS_MYSQLSUOXU
void sasql_LUASQLAdv(char* sqlstr,char* filepath,char* function,int npcindex ,int charaindex,char* msg,int flg)
{
	char* data = sasql_LuaAdv_QueryR(sqlstr,flg);
	NPC_Lua_SQLPushAdvCallBack(retlua.result,retlua.flg,retlua.errnono,retlua.errstr,retlua.fieldCount ,retlua.rowCount ,retlua.rowAt, data,filepath,function,npcindex,charaindex,msg );
}
char* sasql_LuaAdv_QueryR(char *sqlstr,int flg)
{
	int rowAtt = 0;
	static char buff[1024] = "";
	snprintf(buff,1024, "");
	buff[1023]='\0';
	/*初始化信息表*/
	retlua.result = 0;//结果集ERROR
	retlua.flg = flg;//分类
	retlua.errnono = 0;//MYSQL错误编号
	retlua.errstr = "";//MYSQL错误信息
	retlua.fieldCount = 0;//查询的项目数
	retlua.rowCount = 0;//查询结果集行数
	retlua.rowAt = 0;//当前记录行数


	if (flg == 1 || flg == 2){

		if(!mysql_query(&mysql,sqlstr)){
			int num_row=0;
			mysql_result=mysql_store_result(&mysql);
			num_row=mysql_num_rows(mysql_result);//结果集行数
			if (mysql_result  == NULL)
			{
				retlua.result = -2;
			}else if (mysql_eof(mysql_result))
			{
				retlua.result = -3;
			}

			if(num_row>0){

				mysql_row = mysql_fetch_row(mysql_result);//从查询结果中取一条

				retlua.errnono = mysql_errno(&mysql);//错误编号
				retlua.errstr = mysql_error(&mysql);//错误信息
				retlua.fieldCount = mysql_field_count(&mysql);//列数

				retlua.rowCount = num_row;//结果行数量
				rowAtt++;
				retlua.rowAt = rowAtt;//当前行数
				if (flg == 1){
					mysql_free_result(mysql_result);//释放结果集
					mysql_result = NULL;
					rowAtt = 0;
				}

				int i;
				for (i=0;i<retlua.fieldCount;i++)
				{
					snprintf(buff,sizeof(buff), "%s%s;",buff,(char*)mysql_row[i]);
				}
				if (num_row > 15) retlua.result = -1;
				return buff;
			}else {
				retlua.errnono = mysql_errno(&mysql);
				retlua.errstr = mysql_error(&mysql);
			}
		}
	}else if (flg == 3){


		int num_row=0;
		if (mysql_result == NULL) {
			retlua.result = -2;
			return "结果集已经释放。";
		}
		num_row=mysql_num_rows(mysql_result);//结果集行数
		if (mysql_eof(mysql_result))
		{
			retlua.result = -3;//已达到尾端，先给你提个醒，如果还继续查询，下面将会自动释放。
		}
		if (num_row == rowAtt) {
			mysql_free_result(mysql_result);//释放结果集
			mysql_result = NULL;
			rowAtt = 0;
			retlua.result = -3;
			return "已到达尾行，自动释放结果集。";
		}
		if(num_row>0){
			mysql_row = mysql_fetch_row(mysql_result);//从查询结果中取一条
			retlua.errnono = mysql_errno(&mysql);//错误编号
			retlua.errstr = mysql_error(&mysql);//错误信息
			rowAtt++;
			retlua.rowAt = rowAtt;//当前行数
			retlua.fieldCount = mysql_field_count(&mysql);//列数
			retlua.rowCount = num_row;//结果行数量
			if (num_row > 15) retlua.result = -1;
			int i;
			for (i=0;i<retlua.fieldCount;i++)
			{
				snprintf(buff,sizeof(buff), "%s%s;",buff,(char*)mysql_row[i]);
			}
			return buff;
		}else {
			retlua.errnono = mysql_errno(&mysql);
			retlua.errstr = mysql_error(&mysql);
		}

	}else if (flg == 4){
		if (mysql_result == NULL) {
			retlua.result = -2;
			return "结果集已经释放。";
		}
		mysql_free_result(mysql_result);//释放结果集
		mysql_result = NULL;
		rowAtt = 0;
		retlua.rowAt = rowAtt;//当前行数
		retlua.errnono = mysql_errno(&mysql);
		retlua.errstr = mysql_error(&mysql);
		return "";
	}

	retlua.errnono = mysql_errno(&mysql);
	retlua.errstr = mysql_error(&mysql);
	return "";
}
void sasql_LUASQL(char* sqlstr,int row,char* filepath,char* function,int npcindex ,int charaindex,char* msg)
{
	if(row == -1 )
	{
		mysql_query(&mysql,sqlstr);
	}else{
		NPC_Lua_SQLPushCallBack(sasql_Lua_QueryR(row,sqlstr),filepath,function,npcindex, charaindex,msg);
	}
}
char* sasql_Lua_QueryR(int row,char *sqlstr)
{
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		mysql_free_result(mysql_result);
		if(num_row>0){
			mysql_row = mysql_fetch_row(mysql_result);
			return (char*)mysql_row[row];
		}
	}else
		return "SQL Query ERROR";
}
#endif

#ifdef _CAX_NEWMAC_LOGIN
char* sasql_query_mac( char *cdkey )
{
	char sqlstr[256];
	sprintf(sqlstr,"select `MAC` from `%s` where `%s`=BINARY'%s'",sqlconfig.sql_Table,sqlconfig.sql_NAME,cdkey);
	//print("\n sqlstr=%s",sqlstr);
	if(!mysql_query(&mysql,sqlstr)){
		int num_row=0;
		mysql_result=mysql_store_result(&mysql);
		num_row=mysql_num_rows(mysql_result);
		if(num_row>0){
			mysql_row=mysql_fetch_row(mysql_result);
			mysql_free_result(mysql_result);
			return mysql_row[0];
		}else{
			mysql_free_result(mysql_result);
		}
	}
	return "err";
}
#endif
