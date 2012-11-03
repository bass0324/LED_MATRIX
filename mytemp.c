#include <stdio.h>
#include <stdlib.h>

char getTemp();

int main( int argc, char *argv[] )
{
  
  //myTemp = getTemp();
  //printf("%s", myTemp);

  return 0;
}

char getTemp()
{
  FILE *fp;
  int status;
  char path[1035];

  /* Open the command for reading. */
  fp = popen("/opt/vc/bin/vcgencmd measure_temp", "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit;
  }

  /* Read the output a line at a time - output it. */
  while (fgets(path, sizeof(path)-1, fp) != NULL) {
    //printf("%s", path);
  }

  /* close */
  pclose(fp);
  return path
}