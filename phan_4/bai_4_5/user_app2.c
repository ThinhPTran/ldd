#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
 
int8_t read_buf[1024];
int main()
{
        int fd;
 
        fd = open("/dev/char_device", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
 
	read(fd, read_buf, 1024);
	printf("tien trinh 2 doc tu kernel buffer cua char_driver: %s", read_buf);
	close(fd);
}
