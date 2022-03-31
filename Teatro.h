#pragma once

#include <stdio.h>
#include <stdlib.h>


struct _Teatro
{
    char* id;

    char* nome;

    char* localizacao;

};

typedef struct _Teatro Teatro;

Teatro* Criar_Teatro(char*, char*, char*);
char* getNome(Teatro*);
char* getId(Teatro*);
char* getLocalizacao(Teatro*);
void setId(Teatro*, char*);
void setNome(Teatro*, char*);
void setLocalizacao(Teatro*, char*);