#include "types.h"
#include "user.h"
#include "date.h"

int
main(int argc, char *argv[])
{
  // int a=atoi(argv[1]);
  char* putinthis=(char *)malloc(8);
  int temp=recv(putinthis);
//   printf(1,"answer is \n");	
//   for(int i=0;i<8;i++){
//       printf(1,"%s", (char *) (putinthis[i]+'0'));
//   }
//   printf(1,"\n");
  printf(1, "status is %d str is %s\n",temp,putinthis);
  exit();
}
