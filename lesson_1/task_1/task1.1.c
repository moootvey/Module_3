#include "library.h"
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

void main(int argc, char *argv[], char *envp[]) {
    pid_t pid = getpid(); //Вывод можно осуществить через %d
    pid_t ppid = getppid();
    uid_t uid = getuid();
    uid_t euid = getegid();
    gid_t gid = getgid();
    pid_t sid = getsid(pid);

    printf("%d\n", pid);
    printf("%d\n", ppid);
    printf("%d\n", uid);
    printf("%d\n", euid);
    printf("%d\n", gid);
    printf("%d\n", sid);
}