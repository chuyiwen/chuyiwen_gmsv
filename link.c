#include <stdlib.h>
#include "version.h"
#include "link.h"
#include "buf.h"

/*
 * ¦��
 *      top         �������á���
 * ߯Ի��
 */
BOOL Nodeappendtail( Node** top  , Node* add )
{
    Node* c;        /*�������*/
    Node* next;     /*ޥ���������á�����*/

    /*������NULL������������������*/
    if( *top == NULL ){
        *top = allocateMemory( sizeof( Node  ) );
        if( *top== NULL ) return FALSE;
        (*top)->next    = NULL;            /*���巴  ��*/
        (*top)->size    = add->size;   /*Ӯ���綰*/
        (*top)->val     = add->val;         /*���̼������������*/
        return TRUE;
    }

    for( c = *top ; c->next ; c = c->next ); /* c����  ë������ */
    next = allocateMemory( sizeof(Node) );
    if( next == NULL )return FALSE;
    c->next = next;                         /* next���ëɬ������ */
    next->next  = NULL;                      /*���巴  ��*/
    next->val   = add->val;                   /*���̼������������*/
    next->size  = add->size;             /*Ӯ���綰*/
    return TRUE;
}




/*
 *  �����ͼ��年����
 * ¦��
 *      top         �������á���
 * ߯Ի��
 */
BOOL Nodeappendhead( Node** nowtop  , Node* add )
{
    Node* newtop;       /*ޥ������  ��*/

    /*������NULL������������������*/
    if( *nowtop == NULL ){
        *nowtop = allocateMemory( sizeof( Node  ) );
        if( *nowtop == NULL ) return FALSE;
        (*nowtop)->next = NULL;             /*���巴  ��*/
        (*nowtop)->size = add->size;        /*Ӯ���綰*/
        (*nowtop)->val  = add->val;         /*���̼������������*/
        return TRUE;
    }

    /*
     * ޥ�����á���ë������
     */
    newtop = allocateMemory( sizeof(Node) );
    newtop->next    = *nowtop;
    newtop->val     = add->val;
    newtop->size    = add->size;
    *nowtop = newtop;
    return TRUE;
}




/*
 * ¦��
 *          ret         ��Ի�������á��������
 * ߯Ի��
 */
BOOL  Noderemovehead( Node** top , Node* ret)
{
    Node* newtop;         /*ޥ������  ��ئ���á���*/

    if( *top == NULL )return FALSE;

    ret->val    = (*top)->val;
    ret->size   = (*top)->size;
    newtop = (*top)->next;
    freeMemory( *top );
    *top = newtop;

    return TRUE;
}





/*
 * ¦��
 *          ret         ��Ի�������á��������
 * ߯Ի��
 */
BOOL  Noderemovetail( Node** top , Node* ret)
{
    Node* c;             /*�������*/
    Node* c1;            /*������� �й�ƥ�� c->nextë϶��*/

    if( *top == NULL )return FALSE;

    c = *top;           /*�����ɬ��*/
    c1 = c->next;       /*�����ɬ��*/
    while(1){
        if( c1->next == NULL )
            /*
             * ������ƥ          c1
             *                      |
             *         | next |---->+------+
             *         |------|     | next |---->NULL
             *         |      |     +------+
             *         +------+     |      |
             *                      +------+
             *��ئ�Ȼ�����
             */
            break;
        c=c->next;
        c1=c->next;
    }
    c->next = NULL;     /*c1��ؤ�����ּ�ë�������¼�ƥnext��NULL������*/
    /*�����*/
    ret->val    = c1->val;
    ret->size   = c1->size;

    freeMemory( c1 );

    return TRUE;
}
