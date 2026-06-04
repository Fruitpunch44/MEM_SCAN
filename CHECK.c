#include<stdio.h>
#include<stdint.h>

#define Type(x) _Generic((x), \
    int: "int", \
    long: "long", \
    float: "float", \
    default: "unknown")

    
int main(void){
    int i;
    printf("%s\n",Type(i));
}