// Copyright 2019-2019 The TZIOT Authors. All rights reserved.
// �ػ�ģ�����ļ�

#include "tzdaemon.h"
#include "tztime.h"
#include "async.h"
#include "tzmalloc.h"
#include "tzlist.h"
#include "lagan.h"

#include <string.h>

#define TAG "tzdaemon"

// tzmalloc�ֽ���
#define MALLOC_TOTAL 4096

// ��鳬ʱ���.��λ:ms
#define CHECK_TIMEOUT_INTERVAL 100

#define MONITOR_MODE_TIMEOUT 0
#define MONITOR_MODE_RETRY 1

#pragma pack(1)

// tItem ��ؾ��
typedef struct {
    char tag[TZDAEMON_TAG_SIZE_MAX + 1];
    int mode;
    uint64_t timeout;
    int count;
    uint64_t timeStart;
    int retry;
} tItem;

#pragma pack()

static int mid = -1;

// �洢����б�
static intptr_t list = 0;

static TZEmptyFunc gReboot = NULL;
static TZEmptyFunc gFeed = NULL;

static int feedDog(void);
static int checkTimeout(void);
static TZListNode* createNode(void);

// TZDaemonLoad ģ������
// ι������feed����ΪNULL,��ʾ����Ҫι��
bool TZDaemonLoad(TZEmptyFunc reboot, TZEmptyFunc feed) {
    mid = TZMallocRegister(0, TAG, MALLOC_TOTAL);
    if (mid == -1) {
        LE(TAG, "load failed!malloc failed");
        return false;
    }
    list = TZListCreateList(mid);
    if (list == 0) {
        LE(TAG, "load failed!create list failed");
        return false;
    }

    if (AsyncStart(feedDog, TZDAEMON_FEED_DOG_INTERVAL * ASYNC_MILLISECOND) == false) {
        LE(TAG, "load failed!start feed dog task failed!");
        return false;
    }

    if (AsyncStart(checkTimeout, CHECK_TIMEOUT_INTERVAL * ASYNC_MILLISECOND) == false) {
        LE(TAG, "load failed!start check timeout task failed!");
        return false;
    }

    gReboot = reboot;
    gFeed = feed;
    return true;
}

static int feedDog(void) {
    static struct pt pt;

    PT_BEGIN(&pt);

    PT_WAIT_UNTIL(&pt, gFeed != NULL);
    gFeed();

    PT_END(&pt);
}

static int checkTimeout(void) {
    static struct pt pt;
    TZListNode* node = NULL;
    tItem* item = NULL;
    uint64_t timeNow = 0;

    PT_BEGIN(&pt);

    PT_WAIT_UNTIL(&pt, gReboot != NULL);

    timeNow = TZTimeGet();
    node = TZListGetHeader(list);
    for (;;) {
        if (node == NULL) {
            break;
        }

        item = (tItem*)node->Data;
        if (item->mode == MONITOR_MODE_TIMEOUT) {
            if (timeNow < item->timeStart) {
                item->timeStart = timeNow;
                continue;
            }
            if (timeNow - item->timeStart > item->timeout) {
                LE(TAG, "%s timeout:%lldus,reboot!", item->tag, item->timeout);
                gReboot();
            }
        }

        node = node->Next;
    }

    PT_END(&pt);
}

// TZDaemonMonitorTimeout ��س�ʱ
// ��ʱʱ�䵥λ��us
// ���ؾ��.�����0,��ʾ���Ӽ����ʧ��
intptr_t TZDaemonMonitorTimeout(const char* tag, uint64_t timeout) {
    if (strlen(tag) > TZDAEMON_TAG_SIZE_MAX) {
        LE(TAG, "add monitor timeout failed!tag is too long:%d", strlen(tag));
        return 0;
    }
    
    TZListNode* node = createNode();
    if (node == NULL) {
        LE(TAG, "add monitor timeout failed!create node is failed");
        return 0;
    }

    LI(TAG, "add monitor timeout success.tag:%s timeout:%lldus", tag, timeout);
    tItem* item = (tItem*)node->Data;
    strcpy(item->tag, tag);
    item->mode = MONITOR_MODE_TIMEOUT;
    item->timeout = timeout;
    item->timeStart = TZTimeGet();
    TZListAppend(list, node);
    return (intptr_t)item;
}

static TZListNode* createNode(void) {
    TZListNode* node = TZListCreateNode(list);
    if (node == NULL) {
        return NULL;
    }
    node->Data = TZMalloc(mid, sizeof(tItem));
    if (node->Data == NULL) {
        TZFree(node);
        return NULL;
    }
    return node;
}

// TZDaemonUpdateTime ����ʱ��
void TZDaemonUpdateTime(intptr_t handle) {
    if (handle == 0) {
        return;
    }
    tItem* item = (tItem*)handle;
    if (item->mode != MONITOR_MODE_TIMEOUT) {
        return;
    }

    LD(TAG, "%s update time", item->tag);
    item->timeStart = TZTimeGet();
}

// TZDaemonMonitorRetryCount ����ش�����
// ע��:�ش����������������ᵼ�¸�λ
// ���ؾ��.�����0,��ʾ���Ӽ����ʧ��
intptr_t TZDaemonMonitorRetryCount(const char* tag, int count) {
    if (strlen(tag) > TZDAEMON_TAG_SIZE_MAX) {
        LE(TAG, "add monitor retry count failed!tag is too long:%d", strlen(tag));
        return 0;
    }

    TZListNode* node = createNode();
    if (node == NULL) {
        LE(TAG, "add monitor retry count failed!create node is failed");
        return 0;
    }

    LI(TAG, "add monitor retry count success.tag:%s count:%d", tag, count);
    tItem* item = (tItem*)node->Data;
    strcpy(item->tag, tag);
    item->mode = MONITOR_MODE_RETRY;
    item->count = count;
    item->retry = 0;
    TZListAppend(list, node);
    return (intptr_t)item;
}

// TZDaemonAddRetryCount �ش���������
void TZDaemonAddRetryNum(intptr_t handle) {
    if (handle == 0) {
        return;
    }
    tItem* item = (tItem*)handle;
    if (item->mode != MONITOR_MODE_RETRY) {
        return;
    }

    item->retry++;
    LD(TAG, "%s add retry num:%d", item->tag, item->retry);
    if (item->retry >= item->count) {
        LE(TAG, "%s retry max %d,reboot!", item->tag, item->retry);
        gReboot();
    }
}

// TZDaemonClearRetryNum ����ش�����
void TZDaemonClearRetryNum(intptr_t handle) {
    if (handle == 0) {
        return;
    }
    tItem* item = (tItem*)handle;
    if (item->mode != MONITOR_MODE_RETRY) {
        return;
    }

    LD(TAG, "%s clear retry num", item->tag);
    item->retry = 0;
}
