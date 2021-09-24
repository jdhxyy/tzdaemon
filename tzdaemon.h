// Copyright 2019-2020 The TZIOT Authors. All rights reserved.
// �ػ�ģ��ͷ�ļ�

#ifndef TZDAEMON_H
#define TZDAEMON_H

#include "tztype.h"

// ��ǩ����ֽ���
#define TZDAEMON_TAG_SIZE_MAX 32
// ι�����.��λ:ms
#define TZDAEMON_FEED_DOG_INTERVAL 500

// TZDaemonLoad ģ������
bool TZDaemonLoad(TZEmptyFunc reboot, TZEmptyFunc feed);

// TZDaemonMonitorTimeout ��س�ʱ
// ��ʱʱ�䵥λ��us
// ���ؾ��.�����0,��ʾ���Ӽ����ʧ��
intptr_t TZDaemonMonitorTimeout(const char* tag, uint64_t timeout);

// TZDaemonUpdateTime ����ʱ��
void TZDaemonUpdateTime(intptr_t handle);

// TZDaemonMonitorRetryCount ����ش�����
// ע��:�ش����������������ᵼ�¸�λ
// ���ؾ��.�����0,��ʾ���Ӽ����ʧ��
intptr_t TZDaemonMonitorRetryCount(const char* tag, int count);

// TZDaemonAddRetryCount �ش���������
void TZDaemonAddRetryNum(intptr_t handle);

// TZDaemonClearRetryNum ����ش�����
void TZDaemonClearRetryNum(intptr_t handle);

#endif
