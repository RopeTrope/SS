#include "../inc/asembler.hpp"
#include "asembler.hpp"
#include <fstream>
using namespace std;


//Konstruktor napravi tabelu simbola i sekcija i postavi trenutnu sekciju na NULL
//locationCounter na 0 jer pocinje da broji tek
Asembler::Asembler(){
  this->ts = new tabelaSimbola();
  this->tsek = new tabelaSekcija();
  this->trenutnaSekcija = NULL;
  locationCounter = 0;
}

Asembler::~Asembler(){
  delete ts;
  delete tsek;
}

void Asembler::prviProlazakAsemblera(){
  //Ako je fajl prazan greska
  if(globHead == NULL){
    printf("PRAZAN FAJl\n");
    return;
  }
int i  = 0;

  //Obrada svih instrukcija
  for(struct instrukcija* pom = globHead;pom!= NULL;pom=pom->next){
    //Ako je direktiva
    if(pom->direktiva){
      
       i = obradDirektivu(pom);

  //Ako je labela
    }else if(pom->labela){
       i = obradiLabelu(pom);
    }else{
      //Ako je instrukcija
       i = obradiInstrukciju(pom);
    }
  
  //Ovo je ako se desila neka greska u obradi instrukcije
    if(i < 0){
      exit(-1);
    }

  }
  //Ispis tabeleLiterala
  /*
  for(vector<Sekcija*>::iterator it = tsek->sekcije.begin();it<tsek->sekcije.end();it++){
    (*it)->tabLit->ispisiTabeluLiterala();
  }
*/
  proveraVelicineSekcija();
  obradiSveFlinkove();
  obradiSveFlinkoveLiteralne();

  obradiSveRelokacioneZapise();

  proveraDefinisanostiSimbola();
  //Doda se na kraj svake sekcije tabela literala i uveca se duzina sekcije
  dodajTabeluLiterala();
  //ISPIS:RELOKACIONI ZAPISI
/*
  printf("ISPISI SVE RELOK ZAPISE\n");
  for(vector<Sekcija*>::iterator it = tsek->sekcije.begin();it<tsek->sekcije.end();it++){
    (*it)->tabRelZap->ispisiZapise();
    printf("________________________\n");
  }
  printf("GOTOV prvi prolazak\n");

*/

}

void Asembler::proveraDefinisanostiSimbola(){
  for(vector<Simbol*>::iterator it = ts->simboli.begin();it<ts->simboli.end();it++){
    if((*it)->definisan == false && (*it)->externi == false && (*it)->global == true){
      printf("GRESKA SIMBOL %s NIJE DEFINISAN\n",(*it)->naziv);
      exit(-1);
    }
  }
}

void Asembler::proveraVelicineSekcija(){
  //ne sme biti veca od 4096
  for(vector<Sekcija*>::iterator it = tsek->sekcije.begin();it<tsek->sekcije.end();it++){
    if((*it)->getDuzinaSekcije() >= 4096 ){
      printf("GRESKA SEKCIJA %s JE PREVELIKA\n",(*it)->getNaziv());
      exit(-1);
    }
  }
}

void Asembler::dodajTabeluLiterala(){
  for(vector<Sekcija*>::iterator it = tsek->sekcije.begin();it<tsek->sekcije.end();it++){
    vector<Literal*> l = (*it)->tabLit->literali;
    for(auto i = l.begin();i!=l.end();i++){
      if((*i)->getDaLiJeSimbol()){
        //simbol sam
        //ako sam simbol dodam sta nule i ili njegovu vrednost ako ima?
        unsigned char c = 0x00;
        (*it)->dodajMasinskiKod(c);
        (*it)->dodajMasinskiKod(c);
        (*it)->dodajMasinskiKod(c);
        (*it)->dodajMasinskiKod(c);
        //Mora da se poveca duzina sekcije
        (*it)->setDuzinaSekcije((*it)->getDuzinaSekcije() + 4);

      }else{
        //Literal sam
        //ako sam literal samo upisem vrednost tog literala
        unsigned char c1 = (*i)->getVrednost() & 0xFF;
        unsigned char c2 = ((*i)->getVrednost()  >> 8) & 0xFF;
        unsigned char c3 = ((*i)->getVrednost()  >> 16) & 0xFF;
        unsigned char c4 = ((*i)->getVrednost()  >> 24) & 0xFF;
        (*it)->dodajMasinskiKod(c4);
        (*it)->dodajMasinskiKod(c3);
        (*it)->dodajMasinskiKod(c2);
        (*it)->dodajMasinskiKod(c1);
        //Mora i duzina da se poveca za 4
        (*it)->setDuzinaSekcije((*it)->getDuzinaSekcije() + 4);
      }
    }
  }
}

void Asembler::obradiSveFlinkove(){
  for(vector<Simbol*>::iterator it = ts->simboli.begin();it<ts->simboli.end();it++){

    while((*it)->flinkHead != NULL){
      

        if(strcmp((*it)->flinkHead->instr->naziv,"word") == 0){
          if((*it)->flinkHead->sekcija != (*it)->sekcija){
            //nista ne treba da se radi
          }else{
            unsigned char c1 = (*it)->vrednost & 0xFF;
            unsigned char c2 = ((*it)->vrednost >> 8) & 0xFF;
            unsigned char c3 = ((*it)->vrednost >> 16) & 0xFF;
            unsigned char c4 = ((*it)->vrednost >> 24) & 0xFF;
            Sekcija* sek = tsek->dohvatiSekcijuPoId((*it)->sekcija);
            sek->ispraviMasinskiKod(c4,(*it)->flinkHead->flinkLocation);
            sek->ispraviMasinskiKod(c3,(*it)->flinkHead->flinkLocation+1);
            sek->ispraviMasinskiKod(c2,(*it)->flinkHead->flinkLocation+2);
            sek->ispraviMasinskiKod(c1,(*it)->flinkHead->flinkLocation+3);
        
          
          }
          
        }
        (*it)->flinkHead = (*it)->flinkHead->next;
    }
  }
}

