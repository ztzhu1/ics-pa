#include <stdio.h>
#include <sys/time.h>
#include <NDL.h>
#include <assert.h>

int main() {
  int milisec = 500;
  NDL_Init(0);
  for (int i = 0; i < 10; i++) {
    while(NDL_GetTicks() / 1000 < milisec) ;
    printf("%d mili seconds.\n", milisec);
    milisec += 500;
  }

  NDL_Quit();
  printf("\033[32mPASS!!!\033[0m\n");

  return 0;
}
