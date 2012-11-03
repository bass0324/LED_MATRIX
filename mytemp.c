#include <stdio.h>
#include <stdlib.h>

char getTemp(int wChar);
char myTemp1;
char myTemp2;
char myTemp3;

int main( int argc, char *argv[] )
{

 myTemp1 = getTemp(5);
 myTemp2 = getTemp(6);
 myTemp3 = getTemp(8);
 printf("%c %c %c",myTemp1,myTemp2,myTemp3);

 return 0;
}

char getTemp(int wChar)
{
  FILE *fp;
  int status;
  char path[20];

  /* Open the command for reading. */
  fp = popen("/opt/vc/bin/vcgencmd measure_temp", "r");
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
