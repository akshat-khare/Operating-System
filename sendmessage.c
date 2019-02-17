#include "types.h"
#include "user.h"
#include "date.h"

int
main(int argc, char *argv[])
{
  int a=atoi(argv[1]);
  char* b=argv[2];
  int temp=sendmessage(a,b);	
  printf(1, "status is %d\n",temp);
  exit();
}
