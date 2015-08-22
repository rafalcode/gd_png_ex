/* array with mask type */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct
{
    int *a;
    unsigned asz;
    unsigned long *m;
} awm_t;

awm_t *crea_awm_of_sz(int sz)
{
    awm_t *awm=malloc(sizeof(awm_t));
    awm->a=malloc(sz*sizeof(int));
    awm->asz=sz;
    awm->m=calloc(1+((sz-1)%64), sizeof(int));
    return awm; 
}

int test_awm_at_i(awm_t *awm, int i)
{
    if( awm->m[i/64] & (1<<(i%64)) )
        return 1;
    return 0;
}

void set_awm_at_i(awm_t *awm, int i)
{
    awm->m[i/64] |= 1<<(i%64);
    return;
}

void free_awm(awm_t **awm)
{
    awm_t *tawm=*awm;
    free(tawm->a);
    free(tawm->m);
    free(*awm);
    return; 
}

int main(int argc, char *argv[])
{
   /* declarations */
   awm_t *awm= crea_awm_of_sz(140);
   set_awm_at_i(awm, 60);
   set_awm_at_i(awm, 129);

   printf("1st test equals %d\n", test_awm_at_i(awm, 59));
   printf("2nd test equals %d\n", test_awm_at_i(awm, 129));

   free_awm(&awm);

   return 0;
}
