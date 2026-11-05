//Interface grafica do parser preditivo, feita com a biblioteca raylib.
//Ha um campo para digitar a expressao (terminada em $), um botao "Analisar",
//a mensagem com o resultado e o desenho da arvore de derivacao.

#include <string.h>
#include "raylib.h"
#include "ParserPreditivo.h"

#define LARGURA_JANELA 1100
#define ALTURA_JANELA 650
#define TAM_FONTE 20
#define RAIO 18 //Raio do circulo de cada no da arvore

Font fonte; //Fonte usada em todos os textos da janela

//Conta as folhas da subarvore. E o "espaco" horizontal que ela ocupa no desenho.
int contaFolhas(ArvGen *a) {
    ArvGen *p;
    int total = 0;

    if (a->prim == NULL)
        return 1;
    for (p = a->prim; p != NULL; p = p->prox)
        total = total + contaFolhas(p);
    return total;
}

//Calcula a altura da arvore (quantidade de niveis)
int alturaArv(ArvGen *a) {
    ArvGen *p;
    int maior = 0, h;

    for (p = a->prim; p != NULL; p = p->prox) {
        h = alturaArv(p);
        if (h > maior)
            maior = h;
    }
    return maior + 1;
}

//Escreve um texto centralizado no ponto (x,y)
void escreveCentralizado(const char *texto, float x, float y, Color cor) {
    Vector2 tamanho = MeasureTextEx(fonte, texto, TAM_FONTE, 1);
    Vector2 posicao = { x - tamanho.x/2, y - tamanho.y/2 };
    DrawTextEx(fonte, texto, posicao, TAM_FONTE, 1, cor);
}

//Desenha o no "a" e todos os seus descendentes.
//A subarvore ocupa a faixa horizontal que comeca em "esquerda" e tem "largura" pixels.
//O no fica no meio dessa faixa, na altura y.
void desenhaArv(ArvGen *a, float esquerda, float largura, float y, float distNiveis) {
    ArvGen *p;
    float x = esquerda + largura/2;
    float folhas = contaFolhas(a);
    float inicioFilho = esquerda;
    float larguraFilho;
    char rotulo[2];
    Color cor;

    //Cada filho recebe um pedaco da faixa, proporcional ao numero de folhas dele
    for (p = a->prim; p != NULL; p = p->prox) {
        larguraFilho = largura * contaFolhas(p) / folhas;
        DrawLineEx((Vector2){ x, y }, (Vector2){ inicioFilho + larguraFilho/2, y + distNiveis }, 2, DARKGRAY);
        desenhaArv(p, inicioFilho, larguraFilho, y + distNiveis, distNiveis);
        inicioFilho = inicioFilho + larguraFilho;
    }

    //O circulo e desenhado depois das linhas, para ficar por cima delas
    if (a->info == '\0')
        cor = LIGHTGRAY;  //Producao vazia
    else if (simboloTerminal(a->info))
        cor = GOLD;       //Terminal
    else
        cor = SKYBLUE;    //Nao terminal
    DrawCircleV((Vector2){ x, y }, RAIO, cor);
    DrawCircleLinesV((Vector2){ x, y }, RAIO, DARKGRAY);

    //O no vazio guarda o caractere '\0'; na tela ele aparece como epsilon
    if (a->info == '\0')
        escreveCentralizado("ε", x, y, BLACK);
    else {
        rotulo[0] = a->info;
        rotulo[1] = '\0';
        escreveCentralizado(rotulo, x, y, BLACK);
    }
}

//Desenha um item da legenda: um circulo colorido e o texto ao lado
void desenhaLegenda(float x, float y, Color cor, const char *texto) {
    DrawCircleV((Vector2){ x, y }, 8, cor);
    DrawCircleLinesV((Vector2){ x, y }, 8, DARKGRAY);
    DrawTextEx(fonte, texto, (Vector2){ x + 15, y - 9 }, 18, 1, DARKGRAY);
}

