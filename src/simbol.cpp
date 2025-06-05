#include "../inc/simbol.hpp"
#include <stdio.h>
#include <stdlib.h>


Simbol::Simbol(){
    
    this->naziv = NULL;
    this->vrednost = 0; 
    this->velicina = 0;
    this->sekcija = -1;
    this->global = false;
    this->externi = false;
    this->definisan = false;
    this->flinkHead = NULL;
    this->flinkTail = NULL;

} 

Simbol::~Simbol(){
    struct flink* f = this->flinkHead;
    while(f != NULL){
        struct flink* tmp = f;
        f = f->next;
        free(tmp);
    }
}

void Simbol::setNaziv(char* naziv){
    this->naziv = naziv;
} 

void Simbol::setVrednost(int vrednost){
    this->vrednost = vrednost;
} 

void Simbol::setVelicina(int velicina){
    this->velicina = velicina;
} 


void Simbol::setSekcija(int sekcija){
    this->sekcija = sekcija;
}


void Simbol::setGlobal(bool global){
    this->global = global;
} 

void Simbol::setExterni(bool externi){
    this->externi = externi;
} 

void Simbol::setDefinisan(bool definisan){
    this->definisan = definisan;
} 

void Simbol::dodajFlink(struct instrukcija* instr,int locationCount,int sekcija){
    struct flink* f = (struct flink*)malloc(sizeof(struct flink));
    f->flinkLocation = locationCount;
    f->sekcija = sekcija;
    f->instr = instr;
    f->next = NULL;
    if(this->flinkHead == NULL){
        
        this->flinkHead = this->flinkTail = f;

    }else{
        this->flinkTail->next = f;
        this->flinkTail = f;
        
    }
}

int Simbol::prev_id = 0;



