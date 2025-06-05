#include "../inc/literal.hpp"
#include <stdio.h>
#include <stdlib.h>

int Literal::prev_id = 0;

Literal::Literal(){
    this->nazivSimbola = NULL;
    this->vrednost = 0;
    this->pozicijaBazen = -1;
    this->id = prev_id++;
    this->simbol = false;
    this->flinkHead = NULL;
    this->flinkTail = NULL;
}

Literal::~Literal(){
    struct flinkLit* f = this->flinkHead;
    while(f != NULL){
        struct flinkLit* tmp = f;
        f = f->next;
        free(tmp);
    }
    this->flinkHead = NULL;
    this->flinkTail = NULL;
}

void Literal::setVrednost(int vred){
  this->vrednost = vred;
}

void Literal::setPozicijaBazen(int poz){
  this->pozicijaBazen = poz;
}

int Literal::getVrednost(){
  return this->vrednost;
}

int Literal::getPozicijaBazen(){
  return this->pozicijaBazen;
}

int Literal::getId(){
  return this->id;
}

bool Literal::getDaLiJeSimbol(){
  return this->simbol;
}

void Literal::setSimbol(bool b){
    this->simbol = b;
}

void Literal::dodajFlinkLit(struct instrukcija* instr,int locationCount,int sekcija){
    struct flinkLit* f = (struct flinkLit*)malloc(sizeof(struct flinkLit));
    f->flinkLocation = locationCount;
    f->sekcija = sekcija;
    f->instr = instr;
    f->next = NULL;
    if(this->flinkHead == NULL){
        
        this->flinkHead = this->flinkTail = f;

    }else{
        this->flinkTail->next = f;
        this->flinkTail = this->flinkTail->next;
        
    }
}

char* Literal::getNazivSimbola(){
  return  this->nazivSimbola;
}