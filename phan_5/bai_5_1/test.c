#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>

typedef struct {
        int rd;
        int wr;
} status_t;

#define ENABLE 1
#define DISABLE 0
#define RESET_STATISTIC _IO(200, 0)
#define WR_CONTROL_DEV _IOW(200, 1, int32_t *)
#define RD_STATUS_DEV _IOR(200, 2, status_t *)

int8_t write_buf[1024];
int8_t read_buf[1024];
int transfer_data() {
        int fd = open("/dev/char_device", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }

        printf("Enter the string to write into driver :");
        scanf("  %[^\t\n]s", write_buf);
        if(write(fd, write_buf, strlen(write_buf)+1) == 0) return -1;
        printf("Data Reading ...");
        if(read(fd, read_buf, 1024) == 0) return -1;
        printf("Data = %s\n\n", read_buf);
        close(fd);
	return 0;
}

void control_char_device(int32_t control) {
	int fd = open("/dev/char_device", O_RDWR);
	if(fd < 0) {
                printf("Cannot open device file...\n");
                return;
        }
	ioctl(fd, WR_CONTROL_DEV, (int32_t*) &control);

	close(fd);
}

void read_status_char_device() {
	status_t status;
	int fd = open("/dev/char_device", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return;
        }
        ioctl(fd, RD_STATUS_DEV, (status_t*) &status);
	printf("status read (%d), write(%d)\n", status.rd, status.wr);

        close(fd);
}

void reset_statistic() {
        int fd = open("/dev/char_device", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return;
        }
        ioctl(fd, RESET_STATISTIC);

        close(fd);
}

int main()
{
	control_char_device(DISABLE);//the command will make disable data transfering
	if(transfer_data() < 0) printf("transfering data with kernel module has been disable\n");
	read_status_char_device();

	control_char_device(ENABLE);//enable transfering data again
	if(transfer_data() < 0) printf("a problem have occurred while transfering data with kernel module\n");//expect have data transfer
	read_status_char_device();

	reset_statistic();
	return 0;
}
