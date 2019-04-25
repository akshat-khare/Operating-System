#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define MAXSTRLEN 30
// #define MAXCONTFILE 40

char bufcat[512];
char realname[MAXFILE][MAXSTRLEN];
char virtualname[MAXFILE][MAXSTRLEN];
int mappings[MAXFILE];

int pointer = 0;

char* 
itoa(int num, int* digits) 
{
	// Convert number to string
	char *sum = (char *)malloc(MSGSIZE);
	int tsum = num, ch = -1;
	while(tsum > 0) {
		ch++;
		tsum = tsum / 10;
	}
    *digits = ch+1;
	tsum = num;
	while(tsum > 0) {
		int dig = tsum % 10;
		sum[ch--] = dig + '0';
		tsum = tsum / 10;
	}
	return sum;
}

char*
get_filename(char* bufcharme, int cid) {
    int num_digits = 0; 
    char * cidstr = itoa(cid, &num_digits);
    char * actualfilename = (char *)malloc(40*sizeof(char));
    actualfilename[0] = '#';
    for (int i = 0; i < num_digits; i++) {
        actualfilename[i+1] = cidstr[i];
    }
    actualfilename[num_digits+1] = '#';
    for (int i = 0; i < 30; i++) {
        actualfilename[num_digits+i+2] = bufcharme[i];
    }
    printf(1,"Actual file %s\n", actualfilename);
    return actualfilename;
}

int
get_index(char* name) {
    int index = -1;
    for (int i = 0; i < MAXFILE; i++) {
        if (strcmp(virtualname[i], name) == 0) {
            index = i;
            break;
        }
    }
    return index;
}

void
move_ptr() {
    int flag = 0;
    for (int i = pointer+1; i != pointer; i = (i+1)%MAXFILE) {
        if (mappings[i] == 0) {
            pointer = i;
            flag = 1;
            break;
        }
    }
    if (flag == 0)
        pointer = -1;
}

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
int maxprocess=5;
#define HEAPSIZEMAX 300
// #define MAXSTRLEN 30
// #define MAXFILE 40
// char realname[MAXFILE][MAXSTRLEN];
// char virtualname[MAXFILE][MAXSTRLEN];

