#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
 
int8_t write_buf[1024]="application 1 hello world\n";
int main()
{
        int fd;
 
        fd = open("/dev/char_device", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
 
	printf("tien trinh 1 ghi vao kernel buffer cua char_driver: %s", write_buf);
	write(fd, write_buf, strlen(write_buf)+1);
	close(fd);
}
