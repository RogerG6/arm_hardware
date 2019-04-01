#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <linux/poll.h>

/* forth_drv  
 */
int main(int argc, char ** argv)
{
	int fd;
	unsigned char keyval;
	int ret;

	struct pollfd fds[1];	/* poll可以查询多个文件 */

	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0)
		printf("can't open!\n");

	fds[0].fd     = fd;		
	fds[0].events = POLLIN;
	while(1)
	{
		ret = poll(fds, 1, 5000);	/* 查询1个文件，超时时间5000ms */
		if (ret == 0)
			printf("Time out\n");
		else
		{
			read(fd, &keyval, 1);
			printf("keyvals: 0x%x\n", keyval);
		}
	}
	
	return 0;
}
