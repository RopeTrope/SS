#include "../inc/emulator.hpp"

Emulator::Emulator(){
  //svi registri na nula osim pc
  for(int i = 0; i < 16; i++){
    if(i == 15){
      //PC = 0x40000000
      reg[i] = 0x40000000;
    }else{
      reg[i] = 0;
    }
  }
  for(int i = 0; i < 3; i++){
    //Status , handler i cause registri
    creg[i] = 0;
  }
}


Emulator::~Emulator(){
}

void Emulator::procitajFajl(char* fajl){
  ifstream f(fajl);
  if(!f.is_open()){
    cout<<"Greska pri otvaranju fajla"<<endl;
    return;
  }

  string line;
  vector<string> tokens;

  while(getline(f,line)){
    tokens.clear();
    istringstream iss(line);
    string token;
    //Razbio sam na reci 
    while(iss >> token){
      tokens.push_back(token);
    }
    inicijalizujMemoriju(tokens);
  }
  f.close();
} 


void Emulator::inicijalizujMemoriju(vector<string> tokens){
  //pocetna adresa
  unsigned int pocetak = (unsigned int)strtol(tokens[0].substr(0,tokens[0].size()).c_str(),nullptr,16);
    //Ovo je pocetak
   for(int i = 2; i < tokens.size(); i++){

    unsigned char c = (unsigned char)strtol(tokens[i].c_str(),nullptr,16);
  }
  //Ide od treceg jer je drugi ":" a prvi je adresa
  for(int i = 2; i < tokens.size(); i++){
    //Upisujem u memoriju bajtove sa procitane linije 8 bajtova
    unsigned char c = (unsigned char)strtol(tokens[i].c_str(),nullptr,16);
    memorija[pocetak] = c;

    pocetak++;
  }


}

