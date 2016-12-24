#ifndef __LINK_H__
#define __LINK_H__

#include "common.h"

/*
 * ��������հë��������  ��
 * ����������val��    ���巴������ئ�У۹���Ի���̼����������������
 * �浤��
 */
typedef struct tagNode
{
    struct tagNode* next;           /*�ݼ��á���߼����̼�����*/
    char* val;                      /*��  ����  ٯ  */
    int size;                       /*val��������*/
}Node;

BOOL Nodeappendhead( Node** top  , Node* add );
BOOL Nodeappendtail( Node** top  , Node* add );
BOOL Noderemovehead( Node** top , Node* ret);
BOOL Noderemovetail( Node** top , Node* ret);
#endif
