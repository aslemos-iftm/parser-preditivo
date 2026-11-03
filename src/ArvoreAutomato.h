
struct arvgen {
    char info;
    struct arvgen *prim;
    struct arvgen *prox;
    struct arvgen *ant;
    float coluna; //Posicao horizontal do no, calculada por calculaPosicoes
    int nivel;    //Profundidade do no (raiz = 0)
};

typedef struct arvgen ArvGen;

ArvGen* criaArv (char c);
void insereSubarvore(ArvGen* a,char *c);
int insereProd(ArvGen *r,ArvGen *novo);
void imprimeArv (ArvGen* a,int l);
int buscaArv (ArvGen* a, char c);
void liberaArv (ArvGen* a);
void imprimeExp(ArvGen* a,int l,int m);
int calculaPosicoes(ArvGen *a,int nivel,int proxColuna);
