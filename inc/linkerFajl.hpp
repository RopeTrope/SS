#ifndef LINKERFAJL_HPP
#define LINKERFAJL_HPP
#include "tabelaSimbola.hpp"
#include "sekcija.hpp"
#include "relokacioniZapis.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

class LinkerFajl{
private:

  vector<Simbol*> simboli;
  vector<Sekcija*> sekcije;

  



public:

  static int brojSekcije;//renumeracija sekcija
  LinkerFajl();

  ~LinkerFajl();

  void procitajFajl(char* nazivFajla);

  Sekcija* napraviSekciju(vector<string> vec);

  Simbol* napraviSimbol(vector<string> vec);

  RelZapis* napraviRelZapis(vector<string> vec);

  void ispisiSekcije();

  void numerisanjeSekcija();

  void ispraviTabeluSimbola();

  void ispisiSimbole();

  Simbol* dohvatiSimbolPoNazivu(char* naziv);

  void ispisiRelZapise();

  bool dohvatiSekcijuUTabeli(Sekcija* s);

  Sekcija* dohvatiSekcijuNaziv(char* naziv);

  vector<Sekcija*> dohvatiTabeluSekcije(){
    return this->sekcije;
  }

  vector<Simbol*> dohvatiTabeluSimbola(){
    return this->simboli;
  }

  void dodajSekciju(Sekcija* sek){
    sekcije.push_back(sek);
  }

  vector<RelZapis*> dohvatiTabeluRelZapisa(Sekcija* s){
    return s->tabRelZap->zapisi;
  }

  Sekcija* dohvatiSekcijuPoBroju(int broj){
    for(vector<Sekcija*>::iterator it = sekcije.begin();it!=sekcije.end();it++){
      if((*it)->id == broj){
        return *it;
      }
    }
    return NULL;
  }





};

#endif