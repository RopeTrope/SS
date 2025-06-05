#include "../inc/linkerFajl.hpp"

using namespace std;

int LinkerFajl::brojSekcije = 1;

LinkerFajl::LinkerFajl(){
  this->sekcije = vector<Sekcija*>();
  this->simboli = vector<Simbol*>();
}

LinkerFajl::~LinkerFajl(){
  
}
//Citanje fajla koji je napravio asembler i obrada podataka
void LinkerFajl::procitajFajl(char* nazivFajla){
  ifstream file(nazivFajla,ios::binary);
  if(!file.is_open()){
    cout << "Greska pri otvaranju fajla" << endl;
    return;
  }
  //pravljenje sekcija
  vector<string> tokens = vector<string>();
  string line;
  getline(file,line);
  int brojSekcija = stoi(line);
//broj sekcija
  getline(file,line); //zaglavlje
for(int i=0;i<brojSekcija;i++){
    getline(file,line);
    tokens.clear();
    istringstream iss(line);
    string token;
    while(iss >> token){
      tokens.push_back(token) ; 
    }
    Sekcija* s = napraviSekciju(tokens);
    sekcije.push_back(s);
    

  }
  

  //pravljenje simbola
  getline(file,line);
  int brojSimbola = stoi(line);
  getline(file,line); //zaglavlje

  for(int i =0;i<brojSimbola;i++){
    getline(file,line);
    istringstream iss(line);
    tokens.clear();
    string token;
    while(iss >> token){
      tokens.push_back(token);
    }
    Simbol* s = napraviSimbol(tokens);

    simboli.push_back(s);
  }

  for(vector<Sekcija*>::iterator it = sekcije.begin();it<sekcije.end();it++){
    getline(file,line); //ovo je Id sekicje u kojoj se nalazi
    getline(file,line); //broj relokacionih zapisa
    int brojRelZapisa = stoi(line);
    getline(file,line); //zaglavlje

    for(int i =0;i<brojRelZapisa;i++){
      getline(file,line);
      istringstream iss(line);
      tokens.clear();
      string token;
      while(iss >> token){
        tokens.push_back(token);
      }
      RelZapis* r = napraviRelZapis(tokens);

      (*it)->tabRelZap->dodajZapis(r);
    }
  }

  for(vector<Sekcija*>::iterator it = sekcije.begin();it!=sekcije.end();it++){
    Sekcija* s = *it;
    getline(file,line);//sekcija

    getline(file,line);//broj mas instr

    int brojMasInstr = stoi(line);

    getline(file,line);//mas instr
      istringstream iss(line);
      istringstream pom;
      unsigned int x;
      string token;
      while(iss >> token){
            
      s->kodSekcije.push_back(stoi(token,nullptr,16));

      }
  }
}




//Pravi se sekcija iz fajla koji je napravio asembler
Sekcija* LinkerFajl::napraviSekciju(vector<string> vec){
  //Ovo je vezano za pravljenje sekcije iz fajla
  Sekcija* sek = new Sekcija();
  sek->setNaziv(strdup(vec[1].c_str()));
  sek->setPocetakSekcije((unsigned int)stoi(vec[2]));
  sek->setDuzinaSekcije(stoi(vec[3]));
  return sek;
}

//Pravi se simbol iz fajla koji je napravio asembler
Simbol* LinkerFajl::napraviSimbol(vector<string> vec){
  Simbol* s = new Simbol();
  s->setVrednost(stoi(vec[1]));
  s->setVelicina(stoi(vec[2]));
  s->setGlobal((stoi(vec[3]) == 1) ? true : false);
  s->setExterni((stoi(vec[4]) == 1) ? true : false);
  s->setSekcija(stoi(vec[5]));
  s->setNaziv(strdup(vec[6].c_str()));

  return s;

}
//Pravi se rel zapis iz fajla koji je napravio asembler
RelZapis* LinkerFajl::napraviRelZapis(vector<string> vec){
  RelZapis* r = new RelZapis();
  r->setOffset(stoi(vec[0]));
  r->setTipZapisa((TipRelZapisa)stoi(vec[1]));
  r->setNazivSimbola(strdup(vec[2].c_str()));
  r->setAddend(stoi(vec[3]));
  return r;
}


