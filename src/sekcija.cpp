#include "../inc/sekcija.hpp"
#include <stdio.h>


int Sekcija::prev_id = 0;

Sekcija::Sekcija(){
  this->tabLit = new tabelaLiterala();
  setDuzinaSekcije(0);
  setPocetakSekcije(0);
  setNaziv(NULL);
  this->kodSekcije = vector<unsigned char>();
  this->tabRelZap = new tabelaRelZapisa();
}

Sekcija::~Sekcija(){
  //delete this->tabLit;
  //delete this->tabRelZap;
}

void Sekcija::setNaziv(char* naziv){
  this->naziv = naziv;
}

void Sekcija::setDuzinaSekcije(int num){
  this->duzinaSekcije = num;
}

void Sekcija::setPocetakSekcije(unsigned int num){
  this->pocetakSekcije = num;
}

int Sekcija::getDuzinaSekcije(){
  return this->duzinaSekcije;
}

void Sekcija::ispraviMasinskiKod(unsigned char c,int location){
  this->kodSekcije[location] = c;
}


void Sekcija::ispisiMasinskiKod(){
  int i = 1;
  for(vector<unsigned char>::iterator it = this->kodSekcije.begin();it<this->kodSekcije.end();it++){
    printf("%02X",(*it));

    if(i%4 != 0){
      printf(" ");
    }else{
      printf("\n");
    }
    i++;

  }
}

void Sekcija::dodajMasinskiKod(unsigned char c){
  this->kodSekcije.push_back(c);
}