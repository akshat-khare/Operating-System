#include "param.h"
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;
extern int togglestate;
#define MESSAGESIZE  9      //size of message buffer
#define MSGSIZE 8
#define MAXUSERCONTAINERPROCESS 5
#define SAFESLEEP 20000
typedef int MessageBuffer[MESSAGESIZE];
extern MessageBuffer message_buffer[NUMBEROFMESSAGEBUFFERS];
extern int message_queue[NPROC][NUMBEROFMESSAGEBUFFERS];
extern int free_message_buffer;
extern int message_queue_head[NPROC];
extern int message_queue_tail[NPROC];
extern int sigchild[NPROC];
extern MessageBuffer multibuffer;
extern int boolwritewalkpage;
