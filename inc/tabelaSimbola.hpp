#ifndef TABELASIMBOLA_HPP
#define TABELASIMBOLA_HPP
#include "simbol.hpp"
#include <vector>
using namespace std;
class tabelaSimbola{

  public:
    
      vector<Simbol*> simboli;

      tabelaSimbola();
      ~tabelaSimbola();
      void dodajSimbol(Simbol* simbol);
      void ispisiTabeluSimbola();
      Simbol* dohvatiSimbol(char* naziv);
      

  



};

#endif