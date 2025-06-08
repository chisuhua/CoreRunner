#include <stdio.h>

int main(int argc, char* argv[])
{
    int i = 0;
    while(1){
        i++;
        if (i > 100) break;
        printf("Hello world! %d\n", i);
    }
    return 0;
}
