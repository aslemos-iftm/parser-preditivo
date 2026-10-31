#include "ArvoreAutomato.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ArvGen* criaArv (char c) {
    ArvGen *a =(ArvGen *) malloc(sizeof(ArvGen));
    a->info = c;
    a->prim = NULL;
    a->prox = NULL;
    a->ant=NULL;
    return a;
}

void insereSubarvore(ArvGen* a,char *c) {
    ArvGen *sa ;
    ArvGen *q,*p,*r,*t;

    size_t i;
    int res=0;
    sa=(ArvGen *) malloc(sizeof(ArvGen));
    sa->info = c[0];
    sa->prim = NULL;
    sa->prox = NULL;
    sa->ant=a;
    t=sa;

    for(i=1;i<strlen(c);i++) {
        r=(ArvGen *) malloc(sizeof(ArvGen));
        r->info = c[i];
        r->prim = NULL;
        r->prox = NULL;
        t->prox=r;
        t->ant=a;
        t=r;
    }

    if(a->prim==NULL){ //Insere como primeiro filho
        a->prim = sa;
        a->prim->ant=a;
    }
    else { //Localiza posicao de insercao
        q=a->prim;
        q->ant=a;
        for(p=a->prim;p!=NULL;p=p->prox){
            p->ant=a;
        }
        res=insereProd(q,sa,a);
    }
}

int insereProd(ArvGen *r,ArvGen *novo,ArvGen *pai){
    ArvGen *q,*p,*t;
    q=r;p=r;

    while(q!=NULL &&(q->info!='S' && q->info!='R' && q->info!='A' && q->info!='B' && q->info!='X')){
        p=q;
        q=q->prox;
    }
    if(q!=NULL && q->prim==NULL){
        q->prim=novo;
        for(p=q->prim;p!=NULL;p=p->prox){
            p->ant=q;
        }
    }
    else if(q!=NULL && q->prim!=NULL){
            if (!insereProd(q->prim,novo,q)){
                if (!insereProd(q->prox,novo,q->ant)){
                    return 0;
                }
            }
         }
         else if(q==NULL){
             return 0;
         }
    return -1;
}
void imprimeArv (ArvGen* a,int l)
{
    ArvGen* p;
    int i,qtde=0;
    printf("\nNivel:%d - ",l);
    printf("Simbolo:%c - ",a->info);
    if(a->ant!=NULL)
        printf("Pai:%c\n",a->ant->info);
    else
        printf("Raiz\n");

    for (p=a->prim; p!=NULL; p=p->prox){
         imprimeArv(p,l+1);
    }
}

void imprimeExp(ArvGen* a,int l,int m)
{
    ArvGen* p;
    int i=0;
        printf("%c",a->info);
    for (p=a->prim; p!=NULL; p=p->prox){
        if(i==0)
            printf("[");
        i++;

        imprimeExp(p,l+1,m);
        if(p->prox==NULL)
            printf("]");
    }
}


int buscaArv (ArvGen* a, char c)
{
    ArvGen* p;
    if (a->info==c)
        return -1;
    else {
        for (p=a->prim; p!=NULL; p=p->prox) {
            if (buscaArv(p,c))
                return 1;
        }
    }
    return 0;
}

void liberaArv (ArvGen* a)
{
    ArvGen* p = a->prim;
    while (p!=NULL) {
        ArvGen* t = p->prox;
        liberaArv(p);
        p = t;
    }
    free(a);
}
