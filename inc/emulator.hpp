#ifndef EMULATOR_HPP
#define EMULATOR_HPP
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;


class Emulator{


  public:
  int reg[16];
  int creg[3];
  Emulator();
  ~Emulator();
  void procitajFajl(char* fajl);
  map<unsigned int  ,unsigned char> memorija;

  void inicijalizujMemoriju(vector<string> tokens);
  void pokreniEmulaciju();

  void prekidGreska();

  void ispisiMemoriju();

  void ispisiRegistre();

  void ispisiStek();

  void ispisiOCIMod(int OC,int Mod);




};


#endif