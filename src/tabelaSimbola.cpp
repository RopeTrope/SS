#include "../inc/tabelaSimbola.hpp"
#include <cstring>
#include <stdio.h>

tabelaSimbola::tabelaSimbola(){
    this->simboli = vector<Simbol*>();
}

tabelaSimbola::~tabelaSimbola(){
    
}

void tabelaSimbola::dodajSimbol(Simbol* simbol){
    this->simboli.push_back(simbol);
}

void tabelaSimbola::ispisiTabeluSimbola(){
  printf("-------------Tabela simbola-------------------\n");
  printf("ID:\t");
  printf("VREDNOST:\t");
  printf("VELICINA:\t");
  printf("TIP\t");
  printf("GLOB/LOC\t");
  printf("SEKCIJA\t");
  printf("NAZIV\n");
  int i = 0;
  for(vector<Simbol*>::iterator it = this->simboli.begin();it < this->simboli.end();it++){
    printf("%d\t",i++);
    printf("%08X\t",(*it)->vrednost);
    printf("%08X\t",(*it)->velicina);
    printf("\t");
    if((*it)->global){
      printf("GLOBAL\t");
    }else{
      printf("LOCAL\t");
    }
    if((*it)->sekcija == -1){
      printf("*UND\t");
    }else{
    printf("%x\t",(*it)->sekcija);
  }
    printf("%s\n",(*it)->naziv);
    
  }
  }

  
  Simbol* tabelaSimbola::dohvatiSimbol(char* naziv){
    for(vector<Simbol*>::iterator it = this->simboli.begin();it < this->simboli.end();it++){
      if(strcmp((*it)->naziv,naziv) == 0){
        return (*it);
      }
    }
    return NULL;
  }