int main(void) {
    char expressao[INPUTSIZE] = "";  //O que foi digitado no campo
    int tamanho = 0;                 //Quantos caracteres ja foram digitados
    char mensagem[MSGSIZE] = "";     //Resultado da analise, preenchido pelo parser
    Tabela tab[TABSIZE];
    Pilha *pilhaExp;
    ArvGen *a = NULL;                //Arvore da ultima analise (NULL antes da primeira)
    int caractere;
    int altura;
    float distNiveis;
    int codigos[96];
    int i;

    Rectangle campo = { 140, 30, 400, 40 };  //Campo de texto
    Rectangle botao = { 560, 30, 130, 40 };  //Botao "Analisar"
    Vector2 cursor;

    preencheTabela(tab);

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Parser preditivo LL(1)");
    SetTargetFPS(60);

    //A fonte padrao da raylib nao tem a letra grega epsilon.
    //Carregamos a Arial com os caracteres comuns (32 a 126) e mais o epsilon (codigo 0x3B5).
    for (i = 0; i < 95; i++)
        codigos[i] = 32 + i;
    codigos[95] = 0x3B5;
    fonte = LoadFontEx("C:/Windows/Fonts/arial.ttf", TAM_FONTE, codigos, 96);
    if (!IsFontValid(fonte))
        fonte = GetFontDefault(); //Sem a Arial, o epsilon aparece como '?'

    while (!WindowShouldClose()) {

        //---------- Leitura do teclado ----------

        //GetCharPressed devolve, um por vez, os caracteres digitados neste quadro
        caractere = GetCharPressed();
        while (caractere > 0) {
            if (caractere >= 32 && caractere <= 126 && tamanho < INPUTSIZE - 1) {
                expressao[tamanho] = (char) caractere;
                tamanho++;
                expressao[tamanho] = '\0';
            }
            caractere = GetCharPressed();
        }

        //Backspace apaga o ultimo caractere (segurando a tecla, continua apagando)
        if ((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && tamanho > 0) {
            tamanho--;
            expressao[tamanho] = '\0';
        }

        //---------- Analise: tecla Enter ou clique no botao ----------

        if (IsKeyPressed(KEY_ENTER) ||
            (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botao))) {

            if (a != NULL)
                liberaArv(a); //Descarta a arvore da analise anterior

            //Os mesmos passos do main.c
            a = criaArv('S');
            pilhaExp = cria();
            push(pilhaExp, 'S');
            predictiveParse(tab, pilhaExp, expressao, a, mensagem);
            libera(pilhaExp);
        }

        //---------- Desenho da janela ----------

        BeginDrawing();
        ClearBackground(RAYWHITE);

        //Campo de texto
        DrawTextEx(fonte, "Expressao:", (Vector2){ 20, 40 }, TAM_FONTE, 1, BLACK);
        DrawRectangleRec(campo, WHITE);
        DrawRectangleLinesEx(campo, 2, DARKGRAY);
        DrawTextEx(fonte, expressao, (Vector2){ campo.x + 10, campo.y + 10 }, TAM_FONTE, 1, BLACK);

        //Cursor piscando: aparece meio segundo, some meio segundo
        if ((int)(GetTime() * 2) % 2 == 0) {
            cursor = MeasureTextEx(fonte, expressao, TAM_FONTE, 1);
            DrawTextEx(fonte, "_", (Vector2){ campo.x + 12 + cursor.x, campo.y + 10 }, TAM_FONTE, 1, BLACK);
        }

        //Botao
        DrawRectangleRec(botao, SKYBLUE);
        DrawRectangleLinesEx(botao, 2, DARKGRAY);
        escreveCentralizado("Analisar", botao.x + botao.width/2, botao.y + botao.height/2, BLACK);

        //Mensagem com o resultado
        DrawTextEx(fonte, mensagem, (Vector2){ 20, 90 }, TAM_FONTE, 1, BLACK);

        //Arvore: os niveis sao espacados para caber na area abaixo da mensagem
        if (a != NULL) {
            altura = alturaArv(a);
            distNiveis = 70;
            if (altura > 1 && (altura - 1) * distNiveis > ALTURA_JANELA - 230)
                distNiveis = (float)(ALTURA_JANELA - 230) / (altura - 1);
            desenhaArv(a, 20, LARGURA_JANELA - 40, 160, distNiveis);
        }

        //Legenda
        desenhaLegenda(30, ALTURA_JANELA - 25, SKYBLUE, "nao terminal");
        desenhaLegenda(180, ALTURA_JANELA - 25, GOLD, "terminal");
        desenhaLegenda(300, ALTURA_JANELA - 25, LIGHTGRAY, "producao vazia (ε)");

        EndDrawing();
    }

    if (a != NULL)
        liberaArv(a);
    UnloadFont(fonte);
    CloseWindow();
    return 0;
}