void Asembler::obradiSveFlinkoveLiteralne(){
  
  for(vector<Sekcija*>::iterator it = this->tsek->sekcije.begin();it<this->tsek->sekcije.end();it++){
    int pozicija = 0;
    for(vector<Literal*>::iterator it2 = (*it)->tabLit->literali.begin();it2 < (*it)->tabLit->literali.end();it2++){
       //da im odredim pozicije u bazenu
      while((*it2)->flinkHead != NULL){
        //sada za svaki literal obeadjujem svaki njegov flink i ispravljam masinski kod
        //sad ide provera koja je instrukcija u pitanju
        int vrednost = 0;
        if((*it2)->getDaLiJeSimbol()){
          //simbol
          Simbol* s = ts->dohvatiSimbol((*it2)->getNazivSimbola());
          if(s->sekcija != (*it)->id){
            //Razlicite sekcije
            vrednost = (pozicija + (*it)->getDuzinaSekcije())-((*it2)->flinkHead->flinkLocation) - 4;

          }else{
            //Iste sekcije
            vrednost = s->vrednost - ((*it2)->flinkHead->flinkLocation + 4);
          }

        }else{
          //literal
          vrednost = (pozicija + (*it)->getDuzinaSekcije())-(*it2)->flinkHead->flinkLocation - 4;
        }

          if(strcmp((*it2)->flinkHead->instr->naziv,"call") == 0){
            Sekcija* sek = tsek->dohvatiSekcijuPoId((*it2)->flinkHead->sekcija);
            if((*it2)->getDaLiJeSimbol()){
              // simbol u istoj sekciji
              Simbol* s = ts->dohvatiSimbol((*it2)->getNazivSimbola());
                if((*it)->id == s->sekcija){
                  unsigned char c1 = 0x20;
                  unsigned char c2 = 0xF0;
                  unsigned char c3 = (unsigned char)((vrednost >> 8)&0x0F);
                  unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                  (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                  (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                  (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                  (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
                }else{
                  // simbol u razlicitoj sekciji
                  unsigned char c1 = 0x21;
                  unsigned char c2 = 0xF0;
                  unsigned char c3 = ((unsigned char)(vrednost >> 8)&0x0F);
                  unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                  (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                  (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                  (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                  (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
                }
              }else{
                //literal
                unsigned char c1 = 0x21;
                unsigned char c2 = 0xF0;
                unsigned char c3 = (unsigned char)((vrednost >> 8)&0x0F);
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }
          }else if(strcmp((*it2)->flinkHead->instr->naziv,"jmp") == 0){
            Sekcija* sek = tsek->dohvatiSekcijuPoId((*it2)->flinkHead->sekcija);
            if((*it2)->getDaLiJeSimbol()){
              // simbol
              Simbol* s = ts->dohvatiSimbol((*it2)->getNazivSimbola());
               if((*it)->id == s->sekcija){
                //simbol u istoj sekciji
                unsigned char c1 = 0x30;
                unsigned char c2 = 0xF0;
                unsigned char c3 = (unsigned char)((vrednost >> 8)&0x0F);
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                  (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                  (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                  (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
               }else{
                unsigned char c1 = 0x38;
                unsigned char c2 = 0xF0;
                unsigned char c3 = (unsigned char)((vrednost >> 8)&0x0F);
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                  (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                  (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                  (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
                
               }
              }else{
                //literal
                unsigned char c1 = 0x38;
                unsigned char c2 = 0xF0;
                unsigned char c3 = (unsigned char)((vrednost >> 8)&0x0F);
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                sek->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                sek->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                sek->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                sek->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }
        }else if(strcmp((*it2)->flinkHead->instr->naziv,"beq") == 0){
            Sekcija* sek = tsek->dohvatiSekcijuPoId((*it2)->flinkHead->sekcija);
            if((*it2)->getDaLiJeSimbol()){
              // simbol
              Simbol* s = ts->dohvatiSimbol((*it2)->getNazivSimbola());
              if((*it)->id == s->sekcija){
                //simbol u istoj sekciji
                unsigned char c1 = 0x31;
                unsigned char c2 = (unsigned char)(0xF0 | ((*it2)->flinkHead->instr->oper1->brRegistra & 0x0F));
                unsigned char c3 = (unsigned char) ((0xF0 & (*it2)->flinkHead->instr->oper2->brRegistra << 4)|((vrednost >> 8)&0x0F));
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }else{
                  //simbol u razlicitoj sekciji
                unsigned char c1 = 0x39;
                unsigned char c2 = (unsigned char)(0xF0 | ((*it2)->flinkHead->instr->oper1->brRegistra & 0x0F));
                unsigned char c3 = (unsigned char) ((0xF0 & (*it2)->flinkHead->instr->oper2->brRegistra << 4)|((vrednost >> 8)&0x0F));
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                  (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                  (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                  (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }
              }else{
                //literal
                unsigned char c1 = 0x39;
                unsigned char c2 = (unsigned char)(0xF0 | ((*it2)->flinkHead->instr->oper1->brRegistra & 0x0F));
                unsigned char c3 = (unsigned char) ((0xF0 & (*it2)->flinkHead->instr->oper2->brRegistra << 4)|((vrednost >> 8)&0x0F));
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                  (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                  (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                  (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }
            }else if(strcmp((*it2)->flinkHead->instr->naziv,"bne") == 0){
              Sekcija* sek = tsek->dohvatiSekcijuPoId((*it2)->flinkHead->sekcija);
              if((*it2)->getDaLiJeSimbol()){
              // simbol
              Simbol* s = ts->dohvatiSimbol((*it2)->getNazivSimbola());
              if((*it)->id == s->sekcija){
                //simbol u istoj sekciji
                unsigned char c1 = 0x32;
                unsigned char c2 = (unsigned char)(0xF0 | ((*it2)->flinkHead->instr->oper1->brRegistra & 0x0F));
                unsigned char c3 = (unsigned char) ((0xF0 & (*it2)->flinkHead->instr->oper2->brRegistra << 4)|((vrednost >> 8)&0x0F));
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

               (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                  (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                  (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                  (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }else{
                  //simbol u razlicitoj sekciji
                unsigned char c1 = 0x3A;
                unsigned char c2 = (unsigned char)(0xF0 | ((*it2)->flinkHead->instr->oper1->brRegistra & 0x0F));
                unsigned char c3 = (unsigned char) ((0xF0 & (*it2)->flinkHead->instr->oper2->brRegistra << 4)|((vrednost >> 8)&0x0F));
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                  (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                  (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                  (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }
              }else{
                //literal
                unsigned char c1 = 0x3A;
                unsigned char c2 = (unsigned char)(0xF0 | ((*it2)->flinkHead->instr->oper1->brRegistra & 0x0F));
                unsigned char c3 = (unsigned char) ((0xF0 & (*it2)->flinkHead->instr->oper2->brRegistra << 4)|((vrednost >> 8)&0x0F));
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                  (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                  (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                  (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }

            }else if(strcmp((*it2)->flinkHead->instr->naziv,"bgt") == 0){
              Sekcija* sek = tsek->dohvatiSekcijuPoId((*it2)->flinkHead->sekcija);
              if((*it2)->getDaLiJeSimbol()){
              // simbol
              Simbol* s = ts->dohvatiSimbol((*it2)->getNazivSimbola());
              if((*it)->id == s->sekcija){
                //simbol u istoj sekciji
                unsigned char c1 = 0x33;
                unsigned char c2 = (unsigned char)(0xF0 | ((*it2)->flinkHead->instr->oper1->brRegistra & 0x0F));
                unsigned char c3 = (unsigned char) ((0xF0 & (*it2)->flinkHead->instr->oper2->brRegistra << 4)|((vrednost >> 8)&0x0F));
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                  (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                  (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                  (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }else{
                  //simbol u razlicitoj sekciji
                unsigned char c1 = 0x3B;
                unsigned char c2 = (unsigned char)(0xF0 | ((*it2)->flinkHead->instr->oper1->brRegistra & 0x0F));
                unsigned char c3 = (unsigned char) ((0xF0 & (*it2)->flinkHead->instr->oper2->brRegistra << 4)|((vrednost >> 8)&0x0F));
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                  (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                  (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                  (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }
              }else{
                //literal
                unsigned char c1 = 0x3B;
                unsigned char c2 = (unsigned char)(0xF0 | ((*it2)->flinkHead->instr->oper1->brRegistra & 0x0F));
                unsigned char c3 = (unsigned char) ((0xF0 & (*it2)->flinkHead->instr->oper2->brRegistra << 4)|((vrednost >> 8)&0x0F));
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }
            }else if(strcmp((*it2)->flinkHead->instr->naziv,"ld") == 0){
              vrednost = (pozicija + (*it)->getDuzinaSekcije())-((*it2)->flinkHead->flinkLocation) - 4;
              if((*it2)->flinkHead->instr->oper3->adr == ADR::IMMED){

                unsigned char c1 = 0x92;
                unsigned char c2 = (unsigned char)(((*it2)->flinkHead->instr->oper1->brRegistra<<4 & 0xF0)| 0x0F);
                unsigned char c3 = (unsigned char) ((vrednost >> 8)&0x0F);
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);
              }else{
                //MEMDIR
                

                unsigned char c1 = 0x92;
                unsigned char c2 = (unsigned char)(((*it2)->flinkHead->instr->oper1->brRegistra <<4 & 0xF0) | 0x0F);
                unsigned char c3 = (unsigned char)((vrednost >> 8)&0x0F);
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);

                 c1 = 0x92;
                 c2 = (unsigned char)(((*it2)->flinkHead->instr->oper1->brRegistra <<4 & 0xF0) | ((*it2)->flinkHead->instr->oper1->brRegistra  & 0x0F));
                 c3 = 0x00;
                 c4 = 0x00;

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation+4);
                (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+5);
                (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+6);
                (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+7);


        
              }
            }else if(strcmp((*it2)->flinkHead->instr->naziv,"st")==0){
              vrednost = (pozicija + (*it)->getDuzinaSekcije())-((*it2)->flinkHead->flinkLocation) - 4;
                unsigned char c1 = 0x82;
                unsigned char c2 = (unsigned char)(0xF0);
                unsigned char c3 = (unsigned char)(((*it2)->flinkHead->instr->oper1->brRegistra << 4 &0xF0)|((vrednost >> 8)&0x0F));
                unsigned char c4 = (unsigned char)(vrednost & 0xFF);

                (*it)->ispraviMasinskiKod(c1,(*it2)->flinkHead->flinkLocation);
                (*it)->ispraviMasinskiKod(c2,(*it2)->flinkHead->flinkLocation+1);
                (*it)->ispraviMasinskiKod(c3,(*it2)->flinkHead->flinkLocation+2);
                (*it)->ispraviMasinskiKod(c4,(*it2)->flinkHead->flinkLocation+3);

            }
      (*it2)->flinkHead = (*it2)->flinkHead->next;
      }
      pozicija+=4;
    }

  }
}

void Asembler::obradiSveRelokacioneZapise(){
  for(vector<Sekcija*>::iterator it = tsek->sekcije.begin();it<tsek->sekcije.end();it++){
    for(vector<RelZapis*>::iterator it2 = (*it)->tabRelZap->zapisi.begin();it2<(*it)->tabRelZap->zapisi.end();){
      Simbol* s = ts->dohvatiSimbol((*it2)->getNazivSimbola());
      if(!s->definisan && !s->externi){
        printf("GRESKA SIMBOL NIJE DEFINISAN!\n");
        exit(-1);
      }
      if(!s->definisan && s->global && !s->externi){
        printf("Greska simbol je globalan ,a nije definisan\n");
        exit(-1);
      }

      if((*it2)->getTipZapisa() == TipRelZapisa::PC32){
        if(s->sekcija == (*it)->id){
          //uklanjanje relokacionog zapisa

         (*it)->tabRelZap->zapisi.erase(it2);
          
        }else{
          //razlicita sekcija -4, ovde spada i extern
          if(s->global){ 
            (*it2)->setAddend(-4);
          }else{
            (*it2)->setAddend(s->vrednost-4);
            Sekcija* sek = tsek->dohvatiSekcijuPoId(s->sekcija);
            (*it2)->setNazivSimbola(sek->getNaziv());
          }
          it2++;
        }
      }else{
        //apsolutno adresiranje 
        if(s->global){
          (*it2)->setAddend(0);
        }else{
          (*it2)->setAddend(s->vrednost);
          Sekcija* sek = tsek->dohvatiSekcijuPoId(s->sekcija);
          (*it2)->setNazivSimbola(sek->getNaziv());
        }
        it2++;
      }
    }
  }
}


int Asembler::obradDirektivu(struct instrukcija* instr){

  //#Global
  if(strcmp(instr->naziv,"global") == 0){
    //Ide redom po simbolima
    for(struct operand* pom = instr->oper1;pom != NULL;pom = pom->next){
      //Ako je sekcija prosledjena mesto simbola
      Sekcija* sek = tsek->dohvatiSekciju(pom->simbol);
      if(sek){
        printf("GRESKA SEKCIJI SE DODELJUJE GLOBAL!\n");
        return -1;//ovde je greska
      }
      //Dohvata se simbol iz tabele simbola
      Simbol* dohvSimb = ts->dohvatiSimbol(pom->simbol);
      if(dohvSimb == NULL){
        //Ako ne postoji simbol u tabeli simbola
        //Mora da se napravi novi simbol i doda u tabelu simbola
        Simbol* s = new Simbol();
        s->setGlobal(true);
        s->setDefinisan(false);
        s->setNaziv(pom->simbol);
        s->setSekcija(-1);// "UND sekcija"
        ts->dodajSimbol(s);
        
      }else{
        //Ako postoji samo se postavlja global na true
        dohvSimb->setGlobal(true);
      }
    }

  //#Extern
  }else if(strcmp(instr->naziv,"extern") == 0){

    for(struct operand* pom = instr->oper1;pom != NULL;pom = pom->next){
      //Opet pokusaj sekciji da se dodeli extern
      Sekcija* sek = tsek->dohvatiSekciju(pom->simbol);
      if(sek){
        printf("Ne moze sekcija da bude extern!\n");
        return -2; 
      }
      //Dohvatanje simbola iz tabele simbola
      Simbol* dohvSimb = ts->dohvatiSimbol(pom->simbol);
      if(dohvSimb == NULL){
        //Ako ne postoji simbol u tabeli simbola
        Simbol* s = new Simbol();
        s->setExterni(true);
        s->setNaziv(pom->simbol);
        s->setGlobal(true);
        s->setVelicina(0);
        s->setVrednost(0);
        s->setDefinisan(false);
        s->setSekcija(-1); // "UND sekcija"
        ts->dodajSimbol(s);
      }else{
        //Ako je simbol definisan vec u sekciji i naknadno se stavi extern
        //to je greska 
        if(dohvSimb->definisan){
          printf("GRESKA SIMBOL JE VEC DEFINISAN!\n");
          return -3;
        }

        //Simbol je vec u tabeli simbola i ali nije definisan
        dohvSimb->setExterni(true);
        dohvSimb->setVelicina(0);
        dohvSimb->setVrednost(0);
        dohvSimb->setSekcija(-1);
        dohvSimb->setGlobal(true);
      }
    }
  //#Word
  }else if(strcmp(instr->naziv,"word") == 0){

    //Word ne sme da se zove ako nije otvorena sekcija
    if(trenutnaSekcija == NULL){
      printf("NIJEDNA SEKCIJA NIJE OTVORENA\n");
      return -3;
    }
    for(struct operand* pom = instr->oper1;pom != NULL;pom = pom->next){
      if(pom->simbol != NULL){
        //Ako je simbol
      Simbol* dohvSimb = ts->dohvatiSimbol(pom->simbol);
      if(dohvSimb == NULL){

        //Ako taj simbol ne postoji u tabeli simbola
        Simbol* s = new Simbol();
        s->setNaziv(pom->simbol);
        //s->setSekcija(trenutnaSekcija->id);
        s->setDefinisan(false);
        //Doda se flink koji ce posle da se obradi
        s->dodajFlink(instr,locationCounter,trenutnaSekcija->id);
        //4 puta 0x00 jer mora da se obradi rel zapis
        unsigned char c1 = 0x00;
        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c1);
        ts->dodajSimbol(s);

        RelZapis* rz = new RelZapis();
        rz->setOffset(locationCounter);
        rz->setTipZapisa(TipRelZapisa::R_X86_64_32);
        rz->setAddend(0);
        rz->setNazivSimbola(s->naziv);
        trenutnaSekcija->tabRelZap->dodajZapis(rz);

        printf("Dodat je simbol %s\n",s->naziv);

      }else{

        //Simbol postoji u tabeli simbola ali nije definisan
        if(!dohvSimb->definisan){
          dohvSimb->dodajFlink(instr,locationCounter,trenutnaSekcija->id);
          //upisem nule na to mesto
          unsigned char c1 = 0x00;
          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c1);
        }else{
          //Simbol je definisan
          if(dohvSimb->sekcija != trenutnaSekcija->id){
            unsigned char c1 = 0x00;
            trenutnaSekcija->dodajMasinskiKod(c1);
            trenutnaSekcija->dodajMasinskiKod(c1);
            trenutnaSekcija->dodajMasinskiKod(c1);
            trenutnaSekcija->dodajMasinskiKod(c1);
          }else{
            //Vrednost simbola pravim u masinskom kodu
            //Simbol je definisan u toj sekciji vec i onda mu znam vrednost
            unsigned char c1 = dohvSimb->vrednost & 0xFF;
            unsigned char c2 = (dohvSimb->vrednost >> 8) & 0xFF;
            unsigned char c3 = (dohvSimb->vrednost >> 16) & 0xFF;
            unsigned char c4 = (dohvSimb->vrednost >> 24) & 0xFF;
            trenutnaSekcija->dodajMasinskiKod(c4);
            trenutnaSekcija->dodajMasinskiKod(c3);
            trenutnaSekcija->dodajMasinskiKod(c2);
            trenutnaSekcija->dodajMasinskiKod(c1);
          }
        } 

        RelZapis* rz = new RelZapis();
        rz->setOffset(locationCounter);
        rz->setTipZapisa(TipRelZapisa::R_X86_64_32);
        rz->setAddend(0);
        rz->setNazivSimbola(dohvSimb->naziv);
        trenutnaSekcija->tabRelZap->dodajZapis(rz);

       
        //SVAKAKO MORA DA SE NAPRAVI RELOKACIONI ZAPIS BEZ OBZIRA JE L DEF ILI NE!!!
      }
      
    }else{
      //LITERAL
      unsigned char c1 = pom->literal & 0xFF;
      unsigned char c2 = (pom->literal >> 8) & 0xFF;
      unsigned char c3 = (pom->literal >> 16) & 0xFF;
      unsigned char c4 = (pom->literal >> 24) & 0xFF;
      trenutnaSekcija->dodajMasinskiKod(c4);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c1);

    }

      locationCounter += 4;
    }

  //#Section
  }else if(strcmp(instr->naziv,"section") == 0){
    Simbol* s = ts->dohvatiSimbol(instr->oper1->simbol);
    if(s!= NULL){
      //Simbol je vec definisan sa tim imenom
      printf("GRESKA SEKCIJA POSTOJI U TABELI SIMBOLA!\n");
      return -4;
    }
    //Trenutna sekcija je gotova i treba da se postavi duzina sekcije
    if(trenutnaSekcija){
      trenutnaSekcija->setDuzinaSekcije(locationCounter);
    }
    Sekcija* sek = tsek->dohvatiSekciju(instr->oper1->simbol);
    if(!sek){
      //Ukoliko sekcija ne postoji u tabeli sekcija
      sek = new Sekcija();
      sek->setNaziv(instr->oper1->simbol);
      if(trenutnaSekcija){
        //Postavi se da je pocetak na kraju prethodne sekcije
        sek->setPocetakSekcije(trenutnaSekcija->getPocetakSekcije()+trenutnaSekcija->getDuzinaSekcije());
      }else{
        //LocationCounter je vec na 0 
        sek->setPocetakSekcije(locationCounter);
      }
      sek->setDuzinaSekcije(0);
      locationCounter = 0;
      tsek->dodajSekciju(sek);
      trenutnaSekcija = sek;
    }else{
      printf("GRESKA SEKCIJA SA TIM IMENOM VEC POSTOJI!\n");
      return -5;
    }
    //Moram da napravim simbol koji je ustv sekcija
    s = new Simbol();
    s->setNaziv(instr->oper1->simbol);
    s->setSekcija(sek->id);
    s->setVelicina(0);
    s->setDefinisan(true);
    s->setVrednost(0);
    s->setJesamSekcija(true);
    ts->dodajSimbol(s);
  //#End
  }else if(strcmp(instr->naziv,"end") == 0){
    if(!trenutnaSekcija){
      printf("NIJEDNA SEKCIJA NIJE OTVORENA!\n");
      return -6;
    }

    trenutnaSekcija->setDuzinaSekcije(locationCounter);
    struct instrukcija* pom = instr;
    struct instrukcija* pom2 = instr->next;
    while(pom->next != NULL){
      pom = pom2;
      pom2 = pom2->next;
      delete pom;
    }
    instr->next = NULL;


    return 0;
  //#Skip
  }else if(strcmp(instr->naziv,"skip") == 0){
    if(!trenutnaSekcija){
      printf("NIJEDNA SEKCIJA NIJE OTVORENA!\n");
      return -7;
    }
    locationCounter += instr->oper1->literal;
    unsigned char c = 0x00;
    for(int i =0;i<instr->oper1->literal;i++){
      trenutnaSekcija->dodajMasinskiKod(c);
    }
    


  }

  return 0;

}


int Asembler::obradiLabelu(struct instrukcija* instr){


  Simbol* s = ts->dohvatiSimbol(instr->naziv);
  if(s){
     if(s->definisan){
      printf("GRESKA SIMBOL JE VEC DEFINISAN!\n");
      return -8;
     }else if(s->externi){
      //Ne moze ni da bude externi i da se definise
        printf("GRESKA SIMBOL JE EXTERNI!\n");
        return -9;
      }else{
        
        //Ovde ga definisem jer je upisan u tabelu simbola a nije definisan
      s->setVrednost(locationCounter);
      s->setDefinisan(true);
      s->setSekcija(trenutnaSekcija->id);
     }
  }else{
    //Ako ne postoji simbol sad je vreme da se definise
    Simbol* simb = new Simbol();
    simb->setNaziv(instr->naziv);
    simb->setVrednost(locationCounter);
    simb->setDefinisan(true);
    simb->setVelicina(0);
    simb->setSekcija(trenutnaSekcija->id);
    ts->dodajSimbol(simb);
  }
  return 0;

} 

int Asembler::obradiInstrukciju(struct instrukcija* instr){
  if(trenutnaSekcija == NULL){
    printf("NIJEDNA SEKCIJA NIJE OTVORENA!\n");
    return -10;
  }
  //#Halt
 if(strcmp(instr->naziv,"halt") == 0){
  //Generise se kraj programa
  unsigned char c1 = 0x00;
  trenutnaSekcija->dodajMasinskiKod(c1);
  trenutnaSekcija->dodajMasinskiKod(c1);
  trenutnaSekcija->dodajMasinskiKod(c1);
  trenutnaSekcija->dodajMasinskiKod(c1);
  //#Int
 }else if(strcmp(instr->naziv,"int") == 0){
  //Generisu se int instrukcije
    unsigned char c1 = 0x10;
    unsigned char c2 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c2);
  //#Iret
    }else if(strcmp(instr->naziv,"iret") == 0){
    
    //pc=<mem[sp], sp=<sp+8
    unsigned char c1=0x93;
    unsigned char c2=0xFE;
     unsigned char c3=0x00;
    unsigned char c4=0x08;
    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    locationCounter+=4;
    //pop status
     c1=0x96;
     c2=0x0E;
     c3=0x00;
     c4=0x04;
    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    
    }
    else if(strcmp(instr->naziv,"call") == 0){

        if(instr->oper1->adr != ADR::MEMDIR || instr->oper1->brRegistra > -1){
          printf("POGRESAN TIP OPERANDA SE PROSLEDJUJE!\n");
          return -1;
        }

      if(instr->oper1->simbol != NULL){
        //ako je simbol
        Simbol* s = ts->dohvatiSimbol(instr->oper1->simbol);
        if(s!=NULL){
          //ako postoji u tabeli simbola
          Literal* l = trenutnaSekcija->tabLit->dohvatiSimbol(instr->oper1->simbol);

          if(l == NULL){       
            //ne postoji u tabeli literala ali postoji u tabeli simbola
            l = new Literal();
            l->setSimbol(true);
            l->setNazivSimbola(instr->oper1->simbol);
            l->setVrednost(s->vrednost);
            trenutnaSekcija->tabLit->dodajLiteral(l);
          }

          l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

          if(s->sekcija != trenutnaSekcija->id){
            RelZapis* rz = new RelZapis();
            rz->setAddend(0);
            rz->setNazivSimbola(s->naziv);
            rz->setOffset(locationCounter);
            rz->setTipZapisa(TipRelZapisa::PC32);
            trenutnaSekcija->tabRelZap->dodajZapis(rz);
          }


        }else{
          //ako ne postoji u tabeli simbola
          s = new Simbol();
          s->setDefinisan(false);
          s->setGlobal(false);
          s->setExterni(false);
          s->setSekcija(-1);
          s->setVelicina(0);
          s->setVrednost(0);
          s->setJesamSekcija(false);
          s->setNaziv(instr->oper1->simbol);
          ts->dodajSimbol(s);
          Literal* lit = new Literal();
          lit->setNazivSimbola(instr->oper1->simbol);
          lit->setVrednost(0);
          lit->setSimbol(true);
          trenutnaSekcija->tabLit->dodajLiteral(lit);
          lit->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
          RelZapis* rz = new RelZapis();
          rz->setAddend(0);
          rz->setNazivSimbola(s->naziv);
          rz->setOffset(locationCounter);
          rz->setTipZapisa(TipRelZapisa::PC32);
          trenutnaSekcija->tabRelZap->dodajZapis(rz);
        }

        unsigned char c1 = 0x20;
        unsigned char c2 = 0xF0;
        unsigned char c3 = 0x00;
        unsigned char c4 = 0x00;

        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);

      }else{
        //ako je literal

        if(instr->oper1->literal < 4096 && instr->oper1->literal >= -4096){
          unsigned char c1 = 0x20;
          unsigned char c2 = 0xF0;
          unsigned char c3 = (unsigned char) ((0x0F & (instr->oper1->literal >>8 )));
          unsigned char c4 = (unsigned char) (instr->oper1->literal & 0xFF);

          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c2);
          trenutnaSekcija->dodajMasinskiKod(c3);
          trenutnaSekcija->dodajMasinskiKod(c4);
        }else{
        Literal* l = trenutnaSekcija->tabLit->dohvatiLiteral(instr->oper1->literal);

        if(l == NULL){
          //literal ne postoji u tabeli literala
          l = new Literal();
          l->setVrednost(instr->oper1->literal);
          l->setSimbol(false);
          trenutnaSekcija->tabLit->dodajLiteral(l);
        }

        l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
        unsigned char c1 = 0x20;
        unsigned char c2 = 0xF0;
        unsigned char c3 = 0x00;
        unsigned char c4 = 0x00;

        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);

      }
      }
   
      

    }
    //#Ret
    else if(strcmp(instr->naziv,"ret") == 0){

      unsigned char c1 = 0x93;
      unsigned char c2 = 0xFE;
      unsigned char c3 = 0x00;
      unsigned char c4 = 0x04;
      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);
      //#Jmp
    }else if(strcmp(instr->naziv,"jmp") == 0){
      
        if(instr->oper1->adr != ADR::MEMDIR || instr->oper1->brRegistra > -1){
          printf("POGRESAN TIP OPERANDA SE PROSLEDJUJE!\n");
          return -1;
        }

      if(instr->oper1->simbol != NULL){
        //ako je simbol
        Simbol* s = ts->dohvatiSimbol(instr->oper1->simbol);
        if(s!=NULL){
          //ako postoji u tabeli simbola
          Literal* l = trenutnaSekcija->tabLit->dohvatiSimbol(instr->oper1->simbol);

          if(l == NULL){       
            //ne postoji u tabeli literala ali postoji u tabeli simbola
            l = new Literal();
            l->setSimbol(true);
            l->setNazivSimbola(instr->oper1->simbol);
            l->setVrednost(s->vrednost);
            trenutnaSekcija->tabLit->dodajLiteral(l);
          }

          l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

          if(s->sekcija != trenutnaSekcija->id){
            RelZapis* rz = new RelZapis();
            rz->setAddend(0);
            rz->setNazivSimbola(s->naziv);
            rz->setOffset(locationCounter);
            rz->setTipZapisa(TipRelZapisa::PC32);
            trenutnaSekcija->tabRelZap->dodajZapis(rz);
          }


        }else{
          //ako ne postoji u tabeli simbola
          s = new Simbol();
          s->setDefinisan(false);
          s->setGlobal(false);
          s->setExterni(false);
          s->setSekcija(-1);
          s->setVelicina(0);
          s->setVrednost(0);
          s->setJesamSekcija(false);
          s->setNaziv(instr->oper1->simbol);
          ts->dodajSimbol(s);
          Literal* lit = new Literal();
          lit->setNazivSimbola(instr->oper1->simbol);
          lit->setVrednost(0);
          lit->setSimbol(true);
          trenutnaSekcija->tabLit->dodajLiteral(lit);
          lit->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
          RelZapis* rz = new RelZapis();
          rz->setAddend(0);
          rz->setNazivSimbola(s->naziv);
          rz->setOffset(locationCounter);
          rz->setTipZapisa(TipRelZapisa::PC32);
          trenutnaSekcija->tabRelZap->dodajZapis(rz);
        }
        unsigned char c1 = 0x30;
        unsigned char c2 = 0xF0;
        unsigned char c3 = 0x00;
        unsigned char c4 = 0x00;

        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);
      }else{
        //ako je literal
          if(instr->oper1->literal < 4096 && instr->oper1->literal >= -4096){
          unsigned char c1 = 0x30;
          unsigned char c2 = 0x00;
          unsigned char c3 = (unsigned char) ((0x0F & (instr->oper1->literal >>8 )));
          unsigned char c4 = (unsigned char) (instr->oper1->literal & 0xFF);

          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c2);
          trenutnaSekcija->dodajMasinskiKod(c3);
          trenutnaSekcija->dodajMasinskiKod(c4);
        }else{

        Literal* l = trenutnaSekcija->tabLit->dohvatiLiteral(instr->oper1->literal);

        if(l == NULL){
          //literal ne postoji u tabeli literala
          l = new Literal();
          l->setVrednost(instr->oper1->literal);
          l->setSimbol(false);
          trenutnaSekcija->tabLit->dodajLiteral(l);
        }

        l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
        unsigned char c1 = 0x30;
        unsigned char c2 = 0xF0;
        unsigned char c3 = 0x00;
        unsigned char c4 = 0x00;

        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);

      }
      }
      
    //#Beq    
    }else if(strcmp(instr->naziv,"beq") == 0){
      
        if(instr->oper3->adr != ADR::MEMDIR || instr->oper3->brRegistra > -1){
          printf("POGRESAN TIP OPERANDA SE PROSLEDJUJE!\n");
          return -1;
        }

      if(instr->oper3->simbol != NULL){
        //ako je simbol
        Simbol* s = ts->dohvatiSimbol(instr->oper3->simbol);
        if(s!=NULL){
          //ako postoji u tabeli simbola
          Literal* l = trenutnaSekcija->tabLit->dohvatiSimbol(instr->oper3->simbol);

          if(l == NULL){       
            //ne postoji u tabeli literala ali postoji u tabeli simbola
            l = new Literal();
            l->setSimbol(true);
            l->setNazivSimbola(instr->oper3->simbol);
            l->setVrednost(s->vrednost);
            trenutnaSekcija->tabLit->dodajLiteral(l);
          }

          l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

          if(s->sekcija != trenutnaSekcija->id){
            RelZapis* rz = new RelZapis();
            rz->setAddend(0);
            rz->setNazivSimbola(s->naziv);
            rz->setOffset(locationCounter);
            rz->setTipZapisa(TipRelZapisa::PC32);
            trenutnaSekcija->tabRelZap->dodajZapis(rz);
          }


        }else{
          //ako ne postoji u tabeli simbola
          s = new Simbol();
          s->setDefinisan(false);
          s->setGlobal(false);
          s->setExterni(false);
          s->setSekcija(-1);
          s->setVelicina(0);
          s->setVrednost(0);
          s->setJesamSekcija(false);
          s->setNaziv(instr->oper3->simbol);
          ts->dodajSimbol(s);
          Literal* lit = new Literal();
          lit->setNazivSimbola(instr->oper3->simbol);
          lit->setVrednost(0);
          lit->setSimbol(true);
          trenutnaSekcija->tabLit->dodajLiteral(lit);
          lit->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
          RelZapis* rz = new RelZapis();
          rz->setAddend(0);
          rz->setNazivSimbola(s->naziv);
          rz->setOffset(locationCounter);
          rz->setTipZapisa(TipRelZapisa::PC32);
          trenutnaSekcija->tabRelZap->dodajZapis(rz);
        }
        unsigned char c1 = 0x31;
        unsigned char c2 = (unsigned char)((0xF0 | (instr->oper1->brRegistra & 0x0F)));
        unsigned char c3 = (unsigned char)(((instr->oper2->brRegistra << 4) & 0xF0 ));
        unsigned char c4 = 0x00;


        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);

      }else{
        //ako je literal
        if(instr->oper3->literal < 4096 && instr->oper3->literal >= -4096){
          unsigned char c1 = 0x31;
          unsigned char c2 = (unsigned char)((instr->oper1->brRegistra & 0x0F));
          unsigned char c3 = (unsigned char)(((instr->oper2->brRegistra << 4) & 0xF0 ) | (0x0F & (instr->oper3->literal >>8 )));
          unsigned char c4 = (unsigned char) (instr->oper3->literal & 0xFF);

          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c2);
          trenutnaSekcija->dodajMasinskiKod(c3);
          trenutnaSekcija->dodajMasinskiKod(c4);
        }else {
        Literal* l = trenutnaSekcija->tabLit->dohvatiLiteral(instr->oper3->literal);

        if(l == NULL){
          //literal ne postoji u tabeli literala
          l = new Literal();
          l->setVrednost(instr->oper3->literal);
          l->setSimbol(false);
          trenutnaSekcija->tabLit->dodajLiteral(l);
        }

        l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

        unsigned char c1 = 0x31;
        unsigned char c2 = (unsigned char)((0xF0 | (instr->oper1->brRegistra & 0x0F)));
        unsigned char c3 = (unsigned char)(((instr->oper2->brRegistra << 4) & 0xF0 ));
        unsigned char c4 = 0x00;


        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);
      }
      }
      
      //#Bne
    }else if(strcmp(instr->naziv,"bne") == 0){
      
        if(instr->oper3->adr != ADR::MEMDIR || instr->oper3->brRegistra > -1){
          printf("POGRESAN TIP OPERANDA SE PROSLEDJUJE!\n");
          return -1;
        }

      if(instr->oper3->simbol != NULL){
        //ako je simbol
        Simbol* s = ts->dohvatiSimbol(instr->oper3->simbol);
        if(s!=NULL){
          //ako postoji u tabeli simbola
          Literal* l = trenutnaSekcija->tabLit->dohvatiSimbol(instr->oper3->simbol);

          if(l == NULL){       
            //ne postoji u tabeli literala ali postoji u tabeli simbola
            l = new Literal();
            l->setSimbol(true);
            l->setNazivSimbola(instr->oper3->simbol);
            l->setVrednost(s->vrednost);
            trenutnaSekcija->tabLit->dodajLiteral(l);
          }

          l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

          if(s->sekcija != trenutnaSekcija->id){
            RelZapis* rz = new RelZapis();
            rz->setAddend(0);
            rz->setNazivSimbola(s->naziv);
            rz->setOffset(locationCounter);
            rz->setTipZapisa(TipRelZapisa::PC32);
            trenutnaSekcija->tabRelZap->dodajZapis(rz);
          }
          unsigned char c1 = 0x32;
          unsigned char c2 = (unsigned char)((0xF0 | (instr->oper1->brRegistra & 0x0F))& 0xFF);
          unsigned char c3 = (unsigned char)(((instr->oper2->brRegistra << 4) & 0xF0 ));
          unsigned char c4 = 0x00;


          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c2);
          trenutnaSekcija->dodajMasinskiKod(c3);
          trenutnaSekcija->dodajMasinskiKod(c4);

        }else{
          //ako ne postoji u tabeli simbola
          s = new Simbol();
          s->setDefinisan(false);
          s->setGlobal(false);
          s->setExterni(false);
          s->setSekcija(-1);
          s->setVelicina(0);
          s->setVrednost(0);
          s->setJesamSekcija(false);
          s->setNaziv(instr->oper3->simbol);
          ts->dodajSimbol(s);
          Literal* lit = new Literal();
          lit->setNazivSimbola(instr->oper3->simbol);
          lit->setVrednost(0);
          lit->setSimbol(true);
          trenutnaSekcija->tabLit->dodajLiteral(lit);
          lit->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
          RelZapis* rz = new RelZapis();
          rz->setAddend(0);
          rz->setNazivSimbola(s->naziv);
          rz->setOffset(locationCounter);
          rz->setTipZapisa(TipRelZapisa::PC32);
          trenutnaSekcija->tabRelZap->dodajZapis(rz);
        }

      }else{
        //ako je literal

        if(instr->oper3->literal < 4096 && instr->oper3->literal >= -4096){
          unsigned char c1 = 0x32;
          unsigned char c2 = (unsigned char)((instr->oper1->brRegistra & 0x0F));
          unsigned char c3 = (unsigned char)(((instr->oper2->brRegistra << 4) & 0xF0 ) | (0x0F & (instr->oper3->literal >>8 )));
          unsigned char c4 = (unsigned char) (instr->oper3->literal & 0xFF);

          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c2);
          trenutnaSekcija->dodajMasinskiKod(c3);
          trenutnaSekcija->dodajMasinskiKod(c4);
        }else{
        Literal* l = trenutnaSekcija->tabLit->dohvatiLiteral(instr->oper3->literal);

        if(l == NULL){
          //literal ne postoji u tabeli literala
          l = new Literal();
          l->setVrednost(instr->oper3->literal);
          l->setSimbol(false);
          trenutnaSekcija->tabLit->dodajLiteral(l);
        }

        l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
        unsigned char c1 = 0x32;
        unsigned char c2 = (unsigned char)((0xF0 | (instr->oper1->brRegistra & 0x0F))& 0xFF);
        unsigned char c3 = (unsigned char)(((instr->oper2->brRegistra << 4) & 0xF0 ));
        unsigned char c4 = 0x00;


        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);
      }
      }
      
      
      //#Bgt
    }else if(strcmp(instr->naziv,"bgt") == 0){
      
        if(instr->oper3->adr != ADR::MEMDIR || instr->oper3->brRegistra > -1){
          printf("POGRESAN TIP OPERANDA SE PROSLEDJUJE!\n");
          return -1;
        }

      if(instr->oper3->simbol != NULL){
        //ako je simbol
        Simbol* s = ts->dohvatiSimbol(instr->oper3->simbol);
        if(s!=NULL){
          //ako postoji u tabeli simbola
          Literal* l = trenutnaSekcija->tabLit->dohvatiSimbol(instr->oper3->simbol);

          if(l == NULL){       
            //ne postoji u tabeli literala ali postoji u tabeli simbola
            l = new Literal();
            l->setSimbol(true);
            l->setNazivSimbola(instr->oper3->simbol);
            l->setVrednost(s->vrednost);
            trenutnaSekcija->tabLit->dodajLiteral(l);
          }

          l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

          if(s->sekcija != trenutnaSekcija->id){
            RelZapis* rz = new RelZapis();
            rz->setAddend(0);
            rz->setNazivSimbola(s->naziv);
            rz->setOffset(locationCounter);
            rz->setTipZapisa(TipRelZapisa::PC32);
            trenutnaSekcija->tabRelZap->dodajZapis(rz);
          }


        }else{
          //ako ne postoji u tabeli simbola
          s = new Simbol();
          s->setDefinisan(false);
          s->setGlobal(false);
          s->setExterni(false);
          s->setSekcija(-1);
          s->setVelicina(0);
          s->setVrednost(0);
          s->setJesamSekcija(false);
          s->setNaziv(instr->oper3->simbol);
          ts->dodajSimbol(s);
          Literal* lit = new Literal();
          lit->setNazivSimbola(instr->oper3->simbol);
          lit->setVrednost(0);
          lit->setSimbol(true);
          trenutnaSekcija->tabLit->dodajLiteral(lit);
          lit->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
          RelZapis* rz = new RelZapis();
          rz->setAddend(0);
          rz->setNazivSimbola(s->naziv);
          rz->setOffset(locationCounter);
          rz->setTipZapisa(TipRelZapisa::PC32);
          trenutnaSekcija->tabRelZap->dodajZapis(rz);
        }

        unsigned char c1 = 0x33;
        unsigned char c2 = (unsigned char)((0xF0 | (instr->oper1->brRegistra & 0x0F))& 0xFF);
        unsigned char c3 = (unsigned char)(((instr->oper2->brRegistra << 4) & 0xF0 ));
        unsigned char c4 = 0x00;


        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);

      }else{
        //ako je literal
        if(instr->oper3->literal < 4096 && instr->oper3->literal >= -4096){
          unsigned char c1 = 0x33;
          unsigned char c2 = (unsigned char)((instr->oper1->brRegistra & 0x0F));
          unsigned char c3 = (unsigned char)(((instr->oper2->brRegistra << 4) & 0xF0 ) | (0x0F & (instr->oper3->literal >>8 )));
          unsigned char c4 = (unsigned char) (instr->oper3->literal & 0xFF);

          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c2);
          trenutnaSekcija->dodajMasinskiKod(c3);
          trenutnaSekcija->dodajMasinskiKod(c4);
        }else {
        Literal* l = trenutnaSekcija->tabLit->dohvatiLiteral(instr->oper3->literal);

        if(l == NULL){
          //literal ne postoji u tabeli literala
          l = new Literal();
          l->setVrednost(instr->oper3->literal);
          l->setSimbol(false);
          trenutnaSekcija->tabLit->dodajLiteral(l);
        }

        l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

        unsigned char c1 = 0x33;
        unsigned char c2 = (unsigned char)((0xF0 | (instr->oper1->brRegistra & 0x0F))& 0xFF);
        unsigned char c3 = (unsigned char)(((instr->oper2->brRegistra << 4) & 0xF0 ));
        unsigned char c4 = 0x00;


        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);
      }
      }
      
      
    }
    //#Push
    else if(strcmp(instr->naziv,"push")==0){
      unsigned char c1 = 0x81;
      unsigned char c2 = 0xE0;
      unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4)|0x0F);
      unsigned char c4 = 0xFC;
      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);
      //#Pop
    }else if(strcmp(instr->naziv,"pop")==0){
      unsigned char c1 = 0x93;
      unsigned char c2 = (unsigned char)((instr->oper1->brRegistra << 4)|(0x0E));
      unsigned char c3 = 0x00;
      unsigned char c4 = 0x04;

      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);

    }else if(strcmp(instr->naziv,"xchg")==0){

      unsigned char c1 = 0x40;
      unsigned char c2 = (unsigned char)(instr->oper2->brRegistra & 0x0F);
      unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4) & 0xF0);
      unsigned char c4 = 0x00;
      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);
  //#Add
    }else if(strcmp(instr->naziv,"add") == 0){
    unsigned char c1 = 0x50;
    unsigned char c2 = (unsigned char)(((instr->oper2->brRegistra << 4)|(instr->oper2->brRegistra))&0xFF);
    unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4)&0xF0);
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);



  //#Sub
  }else if(strcmp(instr->naziv,"sub") == 0){
    unsigned char c1 = 0x51;
    unsigned char c2 = (unsigned char)(((instr->oper2->brRegistra << 4)|(instr->oper2->brRegistra))&0xFF);
    unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4)&0xF0);
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    //#Mul
  }else if(strcmp(instr->naziv,"mul") == 0){
    unsigned char c1 = 0x52;
    unsigned char c2 = (unsigned char)(((instr->oper2->brRegistra << 4)|(instr->oper2->brRegistra))&0xFF);
    unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4)&0xF0);
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    //#Div
  }else if(strcmp(instr->naziv,"div") == 0){
    unsigned char c1 = 0x53;
    unsigned char c2 = (unsigned char)(((instr->oper2->brRegistra << 4)|(instr->oper2->brRegistra))&0xFF);
    unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4)&0xF0);
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    //#Not
  }else if(strcmp(instr->naziv,"not") == 0){
    unsigned char c1 = 0x60;
    unsigned char c2 = (unsigned char)(((instr->oper1->brRegistra << 4)|(instr->oper1->brRegistra))&0xFF);
    unsigned char c3 = 0x00;
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    //#And
  }else if(strcmp(instr->naziv,"and") == 0){
    unsigned char c1 = 0x61;
    unsigned char c2 = (unsigned char)(((instr->oper2->brRegistra << 4)|(instr->oper2->brRegistra))&0xFF);
    unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4)&0xF0);
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    //#Or
  }else if(strcmp(instr->naziv,"or") == 0){
    unsigned char c1 = 0x62;
    unsigned char c2 = (unsigned char)(((instr->oper2->brRegistra << 4)|(instr->oper2->brRegistra))&0xFF);
    unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4)&0xF0);
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    //#Xor
  }else if(strcmp(instr->naziv,"xor") == 0){

    unsigned char c1 = 0x63;
    unsigned char c2 = (unsigned char)(((instr->oper2->brRegistra << 4)|(instr->oper2->brRegistra))&0xFF);
    unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4)&0xF0);
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    //#Shl
  }else if(strcmp(instr->naziv,"shl") == 0){
    unsigned char c1 = 0x70;
    unsigned char c2 = (unsigned char)(((instr->oper2->brRegistra << 4)|(instr->oper2->brRegistra))&0xFF);
    unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4)&0xF0);
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    //#Shr
  }else if(strcmp(instr->naziv,"shr") == 0){
    unsigned char c1 = 0x71;
    unsigned char c2 = (unsigned char)(((instr->oper2->brRegistra << 4)|(instr->oper2->brRegistra))&0xFF);
    unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4)&0xF0);
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    //#Csrrd
  }else if(strcmp(instr->naziv,"csrrd") == 0){
    unsigned char c1 = 0x90;
    unsigned char c2 = (unsigned char)((instr->oper1->brRegistra << 4)&0xF0)|(instr->oper3->sreg & 0x0F);
    unsigned char c3 = 0x00;
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    //#Csrrw
  }else if(strcmp(instr->naziv,"csrwr") == 0){

    unsigned char c1 = 0x94;
    unsigned char c2 = (unsigned char)(((instr->oper3->sreg << 4)&0xF0)|(instr->oper1->brRegistra & 0x0F))&0xFF;
    unsigned char c3 = 0x00;
    unsigned char c4 = 0x00;

    trenutnaSekcija->dodajMasinskiKod(c1);
    trenutnaSekcija->dodajMasinskiKod(c2);
    trenutnaSekcija->dodajMasinskiKod(c3);
    trenutnaSekcija->dodajMasinskiKod(c4);
    //#Ld
  }else if(strcmp(instr->naziv,"ld") == 0){
    //registarsko direktno
    if(instr->oper3->adr == ADR::REGDIR){
      unsigned char c1 = 0x91;
      unsigned char c2 = (unsigned char)(((instr->oper1->brRegistra << 4) & 0xF0) | (instr->oper3->brRegistra & 0x0F));
      unsigned char c3 = 0x00;
      unsigned char c4 = 0x00;

      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);
    }else if(instr->oper3->adr == ADR::REGIND){
      //registarsko indirektno
      unsigned char c1 = 0x92;
      unsigned char c2 = (unsigned char)(((instr->oper1->brRegistra << 4) & 0xF0) | (instr->oper3->brRegistra & 0x0F));
      unsigned char c3 = 0x00;
      unsigned char c4 = 0x00;

      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);
    }else if(instr->oper3->adr == ADR::REGINDPOM){
      if(instr->oper3->simbol == NULL){
        //literal
        if(instr->oper3->literal > 4095){
          printf("Prevelik pomeraj za instrukciju ld!\n");
          return -1;
        }
      unsigned char c1 = 0x92;
      unsigned char c2 = (unsigned char)(((instr->oper1->brRegistra << 4) & 0xF0) | (instr->oper3->brRegistra & 0x0F));
      unsigned char c3 = (unsigned char)((instr->oper3->literal >> 8) & 0x0F);
      unsigned char c4 = (unsigned char)(instr->oper3->literal & 0xFF);

      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);



      }else{
        //simbol
        //nema .equ direktive na nivou a
        printf("Ne postoji simbol koji je poznat u trenutku asembliranja!\n");
        return -5;
      }
    }else if(instr->oper3->adr == ADR::IMMED){
      //neposredno
       if(instr->oper3->simbol != NULL){
        //ako je simbol
        Simbol* s = ts->dohvatiSimbol(instr->oper3->simbol);
        if(s!=NULL){
          //ako postoji u tabeli simbola
          Literal* l = trenutnaSekcija->tabLit->dohvatiSimbol(instr->oper3->simbol);

          if(l == NULL){       
            //ne postoji u tabeli literala ali postoji u tabeli simbola
            l = new Literal();
            l->setSimbol(true);
            l->setNazivSimbola(instr->oper3->simbol);
            l->setVrednost(s->vrednost);
            trenutnaSekcija->tabLit->dodajLiteral(l);
          }

          l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

          if(s->sekcija != trenutnaSekcija->id){
            RelZapis* rz = new RelZapis();
            rz->setAddend(0);
            rz->setNazivSimbola(s->naziv);
            rz->setOffset(locationCounter);
            rz->setTipZapisa(TipRelZapisa::PC32);
            trenutnaSekcija->tabRelZap->dodajZapis(rz);
          }


        }else{
          //ako ne postoji u tabeli simbola
          s = new Simbol();
          s->setDefinisan(false);
          s->setGlobal(false);
          s->setExterni(false);
          s->setSekcija(-1);
          s->setVelicina(0);
          s->setVrednost(0);
          s->setJesamSekcija(false);
          s->setNaziv(instr->oper3->simbol);
          ts->dodajSimbol(s);
          Literal* lit = new Literal();
          lit->setNazivSimbola(instr->oper3->simbol);
          lit->setVrednost(0);
          lit->setSimbol(true);
          trenutnaSekcija->tabLit->dodajLiteral(lit);
          lit->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
          RelZapis* rz = new RelZapis();
          rz->setAddend(0);
          rz->setNazivSimbola(s->naziv);
          rz->setOffset(locationCounter);
          rz->setTipZapisa(TipRelZapisa::PC32);
          trenutnaSekcija->tabRelZap->dodajZapis(rz);
        }

        unsigned char c1 = 0x92;
        unsigned char c2 = (unsigned char)(((instr->oper1->brRegistra << 4) & 0xF0) | 0x0F);
        unsigned char c3 = 0x00;
        unsigned char c4 = 0x00;

        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);

      }else{
        //ako je literal
        Literal* l = trenutnaSekcija->tabLit->dohvatiLiteral(instr->oper3->literal);
        if(instr->oper3->literal < 4096 && instr->oper3->literal >= -4096){
          unsigned char c1 = 0x91;
          unsigned char c2 = (unsigned char)((instr->oper1->brRegistra << 4) & 0xF0);
          unsigned char c3 = (unsigned char)((instr->oper3->literal>>8) & 0x0F);
          unsigned char c4 = (unsigned char)(instr->oper3->literal & 0xFF);

          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c2);
          trenutnaSekcija->dodajMasinskiKod(c3);
          trenutnaSekcija->dodajMasinskiKod(c4);

        }else{

        if(l == NULL){
          //literal ne postoji u tabeli literala
          l = new Literal();
          l->setVrednost(instr->oper3->literal);
          l->setSimbol(false);
          trenutnaSekcija->tabLit->dodajLiteral(l);
        }

        l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
        unsigned char c1 = 0x92;
        unsigned char c2 = (unsigned char)(((instr->oper1->brRegistra << 4) & 0xF0) | 0x0F);
        unsigned char c3 = 0x00;
        unsigned char c4 = 0x00;

        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);

      }
      }
    }else if(instr->oper3->adr == ADR::MEMDIR){
      //MEMDIR
       if(instr->oper3->simbol != NULL){
        //ako je simbol
        Simbol* s = ts->dohvatiSimbol(instr->oper3->simbol);
        if(s!=NULL){
          //ako postoji u tabeli simbola
          Literal* l = trenutnaSekcija->tabLit->dohvatiSimbol(instr->oper3->simbol);

          if(l == NULL){       
            //ne postoji u tabeli literala ali postoji u tabeli simbola
            l = new Literal();
            l->setSimbol(true);
            l->setNazivSimbola(instr->oper3->simbol);
            l->setVrednost(s->vrednost);
            trenutnaSekcija->tabLit->dodajLiteral(l);
          }

          l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

          if(s->sekcija != trenutnaSekcija->id){
            RelZapis* rz = new RelZapis();
            rz->setAddend(0);
            rz->setNazivSimbola(s->naziv);
            rz->setOffset(locationCounter);
            rz->setTipZapisa(TipRelZapisa::PC32);
            trenutnaSekcija->tabRelZap->dodajZapis(rz);
          }


        }else{
          //ako ne postoji u tabeli simbola
          s = new Simbol();
          s->setDefinisan(false);
          s->setGlobal(false);
          s->setExterni(false);
          s->setSekcija(-1);
          s->setVelicina(0);
          s->setVrednost(0);
          s->setJesamSekcija(false);
          s->setNaziv(instr->oper3->simbol);
          ts->dodajSimbol(s);
          Literal* lit = new Literal();
          lit->setNazivSimbola(instr->oper3->simbol);
          lit->setVrednost(0);
          lit->setSimbol(true);
          trenutnaSekcija->tabLit->dodajLiteral(lit);
          lit->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
          RelZapis* rz = new RelZapis();
          rz->setAddend(0);
          rz->setNazivSimbola(s->naziv);
          rz->setOffset(locationCounter);
          rz->setTipZapisa(TipRelZapisa::PC32);
          trenutnaSekcija->tabRelZap->dodajZapis(rz);
        }
        unsigned char c1 = 0x92;
      unsigned char c2  = (unsigned char) (((instr->oper1->brRegistra << 4) & 0xF0) | 0x0F);
      unsigned char c3 = 0x00;
      unsigned char c4 = 0x00;

      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);

      c1 = 0x92;
      c2 = (unsigned char) (((instr->oper1->brRegistra << 4) & 0xF0) | (instr->oper1->brRegistra & 0x0F));
      c3 = 0x00;
      c4 = 0x00;

      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);

      }else{
        //ako je literal
        if(instr->oper3->literal < 4096 && instr->oper3->literal >= -4096){
          //literal je manji od 2^12
          unsigned char c1 = 0x92;
          unsigned char c2  = (unsigned char) (((instr->oper1->brRegistra << 4) & 0xF0));
          unsigned char c3 = (unsigned char) ((instr->oper3->literal >> 8) & 0x0F);
          unsigned char c4 = (unsigned char) (instr->oper3->literal & 0xFF);

          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c2);
          trenutnaSekcija->dodajMasinskiKod(c3);
          trenutnaSekcija->dodajMasinskiKod(c4);
          
        }else{
        Literal* l = trenutnaSekcija->tabLit->dohvatiLiteral(instr->oper3->literal);

        if(l == NULL){
          //literal ne postoji u tabeli literala
          l = new Literal();
          l->setVrednost(instr->oper3->literal);
          l->setSimbol(false);
          trenutnaSekcija->tabLit->dodajLiteral(l);
        }

        l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

        unsigned char c1 = 0x92;
        unsigned char c2  = (unsigned char) (((instr->oper1->brRegistra << 4) & 0xF0) | 0x0F);
        unsigned char c3 = 0x00;
        unsigned char c4 = 0x00;

        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);

        c1 = 0x92;
        c2 = (unsigned char) (((instr->oper1->brRegistra << 4) & 0xF0) | (instr->oper1->brRegistra & 0x0F));
        c3 = 0x00;
        c4 = 0x00;

        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);

      }
      
      } 
      locationCounter+=4;
    }
    //#St
  }else if(strcmp(instr->naziv,"st") == 0){
    //registarsko direktno
    if(instr->oper3->adr == ADR::REGDIR){
      unsigned char c1 = 0x91;
      unsigned char c2 = (unsigned char)(((instr->oper3->brRegistra << 4) & 0xF0) | (instr->oper1->brRegistra & 0x0F));
      unsigned char c3 = 0x00;
      unsigned char c4 = 0x00;

      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);
    }else if(instr->oper3->adr == ADR::REGIND){
      //registarsko indirektno
      unsigned char c1 = 0x80;
      unsigned char c2 = (unsigned char)((instr->oper3->brRegistra << 4) & 0xF0);
      unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4) & 0xF0);
      unsigned char c4 = 0x00;

      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);
    }else if(instr->oper3->adr == ADR::REGINDPOM){
      if(instr->oper3->simbol == NULL){
        //literal
        if(instr->oper3->literal > 4095){
          printf("Prevelik pomeraj za instrukciju ld!\n");
          return -1;
        }
      unsigned char c1 = 0x80;
      unsigned char c2 = (unsigned char)((instr->oper3->brRegistra << 4) & 0xF0);
      unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4 & 0xF0)|(instr->oper3->literal >> 8) & 0x0F);
      unsigned char c4 = (unsigned char)(instr->oper3->literal & 0xFF);

      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);



      }else{
        //simbol
        //nema .equ direktive na nivou a
        printf("Ne postoji simbol koji je poznat u trenutku asembliranja!\n");
        return -5;
      }
    }else if(instr->oper3->adr == ADR::IMMED){
      printf("GRESKA STORE NE MOZE DA SE IZVRSI SA NEPOSREDNIM OPERANDOM!\n");
      return -1;
    }else if(instr->oper3->adr == ADR::MEMDIR){
      //MEMDIR
       if(instr->oper3->simbol != NULL){
        //ako je simbol
        Simbol* s = ts->dohvatiSimbol(instr->oper3->simbol);
        if(s!=NULL){
          //ako postoji u tabeli simbola
          Literal* l = trenutnaSekcija->tabLit->dohvatiSimbol(instr->oper3->simbol);

          if(l == NULL){       
            //ne postoji u tabeli literala ali postoji u tabeli simbola
            l = new Literal();
            l->setSimbol(true);
            l->setNazivSimbola(instr->oper3->simbol);
            l->setVrednost(s->vrednost);
            trenutnaSekcija->tabLit->dodajLiteral(l);
          }

          l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

          if(s->sekcija != trenutnaSekcija->id){
            RelZapis* rz = new RelZapis();
            rz->setAddend(0);
            rz->setNazivSimbola(s->naziv);
            rz->setOffset(locationCounter);
            rz->setTipZapisa(TipRelZapisa::PC32);
            trenutnaSekcija->tabRelZap->dodajZapis(rz);
          }


        }else{
          //ako ne postoji u tabeli simbola
          s = new Simbol();
          s->setDefinisan(false);
          s->setGlobal(false);
          s->setExterni(false);
          s->setSekcija(-1);
          s->setVelicina(0);
          s->setVrednost(0);
          s->setJesamSekcija(false);
          s->setNaziv(instr->oper3->simbol);
          ts->dodajSimbol(s);
          Literal* lit = new Literal();
          lit->setNazivSimbola(instr->oper3->simbol);
          lit->setVrednost(0);
          lit->setSimbol(true);
          trenutnaSekcija->tabLit->dodajLiteral(lit);
          lit->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);
          RelZapis* rz = new RelZapis();
          rz->setAddend(0);
          rz->setNazivSimbola(s->naziv);
          rz->setOffset(locationCounter);
          rz->setTipZapisa(TipRelZapisa::PC32);
          trenutnaSekcija->tabRelZap->dodajZapis(rz);
        }
      unsigned char c1 = 0x82;
      unsigned char c2  = 0xF0;
      unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4) & 0xF0);
      unsigned char c4 = 0x00;

      trenutnaSekcija->dodajMasinskiKod(c1);
      trenutnaSekcija->dodajMasinskiKod(c2);
      trenutnaSekcija->dodajMasinskiKod(c3);
      trenutnaSekcija->dodajMasinskiKod(c4);

      }else{
        //ako je literal
        if(instr->oper3->literal < 4096 && instr->oper3->literal >= -4096){
          //literal je manji od 2^12
          unsigned char c1 = 0x80;
          unsigned char c2  =0x00;
          unsigned char c3 = (unsigned char) (((instr->oper1->brRegistra << 4)&0xF0)|(instr->oper3->literal >> 8) & 0x0F);
          unsigned char c4 = (unsigned char) (instr->oper3->literal & 0xFF);

          trenutnaSekcija->dodajMasinskiKod(c1);
          trenutnaSekcija->dodajMasinskiKod(c2);
          trenutnaSekcija->dodajMasinskiKod(c3);
          trenutnaSekcija->dodajMasinskiKod(c4);
          
        }else{
        Literal* l = trenutnaSekcija->tabLit->dohvatiLiteral(instr->oper3->literal);

        if(l == NULL){
          //literal ne postoji u tabeli literala
          l = new Literal();
          l->setVrednost(instr->oper3->literal);
          l->setSimbol(false);
          trenutnaSekcija->tabLit->dodajLiteral(l);
        }

        l->dodajFlinkLit(instr,locationCounter,trenutnaSekcija->id);

        unsigned char c1 = 0x82;
        unsigned char c2  = 0xF0;
        unsigned char c3 = (unsigned char)((instr->oper1->brRegistra << 4) & 0xF0);
        unsigned char c4 = 0x00;

        trenutnaSekcija->dodajMasinskiKod(c1);
        trenutnaSekcija->dodajMasinskiKod(c2);
        trenutnaSekcija->dodajMasinskiKod(c3);
        trenutnaSekcija->dodajMasinskiKod(c4);

      }
      
      } 
    }
    
  }
  locationCounter += 4;
  return 0;
} 

