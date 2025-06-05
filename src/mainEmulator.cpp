#include "../inc/emulator.hpp"


int main(int argc, char* argv[]){
  Emulator* e =new  Emulator();
  e->procitajFajl(argv[1]);
  e->pokreniEmulaciju();
  //e->ispisiMemoriju();
  e->ispisiRegistre();
  
  delete e;
  return 0;
}