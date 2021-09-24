// Copyright 2019-2021 The jdh99 Authors. All rights reserved.
// 守护模块
// Authors: jdh99 <jdh821@163.com>

#ifndef TZDAEMON_H
#define TZDAEMON_H

#include "tztype.h"

// 标签最大字节数
#define TZDAEMON_TAG_SIZE_MAX 32
// 喂狗间隔.单位:ms
#define TZDAEMON_FEED_DOG_INTERVAL 500

// TZDaemonLoad 模块载入
// 喂狗函数feed可以为NULL,表示不需要喂狗
bool TZDaemonLoad(TZEmptyFunc reboot, TZEmptyFunc feed);

// TZDaemonMonitorTimeout 监控超时
// 超时时间单位是us
// 返回句柄.如果是0,表示增加监控项失败
intptr_t TZDaemonMonitorTimeout(const char* tag, uint64_t timeout);

// TZDaemonUpdateTime 更新时间
void TZDaemonUpdateTime(intptr_t handle);

// TZDaemonMonitorRetryCount 监控重传次数
// 注意:重传次数等于最大次数会导致复位
// 返回句柄.如果是0,表示增加监控项失败
intptr_t TZDaemonMonitorRetryCount(const char* tag, int count);

// TZDaemonAddRetryCount 重传次数自增
void TZDaemonAddRetryNum(intptr_t handle);

// TZDaemonClearRetryNum 清除重传次数
void TZDaemonClearRetryNum(intptr_t handle);

#endif
