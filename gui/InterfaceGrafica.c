/*
 * Interface grafica do parser preditivo, feita com a biblioteca raylib.
 *
 * A pessoa digita a expressao num campo de texto e tecla Enter.
 * A janela mostra a mensagem do parser e desenha a arvore de derivacao.
 *
 * Este arquivo tem o seu proprio main; por isso fica fora de src/.
 * Para compilar, a partir da pasta do projeto:
 *
 *   gcc -Wall -Wextra -std=c11 gui/InterfaceGrafica.c src/ArvoreAutomato.c src/PilhaAutomato.c src/ParserPreditivo.c -o parser-gui -lraylib
 */

#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "../src/ParserPreditivo.h"

#define LARGURA_JANELA 1100
#define ALTURA_JANELA 700
#define MARGEM 30
#define TOPO_ARVORE 200     //Altura (em pixels) onde comeca a area da arvore

#define MAX_LARGURA_FOLHA 70 //Espaco horizontal maximo de cada folha da arvore
#define MAX_ALTURA_NIVEL 80  //Espaco vertical maximo de cada nivel da arvore
#define MAX_RAIO 22          //Raio maximo do circulo de cada no

//Fonte com acentos e com a letra grega epsilon. Se o arquivo nao existir
//(por exemplo, fora do Windows), a raylib usa a sua fonte padrao, que nao
//tem esses caracteres: eles aparecem como '?'.
#define ARQUIVO_FONTE "C:/Windows/Fonts/arial.ttf"

//Cores dos nos da arvore
#define COR_NAO_TERMINAL (Color){ 120, 170, 230, 255 }
#define COR_TERMINAL     (Color){ 250, 200, 90, 255 }
#define COR_VAZIO        (Color){ 215, 215, 215, 255 }

//Fonte usada por todas as funcoes que escrevem na janela
Font fonte;

//Medidas usadas para desenhar a arvore. Sao recalculadas a cada desenho,
//para que a arvore caiba na janela, seja ela grande ou pequena.
struct escala {
    float larguraFolha; //Espaco horizontal ocupado por uma folha
    float alturaNivel;  //Distancia vertical entre um nivel e o seguinte
    float raio;         //Raio do circulo de cada no
    float tamanhoFonte; //Tamanho da letra dentro do circulo
};
typedef struct escala Escala;


/* ---------- Texto ---------- */

//Carrega a fonte com os caracteres que vamos usar:
//o ASCII comum, as letras acentuadas do portugues e o epsilon.
Font carregaFonte(void)
{
    int codigos[200];
    int n = 0;
    int c;
    Font f;

    for (c = 32; c < 127; c++)    //ASCII visivel
        codigos[n++] = c;
    for (c = 0xC0; c <= 0xFF; c++) //Letras acentuadas (á, ã, ç, é, ...)
        codigos[n++] = c;
    codigos[n++] = 0x03B5;         //Letra grega epsilon (ε)

    f = LoadFontEx(ARQUIVO_FONTE, 64, codigos, n);
    SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR); //Suaviza a letra ao reduzi-la
    return f;
}

//Escreve um texto com o canto superior esquerdo em (x, y)
void escreve(const char *texto, float x, float y, float tamanho, Color cor)
{
    DrawTextEx(fonte, texto, (Vector2){ x, y }, tamanho, 1, cor);
}

//Escreve um texto com o centro em (x, y)
void escreveCentralizado(const char *texto, float x, float y, float tamanho, Color cor)
{
    Vector2 medida = MeasureTextEx(fonte, texto, tamanho, 1);
    DrawTextEx(fonte, texto, (Vector2){ x - medida.x/2, y - medida.y/2 }, tamanho, 1, cor);
}


/* ---------- Campo de texto ---------- */

