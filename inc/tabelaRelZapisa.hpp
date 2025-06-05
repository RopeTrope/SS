#ifndef TABELARELZAPISA_HPP
#define TABELARELZAPISA_HPP
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "relokacioniZapis.hpp"
#include <vector>
using namespace std;
class tabelaRelZapisa{


  public:


  vector<RelZapis*> zapisi;

  tabelaRelZapisa();

  ~tabelaRelZapisa();

  void dodajZapis(RelZapis* zapis);

  void ispisiZapise();

  RelZapis* dohvatiZapis(int i);




};





#endif