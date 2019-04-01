#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/* second_drv  
 */
int main(int argc, char ** argv)
{
	int fd;
	unsigned char keyval;

	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0)
		printf("can't open!\n");

	while(1)
	{
		read(fd, &keyval, 1);
		printf("keyvals: 0x%x\n", keyval);
	}
	
	return 0;
}
