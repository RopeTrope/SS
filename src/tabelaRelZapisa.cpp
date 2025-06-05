#include "../inc/tabelaRelZapisa.hpp"


tabelaRelZapisa::tabelaRelZapisa(){
  this->zapisi =  vector<RelZapis*>();
}

tabelaRelZapisa::~tabelaRelZapisa(){

}


void tabelaRelZapisa::dodajZapis(RelZapis* zapis){
  this->zapisi.push_back(zapis);
}

void tabelaRelZapisa::ispisiZapise(){
  for(vector<RelZapis*>::iterator it = this->zapisi.begin();it<this->zapisi.end();it++){
    printf("Offset: %d\n",(*it)->getOffset());
    printf("Tip zapisa: %d\n",(*it)->getTipZapisa());
    printf("Naziv simbola: %s\n",(*it)->getNazivSimbola());
    printf("Addend: %d\n",(*it)->getAddend());
  }
}

RelZapis* tabelaRelZapisa::dohvatiZapis(int i){
  return this->zapisi.at(i);
}

