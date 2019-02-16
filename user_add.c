#include "types.h"
#include "user.h"
#include "date.h"

int
main(int argc, char *argv[])
{
  int a=atoi(argv[1]);
  int b=atoi(argv[2]);
  int temp=add(a,b);	
  printf(1, "answer is %d\n",temp);
  exit();
}
