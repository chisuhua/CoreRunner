#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int host_fd = open("/tmp/testfile", O_CREAT | O_RDWR, 0644);
    if (host_fd < 0 ) perror("open host file failed");
}
