#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/* second_drv  
 */
int main(int argc, char ** argv)
{
	int fd;
	unsigned int keyval[4];
	int cnt = 1;

	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0)
		printf("can't open!\n");

	while(1)
	{
		read(fd, keyval, sizeof(keyval));
		if (!keyval[0] || !keyval[1] || !keyval[2] || !keyval[3])
			printf("%d keyvals: %d %d %d %d\n", cnt++, keyval[0], keyval[1], keyval[2], keyval[3]);
	}
	
	return 0;
}
