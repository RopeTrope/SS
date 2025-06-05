#ifndef TABELASEKCIJA_HPP
#define TABELASEKCIJA_HPP
#include "sekcija.hpp"
#include <vector>
using namespace std;
class tabelaSekcija{

  public:
    
      vector<Sekcija*> sekcije;

      tabelaSekcija();
      ~tabelaSekcija();
      void dodajSekciju(Sekcija* simbol);
      void ispisiTabeluSekcija();
      Sekcija* dohvatiSekciju(char* naziv);

      void ispisiMasKodSveSekcije();
      
      Sekcija* dohvatiSekcijuPoId(int id);




};

#endif