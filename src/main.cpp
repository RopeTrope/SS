
#include "../misc/lexer.hpp"
#include "../misc/parser.hpp"
#include "../inc/instrukcija.hpp"
#include "../inc/asembler.hpp"
#include <stdio.h>
#include "../inc/tabelaSimbola.hpp"

extern int yyparse(void);
extern FILE* yyin;


int main(int argc, char** argv){

  if(argc < 4){
    return -1;
  }

  if(strcmp(argv[1],"-o") != 0){
    return -1;
  }

  FILE* file = fopen(argv[3],"r");
  if(!file){
    return -2;
  }

  yyin = file;

  int i = yyparse();
  if(i!= 0){
    printf("GRESKA PRI PARSIRANJU!\n");
    return -3;
  }



  Asembler* a = new Asembler();
  a->prviProlazakAsemblera();

  
/*
  a->ts->ispisiTabeluSimbola();

  a->tsek->ispisiTabeluSekcija();

  a->tsek->ispisiMasKodSveSekcije();

*/
  a->upisiUFajl(argv[2]);

  delete a;
  return 0;

  

}