void Emulator::ispisiOCIMod(int OC,int Mod){
  cout<<"OC, MOD :"<<(int)OC <<"," << (int)Mod<<endl;
}
void Emulator::pokreniEmulaciju(){
  bool end = false;
  while(!end){
    unsigned char OCandMOD = memorija[reg[15]+3];
    unsigned char RegAandRegB = memorija[reg[15] + 2];
    unsigned char RegCandDisp = memorija[reg[15] + 1];
    unsigned char Disp = memorija[reg[15]];
    printf("PC: %08X\n",reg[15]);
    reg[15] += 4;

    unsigned char OC = OCandMOD>>4 & 0x0F;
    unsigned char MOD = OCandMOD & 0x0F;
    unsigned char regA = RegAandRegB >> 4 & 0x0F;
    unsigned char regB = RegAandRegB & 0x0F;
    unsigned char regC = RegCandDisp >> 4 & 0x0F;
    int pom;
    //Ispis za OC i MOD
    ispisiOCIMod(OC,MOD);
    unsigned char znak = (RegCandDisp & 0x08) >> 3;
    if(znak == 1){
      //dobije se pomeraj  i pretvori se u negativan broj
      pom = 0xFFFFF000|(((RegCandDisp & 0x0F)<<8) | Disp);
    }else{
      //pozitivan broj
      pom = 0x00000FFF&(((RegCandDisp & 0x0F)<<8) | Disp);
    }
    
    switch(OC){
      case 0x0:{
        //HALT
        if(MOD != 0x0){
          //greska
          prekidGreska();
        }
        end = true;
        break;
      }

      case 0x1:{
        
        if(MOD == 0x0){

          //INT
          //push status
          reg[14] = reg[14] - 4;
          memorija[reg[14]] = (unsigned char)(creg[0] & 0xFF);
          memorija[reg[14]+1] = (unsigned char)((creg[0] >> 8) & 0xFF);
          memorija[reg[14]+2] = (unsigned char)((creg[0] >> 16) & 0xFF);
          memorija[reg[14]+3] = (unsigned char)((creg[0] >> 24) & 0xFF);
          //push pc
          reg[14] = reg[14] - 4;
          memorija[reg[14]] = (unsigned char)(reg[15] & 0xFF);
          memorija[reg[14]+1] = (unsigned char)((reg[15] >> 8) & 0xFF);
          memorija[reg[14]+2] = (unsigned char)((reg[15] >> 16) & 0xFF);
          memorija[reg[14]+3] = (unsigned char)((reg[15] >> 24) & 0xFF);
          //cause = 4
          creg[2] = 4;
          //status = status &(~0x1)
          creg[0] = creg[0] & (~0x1);

          //pc = handler
          reg[15] = creg[1];

        }else{
          //greska
          prekidGreska();
        }

        break;

      }

      case 0x2:{
        if(MOD == 0x0){
          //CALL --> ista sekcija
          reg[14] = reg[14] - 4;
          memorija[reg[14]] = (unsigned char)(reg[15] & 0xFF);
          memorija[reg[14]+1] = (unsigned char)((reg[15] >> 8) & 0xFF);
          memorija[reg[14]+2] = (unsigned char)((reg[15] >> 16) & 0xFF);
          memorija[reg[14]+3] = (unsigned char)((reg[15] >> 24) & 0xFF);

          //Odradio se push pc
          reg[15] = reg[regA]+reg[regB]+pom;
        }else if(MOD == 0x1){
          //CALL--> razlicita sekcija
          reg[14] = reg[14] - 4;
          //Push PC
          memorija[reg[14]] = (unsigned char)(reg[15] & 0xFF);
          memorija[reg[14]+1] = (unsigned char)((reg[15] >> 8) & 0xFF);
          memorija[reg[14]+2] = (unsigned char)((reg[15] >> 16) & 0xFF);
          memorija[reg[14]+3] = (unsigned char)((reg[15] >> 24) & 0xFF);
          
          //mem[rega+regb+pom]
          unsigned char c1 = memorija[reg[regA]+reg[regB]+pom];
          unsigned char c2 = memorija[reg[regA]+reg[regB]+pom+1];
          unsigned char c3 = memorija[reg[regA]+reg[regB]+pom+2];
          unsigned char c4 = memorija[reg[regA]+reg[regB]+pom+3];
          //pc = mem[rega+regb+pom]
          reg[15] = c4 << 24 | c3 << 16 | c2 << 8 | c1;
        }else{

          //greska
          prekidGreska();
        }

        break;
      }

      case 0x3:{

        if(MOD == 0x0){
          //JMP --> ista sekcija
          reg[15] = reg[regA]+pom;

        }else if(MOD == 0x1){
          //BEQ --> ista sekcija
          if(reg[regB] == reg[regC]){
            reg[15] = reg[regA]+pom;
          }

        }else if(MOD == 0x2){
          //BNE --> ista sekcija
          if(reg[regB] != reg[regC]){
            reg[15] = reg[regA]+pom;
          }
        }else if(MOD == 0x3){
          //BGT --> ista sekcija
          if(reg[regB] > reg[regC]){
            reg[15] = reg[regA]+pom;
          }
        }else if(MOD == 0x8){
          //JMP --> razlicita sekcija
          //mem[rega+pom]
          unsigned char c1 = memorija[reg[regA]+pom];
          unsigned char c2 = memorija[reg[regA]+pom+1];
          unsigned char c3 = memorija[reg[regA]+pom+2];
          unsigned char c4 = memorija[reg[regA]+pom+3];
          //pc = mem[rega+pom]
          reg[15] = c4 << 24 | c3 << 16 | c2 << 8 | c1;
        }else if(MOD == 0x9){
          //BEQ --> razlicita sekcija
          if(reg[regB] == reg[regC]){
            //mem[rega+pom]
            unsigned char c1 = memorija[reg[regA]+pom];
            unsigned char c2 = memorija[reg[regA]+pom+1];
            unsigned char c3 = memorija[reg[regA]+pom+2];
            unsigned char c4 = memorija[reg[regA]+pom+3];
            //pc = mem[rega+pom]
            reg[15] = c4 << 24 | c3 << 16 | c2 << 8 | c1;
          }
        }else if(MOD == 0xA){
          //BNE --> razlicita sekcija
          if(reg[regB] != reg[regC]){
            //mem[rega+pom]
            unsigned char c1 = memorija[reg[regA]+pom];
            unsigned char c2 = memorija[reg[regA]+pom+1];
            unsigned char c3 = memorija[reg[regA]+pom+2];
            unsigned char c4 = memorija[reg[regA]+pom+3];
            //pc = mem[rega+pom]
            reg[15] = c4 << 24 | c3 << 16 | c2 << 8 | c1;
          }
        }else if(MOD == 0xB){
          //BGT --> razlicita sekcija
          if(reg[regB] > reg[regC]){
            //mem[rega+pom]
            unsigned char c1 = memorija[reg[regA]+pom];
            unsigned char c2 = memorija[reg[regA]+pom+1];
            unsigned char c3 = memorija[reg[regA]+pom+2];
            unsigned char c4 = memorija[reg[regA]+pom+3];
            //pc = mem[rega+pom]
            reg[15] = c4 << 24 | c3 << 16 | c2 << 8 | c1;
          }

        }else{
          //greska
          prekidGreska();
        }

        break;
      }

      case 0x4:{
        if(MOD == 0x0){
          //XCHG
          int pom = reg[regA];
          reg[regA] = reg[regB];
          reg[regB] = pom;
        }else{
          //greska
          prekidGreska();
        }
        break;
      }

      case 0x5:{
        if(MOD == 0x0){
          //ADD
          reg[regA] = reg[regB] + reg[regC];
        }else if(MOD == 0x1){
          //SUB
          reg[regA] = reg[regB] - reg[regC];
        }else if(MOD == 0x2){
          //MUL
          reg[regA] = reg[regB] * reg[regC];
        }else if(MOD == 0x3){
          //DIV
          reg[regA] =(int)reg[regB] / reg[regC];
        }else{
          //greska
          prekidGreska();
        }

        break;


      }

      case 0x6:{
        if(MOD == 0x0){
          //NOT
          reg[regA] = ~reg[regB];
        }else if(MOD == 0x1){
          //AND
          reg[regA] = reg[regB] & reg[regC];
        }else if(MOD == 0x2){
          //OR
          reg[regA] = reg[regB] | reg[regC];
        }else if(MOD == 0x3){
          //XOR
          reg[regA] = reg[regB] ^ reg[regC];
        }else{
          //greska
          prekidGreska();

        }
        break;

      }

      case 0x7:{
        if(MOD==0x0){
          //SHL
          reg[regA] = reg[regB] << reg[regC];
        }else if(MOD == 0x1){
          //SHR
          reg[regA] = reg[regB] >> reg[regC];
        }else{
          //greska
          prekidGreska();
        }
        break;
      }

      case 0x8:{
        if(MOD == 0x0){
          //ST --> mem[rega+regb+pom] = reg[regC]
          unsigned char c1 = reg[regC] >> 24 & 0xFF;
          unsigned char c2 = reg[regC] >> 16 & 0xFF;
          unsigned char c3 = reg[regC] >> 8 & 0xFF;
          unsigned char c4 = reg[regC] & 0xFF;
          memorija[reg[regA]+reg[regB]+pom] = c4;
          memorija[reg[regA]+reg[regB]+pom+1] = c3;
          memorija[reg[regA]+reg[regB]+pom+2] = c2;
          memorija[reg[regA]+reg[regB]+pom+3] = c1;
        }else if(MOD == 0x1){
          //PUSH
          reg[regA] = reg[regA]+pom;
          unsigned char c1 = reg[regC] >> 24 & 0xFF;
          unsigned char c2 = reg[regC] >> 16 & 0xFF;
          unsigned char c3 = reg[regC] >> 8 & 0xFF;
          unsigned char c4 = reg[regC] & 0xFF;
          memorija[reg[regA]] = c4;
          memorija[reg[regA]+1] = c3;
          memorija[reg[regA]+2] = c2;
          memorija[reg[regA]+3] = c1;
        }else if(MOD == 0x2){
          //ST--->mem[mem[rega+regb+pom]] = reg[regC]

          //mem[rega+regb+pom]
          unsigned char c1 = memorija[reg[regA]+reg[regB]+pom];
          unsigned char c2 = memorija[reg[regA]+reg[regB]+pom+1];
          unsigned char c3 = memorija[reg[regA]+reg[regB]+pom+2];
          unsigned char c4 = memorija[reg[regA]+reg[regB]+pom+3];
          unsigned int adresa = c4 << 24 | c3 << 16 | c2 << 8 | c1;

          //mem[mem[rega+regb+pom]]
          memorija[adresa] = reg[regC] & 0xFF;
          memorija[adresa+1] = reg[regC] >> 8 & 0xFF;
          memorija[adresa+2] = reg[regC] >> 16 & 0xFF;
          memorija[adresa+3] = reg[regC] >> 24 & 0xFF;
          //mem[mem[rega+regb+pom]] = reg[regC]
        }else{
          //greska
          prekidGreska();
        }

        break;
      }

      case 0x9:{
        if(MOD == 0x0){
          //CSRRD
          reg[regA] = creg[regB];
        }else if(MOD == 0x1){
          //rega = regb + pom
          reg[regA] = reg[regB] + pom;

        }else if(MOD == 0x2){
          //rega = mem[regb+regc+pom]
          unsigned char c1 = memorija[reg[regB]+reg[regC]+pom];
          unsigned char c2 = memorija[reg[regB]+reg[regC]+pom+1];
          unsigned char c3 = memorija[reg[regB]+reg[regC]+pom+2];
          unsigned char c4 = memorija[reg[regB]+reg[regC]+pom+3];
          reg[regA] = c4 << 24 | c3 << 16 | c2 << 8 | c1;
        }else if(MOD == 0x3){
          //POP 
          reg[regA] = memorija[reg[regB]+3] << 24 | memorija[reg[regB]+2] << 16 | memorija[reg[regB]+1] << 8 | memorija[reg[regB]] ;
          reg[regB] = reg[regB] + pom;
        }else if(MOD == 0x4){
          //CSRWR
          creg[regA] = reg[regB];
        }else if(MOD == 0x6){
          //IRET creg = mem[regb+regc+pom]
          unsigned char c1 = memorija[reg[regB]+reg[regC]+pom];
          unsigned char c2 = memorija[reg[regB]+reg[regC]+pom+1];
          unsigned char c3 = memorija[reg[regB]+reg[regC]+pom+2];
          unsigned char c4 = memorija[reg[regB]+reg[regC]+pom+3];
          creg[regA] = c4 << 24 | c3 << 16 | c2 << 8 | c1;
        }else{
          //greska
          prekidGreska();
        }

        break;
      }

      default:{
        //greska
        prekidGreska();
        break;
      }


    
    }
    reg[0] = 0;
    /*
    printf("Reg1 = %08X, Reg2 = %08X, Reg3 = %08X, Reg4 = %08X\n",reg[1],reg[2],reg[3],reg[4]);
    printf("SP = %08X, PC = %08X\n",reg[14],reg[15]);
    printf("Status = %08X, Handler = %08X, Cause = %08X\n",creg[0],creg[1],creg[2]);
    ispisiStek();
    */
  }
}

