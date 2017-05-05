//
// Created by killer on 5/5/17.
//

#include "CheckBinaryModification.h"

bool ModFindHash(char *hash) {

	long lSize;
	size_t result;
	int result_val;

	char *buffer;
	char patternFile[MAX_PATH] = "/mnt/sdcard/pattern";

	FILE *fp = fopen(patternFile, "rb");

	if (fp) {
		fseek(fp, 0, SEEK_END);
		lSize = ftell(fp);
		rewind(fp);

		buffer = (char*) malloc(sizeof(char) * lSize);
		if (buffer == NULL) {
			fputs("Memory error", stderr);
			exit(2);
		}

		result = fread(buffer, 1, lSize, fp);
		if (result != lSize) {
			fputs("Reading error", stderr);
			exit(3);
		}

		if (result > 0) {
			if(strstr(buffer,hash)){
				result_val = true;
			}else{
				result_val = false;
			}
		}
		fclose(fp);
		free(buffer);
	} else {
		return false;
	}
	return result_val;
}

void CheckModBinary(){
	pid_t pid= getpid();

	char sha256[65] = "";

	char packageName[MAX_PATH];
	char apkFilePath[MAX_PATH];
	char libFilePath[MAX_PATH];
	char tmpLibFilePath[MAX_PATH];

	int zipcount = 0;
	unsigned int crc;

	LOGD("Modification Check");

	getCmdline(pid,packageName,sizeof(packageName));
	getApkFilePath(packageName,apkFilePath);

	sprintf(libFilePath, "/data/%s-1/lib", packageName);

	while(1)
	{
		fileToSha256(sha256,apkFilePath);

		if(ModFindHash(sha256) == false)
		{
			LOGD("File Path : %s, SHA256 : %s - Modfication",apkFilePath,sha256);
			LOGD("Modfication.");
			sleep(10);
			exit(0);
		}

		DIR *dirInfo;
		struct dirent *dirEntry;

		dirInfo = opendir(libFilePath);
		dirEntry == readdir(dirInfo);
		if (NULL != dirInfo) {
			while (dirEntry)
			{
				sprintf(tmpLibFilePath, "/data/%s-1/lib/%s", packageName,dirEntry->d_name);

				fileToSha256(sha256,tmpLibFilePath);
				if (ModFindHash(sha256) == false) {
					LOGD(".SO File Hash Check - Modfication.");
					sleep(10);
					exit(0);
				}
			}
			closedir(dirInfo);
		}
		sleep(1000);
	}
}