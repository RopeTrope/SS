#include "../inc/literal.hpp"
#include <cstring>
#include <stdio.h>
#include <vector>
using namespace std;
class tabelaLiterala{

  private:
  
  

  int duzinaTabele;

  public:

  vector <Literal*> literali;

  void ispisiTabeluLiterala();

  Literal* dohvatiLiteral(int vred);

  void dodajLiteral(Literal* l);

  Literal* dohvatiSimbol(char* naziv);

  



};