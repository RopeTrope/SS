#ifndef SEKCIJA_HPP
#define SEKCIJA_HPP

#include "tabelaLiterala.hpp"
#include "tabelaRelZapisa.hpp"
#include <vector>

class Sekcija{

private:
friend class tabelaSekcija;
char* naziv;
unsigned int pocetakSekcije;
int duzinaSekcije;







public:
  vector <unsigned char> kodSekcije;
  tabelaLiterala* tabLit;
  int id = ++prev_id;
  Sekcija();
  tabelaRelZapisa* tabRelZap;

  static int prev_id;

  ~Sekcija();

  void setNaziv(char* naziv);

  void setPocetakSekcije(unsigned int num);
  void setDuzinaSekcije(int num);
  int getDuzinaSekcije();

  unsigned int getPocetakSekcije(){
    return this->pocetakSekcije;
  }

  void ispisiMasinskiKod();
  
  void dodajMasinskiKod(unsigned char c);

  void ispraviMasinskiKod(unsigned char c,int location);



  char* getNaziv(){
    return this->naziv;
  }

};

#endif  