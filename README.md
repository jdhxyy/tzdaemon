# tzdaemon

## 1. 介绍
守护模块，可以跟踪程序异常并主动复位。

## 2. 功能
- 监控超时项
- 监控重传超过最大次数

## 3. API
```c
// TZDaemonLoad 模块载入
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
```