void schedulercustom(int cid){
    int numprocess=0;
    int * processstate=(int *)malloc(MAXUSERCONTAINERPROCESS*sizeof(int));
    int * mallocheap=(int *)malloc(sizeof(int)*HEAPSIZEMAX);
    int heapfreeptr=0;
    int * sleepschedule=(int *)malloc(MAXUSERCONTAINERPROCESS*sizeof(int));
    char * bufcharme =(char *)malloc(30*sizeof(char));
    int fdarg=-1;
    int count=0;
    int mallocaddr=0;
    int mallocbuf=0;
    int togglelog=0;
    // int numprocess=0;
    // int * statusprocess=(int *)malloc(maxprocess*sizeof(int));
    // samplecall(&numprocess,&statusprocess);
    // int repeat=-1;
    int containerjustcalled=1;
    int * syscallping = (int *)malloc(MAXUSERCONTAINERPROCESS*sizeof(int));
    int typesyscall = -1;
    int issyscalldone=0;
    int whichchildsyscalled=-1;
    for(;;){
        count++;
        // ps();
        if(count%200==0){
            // printf(1,"&");
            // ps();
            int res = registerState(1,&numprocess,processstate,sleepschedule,&containerjustcalled, syscallping, &typesyscall,bufcharme,&fdarg, &mallocaddr, &mallocbuf, &togglelog);
            // printf(1,"^");
            if(res!=0){
                printf(1,"error");
            }
            count=0;
        }else{
            continue;
        }
        // printf(1,"scheduler\n");
        // while
        // printf(1,"numprocess are %d\n",numprocess);
        // for(int i=0;i<numprocess;i++){
        //     printf(1, "state %d sleep %d\n",processstate[i],sleepschedule[i]);
        // }
        issyscalldone=0;
        whichchildsyscalled=-1;
        for(int i=0;i<numprocess;i++){
            if(syscallping[i]==1){
                issyscalldone=1;
                whichchildsyscalled=i;
                break;
            }
        }
        if(issyscalldone==1){
            // printf(1,"syscall by %d of type %d\n",whichchildsyscalled, typesyscall);
            if(typesyscall==PS){
                //ps
                ps();
            }else if(typesyscall==CREATE){
                printf(1,"create encountered %s\n",bufcharme);
                fdarg=0;
                char* actualname = get_filename(bufcharme, cid);
                strcpy(realname[pointer], actualname);
                strcpy(virtualname[pointer], bufcharme);
                printf(1, "Creating real filename %s\n", actualname);
                mappings[pointer] = open(actualname, O_CREATE | O_RDWR);
                fdarg = pointer;
                printf(1, "Real address: %d and returning virtual address %d\n", mappings[pointer], fdarg);
                move_ptr();
            }else if(typesyscall==OPEN){
                printf(1,"open encountered %s\n",bufcharme);
                fdarg=0;
                int index = get_index(bufcharme);
                if (index == -1) {
                    // File not open yet
                    printf(1, "Opening real filename %s\n", bufcharme);
                    int afid = open(bufcharme, fdarg);
                    mappings[pointer] = afid;
                    strcpy(virtualname[pointer], bufcharme);
                    strcpy(realname[pointer], bufcharme);
                    fdarg = pointer;
                    // Move pointer to next empty space
                    move_ptr();
                } else {
                    // File already open
                    fdarg = index;
                }
                printf(1, "Returning virtual address %d\n", fdarg);
            }else if(typesyscall==WRITE){
                printf(1,"write encountered %s\n",bufcharme);
                if (strcmp(virtualname[fdarg], realname[fdarg]) == 0) {
                    // It was an existing file. Make new file (COW)
                    char* actualname = get_filename(virtualname[fdarg], cid);
                    strcpy(realname[fdarg], actualname);
                    int cow_fid = dup(mappings[fdarg]);
                    // Copied (duplicated) the file. Now write
                    printf(1, "Duplicating : Writing into file at id %d\n", cow_fid);
                    write(cow_fid, bufcharme, 30*sizeof(char));
                    mappings[fdarg] = cow_fid;
                } else {
                    int actualfid = mappings[fdarg];
                    printf(1, "Writing into file at id %d\n", actualfid);
                    write(actualfid, bufcharme, 30*sizeof(char));
                }
            }else if(typesyscall==CAT){
                printf(1,"cat encountered %s\n",bufcharme);
                char* actualname = get_filename(bufcharme, cid);
                int fdr = open(actualname,O_RDONLY);
                cat(fdr);
                printf(1, "\n");
                
            }else if(typesyscall==LS){
                ls(".");
       
            }else if(typesyscall==MALLOC){
                // printf(1,"malloc encountered %d\n",mallocbuf);

                mallocaddr=heapfreeptr;
                heapfreeptr =heapfreeptr+mallocbuf;
            }else if(typesyscall==MALLOC_WRITE){
                // printf(1,"malloc write encountered %d %d\n",mallocaddr,mallocbuf);
                mallocheap[mallocaddr]=mallocbuf;
            }else if(typesyscall==MALLOC_READ){
                // printf(1,"malloc read encountered %d\n",mallocaddr);
                mallocbuf=mallocheap[mallocaddr];

            }else if(typesyscall==CLOSE){
                printf(1,"close encountered %d, closing %d\n", fdarg, mappings[fdarg]);
                close(mappings[fdarg]);
                mappings[fdarg] = 0;
                strcpy(realname[fdarg], "#nofilehere#");
                strcpy(virtualname[fdarg], "#nofilehere#");
            }
            syscallping[whichchildsyscalled]=2;
            typesyscall=-1;
            registerState(2,&numprocess,processstate,sleepschedule,&containerjustcalled, syscallping, &typesyscall,bufcharme,&fdarg,&mallocaddr,&mallocbuf, &togglelog);
   
  
            // containerjustcalled=0;
        }else{
            if(containerjustcalled==0){
                // sleep(2);
                continue;
            }
            // printf(1,"*");
            if(numprocess==0){
                // do nothing maybe
            }else if(numprocess==1){
                if(processstate[0]==2 || sleepschedule[0]==1){
                    printf(1, "Container %d : Scheduling its process number %d\n",cid,0);
                    sleepschedule[0]=0;
                }
            }else{
                int startset=0;
                int schedset=0;
                int ptobeslept=-1;
                int ptobewaken=-1;
                for (int i=0;i<numprocess;i++){
                    if(startset==0){
                        if((processstate[i]==3 || processstate[i]==4) && sleepschedule[i]==0){
                            ptobeslept=i;
                            startset=1;
                        }
                    }else if(startset==1){
                        if(processstate[i]==2 || sleepschedule[i]==1){
                            ptobewaken = i;
                            schedset=1;
                            break;
                        } 
                    }
                }
                if(startset==1 && schedset==0){
                    if(ptobeslept==numprocess-1){
                        if(processstate[0]==2 || sleepschedule[0]==1){

                            ptobewaken=0;
                            schedset=1;
                        }else{
                            //idk
                        }
                    }else{
                        //just sleep the waken up
                    }
                }
                if(startset==0){
                    ptobewaken=0;
                    schedset=1;
                }
                if(schedset==1){
                    if(togglelog==1){
                        printf(1, "Container %d : Scheduling %d\n",cid,ptobewaken);
                    }
                    sleepschedule[ptobewaken]=0;
                }
                if(startset==1){
                    sleepschedule[ptobeslept]=1;
                    // repeat = ptobeslept;
                }


            }

            // scheduler_call();
            // printf(1, "size is %d\n")
            // count++;
            // if(count>100000){
            //     ps();
            // }
            // sleepschedule[0]=1;
            // sleepschedule[1]=0;
            containerjustcalled=0;
            // printf(1,"decided\n");
            // for(int i=0;i<numprocess;i++){
            //     printf(1,"%d ",sleepschedule[i]);
            // }
            // printf(1,"\n");
            registerState(2,&numprocess,processstate,sleepschedule,&containerjustcalled, syscallping, &typesyscall, bufcharme,&fdarg,&mallocaddr,&mallocbuf,&togglelog);
        }

    }
}

