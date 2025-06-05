#include "../inc/tabelaSekcija.hpp"
#include <stdio.h>
#include <cstring>

tabelaSekcija::tabelaSekcija(){
  this->sekcije = vector<Sekcija*>();
}

tabelaSekcija::~tabelaSekcija(){

}

void tabelaSekcija::dodajSekciju(Sekcija* sekcija){
  this->sekcije.push_back(sekcija);
}

void tabelaSekcija::ispisiTabeluSekcija(){
  printf("-------------------TABELA_SEKCIJA---------------------\n");
  printf("ID\t");
  printf("Naziv\t");
  printf("Pocetak_Sekcije\t");
  printf("Duzina_sekcije\n");
  for(vector<Sekcija*>::iterator it = this->sekcije.begin();it<this->sekcije.end();it++){

    printf("%d\t",(*it)->id);
    printf("%s\t",(*it)->naziv);
    printf("%x\t",(*it)->pocetakSekcije);
    printf("%x\n",(*it)->duzinaSekcije);
    


  }
}

Sekcija* tabelaSekcija::dohvatiSekciju(char* naziv){
    for(vector<Sekcija*>::iterator it = this->sekcije.begin();it < this->sekcije.end();it++){
      if(strcmp((*it)->naziv,naziv) == 0){
        return (*it);
      }
    }
    return NULL;
  }

Sekcija* tabelaSekcija::dohvatiSekcijuPoId(int id){
  return this->sekcije.at(id-1);
}

void tabelaSekcija::ispisiMasKodSveSekcije(){
  for(vector<Sekcija*>::iterator it = this->sekcije.begin();it < this->sekcije.end();it++){
    printf("%s\n",(*it)->naziv);
    (*it)->ispisiMasinskiKod();
    printf("\n___________________\n");

  }
}
