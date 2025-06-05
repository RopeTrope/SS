#ifndef INSTRUKCIJA_HPP
#define INSTRUKCIJA_HPP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


//Tip adresiranja
enum ADR{
  REGDIR,
  REGIND,
  IMMED,
  MEMDIR,
  REGINDPOM
};

//Specijalni registri
enum SREG{

  STATUS,
  HANDLER,
  CAUSE

};

struct operand{
  int brRegistra;
  char* simbol;
  int literal;
  ADR adr;
  SREG sreg;

  operand* next;
};


struct instrukcija{

char* naziv;
operand* oper1;
operand* oper2;
operand* oper3;
bool direktiva; // da li je direktiva
bool labela; // da li je labela

instrukcija* next;

};

extern struct  instrukcija* globHead ;
extern struct instrukcija* globTail ;


void napravi_instrukciju(char* n,struct operand* o1,struct operand* o2,struct operand* o3);

struct instrukcija* izvadi_instrukciju();

struct operand* napravi_operand(int regBr,char* simb,char* l,ADR a);

struct operand* napraviSpecRegOperand(char* sr);

void napravi_direktivu(char* naziv,struct operand* op);

void napravi_labelu(char* naziv);

#endif