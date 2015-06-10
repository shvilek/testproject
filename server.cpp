#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "PipeServer.h"
#include <iostream>

int main()
{
	std::cout << "input 'q' or 'Q' to exit server\r\n";
	PipeServer srv;
	srv.StartThread();
	char cmd = '\0';
	std::cin >> cmd;
	while (cmd != 'q' && cmd != 'Q') {
		std::cin >> cmd;
	}
    	srv.Stop();
	return 0;
}
