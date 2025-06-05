#include "../inc/tabelaLiterala.hpp"
#include <stdio.h>

void tabelaLiterala::dodajLiteral(Literal* l){
  this->literali.push_back(l);
}

Literal* tabelaLiterala::dohvatiLiteral(int vred){
  for(vector<Literal*>::iterator it = literali.begin();it < literali.end();it++){
    if((*it)->getVrednost() == vred){
      return (*it);
    }

  }
  return NULL;
}

void tabelaLiterala::ispisiTabeluLiterala(){
  printf("_________________\n");
  printf("TABELA LITERALA\n");
  printf("ID\t");
  printf("VREDNOST\t");
  printf("Naziv Simbola\n");
  for(vector<Literal*>::iterator it = literali.begin();it < literali.end();it++){
    printf("%d\t",(*it)->getId());   
    if((*it)->getDaLiJeSimbol()){
      printf("%d\t",(*it)->getVrednost());
      printf("%s\n",(*it)->getNazivSimbola());
    }else{
      printf("%d\n",(*it)->getVrednost());
    }
  }

  printf("\n_________________\n");
}

Literal* tabelaLiterala::dohvatiSimbol(char* naziv){
  for(vector<Literal*>::iterator it = literali.begin();it < literali.end();it++){
    if((*it)->getDaLiJeSimbol() == true){
    if(strcmp((*it)->getNazivSimbola(),naziv) == 0){
      return (*it);
    }

  }
  }
  return NULL;
}