int main(void){
    int pid;
    int numcontainer=1;
    pid =fork();
    if(pid==0){
        create_container(numcontainer);
        schedulercustom(numcontainer);
    }else{
        numcontainer++;

    }
    pid =fork();
    if(pid==0){
        create_container(numcontainer);
        schedulercustom(numcontainer);
    }else{
        numcontainer++;
    }
    pid =fork();
    if(pid==0){
        create_container(numcontainer);
        schedulercustom(numcontainer);
    }else{
        numcontainer++;
    }
    sleep(20);
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(1);
        // printf(1,"----------joined container-------\n");
        sleep(20);
        registerSysCall(PS);
        int waitps=-1;
        while(waitps==-1){
            waitps=getStatusSysCall();
        }
        ls_sys();

        char * buftempproc=(char*)malloc(30*sizeof(char));
        strcpy(buftempproc,"file_");
        // printf(1,"doing cat \n");
        int fd = open("myfile",O_CREATE | O_RDWR);
        // write(0,buftempproc,30);
        // cat_sys(buftempproc);
        // printf(1,"write cat registered\n");
        int waittemp = -1;
        while(waittemp==-1){
            waittemp=getStatusSysCall();
        }
        printf(1,"done waiting for answer\n");
        getfd(&fd);
        printf(1,"fd found %d\n",fd);
        char mywrite[30];
        strcpy(mywrite,"hi\0");
        write(fd,&mywrite,sizeof(char)*30);
        waittemp=-1;
        while(waittemp==-1){
            waittemp=getStatusSysCall();
        }
        cat_sys("myfile");
        waittemp=-1;
        while(waittemp==-1){
            waittemp=getStatusSysCall();
        }
        close(fd);
        waittemp=-1;
        while(waittemp==-1){
            waittemp=getStatusSysCall();
        }
        for(;;){
            // printf(1,"$1_1$\n");
            // count++;
            // if(count%500==0){
            //     printf(1, "doing sys\n");
            //     registerSysCall(1);
            //     int waittemp = -1;
            //     while(waittemp==-1){
            //         printf(1,".");
            //         waittemp=getStatusSysCall();
            //     }
            // //     leave_container();
            // //     exit();
            // }
        }
    }
    
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(1);
        // printf(1,"joined container\n");
        sleep(2);
        // int count =0;
        for(;;){
            // printf(1,"$1_2$\n");
            // count++;
            // if(count>2000){
            //     leave_container();
            //     destroy_container(1);
            //     ps();
            //     sleep(200);
            //     ps();
            //     exit();
            // }
            // if(count%500==0){
            //     printf(1, "doing sys\n");
            // registerSysCall(1);
            // int waittemp = -1;
            // while(waittemp==-1){
            //     printf(1,".");
            //     waittemp=getStatusSysCall();
            // }
            // //     leave_container();
            // //     exit();
            // }
        }
    }
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(1);
        // printf(1,"joined container\n");
        sleep(2);
        // int count =0;
        for(;;){
            // printf(1,"$1_3$\n");
            // count++;
            // if(count%500==0){
            //     printf(1, "doing sys\n");
            //     registerSysCall(1);
            //     int waittemp = -1;
            //     while(waittemp==-1){
            //         printf(1,".");
            //         waittemp=getStatusSysCall();
            //     }
            // //     leave_container();
            // //     exit();
            // }
        }
    }
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(2);
        // printf(1,"joined container\n");
        sleep(2);
        // int count =0;
        for(;;){
            // printf(1,"$2_1$\n");
            // count++;
            // if(count%500==0){
            //     printf(1, "doing sys\n");
            //     registerSysCall(1);
            //     int waittemp = -1;
            //     while(waittemp==-1){
            //         printf(1,".");
            //         waittemp=getStatusSysCall();
            //     }
            // //     leave_container();
            // //     exit();
            // }
        }
    }
    pid = fork();
    if(pid==0){
        // printf(1,"trying joining\n");
        join_container(3);
        // printf(1,"joined container\n");
        sleep(2);
        // int count =0;
        for(;;){
            // printf(1,"$3_1$\n");
            // count++;
            // if(count%500==0){
            //     printf(1, "doing sys\n");
            //     registerSysCall(1);
            //     int waittemp = -1;
            //     while(waittemp==-1){
            //         printf(1,".");
            //         waittemp=getStatusSysCall();
            //     }
            // //     leave_container();
            // //     exit();
            // }
        }
    }
    sleep(20);
    int tmplog=1;
    toggle_log(&tmplog);
    sleep(400);
    tmplog=0;
    toggle_log(&tmplog);
    for(;;){

    }
    // sleep(1000);
    exit();

}