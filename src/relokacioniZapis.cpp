#include "../inc/relokacioniZapis.hpp"

RelZapis::RelZapis() {
    this->offset = 0;
    this->tipZapisa = PC32;
    this->nazivSimbola = NULL;
    this->addend = 0;
}

RelZapis::~RelZapis() {
}

int RelZapis::getOffset() {
    return this->offset;
}

TipRelZapisa RelZapis::getTipZapisa() {
    return this->tipZapisa;
}

char* RelZapis::getNazivSimbola() {
    return this->nazivSimbola;
}

int RelZapis::getAddend() {
    return this->addend;
}

void RelZapis::setOffset(int offset) {
    this->offset = offset;
}

void RelZapis::setTipZapisa(TipRelZapisa tipZapisa) {
    this->tipZapisa = tipZapisa;
}

void RelZapis::setNazivSimbola(char* nazivSimbola) {
    this->nazivSimbola = nazivSimbola;
}

void RelZapis::setAddend(int addend) {
    this->addend = addend;
}

