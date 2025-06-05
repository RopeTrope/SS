#ifndef REL_ZAPIS_HPP
#define REL_ZAPIS_HPP
#include <stdio.h>

enum  TipRelZapisa{
  PC32,
  R_X86_64_32
};

class RelZapis{



  private:
    int offset;
    TipRelZapisa tipZapisa;
    char* nazivSimbola;
    int addend;

  public:

    RelZapis();

    ~RelZapis();

    int getOffset();

    TipRelZapisa getTipZapisa();

    char* getNazivSimbola();

    int getAddend();

    void setOffset(int offset);

    void setTipZapisa(TipRelZapisa tipZapisa);

    void setNazivSimbola(char* nazivSimbola);

    void setAddend(int addend);




};


#endif