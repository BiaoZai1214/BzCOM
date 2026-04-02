#include <stdlib.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
 * all the API functions to use the MPU wrappers.  That should only be done when
 * task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "list.h"

/* Lint e9021, e961 and e750 are suppressed as a MISRA exception justified
 * because the MPU ports require MPU_WRAPPERS_INCLUDED_FROM_API_FILE to be
 * defined for the header files above, but not in this file, in order to
 * generate the correct privileged Vs unprivileged linkage and placement. */
#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE /*lint !e961 !e750 !e9021. */

/*-----------------------------------------------------------
* PUBLIC LIST API documented in list.h
*----------------------------------------------------------*/

void vListInitialise( List_t * const pxList )
{
    /* The list structure contains a list item which is used to mark the
     * end of the list.  To initialise the list the list end is inserted
     * as the only list entry. */
    pxList->pxIndex = ( ListItem_t * ) &( pxList->xListEnd ); /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

    listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE( &( pxList->xListEnd ) );

    /* The list end value is the highest possible value in the list to
     * ensure it remains at the end of the list. */
    pxList->xListEnd.xItemValue = portMAX_DELAY;

    /* The list end next and previous pointers point to itself so we know
     * when the list is empty. */
    pxList->xListEnd.pxNext = ( ListItem_t * ) &( pxList->xListEnd );     /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */
    pxList->xListEnd.pxPrevious = ( ListItem_t * ) &( pxList->xListEnd ); /*lint !e826 !e740 !e9087 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

    /* Initialize the remaining fields of xListEnd when it is a proper ListItem_t */
    #if ( configUSE_MINI_LIST_ITEM == 0 )
    {
        pxList->xListEnd.pvOwner = NULL;
        pxList->xListEnd.pxContainer = NULL;
        listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE( &( pxList->xListEnd ) );
    }
    #endif

    pxList->uxNumberOfItems = ( UBaseType_t ) 0U;

    /* Write known values into the list if
     * configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
    listSET_LIST_INTEGRITY_CHECK_1_VALUE( pxList );
    listSET_LIST_INTEGRITY_CHECK_2_VALUE( pxList );
}
/*-----------------------------------------------------------*/

void vListInitialiseItem( ListItem_t * const pxItem )
{
    // 初始化节点所属链表指针为NULL: 未添加到任何链表中
    pxItem->pxContainer = NULL;

    // 完整性校验,写入特征值
	// 用于持续监测节点内存是否被异常修改
    listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem );
    listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE( pxItem );
}
/*-----------------------------------------------------------*/

void vListInsertEnd( List_t * const pxList,
                     ListItem_t * const pxNewListItem )
{
    // 获取当前遍历索引位置
    ListItem_t * const pxIndex = pxList->pxIndex;

    // 完整性检查（调试用）
    listTEST_LIST_INTEGRITY( pxList );
    listTEST_LIST_ITEM_INTEGRITY( pxNewListItem );

    // 将新节点插入到 pxIndex 之前（作为逻辑上的“最后”一项）
    pxNewListItem->pxNext = pxIndex;
    pxNewListItem->pxPrevious = pxIndex->pxPrevious;
    // 覆盖测试延迟（仅用于代码覆盖率测试）
    mtCOVERAGE_TEST_DELAY();
    // 断开原链接，接入新节点
    pxIndex->pxPrevious->pxNext = pxNewListItem;
    pxIndex->pxPrevious = pxNewListItem;
    // 记录新节点所属的列表
    pxNewListItem->pxContainer = pxList;
    // 列表项数量加 1
    ( pxList->uxNumberOfItems )++;
}
/*-----------------------------------------------------------*/

void vListInsert( List_t * const pxList,
                  ListItem_t * const pxNewListItem )
{
    ListItem_t * pxIterator;
    // 获取新节点的键值（用于排序）
    const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;

    // 完整性检查（调试用）
    listTEST_LIST_INTEGRITY( pxList );
    listTEST_LIST_ITEM_INTEGRITY( pxNewListItem );

    // 特殊情况：如果新节点的值是最大值（portMAX_DELAY）
    if( xValueOfInsertion == portMAX_DELAY )
    {
        // 直接定位到链表尾部（哨兵的前一个节点），避免遍历死循环
        pxIterator = pxList->xListEnd.pxPrevious;
    }
    else
    {
        // 普通情况：从头遍历，找到第一个“下一项的值 > 新节点值”的位置
        // 注意：如果值相等，循环继续，确保新节点插在相同值节点的后面（公平性）
        for( pxIterator = ( ListItem_t * ) &( pxList->xListEnd ); 
             pxIterator->pxNext->xItemValue <= xValueOfInsertion; 
             pxIterator = pxIterator->pxNext )
        {
            // 空循环体，仅移动迭代器
        }
    }

    // 四步指针操作，将新节点插入到 pxIterator 和 pxIterator->pxNext 之间
    pxNewListItem->pxNext = pxIterator->pxNext;
    pxNewListItem->pxNext->pxPrevious = pxNewListItem;
    pxNewListItem->pxPrevious = pxIterator;
    pxIterator->pxNext = pxNewListItem;

    // 记录新节点所属的列表
    pxNewListItem->pxContainer = pxList;

    // 列表项数量加 1
    ( pxList->uxNumberOfItems )++;
}
/*-----------------------------------------------------------*/

UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove )
{
    // 通过节点自带的指针，找到它所属的列表
    List_t * const pxList = pxItemToRemove->pxContainer;

    // 双向链表删除操作：断开前后连接，绕过当前节点
    pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
    pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

    // 覆盖测试延迟（仅用于代码覆盖率测试）
    mtCOVERAGE_TEST_DELAY();

    // 关键保护：如果被删节点正好是当前遍历索引指向的节点
    if( pxList->pxIndex == pxItemToRemove )
    {
        // 将索引回退到前一个节点，防止索引悬空（野指针）
        pxList->pxIndex = pxItemToRemove->pxPrevious;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    // 清空节点的所属列表标记（表示已移除）
    pxItemToRemove->pxContainer = NULL;

    // 列表项数量减 1
    ( pxList->uxNumberOfItems )--;

    // 返回剩余项数
    return pxList->uxNumberOfItems;
}
/*-----------------------------------------------------------*/
