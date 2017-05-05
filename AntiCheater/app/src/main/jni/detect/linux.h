//
// Created by killer on 5/5/17.
//

#ifndef ANTICHEATER_LINUX_H
#define ANTICHEATER_LINUX_H

#include <stdio.h>
#include <unistd.h>

#include "Debug.h"

void getLibFilePath(char *cmdline, char *buf){
	struct stat *info;

	for(int i=1;i<=100;i++){
		sprintf(buf, "/data/%s-1/lib", cmdline);
		if(access(buf,F_OK) == 0){
		//if(stat(buf,info) == 0){
			break;
		}
	}
}

void getApkFilePath(char *cmdline, char *buf){
	struct stat *info;

	for(int i=0;i<=100;i++){
		//Android 5.0
		sprintf(buf, "/data/app/%s-%d/base.apk", cmdline,i);
		LOGD("file path :  %s",buf);
		LOGD("access result :  %d",access(buf,F_OK));
		if(access(buf,F_OK) == 0){
			break;
		}else{
			sprintf(buf, "/data/app-asec/%s-%d/pkg.apk", cmdline,i);
			if(access(buf,F_OK) == 0){
				break;
			}
		}
	}
	LOGD("stat(filePath,info) %s",buf);
}

void getCmdline(int pid, char *buf, int len)
{
	char file[260];
	sprintf(file, "/proc/%d/cmdline", pid);

	buf[0] = 0;
	FILE *fp = fopen(file, "r");
	if(fp)
	{
		fgets(buf, len, fp);
		fclose(fp);
	}
}

void getComm(int pid, char *buf, int len){
	char file[260];
	sprintf(file, "/proc/%d/comm", pid);

	buf[0] = 0;
	FILE *fp = fopen(file, "r");
	if(fp)
	{
		fgets(buf, len, fp);
		fclose(fp);
	}
}

int getUid(int pid)
{
	int uid = 0;

	char file[260];
	sprintf(file, "/proc/%d/status", pid);

    FILE *fp = fopen(file, "r");
	if(fp)
	{
		while(1)
		{
			char buf[PATH_MAX];
			if(!fgets(buf, sizeof(buf), fp))
			{
				break;
			}
			if(!strncmp("Uid:", buf, 4))
			{
				uid = atoi(buf+5);
				break;
			}
		}
		fclose(fp);
	}

	return uid;
}

#endif //ANTICHEATER_LINUX_H