//Ispis svih Sekicja
void LinkerFajl::ispisiSekcije(){
  for(vector<Sekcija*>::iterator it = sekcije.begin();it!=sekcije.end();it++){
    Sekcija* s = *it;
    cout<< "+---------------------+" << endl;
    cout << "ID: " << s->id << endl;
    cout << "Sekcija: " << s->getNaziv() << endl;
    cout << "Pocetak: " << hex <<(unsigned int) s->getPocetakSekcije() << endl;
    cout << "Duzina: " << s->getDuzinaSekcije() << endl;
    cout << "Relokacioni zapisi: " << endl;
    cout << "Offset\tTip\tNaziv simbola\tAddend" << endl;
    for(vector<RelZapis*>::iterator it2 = s->tabRelZap->zapisi.begin();it2!=s->tabRelZap->zapisi.end();it2++){
      RelZapis* r = *it2;
      cout << setw(6)<<r->getOffset() << "\t" <<setw(3)<< r->getTipZapisa() << "\t" <<setw(5) << r->getNazivSimbola() << "\t" <<setw(15)<< r->getAddend() << endl;
    }
    cout<<"Masinski kod: " << endl;
    s->ispisiMasinskiKod();
  }
}
//ispis svih simbola
void LinkerFajl::ispisiSimbole(){
  for(vector<Simbol*>::iterator it = simboli.begin();it<simboli.end();it++){
    Simbol* s = *it;
    cout<< "+---------------------+" << endl;
    cout << "Simbol: "<< s->naziv << endl;
    cout << "Vrednost: "<< (unsigned int)s->vrednost << endl;
    cout << "Velicina: "<< s->velicina << endl;
    cout << "Global: " <<s->global << endl;
    cout << "Extern:" <<s->externi << endl;
    cout << "Sekcija: "<<s->sekcija << endl;
    cout<< "+---------------------+" << endl;
  }
}

void LinkerFajl::ispisiRelZapise(){
  for(vector<Sekcija*>::iterator it = sekcije.begin();it!=sekcije.end();it++){
    Sekcija* s = *it;
    cout << "ID: " << s->id << endl;
    cout << "Sekcija: " << s->getNaziv() << endl;
    cout << "Pocetak: " << s->getPocetakSekcije() << endl;
    cout << "Duzina: " << s->getDuzinaSekcije() << endl;
    cout << "Relokacioni zapisi: " << endl;
    cout << "Offset\tTip\tNaziv simbola\tAddend" << endl;
    for(vector<RelZapis*>::iterator it2 = s->tabRelZap->zapisi.begin();it2!=s->tabRelZap->zapisi.end();it2++){
      RelZapis* r = *it2;
      cout << r->getOffset() << "\t" << r->getTipZapisa() << "\t" << r->getNazivSimbola() << "\t" << r->getAddend() << endl;
    }
  }
}


void LinkerFajl::ispraviTabeluSimbola(){
  for(vector<Sekcija*>::iterator sek = sekcije.begin();sek!= sekcije.end();sek++){
    int sekNum = (*sek)->id;
    int oldNum = 0;
    
    for(vector<Simbol*>::iterator sim = simboli.begin();sim!=simboli.end();sim++){
      //Moram da nadjem njegov stari id
      if(strcmp((*sim)->naziv,(*sek)->getNaziv()) == 0){
        //uzmem staru vrednost sekcije
        oldNum = (*sim)->sekcija;
      }
    }

    for(vector<Simbol*>::iterator sim = simboli.begin();sim!=simboli.end();sim++){
      //Dodeli sada celoj tabeli novi taj id
      if((*sim)->sekcija == oldNum){
        (*sim)->sekcija = sekNum;
      }
    }
  }

}

bool LinkerFajl::dohvatiSekcijuUTabeli(Sekcija* s){
  for(vector<Sekcija*>::iterator sek = sekcije.begin();sek!= sekcije.end();sek++){
      if(strcmp((*sek)->getNaziv(),s->getNaziv())==0 ){
        return true;
      }

    }

    return false;


}

Sekcija* LinkerFajl::dohvatiSekcijuNaziv(char* naziv){
  //DOhvati sekciju  nazivu
  for(vector<Sekcija*>::iterator sek = sekcije.begin();sek!= sekcije.end();sek++){
      if(strcmp((*sek)->getNaziv(),naziv)==0 ){
        return (*sek);
      }

    }

    return nullptr;
}

Simbol* LinkerFajl::dohvatiSimbolPoNazivu(char* naziv){
  //Dohvati simbol po nazivu
  for(vector<Simbol*>::iterator sim = simboli.begin();sim!=simboli.end();sim++){
    if(strcmp((*sim)->naziv,naziv) == 0){
      return (*sim);
    }
  }
  return nullptr;
}