//Le as teclas digitadas neste quadro e atualiza o texto do campo.
//O limite e o mesmo do programa de console: INPUTSIZE-1 caracteres,
//pois o ultimo espaco do vetor e do '\0'.
void atualizaCampo(char *texto)
{
    size_t tamanho;
    int c = GetCharPressed(); //Proximo caractere digitado (0 se nao houver)

    while (c > 0) {
        tamanho = strlen(texto);
        //Aceita so caracteres ASCII visiveis, sem espaco (como o scanf("%s"))
        if (c > 32 && c < 127 && tamanho < INPUTSIZE-1) {
            texto[tamanho] = (char) c;
            texto[tamanho+1] = '\0';
        }
        c = GetCharPressed();
    }

    //Backspace apaga o ultimo caractere (e repete se a tecla ficar apertada)
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        tamanho = strlen(texto);
        if (tamanho > 0)
            texto[tamanho-1] = '\0';
    }
}

void desenhaCampo(char *texto, float x, float y)
{
    float largura = 360, altura = 40;
    Vector2 medida = MeasureTextEx(fonte, texto, 28, 1);

    DrawRectangle(x, y, largura, altura, WHITE);
    DrawRectangleLines(x, y, largura, altura, DARKGRAY);
    escreve(texto, x + 10, y + 6, 28, BLACK);

    //Cursor piscando: aparece meio segundo, some meio segundo
    if ((int)(GetTime()*2) % 2 == 0)
        escreve("|", x + 10 + medida.x, y + 5, 28, BLACK);
}


/* ---------- Analise ---------- */

//Roda o parser sobre a expressao, como faz o main.c do console,
//e devolve a arvore construida (completa ou, se houve erro, parcial).
ArvGen* analisa(Tabela *tab, char *expressao, char *mensagem)
{
    ArvGen *a = criaArv('S');
    Pilha *pilhaExp = cria();

    push(pilhaExp, 'S');
    predictiveParse(tab, pilhaExp, expressao, a, mensagem);
    libera(pilhaExp);
    return a;
}


/* ---------- Desenho da arvore ---------- */

//Conta as folhas (nos sem filhos) da arvore
int contaFolhas(ArvGen *a)
{
    ArvGen *p;
    int total = 0;

    if (a->prim == NULL)
        return 1;
    for (p = a->prim; p != NULL; p = p->prox)
        total = total + contaFolhas(p);
    return total;
}

//Conta os niveis da arvore (a raiz sozinha tem 1 nivel)
int contaNiveis(ArvGen *a)
{
    ArvGen *p;
    int maior = 0, n;

    for (p = a->prim; p != NULL; p = p->prox) {
        n = contaNiveis(p);
        if (n > maior)
            maior = n;
    }
    return 1 + maior;
}

//Escolhe as medidas para que a arvore inteira caiba na janela
Escala calculaEscala(ArvGen *a)
{
    Escala e;
    float larguraArea = GetScreenWidth() - 2*MARGEM;
    float alturaArea = GetScreenHeight() - TOPO_ARVORE - MARGEM;

    e.larguraFolha = larguraArea / contaFolhas(a);
    if (e.larguraFolha > MAX_LARGURA_FOLHA)
        e.larguraFolha = MAX_LARGURA_FOLHA;

    e.alturaNivel = alturaArea / contaNiveis(a);
    if (e.alturaNivel > MAX_ALTURA_NIVEL)
        e.alturaNivel = MAX_ALTURA_NIVEL;

    //O circulo nao pode ser maior que o espaco da folha nem que o do nivel
    if (e.larguraFolha < e.alturaNivel)
        e.raio = e.larguraFolha * 0.4;
    else
        e.raio = e.alturaNivel * 0.4;
    if (e.raio > MAX_RAIO)
        e.raio = MAX_RAIO;

    e.tamanhoFonte = e.raio * 1.3;
    return e;
}

//Desenha um no: um circulo colorido com o simbolo dentro.
//O no vazio ('\0'), criado pelas producoes vazias, aparece como ε.
void desenhaSimbolo(char info, float x, float y, Escala e)
{
    char texto[3]; //O ε ocupa 2 bytes no texto (UTF-8), mais o '\0'
    Color cor;

    if (info == '\0') {
        strcpy(texto, "ε");
        cor = COR_VAZIO;
    }
    else {
        texto[0] = info;
        texto[1] = '\0';
        if (simboloTerminal(info))
            cor = COR_TERMINAL;
        else
            cor = COR_NAO_TERMINAL;
    }

    DrawCircle(x, y, e.raio, cor);
    DrawCircleLines(x, y, e.raio, DARKGRAY);
    escreveCentralizado(texto, x, y, e.tamanhoFonte, BLACK);
}

