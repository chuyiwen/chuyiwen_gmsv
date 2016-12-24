#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include "autil.h"
#include "util.h"
#include "net.h"
#include "char.h"
#include "char_base.h"
#include "mclient.h"
#include "log.h"
#include "handletime.h"
#include "chatmagic.h"

#ifdef _GSERVER_RUNTIME //����GSERVERִ�ж���ʱ���MSERVER
void gserver_runtime()
{
	int checksum=0;
	char buffer[1024];
    time_t starttime1;

	if(mfd == -1) return;
    if( gserver_runtime_starttime0_flag ){
	    gserver_runtime_starttime0_flag = FALSE;
		gserver_runtime_starttime0 = time(NULL);
	}
	print("\nChange->����ʱ���mserver\n");
	strcpy(buffer, "");
	strcpy(PersonalKey, MSPERSIONALKEY);
    starttime1 = time(NULL);   
	checksum += util_mkint(-1,buffer, (int)difftime(starttime1,gserver_runtime_starttime0));//���͵ĵ�λΪ��
	util_mkint(-1,buffer, checksum);
	util_SendMesg(mfd, MPROTO_RECALL_GSERVER_RUNTIME, buffer);
}
#endif
