#include <stdio.h>
#include <stdlib.h>
#include "PilhaAutomato.h"


//Aloca a estrutura da pilha e inicializa a lista como sendo vazia
Pilha* cria (void)
{
    Pilha* p = (Pilha*) malloc(sizeof(Pilha));
    p->topo = NULL;
    return p;
}
//Insere elemento no topo
void push (Pilha* p, char v)
{
    Nodo* n= (Nodo*) malloc(sizeof(Nodo));
    n->info = v;
    n->prox = p->topo;
    p->topo =n;
}
//Retira elemento do topo
char pop (Pilha* p)
{
    char v;
    Nodo*n;
    if (vazia(p)) {
        printf("Pilha vazia.\n");
        exit(1); // aborta programa
    }
    n=p->topo;
    v = n->info;
    p->topo = n->prox;
    free(n);
    return v;
}
//Verifica se a pilha esta vazia
int vazia (Pilha* p)
{
    if(p->topo==NULL)
        return (1);
    else
        return (0);
}
//Libera memoria alocada para elementos da pilha
void libera (Pilha* p)
{
    Nodo*q = p->topo;
    while (q!=NULL) {
        Nodo*t = q->prox;
        free(q);
        q = t;
    }
    free(p);
}
//Imprime o conteúdo da pilha
void imprime (Pilha* p)
{
    Nodo*q;
    for (q=p->topo; q!=NULL; q=q->prox)
    printf("%c\n",q->info);
}

int comprimento (Pilha* p)
{
    Nodo*q;
    int qtde=0;
    for (q=p->topo; q!=NULL; q=q->prox)
        qtde++;
    return qtde;
}


int qtdeOcorrencias (Pilha* p,char elem)
{
    Nodo*q;
    int qtde=0;
    for (q=p->topo; q!=NULL; q=q->prox){
        if(q->info==elem)
            qtde++;
    }
    return qtde;
}

char maiorPilha (Pilha* p)
{
    Nodo*q;
    char maior=p->topo->info;
    for (q=p->topo; q!=NULL; q=q->prox){
        if(q->info>maior)
        maior=q->info;
    }
    return maior;
}
