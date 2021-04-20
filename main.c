#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>  /* 错误号定义 */

#include "analysis_data.h"
  
#define DEV_NAME   "/dev/ttyS5"

#define FALSE -1
#define TRUE 0

unsigned char g_protocol_data[] =
{
    0x59, 0x53, 0xBC, 0x93, 0x80, 0x10, 0x0C, 0x65, 0x07, 0xFF, 0xFF, 0x7B, 0xFD, 0x00, 0x00, 0x41, 0xFB, 0x94, 0x00, 0x20, 0x0C, 0x67, 0xDE, 0x01, 0x00, 0x2A, 0x3E, 0x00,
    0x00, 0x01, 0xE2, 0xFF, 0xFF, 0x30, 0x0C, 0xDC, 0xA7, 0xC8, 0xEF, 0xA8, 0x72, 0x4D, 0x0B, 0x18, 0xCD, 0x4C, 0xF1, 0x31, 0x0C, 0x42, 0xD9, 0xFB, 0xFF, 0xB9, 0xE4, 0x02,
    0x00, 0x9F, 0x3C, 0xFC, 0xFF, 0x40, 0x0C, 0x9C, 0x18, 0x16, 0x00, 0x69, 0x9F, 0xF8, 0xFF, 0x61, 0x1E, 0x40, 0x00, 0x41, 0x10, 0x45, 0x3F, 0x0F, 0x00, 0xB9, 0xED, 0xFF,
    0xFF, 0xB9, 0x30, 0x00, 0x00, 0x67, 0x8F, 0x00, 0x00, 0x60, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x98, 0xBD, 0xFF, 0xFF, 0x70, 0x0C, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x04, 0x7C, 0x2A, 0x76, 0x2D, 0x52, 0x04, 0xEC, 0x31, 0x76, 0x2D, 0x53, 0x54
};

int set_interface_attribs(int fd, unsigned int speed)
{
	struct termios tty;

	if (tcgetattr(fd, &tty) < 0) {
        	printf("Error from tcgetattr: %s\n", strerror(errno));
        	return FALSE;
    	}

    	cfsetospeed(&tty, B460800);
    	cfsetispeed(&tty, B460800);

    	tty.c_cflag |= (CLOCAL|CREAD);    /* ignore modem controls */
    	tty.c_cflag &= ~CSIZE;
    	tty.c_cflag |= CS8;         /* 8-bit characters */
    	tty.c_cflag &= ~PARENB;     /* no parity bit */
    	tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    	tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    	/* setup for non-canonical mode */
    	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    	tty.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    	tty.c_oflag &= ~OPOST;

    	/* fetch bytes as they become available */
    	tty.c_cc[VMIN] = 1;
    	tty.c_cc[VTIME] = 1;

	/* 如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读 */
	tcflush(fd,TCIFLUSH);

    	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        	printf("Error from tcsetattr: %s\n", strerror(errno));
        	return FALSE;
    	}

    	return TRUE;
}

int main(int argc, char *argv[])
{
	int fd;
	int i;
	int ret;
	unsigned int len;
	unsigned char recv_buf[1024];

	printf("Read Yesense data\n");

#if 1
	fd = open(DEV_NAME, O_RDWR|O_NOCTTY|O_SYNC);
	if(fd < 0)
	{
		printf("Error open %s: %s\n", DEV_NAME, strerror(errno));
		return -1;
	}

	do
	{
   		/* baudrate , 8 bits, no parity, 1 stop bit */
    		ret = set_interface_attribs(fd, 460800);
		printf("SET PORT EXACTLY \n");
		sleep(1);
	}while(FALSE == ret || FALSE == fd);
#else

	ret = analysis_data(g_protocol_data, sizeof(g_protocol_data));
	if(ret != analysis_ok)
	{
		printf("analysis_data FAILED!!! %d\r\n", ret);
	}
	else
	{
             //TODO
	     printf("publish imu INFO\r\n");
	}
	

#endif
	while(1) /* 循环读取数据 */
	{
		
		len = read(fd, recv_buf, 19);
		printf("len is %d\n", len);
		sleep(1);
		if(len < 0) printf("read data error\n");
		for(i = 0; i < len; i++)
		{
			printf("0x%X ", recv_buf[i]);
		}
	}

	printf("finished \n");

	return TRUE;
}
