#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include<wait.h>

#define SIGMAX 32
#define MAXFORK 5

// Async safe functions & ways to raise signals can be found here:
// http://manpages.ubuntu.com/manpages/trusty/man7/signal.7.html


void alarm_handler(int code){
	write(STDOUT_FILENO, "Caught alarm()\n", 14);
}

void fork_others(int n)
{
	int pid = fork();
	int status;
	if (pid == 0)
	{
		if (n < MAXFORK)
			fork_others(n + 1);
		return;
	}
	else
	{
		if (n == 0)
		{
			write(STDOUT_FILENO, "sleep()\n", 8);
			sleep(1);
		}
		else if (n == 1)
		{
			write(STDOUT_FILENO, "abort()\n", 8);
			abort();
		}
		else if (n == 2)
		{
			write(STDOUT_FILENO, "wait()\n", 7);
			wait(0);
		}
		else if (n == 3)
		{
			write(STDOUT_FILENO, "alarm()\n", 8);
  			alarm(0);  // Set alarm in 2 seconds
  			sleep(0.1); // Wait for the alarm to set
		}
		else if (n == 4)
		{
			int idk = fork(); // Create subprocess we want to actually kill
			if (idk == 0)
			{
				sleep(10); // Set to sleep 5 seconds
				exit(0);
			}
			else
			{
				write(STDOUT_FILENO, "kill()\n", 7);
  				kill(idk, 1);
  			}
		}
		else if (n == 5)
		{
			//write(STDOUT_FILENO, "sigvec()\n", 10);
			//sigvec(); // Didn't work!!!
			write(STDOUT_FILENO, "segmentation fault - illegal acess [NULL Pointer] (signal 11)\n", 50);
			// Other ways to mimic it can be found at: https://www.tutorialspoint.com/List-of-Common-Reasons-for-Segmentation-Faults-in-C-Cplusplus
			char *p = NULL; // Creating segmentation error
			*p = 2;
		}
		else
		{
			char buffer[80]; // https://stackoverflow.com/a/58259970
			int LengthUsed = sprintf(buffer, "At %d | Not set to raise\n", n);  // Format n in decimal.
			write(STDOUT_FILENO, buffer, LengthUsed); 
		}
		if (n == 0)
			write(STDOUT_FILENO, "waitpid()\n", 10);
		waitpid(pid, &status, 0);
	}
}

void handler(int i) {
	char buffer[80]; // https://stackoverflow.com/a/58259970
	int LengthUsed = sprintf(buffer, "\nCought signal %d\n", i);  // Format n in decimal.
	write(STDOUT_FILENO, buffer, LengthUsed); 
	if (i == 1 || i == 8 || i == 18)
		i++; // 9 is kill
	if (i < 31)
	{	
		LengthUsed = sprintf(buffer, "Raising signal %d\n", (i+1)%SIGMAX);  // Format n in decimal.
		write(STDOUT_FILENO, buffer, LengthUsed); 
		raise(i+1);
	}
	else
	{
		// Now forking for other signal calls
		fork_others(0);
	}
}


void ctrl_c(int code) {
	write(STDOUT_FILENO, "\nCaught ^C\n", 11);
	exit(0);
}

void exit_handler(void) {
	write(STDOUT_FILENO, "\nCaught exit()\n", 15);
}

int main() {
	struct sigaction sa;
	sa.sa_handler = handler;
	for (int i = 1; i <= SIGMAX; i++) {
		sigaction(i, &sa, NULL);
	}
	signal(SIGINT, ctrl_c); // Use this handler for exit | sigaction and signal are about the same so I consider it as one method.
	atexit(exit_handler);
	
	raise(1); // Raising 1
	write(STDOUT_FILENO, "\npause()\n", 9);
    	pause(); // wait for signal BY USING A SIGNAL
    	return 0;
}