int Asembler::upisiUFajl(char* nazivFajla){
    
    ofstream fajl(nazivFajla,ios::binary);

    

    fajl << tsek->sekcije.size() << endl;//broj sekcija
    fajl << "ID" << "Naziv"<< "Pocetak" << "Duzina"<<endl;
  //sve sekcije
    for(vector<Sekcija*>::iterator it = tsek->sekcije.begin(); it != tsek->sekcije.end(); it++){
        Sekcija* s = *it;
        fajl << s->id<< " " << s->getNaziv()<< " "  << s->getPocetakSekcije()<< " "  << s->getDuzinaSekcije()<< " " << endl;
    }


    fajl << ts->simboli.size() << endl;//broj simbola
    fajl << "ID" << "Vrednost" << "Velicina" <<  "GLOB/LOC"<< "Externi" << "Sekcija" << "Naziv" << endl;
    //svi simboli
    for(vector<Simbol*>::iterator it = ts->simboli.begin(); it != ts->simboli.end(); it++){
        Simbol* s = *it;
        fajl << s->getId()<< " "  << s->vrednost<< " " << s->velicina<< " "  << s->global<< " "  << s->externi << " " << s->sekcija<< " " << s->naziv << endl;
    }


    for(vector<Sekcija*>::iterator it = tsek->sekcije.begin(); it != tsek->sekcije.end(); it++){
        Sekcija* s = *it;
        fajl << s->id << endl;//id sekcije
        fajl << s->tabRelZap->zapisi.size() << endl;//broj rel zapisa
        fajl <<  "Offset" << "Tip" << "Naziv"<<"Addend" << endl;
        for(vector<RelZapis*>::iterator it2 = s->tabRelZap->zapisi.begin(); it2 != s->tabRelZap->zapisi.end(); it2++){
            RelZapis* rz = *it2;
            fajl << rz->getOffset() << " " << rz->getTipZapisa() << " " << rz->getNazivSimbola()<< " "<<rz->getAddend() << endl;
        }
    }


    for(vector<Sekcija*>::iterator it = tsek->sekcije.begin();it != tsek->sekcije.end();it++){
        Sekcija* s = *it;
        fajl <<  s->id << endl;//id sekcije
        fajl << s->kodSekcije.size() << endl;//duzina koda sekcije
        for(vector<unsigned char>::iterator it2 = s->kodSekcije.begin(); it2 != s->kodSekcije.end(); it2++){
            char c1[5];
            sprintf(c1,"%02X",*it2);
            fajl << string(c1) << " ";
        }
        fajl << endl;
    }


    fajl.close();

    return 0;

}
