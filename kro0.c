#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ow 3
#define oh ow
#define magn 4

int main(int argc, char *argv[])
{
    int i,j, k, m;
    int A[oh][ow]={ {7,5,3}, {6,9,8}, {3,2,1}};
    /* print stuff out */
    for(i=0;i<oh;++i) {
        for(j=0;j<ow;++j) 
            printf("%d ", A[i][j]);
        printf("\n"); 
    }

    int B[oh*magn][ow*magn]={0};
    int *BA;
    for(i=0;i<oh;++i) 
        for(k=0;k<magn;++k) {
            BA=B[magn*i+k];
            for(j=0;j<ow;++j) 
                for(m=0;m<magn;++m) 
                    // B[magn*i+k][magn*j+m] = A[i][j];
                    BA[magn*j+m] = A[i][j];
        }

    printf("------------------------\n"); 
    for(i=0;i<oh*magn;++i) {
        for(j=0;j<ow*magn;++j) 
            printf("%d ", B[i][j]);
        printf("\n"); 
    }


    return 0;
}
