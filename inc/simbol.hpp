#ifndef SIMBOL_HPP
#define SIMBOL_HPP
struct flink{
  struct instrukcija* instr;
  int flinkLocation;
  struct flink* next;
  int sekcija;
};

class Simbol{

  public:
  char* naziv;
  int vrednost;
  int velicina;
  int sekcija; 
  bool global;
  bool externi;
  bool definisan; // da proverimo na kraju da l je definisan simbol
  bool jesamSekcija;
  struct flink* flinkHead;
  struct flink* flinkTail;
  int id = prev_id++;

  static int prev_id;



  Simbol(); 

  ~Simbol();


bool getJesamSekcija(){
  return jesamSekcija;
}

void setJesamSekcija(bool b){
  this->jesamSekcija = b;
}

  void dodajFlink(struct instrukcija* instr,int locationCount,int sekcija);

  void setNaziv(char* naziv);

  void setVrednost(int vrednost);

  void setVelicina(int velicina);

  void setSekcija(int sekcija);

  void setGlobal(bool global);

  void setExterni(bool externi);

  void setDefinisan(bool definisan);

  int getId(){
    return this->id;
  }


};
#endif