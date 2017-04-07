#include "func.h"
void  myls(char *lspath, char *retbuf){
	DIR *pd;
	char tmppath[512];
	if(NULL == lspath){
		pd = opendir(".");
		strcpy(tmppath, ".");
	}else{
		pd = opendir(lspath);
		strcpy(tmppath, lspath);
	}
	if(NULL == pd){
		perror("opendir");
		return;
	}
	struct tm *ptminfo;
//	char timebuf[50];
	char chmodsets[8][4] = {"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
	char filetype_sets[16] = {' ',' ',' ',' ','d',' ',' ',' ','-',' ',' ',' ',' ',' ',' ',' '};
	char tmp[128];
	int i, filetype;
	unsigned int temp;
	struct dirent *p;
	off_t position;
	int	ret;
	struct stat buf;
	char path[512] = {0};
	while((p = readdir(pd))!= NULL){
		if(strcmp(p->d_name,".")==0 ||strcmp(p->d_name,"..")==0){
			continue;
		}
		sprintf(path, "%s%s%s",tmppath,"/",p->d_name);
		ret = stat(path, &buf);
		if(ret!=0){
			perror("stat");
			return;
		}
		filetype = buf.st_mode>>12 & 15;
//		printf("%c", filetype_sets[filetype]);
//		for(i = 2; i >= 0; i--){
//			temp = buf.st_mode>>i*3  & 7;
//			printf("%s",chmodsets[temp]);
//		}
		
//		ptminfo = localtime(&buf.st_mtime); 
//		strftime(timebuf, sizeof(timebuf), "%b %d %H:%M",ptminfo);
//		printf("%3ld %s %s %8ld %s %-s\n",buf.st_nlink,getpwuid(buf.st_uid)->pw_name,getgrgid(buf.st_gid)->gr_name,buf.st_size,timebuf,p->d_name);
		bzero(tmp, sizeof(tmp));
		sprintf(tmp, "%c %s\n", filetype_sets[filetype], p->d_name);
		strcat(retbuf, tmp);
	}
	closedir(pd);
	return;
}
/*
int main(int argc, char* args[]){
	char buf[1000] = {0};
	myls(args[1], buf);
	printf("buf\n%s", buf);
}
*/
