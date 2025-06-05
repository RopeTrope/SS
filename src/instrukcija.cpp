#include "../inc/instrukcija.hpp"


struct instrukcija* globHead = NULL;
struct instrukcija* globTail = NULL;


//Napravi instrukciju na osnovu parametara i uveze u globalnu listu instrukcija
void napravi_instrukciju(char* n,struct operand* o1,struct operand* o2,struct operand* o3){

    struct instrukcija* instr = (struct instrukcija*) malloc(sizeof(struct instrukcija));
    instr->naziv = n;
    instr->oper1 = o1;
    instr->oper2 = o2;
    instr->oper3 = o3;
    instr->direktiva = false;
    instr->labela = false;
    instr->next = NULL;

    if(globHead == NULL){
      globHead = globTail = instr;
    }else {
      globTail->next = instr;
      globTail = instr;
    }


}

//Vadim instrukciju iz globalne liste instrukcija od pocetka tj sa head-a
struct instrukcija* izvadi_instrukciju(){

  if(globHead == NULL){
    return NULL;
  }

  struct instrukcija* i = globHead;
  globHead = globHead->next;
  if(globHead == NULL){
    globTail = NULL;
  }

  return i;

}

//Napravi operand na osnovu parametara
struct operand* napravi_operand(int regBr,char* simb,char* l,ADR a){
  struct operand* op = (struct operand*) malloc(sizeof(struct operand));
  op->brRegistra = regBr;
  op->simbol = simb;
  
  if(l != NULL){
    
    int lit = (int)atoi(l);
    op->literal = lit;
  }
  op->adr = a;
  op->next = NULL;
  return op;
}
//Napravi operand koji je specijalni registar
struct operand* napraviSpecRegOperand(char* sr){
  struct operand* op = (struct operand*) malloc(sizeof(struct operand)); 
  if(strcmp(sr,"handler")==0){
    op->sreg = SREG::HANDLER;
  }else if(strcmp(sr,"cause") == 0){
    op->sreg = SREG::CAUSE;
  }else if(strcmp(sr,"status") == 0){
    op->sreg = SREG::STATUS;
  }

  op->adr = ADR::REGDIR;

  return op;
}
//Specijalni tip instrukcije koji je ustv direktiva pa ima bool direktiva = true
void napravi_direktivu(char* naziv,struct operand* op){
  
  struct instrukcija* instr = (struct instrukcija*)malloc(sizeof(struct instrukcija));
  instr->naziv = naziv;
  instr->oper1 = op;
  instr->direktiva = true;
  instr->next = NULL;

  if(globHead == NULL){
      globHead = globTail = instr;
    }else {
      globTail->next = instr;
      globTail = instr;
    }

}

//Napravi labelu koja je specijalni tip instrukcije pa ima bool labela = true
void napravi_labelu(char* naziv){
  struct instrukcija* instr = (struct instrukcija*)malloc(sizeof(struct instrukcija));
  naziv[strlen(naziv)-1] = '\0';
  instr->naziv = naziv;
  instr->labela = true;
  instr->direktiva = false;
  instr->oper1 = NULL;
  instr->oper2 = NULL;
  instr->oper3 = NULL;

  if(globHead == NULL){
      globHead = globTail = instr;
    }else {
      globTail->next = instr;
      globTail = instr;
    }

}