//Desenha a arvore a partir do no a.
//
//Regra de posicionamento: cada no ocupa uma faixa horizontal com largura
//proporcional ao numero de folhas abaixo dele, e fica no meio da sua faixa.
//A faixa do pai e dividida entre os filhos, da esquerda para a direita.
//O nivel do no define a sua altura na janela.
//
//esquerda: onde comeca a faixa deste no.
void desenhaArvore(ArvGen *a, float esquerda, int nivel, Escala e)
{
    ArvGen *p;
    float largura = contaFolhas(a) * e.larguraFolha;
    float x = esquerda + largura/2;
    float y = TOPO_ARVORE + nivel*e.alturaNivel + e.alturaNivel/2;
    float esquerdaFilho = esquerda;
    float larguraFilho, xFilho, yFilho;

    for (p = a->prim; p != NULL; p = p->prox) {
        larguraFilho = contaFolhas(p) * e.larguraFolha;
        xFilho = esquerdaFilho + larguraFilho/2;
        yFilho = y + e.alturaNivel;

        DrawLineEx((Vector2){ x, y }, (Vector2){ xFilho, yFilho }, 2, GRAY);
        desenhaArvore(p, esquerdaFilho, nivel+1, e);

        esquerdaFilho = esquerdaFilho + larguraFilho;
    }

    //O circulo e desenhado por ultimo, para ficar por cima das linhas
    desenhaSimbolo(a->info, x, y, e);
}

//Desenha a legenda das cores, no canto superior direito
void desenhaLegenda(void)
{
    float x = GetScreenWidth() - 230;

    DrawCircle(x, 35, 10, COR_NAO_TERMINAL);
    escreve("não terminal", x + 18, 25, 20, DARKGRAY);
    DrawCircle(x, 65, 10, COR_TERMINAL);
    escreve("terminal", x + 18, 55, 20, DARKGRAY);
    DrawCircle(x, 95, 10, COR_VAZIO);
    escreve("ε (produção vazia)", x + 18, 85, 20, DARKGRAY);
}


/* ---------- Programa principal ---------- */

int main(void)
{
    char expressao[INPUTSIZE] = "";
    char mensagem[MSGSIZE] = "Digite uma expressão terminada em $ e tecle Enter.";
    Tabela tab[TABSIZE];
    ArvGen *arvore = NULL; //Nenhuma arvore antes da primeira analise
    int analisou = 0;      //Vira 1 depois da primeira analise
    Color corMensagem;
    Escala e;
    float esquerda;

    preencheTabela(tab);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Parser preditivo LL(1)");
    SetTargetFPS(60);
    fonte = carregaFonte();

    //Cada volta do laco desenha um quadro da janela
    while (!WindowShouldClose()) {

        atualizaCampo(expressao);

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
            if (arvore != NULL)
                liberaArv(arvore); //Descarta a arvore da analise anterior
            arvore = analisa(tab, expressao, mensagem);
            analisou = 1;
        }

        //Verde se a expressao foi reconhecida, vermelho se houve erro
        if (!analisou)
            corMensagem = DARKGRAY;
        else if (strcmp(mensagem, "Expressao reconhecida pela linguagem!") == 0)
            corMensagem = DARKGREEN;
        else
            corMensagem = MAROON;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        escreve("Parser preditivo LL(1)", MARGEM, 20, 32, BLACK);
        escreve("Expressão:", MARGEM, 80, 26, BLACK);
        desenhaCampo(expressao, MARGEM + 130, 72);
        escreve("Enter: analisar     Backspace: apagar", MARGEM + 130, 117, 18, GRAY);
        escreve(mensagem, MARGEM, 150, 26, corMensagem);
        desenhaLegenda();

        if (arvore != NULL) {
            e = calculaEscala(arvore);
            //Centraliza a arvore na janela
            esquerda = (GetScreenWidth() - contaFolhas(arvore) * e.larguraFolha) / 2;
            desenhaArvore(arvore, esquerda, 0, e);
        }

        EndDrawing();
    }

    if (arvore != NULL)
        liberaArv(arvore);
    UnloadFont(fonte);
    CloseWindow();
    return 0;
}
