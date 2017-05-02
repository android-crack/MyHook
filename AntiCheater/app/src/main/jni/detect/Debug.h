//
// Created by Administrator on 2017/4/27 0027.
//

#ifndef ANTICHEATER_DEBUG_H
#define ANTICHEATER_DEBUG_H

#include <android/log.h>

#define TAG "LMDAnti"

#define LOGV(...)	__android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...)	__android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...)	__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...)	__android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

#define	_JAVA(x)		(*env)->x(env,
#define	_ARGS(...)		__VA_ARGS__)

#endif //ANTICHEATER_DEBUG_H
