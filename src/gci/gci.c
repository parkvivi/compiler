#include "gci.h"
#include <string.h>

Quadrupla *quads = NULL;
int quads_cont = 0;
char *funcao_atual = NULL;
int temp_cont = 0;
int label_cont = 0;

char* novoTemp() {
    char* t = malloc(sizeof(char)*20);
    sprintf(t, "t%d", temp_cont++);
    return t;
}

char* novoLabel() {
    char* l = malloc(sizeof(char)*20);
    sprintf(l, "L%d", label_cont++);
    return l;
}

void emitir_quads(const char *op, const char *a1, const char *a2, const char *res) {
    quads = realloc(quads, sizeof(Quadrupla)*(quads_cont + 1));

    quads[quads_cont].operacao = strdup(op);
    quads[quads_cont].argumento1 = a1  ? strdup(a1) : NULL;
    quads[quads_cont].argumento2 = a2  ? strdup(a2) : NULL;
    quads[quads_cont].resultado  = res ? strdup(res) : NULL;

    quads_cont++;
}

void limpa_quads() {
    for(int i=0;i<quads_cont;i++) {
        free(quads[i].operacao);
        if(quads[i].argumento1) free(quads[i].argumento1);
        if(quads[i].argumento2) free(quads[i].argumento2);
        if(quads[i].resultado) free(quads[i].resultado);
    }

    free(quads);
    quads = NULL;
    quads_cont = 0;
    quads = malloc(sizeof(Quadrupla)*quads_cont);
    temp_cont = 0;
    label_cont = 0;
}

char* gerar_quads(AST* no){
    if(!no) return NULL;

    if((no->tipo == TipoDeclaracaoVariavel)) { // declaracao de variaveis
        char* tipo = no->filhos[0]->dado.nome; 
        char *nome = no->filhos[1]->dado.nome;

        emitir_quads("VAR", tipo, nome, funcao_atual);

        if(no->filhos[2]) { // vetores
            char buffer[32];
            sprintf(buffer, "%d", no->filhos[2]->dado.valor);
            emitir_quads("VETOR", nome, buffer, funcao_atual);
        }

        return NULL;
    }

    else if((no->tipo == TipoDeclaracaoFuncao)) { // funcoes
        funcao_atual = no->filhos[1]->dado.nome;
        emitir_quads("FUNCAO", no->filhos[0]->dado.nome, no->filhos[1]->dado.nome, "-");
        gerar_quads(no->filhos[2]); // parametros
        gerar_quads(no->filhos[3]); // escopo
        return NULL;
    }

    else if(no->tipo == TipoVar) { // variavel
        if(no->filhos[1] == NULL) // inteiro
            return no->filhos[0]->dado.nome;
        
        char* indice = gerar_quads(no->filhos[1]);
        char* nome = no->filhos[0]->dado.nome;

        char* var = malloc(strlen(nome) + strlen(indice) + 3);
        sprintf(var, "%s[%s]", nome, indice);

        return var;
    }

    else if(no->tipo == TipoNum){ // numero
        char* buffer = malloc(sizeof(char)*64);
        sprintf(buffer, "%d", no->dado.valor);
        return buffer;
    }

    else if(no->tipo == TipoChamadaFuncao) {
        int args_cont = 0;
        if(no->filhos[1]) { // se houver argumentos (pode ser NULL)
            AST *args = no->filhos[1]->filhos[0]; // pega a lista de argumentos
            while(args) {
                char *arg = gerar_quads(args->filhos[0]); // gera cada argumento
                emitir_quads("ARG", arg, "-", "-");
                args_cont++;
                args = args->filhos[1]; // próximo argumento da lista
            }
        }

        char *tmp = novoTemp();
        char* contador = malloc(sizeof(char)*64);
        sprintf(contador, "%d", args_cont);

        emitir_quads("CALL", no->filhos[0]->dado.nome, contador, tmp);
        return tmp;
    }

    else if((no->tipo == TipoDeclaracaoSelecao)) { // if, else
        char *cond = gerar_quads(no->filhos[0]);

        char *L_else = novoLabel();
        char *L_end  = novoLabel();
        
        emitir_quads("IF_FALSE", cond, "-", L_else);
        gerar_quads(no->filhos[1]); // then

        if(no->filhos[3]) { // else
            emitir_quads("GOTO", "-", "-", L_end);
            emitir_quads("LABEL", "-", "-", L_else);
            gerar_quads(no->filhos[3]); // else
            emitir_quads("LABEL", "-", "-", L_end);
        } else {
            emitir_quads("LABEL", "-", "-", L_else);
        }
        return NULL;
    }

    else if((no->tipo == TipoDeclaracaoIteracao)) { //while
        char *L_ini = novoLabel();
        char *L_end = novoLabel();

        emitir_quads("LABEL", "-", "-", L_ini);

        char *cond = gerar_quads(no->filhos[0]);
        emitir_quads("IF_FALSE", cond, "-", L_end);

        gerar_quads(no->filhos[1]);

        emitir_quads("GOTO", "-", "-", L_ini);
        emitir_quads("LABEL", "-", "-", L_end);

        return NULL;
    }

    else if(no->tipo == TipoParametro) {
        char *tipo = no->filhos[0]->dado.nome;
        char *nome = no->filhos[1]->dado.nome;

        emitir_quads("PARAM", tipo, nome, funcao_atual);
        if(no->filhos[2]) {
            emitir_quads("VETOR", nome, "-", funcao_atual);
        }

        return NULL;
    }

    else if(no->tipo == TipoDeclaracaoRetorno) {
        if(no->filhos[0]) {
            char *val = gerar_quads(no->filhos[0]);
            emitir_quads("RETURN", val, "-", "-");
        } else {
            emitir_quads("RETURN", "-", "-", "-");
        }
        return NULL;
    }

    for(int i=0;i<MAXFILHOS && no->filhos[i] != NULL;i++) 
        gerar_quads(no->filhos[i]);
    
    return NULL;
}

void print_quads(FILE *out){
    for(int i=0;i<quads_cont;i++){
        Quadrupla *q = &quads[i];
        fprintf(out, "%3d: (%s, %s, %s, %s)\n",
                i,
                q->operacao ? q->operacao : "-",
                q->argumento1 ? q->argumento1 : "-",
                q->argumento2 ? q->argumento2 : "-",
                q->resultado  ? q->resultado  : "-");
    }
}