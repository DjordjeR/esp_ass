#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
char buffer[10];
int value = 234452;
snprintf(buffer, 10, "%d", value);
printf("%s\n", buffer);
  return 0;
}