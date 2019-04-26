#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
char bufcat[512];

void
cat(int fd)
{
  int n;

  while((n = read(fd, bufcat, sizeof(bufcat))) > 0) {
    if (write(1, bufcat, n) != n) {
      printf(1, "cat: write error\n");
      exit();
    }
  }
  if(n < 0){
    printf(1, "cat: read error\n");
    exit();
  }
}

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf(1,"this is file\n");
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    printf(1,"this is dir\n");
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
    //   if(fmtname(buf)[0]=='R'){
    //       continue;
    //   }
      printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[]){
    // printf(1,"samplerun called %s\n",argv[1]);
    // if(atoi(argv[1])==1){
    //     // printf(1,"lol\n");
    //     exit();
    // }
    // // if(argv)
    // // samplecall();
    // // printf(1,"hi\n");
    // // printf(1,"malloc1\n");
    // // int temp = (int)malloc(sizeof(int));
    // // // int temp;
    // // temp=4;
    // // // int * temparr = (int *)
    // // printf(1,"malloc2\n");
    // // int temp1 = (int)malloc(sizeof(int));
    // // // int temp1;
    // // temp1=5;
    // // printf(1,"samplerun %p\n",(int *) &temp);
    // // printf(1,"samplerun %p\n",(int *) &temp1);
    // // printf(1,"%d\n",temp+temp1);
    // printf(1,"calling samplerun\n");
    // char*argcus[] = {"samplerun","1"};
    // int pid =fork();
    // if(pid==0){
    // exec("samplerun",argcus);

    // }else{

    // printf(1,"was this line executed\n");
    // exit();
    // }
    // printf(1,"size is %d\n",sizeof(int));
    // int sampleint =4;
    // int * samplearr = (int *)malloc(4*sizeof(int));
    // samplearr[0]=1;
    // samplearr[1]=2;
    // samplearr[2]=3;
    // samplearr[3]=4;
    // samplecall(&sampleint,samplearr);
    // // printf(1, "user numer is %d\n", sampleint);
    // printf(1,"user arr ele is %d\n",samplearr[3]);
    // samplearr[3]=7;
    // samplecall(&sampleint,samplearr);
    // printf(1,"user arr ele is %d\n",samplearr[3]);
    // int sampleint=4;
    // struct{
    //     char name[2];
    // } strme;
    // strme.name[0]='a';
    // strme.name[1]='b';
    // sampleint= sizeof(strme);
    // printf(1,"size is %d\n", sampleint);
    // printf(1, "str is %s\n",strme.name);
    // samplecall(&sampleint,(void *) &strme);
    // printf(1,"%d num\n",sampleint);
    // ls();

    // ls(".");

    int fd = open("lol",O_CREATE | O_RDWR);
    // char * lolstr = (char *)malloc(sizeof(char)*30);
    char lolstr[30];
    // printf(1, "writing %s\n", lolstr);
    write(fd, &lolstr, sizeof(char)*30);
    close(fd);
    fd = open("lol",O_RDONLY);
    cat(fd);
    close(fd);

    exit();

}