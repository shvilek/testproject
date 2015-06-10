#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <string>
#define MAX_BUF 1024

int main(int argc, char** argv)
{
    int fds, fdc;
    const char * myfifos = "/tmp/myfifo_server";
    const char * myfifoc = "/tmp/myfifo_client";
    char buf[MAX_BUF];
    std::string cmd_ = "";
    for (int i = 1; i < argc; ++i) {
	cmd_ += argv[i];
	if  (i+1 < argc) cmd_ += ' ';
    }
    if (cmd_.empty()) return 0;
    //const char* cmd = argv[1];
    //const char* arg = argv[2];
    
    /* open, read, and display the message from the FIFO */

    fds = open(myfifos, O_RDONLY);
    fdc = open(myfifoc, O_WRONLY);

    
    int r = flock(fdc, LOCK_EX);
    if (r != 0) return r;
    //sprintf(buf, "%s %s\n", cmd, arg);
    write(fdc, cmd_.c_str(), cmd_.size());
    int count = read(fds, buf, MAX_BUF);
    buf[count] = '\0';
    printf("Received: %s\n", buf);
    close(fdc);
    close(fds);
    return 0;
}
