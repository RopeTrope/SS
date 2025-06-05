
#include "../inc/linkerFajl.hpp"
#include <string>
#include <vector>
#include <map>

using namespace std;



int main(int argc,char* argv[]){


  string izlazni_fajl = "";

  vector<LinkerFajl*> fajlovi;

  multimap<unsigned int,string> map_places;



  bool hex = false;
  int i = 1;
  //Znaci ako stoji samo ime programa to su programi za ucitavane
  //Ako se pojavi hex onda treba da se napravi hex fajl 
  //Ako se pojavi -o onda treba da se napravi izlazni fajl tj. sledeca rec
  //je ime fajla u koji se ispisuje
  //I jos jedna opcija i poslednja je -place gde se smesta u memoriji
  while(i < argc){
    string s(argv[i]);

    if(s.compare("-o") == 0){
      //Ovde treba da se upise ime fajla u koji se upisuje
      if(izlazni_fajl != ""){
        cout<<"Izlazni fajl se moze navesti samo jednom!"<<endl;
        return -1;
      }
      i++;
      izlazni_fajl = string(argv[i]);
    }else if(s.compare("-hex") == 0){
      //Ovde ce biti neki boolean koji ce reci da treba da se generise hex fajl
      if(hex){
        cout << "Hex se poziva samo jedanput!" << endl;
        return -1;
      }
      hex = true;
      
    }else if(s.substr(0,7) == "-place="){
      //Ovde ce biti neki broj koji ce reci gde se smesta u memoriji
      //Mora da se cuva za svaku sekciju njeno mesto
      string pom;
      int j = 7;
      while(s[j] != '@'){
        //Cita se ime sekcije
        pom += s[j];
        j++;
      } 
      j++;//preskoci @
      string numero;
      while(s[j] != '\0'){
        //Cita se mesto u memoriji
        numero += s[j];
        j++;
      }

      char* co = new char[numero.size() + 1];

      strcpy(co,numero.c_str());

      //napravio sam novi par koji je sekcija - pozicija
      map_places.insert(make_pair((unsigned int)strtol(co,nullptr,16),pom));

        
      }else{
        //Ovde se cita fajl sa tim imenom i doda se u procitane fajlove
        LinkerFajl* l = new LinkerFajl();
        l->procitajFajl(argv[i]);

        l->ispraviTabeluSimbola();
        //l->ispisiSekcije();
        //l->ispisiSimbole();
        fajlovi.push_back(l);
      }
      i++;
  }

  //Sada treba da se poredjaju sve sekcije kako treba
  //Pravim novi Linker Fajl od ovog jednog
  /*
  for(auto it = map_places.begin();it != map_places.end();it++){
    //redja ga po redu sada
    printf("%d %s\n",it->first,it->second.c_str());
  }

  */

 for(auto it = map_places.begin();it != map_places.end();it++){
    bool pronadjena = false;
    for(auto f = fajlovi.begin();f != fajlovi.end();f++){
      Sekcija* s = (*f)->dohvatiSekcijuNaziv((char*)it->second.c_str());
      if(s != nullptr){
        pronadjena = true;
        break;
      }
    }
    if(!pronadjena){
      cout << "Greska: Sekcija " <<it->second<<" ne postoji!"   << endl;
      exit(-1);
    }
  }

  unsigned int pocetak_sekcije_bez_place = 0;
  //Ovde treba da se poredjaju sekcije prvo sa place-om

  unsigned int preth_pocetak = 0;
  for(auto it = map_places.begin();it!=map_places.end();it++){
    bool first = true;
    unsigned int produzetak = 0;
    for(auto it2 = fajlovi.begin();it2 != fajlovi.end();it2++){
      if((*it2)->dohvatiSekcijuNaziv((char*)it->second.c_str()) != nullptr){
        //Ako nadjem sekciju sa tim imenom treba da ga postavim na place
        //Ako se drugi put pojavim onda moram da nastavim
        if(first){
          //Postavim samo pocetak al moram i da gde da nastavim
          (*it2)->dohvatiSekcijuNaziv((char*)it->second.c_str())->setPocetakSekcije((unsigned int)it->first);
          first = false;
          produzetak = (*it2)->dohvatiSekcijuNaziv((char*)it->second.c_str())->getDuzinaSekcije() + it->first;
          //place + duzina je mesto gde ide nastavak
        }else{
          //Ovde se nastavlja na prethodnu
          (*it2)->dohvatiSekcijuNaziv((char*)it->second.c_str())->setPocetakSekcije((unsigned int)produzetak);
          produzetak += (*it2)->dohvatiSekcijuNaziv((char*)it->second.c_str())->getDuzinaSekcije();
          //Sad nastavak ide produzetak + duzina
        } 
          //Ovde se zavrsava
          pocetak_sekcije_bez_place = produzetak;
      }
    }

    if(preth_pocetak  > it->first){
      cout << "Sekcije se preklapaju!"<< endl;
      return -1;
    }
    preth_pocetak = produzetak;

  }

  //vektor koji sluzi da vidimo koje su sekcije obradjene
  vector<char*> obradjeneSekcije;

  for(auto it = map_places.begin();it != map_places.end();it++){
    obradjeneSekcije.push_back((char*)it->second.c_str());
  }
  //Sada se obradjuju sekcije bez place-a

    for(auto it2 = fajlovi.begin();it2 != fajlovi.end();it2++){

      //Ako je ta sekcija vec obradjena
      vector<Sekcija*> sve_sekcije_fajla = (*it2)->dohvatiTabeluSekcije();

      for(auto i = sve_sekcije_fajla.begin();i!= sve_sekcije_fajla.end();i++){
        bool obradjena = false;
        for(auto j = obradjeneSekcije.begin();j != obradjeneSekcije.end();j++){
          //Da li postoji u tabeli obradjenih
          if(strcmp((*j),(*i)->getNaziv()) == 0){
            //Vec je obradjena idemo na sledecu 
            obradjena = true;
            break;
          }
        }
        if(obradjena){
          continue;
        }else{
          //Obradim nju samu
          (*i)->setPocetakSekcije((unsigned int)pocetak_sekcije_bez_place);
          pocetak_sekcije_bez_place += (*i)->getDuzinaSekcije();
          //ta sekcija nije obradjena i sad cemo da dodamo u obradjen
          for(auto f = fajlovi.begin();f != fajlovi.end();f++){
            if(f == it2){
              //Preskacemo isti fajl
              continue;
            }
            Sekcija* s = (*f)->dohvatiSekcijuNaziv((*i)->getNaziv());
            if(s != nullptr){
              //Ako je to ta sekcija
                //Ovde se nastavlja na prethodnu
                s->setPocetakSekcije((unsigned int)pocetak_sekcije_bez_place);
                pocetak_sekcije_bez_place += s->getDuzinaSekcije();
              }
              //Ovde se zavrsava   
          }
          obradjeneSekcije.push_back((*i)->getNaziv());
        }
        
      }
    }
    //Provera da li su sekcije preklapaju

   

  //Sada ide provera simbola da li postoje dva globalna simbola sa istim imenom
  //Ili extern a nema global

  

  //Sada ide provera za Simbole

  for(auto it2 = fajlovi.begin();it2 != fajlovi.end();it2++){
    vector<Simbol*> simboli = (*it2)->dohvatiTabeluSimbola();
    for(auto i = simboli.begin();i != simboli.end();i++){
      //Znaci sada za svaki fajl provera da li je globalan simbol
      if((*i)->global && (*i)->externi){
        bool postoji = false;
        for(auto j = fajlovi.begin(); j !=fajlovi.end();j++){
          if(j == it2){
            //Preskacem isti fajl
            continue;
          }

          vector<Simbol*> simboli2 = (*j)->dohvatiTabeluSimbola();



          for(auto k = simboli2.begin();k != simboli2.end();k++){
            if(strcmp((*k)->naziv,(*i)->naziv) == 0){
              if((*k)->global && !(*k)->externi){
                postoji = true;
              }
            }
        }
        }

        if(!postoji){
          cout << "Nema globalnog simbola!" << (*i)->naziv << endl;
          return -1;
        }


      }else if((*i)->global && !(*i)->externi){
        //Ako jesam onda moram da proverim da li u ostalim fajlovima postoji isti globalan
        for(auto j = fajlovi.begin(); j !=fajlovi.end();j++){
          if(j == it2){
            //Preskacem isti fajl
            continue;
          }

          vector<Simbol*> simboli2 = (*j)->dohvatiTabeluSimbola();

          for(auto k = simboli2.begin();k != simboli2.end();k++){
            if(strcmp((*k)->naziv,(*i)->naziv) == 0){
              if((*k)->global && !(*k)->externi){
                cout << "GRESKA!Postoji isti globalan simbol: " << (*k)->naziv << endl;
                exit(-1);
              }
            }
          }
        }
      }
    }     
  }

  
  //Prepravka simbola

  vector<Simbol*> externSimboli;

  //Prvo prepravka sve osim extern

  for(auto f1 = fajlovi.begin();f1!=fajlovi.end();f1++){
    vector<Simbol*> simboli = (*f1)->dohvatiTabeluSimbola();

    for(auto simbol = simboli.begin();simbol != simboli.end();simbol++){

      if((*simbol)->getJesamSekcija()){
        //Naleteo sam na sekciju
        continue;
      }
      if((*simbol)->externi){
        //posle za obradu externih lakse je
        externSimboli.push_back((*simbol));
      }else{
        //simbol je sada sekcija + vrednost
        Sekcija* sek = (*f1)->dohvatiSekcijuPoBroju((*simbol)->sekcija);
        (*simbol)->vrednost += sek->getPocetakSekcije();
      }

    }
  }

  for(auto ext = externSimboli.begin();ext != externSimboli.end();ext++){
    //Sad bi trebalo da su svi simboli sredjeni
    for(auto f1 = fajlovi.begin();f1!=fajlovi.end();f1++){
      vector<Simbol*> simboli = (*f1)->dohvatiTabeluSimbola();
       for(auto simbol = simboli.begin();simbol != simboli.end();simbol++){
          if(strcmp((*ext)->naziv,(*simbol)->naziv) == 0  && !(*simbol)->externi 
                  && (*simbol)->global){
                    //dodelim joj vrednost gde je definisan
                (*ext)->vrednost = (*simbol)->vrednost;
          }
       }
    }

  }
  //Ispis Simbola
/*
  for(auto it2 = fajlovi.begin();it2 != fajlovi.end();it2++){
    (*it2)->ispisiSimbole();
  }
*/
//Ispis Relokacionih zapisa
/*
  for(auto it2 = fajlovi.begin();it2 != fajlovi.end();it2++){
    (*it2)->ispisiRelZapise();
  }

  */

  //Sredjivanje relokacionih zapisa i ispravka masinskog koda
  for(auto it2 = fajlovi.begin();it2 != fajlovi.end();it2++){
    vector<Sekcija*> sekcije = (*it2)->dohvatiTabeluSekcije();
    for(auto s = sekcije.begin();s != sekcije.end();s++){
      vector<RelZapis*> zapisi = (*it2)->dohvatiTabeluRelZapisa((*s));
      for(auto zapis = zapisi.begin();zapis != zapisi.end();zapis++){
        //Sada ide da li je PC32 ili APSOLUTNO

        if((*zapis)->getTipZapisa() == TipRelZapisa::PC32){
          
          //Dohvatim pomeraj
        unsigned char c1 =  (*s)->kodSekcije.at((*zapis)->getOffset() + 2) & 0x0F;
        unsigned char c2 =  (*s)->kodSekcije.at((*zapis)->getOffset() + 3) & 0xFF;

        int pomeraj = (c1 << 8 | c2)+4;

        //Upise se vrednost simbola ustv
        Simbol* sim = (*it2)->dohvatiSimbolPoNazivu((*zapis)->getNazivSimbola());
        c1 = sim->vrednost & 0xFF;
        c2 = sim->vrednost >> 8 & 0xFF;
        unsigned char c3 = sim->vrednost >> 16 & 0xFF;
        unsigned char c4 = sim->vrednost >> 24 & 0xFF;

        (*s)->ispraviMasinskiKod(c4,(*zapis)->getOffset() + pomeraj);
        (*s)->ispraviMasinskiKod(c3,(*zapis)->getOffset()+ pomeraj + 1);
        (*s)->ispraviMasinskiKod(c2,(*zapis)->getOffset()+ pomeraj + 2);
        (*s)->ispraviMasinskiKod(c1,(*zapis)->getOffset()+ pomeraj + 3);


        }else{
          //Apsolutno
         Sekcija* sek = (*it2)->dohvatiSekcijuNaziv((*zapis)->getNazivSimbola());
         if(sek){
          //Ako je naziv sekcije Sekcije onda je lokalan simbol
            //Lokalan simbol
            //masinski kod se sada ispravlja na poziciji offset
            //Upisuje se vrednost pocetak sekcije + addend
            unsigned char c1 = (sek->getPocetakSekcije() + (*zapis)->getAddend()) & 0xFF;
            unsigned char c2 = ((sek->getPocetakSekcije() + (*zapis)->getAddend()) >> 8) & 0xFF;
            unsigned char c3 = ((sek->getPocetakSekcije() + (*zapis)->getAddend()) >> 16) & 0xFF;
            unsigned char c4 = ((sek->getPocetakSekcije() + (*zapis)->getAddend()) >> 24) & 0xFF;
            (*s)->ispraviMasinskiKod(c4,(*zapis)->getOffset());
            (*s)->ispraviMasinskiKod(c3,(*zapis)->getOffset() + 1);
            (*s)->ispraviMasinskiKod(c2,(*zapis)->getOffset() + 2);
            (*s)->ispraviMasinskiKod(c1,(*zapis)->getOffset() + 3);
            
            //Znaci sad sam ispravio masinski kod

          }else{
            //Globalan simbol
            //masinski kod se sada ispravlja na poziciji offset
            //Upisuje se vrednost simbola + addend

            Simbol* sim = (*it2)->dohvatiSimbolPoNazivu((*zapis)->getNazivSimbola());
            unsigned char c1 = (sim->vrednost) & 0xFF;
            unsigned char c2 = ((sim->vrednost) >> 8) & 0xFF;
            unsigned char c3 = ((sim->vrednost) >> 16) & 0xFF;
            unsigned char c4 = ((sim->vrednost) >> 24) & 0xFF;
            (*s)->ispraviMasinskiKod(c4,(*zapis)->getOffset());
            (*s)->ispraviMasinskiKod(c3,(*zapis)->getOffset() + 1);
            (*s)->ispraviMasinskiKod(c2,(*zapis)->getOffset() + 2);
            (*s)->ispraviMasinskiKod(c1,(*zapis)->getOffset() + 3);
            //Znaci sad sam ispravio masinski kod
           }
        }
      }
    } 
  }

  //Ispis svih Sekcija

  for(auto it2 = fajlovi.begin();it2 != fajlovi.end();it2++){
    (*it2)->ispisiSekcije();
  }

  for(auto it2 = fajlovi.begin();it2 != fajlovi.end();it2++){
    (*it2)->ispisiSimbole();
  }

  //Ovde sada ispis da se proveri pred pravljenje hex fajla
  //hex fajl se pravi samo ako je hex true
  multimap<unsigned int,vector<unsigned char>> masinski_kod_sort_po_sek;
  for(auto it2 = fajlovi.begin();it2 != fajlovi.end();it2++){
    vector<Sekcija*> sekcije = (*it2)->dohvatiTabeluSekcije();
    for(auto sekcija=sekcije.begin();sekcija!=sekcije.end();sekcija++){
      masinski_kod_sort_po_sek.insert(make_pair((*sekcija)->getPocetakSekcije(),(*sekcija)->kodSekcije));
    }
  }

  if(hex){
    //Ovde se pravi hex fajl
    FILE* f = fopen(izlazni_fajl.c_str(),"w");

    for(auto it = masinski_kod_sort_po_sek.begin();it != masinski_kod_sort_po_sek.end();it++){
      int pocetak_sek = (*it).first;
      int i = 0;

      //Dopuna da bude deljiv sa 4
      if(it->second.size() % 4 != 0){
        int razlika = 4 - it->second.size() % 4;
        for(int j = 0;j<razlika;j++){
          it->second.push_back(0);
        }
      }
      for(auto it2 = it->second.begin();it2 != it->second.end();it2+=4){
        if(i==0){
          fprintf(f,"%08X : ",pocetak_sek);
          printf("%08X : ",pocetak_sek);
        }
        pocetak_sek+=4;
        unsigned char c1 = *(it2+3);
        unsigned char c2 = *(it2+2);
        unsigned char c3 = *(it2+1);
        unsigned char c4 = *(it2+0);
        fprintf(f,"%02X ",c1);
        fprintf(f,"%02X ",c2);
        fprintf(f,"%02X ",c3);
        fprintf(f,"%02X ",c4);
        printf("%02X ",c1);
        printf("%02X ",c2);
        printf("%02X ",c3);
        printf("%02X ",c4);
        i+=4;
        if(i == 8){
          fprintf(f,"\n");
          printf("\n");
          i = 0;
        }
      }
      if(i != 0){
        while(i!=0){
          fprintf(f,"%02X ",0);
          printf("%02X ",0);
          i++;
          i = i % 8;
        }
        fprintf(f,"\n");
        printf("\n");
      }

    }
    fclose(f);
  }



  return 0;

}