#include <stdio.h>
#include <string.h>

int main()
{
  int testArr[100];
  memset(testArr, -1, sizeof(testArr));
  int testArrLen = sizeof(testArr)/sizeof(testArr[0]);
  printf("%d\n", testArrLen);
  for(int i=0; i<testArrLen; i++)
  {
    printf("element %d: %d\n", i, testArr[i]);
  }

  /*printf("Order of Ops Test\n");*/
  /*int noGrouping = 4000 / 40 / 5; // should be 20*/
  /*int grouping = 4000 / (40 / 5); // should be 500*/
  /*printf("No grouping: %d\n", noGrouping);*/
  /*printf("Grouping: %d\n", grouping);*/
  
  /*int floor = 4/5; // should be 0*/
  /*printf("4 / 5 = %d\n", floor);*/
  /*return 0;*/
  
  /*int x = 10;*/
  /*x += 5;*/
  /*printf("%d\n", x);*/
}
