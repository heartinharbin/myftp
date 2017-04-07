#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	int fd = open("file1", O_RDWR);
	struct stat buf;
	//stat("file1", &buf);
	fstat(fd, &buf);
	printf("size:%ld\n", buf.st_size);
}
