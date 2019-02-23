#include "types.h"
#include "user.h"
#include "date.h"

int
main(int argc, char *argv[])
{
  int a=atoi(argv[1]);
  int b=atoi(argv[2]);
  char* c=argv[3];
  int temp=send(a,b,c);	
  printf(1, "status is %d\n",temp);
  exit();
}
