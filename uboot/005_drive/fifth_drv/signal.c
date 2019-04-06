#include <stdio.h>
#include <signal.h>

void my_signal_func(int signum)
{
	static int cnt = 0;
	printf("signal = %d, %d times\n", signum, cnt++);
}

int main(void)
{
	signal(SIGUSR1, my_signal_func);

	while (1)
	{
		sleep(1000);
	}
	
	return 0;
}