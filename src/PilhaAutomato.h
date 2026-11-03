#define TABSIZE 19
#define INPUTSIZE 20

struct nodo { //Define o elemento da pilha
    char info;
    struct nodo* prox;
};

typedef struct nodo Nodo;

struct pilha {
    Nodo* topo;
};

typedef struct pilha Pilha;

//Aloca a estrutura da pilha e inicializa a lista como sendo vazia
Pilha* cria (void);
//Insere elemento no topo
void push (Pilha* p, char v);
//Retira elemento do topo
char pop (Pilha* p);
//Verifica se a pilha esta vazia
int vazia (Pilha* p);
//Libera memoria alocada para elementos da pilha
void libera (Pilha* p);
//Imprime o conteúdo da pilha
void imprime (Pilha* p);
int comprimento (Pilha* p);

int qtdeOcorrencias (Pilha* p,char elem);
char maiorPilha (Pilha* p);