void Emulator::prekidGreska(){

    printf("Pogresna instrukcija!");
    reg[14] = reg[14] - 4;
    memorija[reg[14]] = (unsigned char)(creg[0] & 0xFF);
    memorija[reg[14]+1] = (unsigned char)((creg[0] >> 8) & 0xFF);
    memorija[reg[14]+2] = (unsigned char)((creg[0] >> 16) & 0xFF);
    memorija[reg[14]+3] = (unsigned char)((creg[0] >> 24) & 0xFF);
    //push pc
    reg[14] = reg[14] - 4;
    memorija[reg[14]] = (unsigned char)(reg[15] & 0xFF);
    memorija[reg[14]+1] = (unsigned char)((reg[15] >> 8) & 0xFF);
    memorija[reg[14]+2] = (unsigned char)((reg[15] >> 16) & 0xFF);
    memorija[reg[14]+3] = (unsigned char)((reg[15] >> 24) & 0xFF);
    //cause = 1
    creg[2] = 1;
    //status = status &(~0x1)
    creg[0] = creg[0] & (~0x1);
    //pc = handler
    printf("creg[0] = %08X, creg[1] = %08X, creg[2] = %08X\n",creg[0],creg[1],creg[2]);
    reg[15] = creg[1];
    
}

void Emulator::ispisiMemoriju(){

  for(map<unsigned int,unsigned char>::iterator it = memorija.begin();it != memorija.end();it++){
    printf("%08X : %02X\n",it->first,it->second);
  }

}

void Emulator::ispisiStek(){
  printf("STEK: \n");
  printf("-----------------------------------------\n");
  for(int i = 0xFFFFFEFE;i >= 0xFFFFFED2;i-=4){
    unsigned char c1 = memorija[i];
    unsigned char c2 = memorija[i+1];
    unsigned char c3 = memorija[i+2];
    unsigned char c4 = memorija[i+3];
    printf("%08X : %02X %02X %02X %02X\n",i,c1,c2,c3,c4);
  }
  printf("\n");
}

void Emulator::ispisiRegistre(){
  cout<<"-----------------------------------------"<<endl;
  cout<<"Emulated processor executed halt instruction"<<endl;
  cout<<"Emulated processor state:"<<endl;
  for(int i =0;i<16;i++){
    cout<<"r"<<i<<"=0x";
    printf("%08X",reg[i]);
    if(i % 4 == 3){
      cout<<endl;
    }else{
      cout<<"\t";
    }
  }
/*
  for(int i = 0;i<3;i++){
    printf("c%d=0x%08X\n",i,creg[i]);
    
  }
*/
}

