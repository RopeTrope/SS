#ifndef LITERAL_HPP
#define LITERAL_HPP
struct flinkLit{
  struct instrukcija* instr;
  int flinkLocation;
  struct flinkLit* next;
  int sekcija;
};


class Literal{

private:
  int id;

  int vrednost;
  char* nazivSimbola;
  bool simbol;
  int pozicijaBazen;

  static int prev_id;

public:

  struct flinkLit* flinkHead;
  struct flinkLit* flinkTail;


  Literal();
  
  ~Literal();

  void setVrednost(int vred);

  void setNazivSimbola(char* naziv){
    this->nazivSimbola = naziv;
  }

  void setPozicijaBazen(int poz);

  int getVrednost();

  bool getDaLiJeSimbol();

  int getPozicijaBazen();

  char* getNazivSimbola();

  void setSimbol(bool b);

  int getId();

  void dodajFlinkLit(struct instrukcija* instr,int locationCount,int sekcija);

};

#endif