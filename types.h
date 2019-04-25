#include "param.h"
#define MESSAGESIZE  9      //size of message buffer
#define MSGSIZE 8
#define MAXUSERCONTAINERPROCESS 5
#define SAFESLEEP 200

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

extern int togglestate;
typedef int MessageBuffer[MESSAGESIZE];
extern MessageBuffer message_buffer[NUMBEROFMESSAGEBUFFERS];
extern int message_queue[NPROC][NUMBEROFMESSAGEBUFFERS];
extern int free_message_buffer;
extern int message_queue_head[NPROC];
extern int message_queue_tail[NPROC];
extern int sigchild[NPROC];
extern MessageBuffer multibuffer;
extern int boolwritewalkpage;

struct args {
  int sender_pid;
  int call_type;
  int num_int_args;  
  int num_str_args;  
  int integers[MAXARGUMENTS];
  char strings[MAXARGUMENTS][MAXWRITECHARS];
};