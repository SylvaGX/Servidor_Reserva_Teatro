#include "Teatro.h"

Teatro* Criar_Teatro(char* s1, char* s2, char* s3) {
    Teatro* t;
    t = (Teatro*)malloc(sizeof(Teatro));
    if (t != NULL) {
        t->id = s1;
        t->nome = s2;
        t->localizacao = s3;
        return t;
    }
    return NULL;
}

char* getNome(Teatro* t) {
    return t->nome;
}
char* getId(Teatro* t) {
    return t->id;
}
char* getLocalizacao(Teatro* t) {
    return t->localizacao;
}
void setId(Teatro* t, char* s) {
    t->id = s;
}
void setNome(Teatro* t, char* s) {
    t->nome = s;
}
void setLocalizacao(Teatro* t, char* s) {
    t->localizacao = s;
}