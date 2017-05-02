//
// Created by Administrator on 2017/4/27 0027.
//

#include "CheckDebug.h"

#define MAX 128
#define CHECK_TIME 10
#define T_NUMS 5

void CheckDbg(){
    LOGD("Just for call CheckDbg.\n");

    int t = 1;

    if( t > 0)
    {
        soData();
     }

     while(t < 2)
     {
        //GetAn();
        GetT();
        t++;
     }
}

void badData()
{
    int  pid;
    FILE *fd;
    char fileName[MAX];
    char line[MAX];
    pid = getpid();
    sprintf(fileName, "/proc/%d/status", pid);

    if(fork() == 0)
    {
        fd = fopen(fileName, "r");
        while(fgets(line, MAX, fd)) {
            if(strncmp(line, "TracerPid", 9) == 0)
            {
                LOGD("Line is %s\n", line);
                int statue = atoi(&line[10]);
                LOGD("@@@@ status = %d, %s \n", statue, line);
                fclose(fd);
                if (statue != 0) {
                    LOGD(" @@@@@ junk data here.\n");
                    int ret = kill(pid, SIGKILL);
                    LOGD(" @@@@@@@@@@@ kill-self..\n");
                    return;
                }
                break;
            }
        }
    }
}

void soData() {
    int pid;
    FILE *fd;
    int t;
    char fileName[MAX];
    char line[MAX];
    pid = getpid();

    if(fork() == 0)
    {
        xxoo01(); //ptrace self

        while(true) {
            fd = fopen(fileName, "r");
            while(fgets(line, MAX, fd)) {
                if (strncmp(line, "TracerPid", 9) == 0)
                {
                    LOGD("line is %s.\n", line);
                    int statue = atoi(&line[10]);
                    LOGD("@@@@@ status = %d, %s.\n", statue, line);
                    fclose(fd);
                    if(statue != 0)
                    {
                        LOGD("@@@@@@@ here.\n");
                        badData();
                        return;
                    }
                    break;
                }
            }

            sleep(CHECK_TIME);
        }
    }
}

bool getSo() {
    char libName[] = "libghelper.so";
    char buffer[4096] = {0};
    char *temp;
    int pid;
    FILE *fp;

    pid = getpid();
    sprintf(buffer, "/proc/%d/maps", pid);
    fp = fopen(buffer, "r");
    if (fp == NULL) {
        LOGD("Open maps file failed.\n");
        fclose(fp);
        return false;
    }

    while(fgets(buffer, sizeof(buffer), fp))
    {
        if(strstr(buffer, libName))
        {
            fclose(fp);
            return true;
        }
    }
}

unsigned long getSoBase() {
    unsigned long ret = 0;
    char libName[] = "libptrace.so";
    char buf[4096], *tmp;
    int pid;
    FILE *fp;
    pid = getpid();
    sprintf(buf, "/proc/%d/maps", pid);
    fp = fopen(buf, "r");
    if(fp == NULL) {
        puts("Open failed!\n");
        goto _error;
    }

    while(fgets(buf, sizeof(buf), fp)) {
        if(strstr(buf, libName)) {
            tmp = strtok(buf, "-");
            ret = strtoul(tmp, NULL, 16);
            break;
        }
    }
    _error: fclose(fp);

    return ret;
}

void xxoo01()
{
    ptrace(PTRACE_TRACEME, 0, 0, 0);
}

void GetAn()
{
    int pid1 = getpid();
    Elf32_Ehdr *elfhdr;
    Elf32_Phdr *pht;

    unsigned int size, base, offset, phtable;
    int n, i, j;
    char *p;

    // 从maps中读取elf文件在内存中的起始地址
    base = getSoBase();
    if (base == 0)
    {
        return;
    }

    elfhdr = (Elf32_Ehdr *)base;
    phtable = elfhdr->e_phoff + base;
    for(i = 0; i < elfhdr->e_phnum; i++) {
        pht = (Elf32_Phdr*)(phtable + i * sizeof(Elf32_Phdr));
        if(pht->p_flags & 1) {
            offset = pht->p_vaddr + base + sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr)*elfhdr->e_phnum;

            p = (char *)offset;
            size = pht->p_memsz;
            for(j = 0, n = 0; j < size; ++j, ++p) {
                if( *p == 0x10 && *(p+1) == 0xde){
                    n++;
                    int ret1 = kill(pid1, SIGKILL);
                    break;
                } else if( *p == 0xf0 && *(p+1) == 0xf7 && *(p+2) == 0x00 && *(p+3) == 0xa0) {
                    n++;
                    int ret2 = kill(pid1, SIGKILL);
                     break;
                } else if(*p = 0x01 && *(p+1) == 0x00 && *(p+2) == 0x9f && *(p+3) == 0xef) {
                    n++;
                    int ret3 = kill(pid1, SIGKILL);
                    break;
                }
            }
        }
    }
}

// 通过inotify特性来对文件的读写以及打开等权限进行监控
void GetT() {
    int ret, len, i;
    int pid = getpid();
    const int MAX_LEN = 2048;
    char buf[MAX_LEN];
    char readbuf[MAX_LEN];
    int fd, wd;
    fd_set readfds;
    fd = inotify_init();
    sprintf(buf, "/proc/%d/maps", pid);
    wd = inotify_add_watch(fd, buf, IN_ALL_EVENTS);
    if(wd > 0) {
        while(1)
        {
            i = 0;
            FD_ZERO(&readfds); //清零
            FD_SET(fd, &readfds); // 添加fd到readfds集合
            ret = select(fd + 1, &readfds, 0, 0, 0);
            if(ret == -1)
            {
                break;
            }

            if(ret) {
                len = read(fd, readbuf, MAX_LEN);
                while(i < len) {
                    struct inotify_event *event = (struct inotify_event *)&readbuf[i];
                    if((event->mask & IN_ACCESS) || (event->mask & IN_OPEN)){
                        badData();
                        return;
                    }
                    i += sizeof(struct inotify_event) + event->len;
                }
            }
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}