#ifndef ASEMBLER_HPP
#define ASEMBLER_HPP
#include "instrukcija.hpp"
#include "tabelaSimbola.hpp"
#include "tabelaSekcija.hpp"
#include <stdio.h>



class Asembler{

  public:
   int locationCounter;

   tabelaSimbola* ts;

   tabelaSekcija* tsek;


  Sekcija* trenutnaSekcija;
   Asembler();

   ~Asembler();

  void prviProlazakAsemblera();

  void ispisiSveFlinkove();

  int obradDirektivu(struct instrukcija* instr);

  int obradiLabelu(struct instrukcija* instr);

  int obradiInstrukciju(struct instrukcija* instr);

  void obradiSveFlinkove();

  void obradiSveFlinkoveLiteralne();

  void obradiSveRelokacioneZapise();

  void proveraVelicineSekcija();

  int upisiUFajl(char* izlazniFajl);

  void proveraDefinisanostiSimbola();

  void dodajTabeluLiterala();

};

#endif