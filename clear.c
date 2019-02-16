
#include "types.h"
#include "user.h"
int hi=0;
void clear(int x){
    // if(x=='x')return;
    // printf(1,"\xa");
    // clear(x+('1'-48));
    printf(1,"hello\n");
    printf(1,"%d\n",hi);
    hi = hi +1;
    printf(1,hi+"\n");
}
int main(void){
    printf(1, "called%d\n",hi);
    hi = hi+1;
    clear('A');exit();
}