#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

char getTemp(int wChar);
char myTime1;
char myTime2;
char myTime3;
char myTime4;

int main( int argc, char *argv[] )
{
while(1)
{
 myTime1 = getTime(12);
 myTime2 = getTime(13);
 myTime3 = getTime(15);
 myTime4 = getTime(16);
 printf("%c%c:%c%c /n",myTime1,myTime2,myTime3,myTime4);
 delay(.2);
}
 return 0;
}

char getTime(int wChar)
{
  FILE *fp;
  int status;
  char path[50];

  /* Open the command for reading. */
  fp = popen("date", "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit;
  }

  /* Read the output a line at a time - output it. */
  //while (fgets(path, sizeof(path)-1, fp) != NULL) {
  //  printf("%s", path);
  //}

  fscanf(fp,"%s",path);
  //printf("%s",path);
  /* close */
  pclose(fp);
  return path[wChar];
}