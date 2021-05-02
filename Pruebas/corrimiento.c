#include <stdio.h>


int main(){
    int num=65535;
    //de int a unsigned char
    unsigned char unchar_num[2];
    unchar_num[0]=num>>8;
    unchar_num[1]=(unsigned char)num;

    printf("unsigned char=> %c , %c | %d , %d \n\n",unchar_num[0],unchar_num[1],unchar_num[0],unchar_num[1]);

    //de unsigned a int

    int n,n1,n2;

    n1=unchar_num[0]<<8;
    n2=(int)unchar_num[1];
    printf("n1=> %d ,n2=> %d\n\n",n1,n2);
    n=n1+n2;
    printf("int=> %d",n);
    return 0;
}