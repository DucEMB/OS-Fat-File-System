#include <stdio.h>
#include <stdlib.h>
#include "Fat.h"
int main()
{
    Init();
    if(Init() == 1)
    {
        Read_Boot();
        menu();
    }
    else
    {
        printf("Input dau vao loi...");
    }
    return 0;
}
