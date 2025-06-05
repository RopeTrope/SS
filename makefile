ASSEMBLER= src/main.cpp \
	misc/parser.cpp \
	misc/lexer.cpp \
	src/instrukcija.cpp \
	src/asembler.cpp \
	src/tabelaSimbola.cpp \
	src/simbol.cpp \
	src/tabelaSekcija.cpp \
	src/sekcija.cpp \
	src/literal.cpp \
	src/tabelaLiterala.cpp \
	src/relokacioniZapis.cpp \
	src/tabelaRelZapisa.cpp \

LINKER= src/linkerFajl.cpp \
	src/tabelaSimbola.cpp \
	src/simbol.cpp \
	src/tabelaSekcija.cpp \
	src/sekcija.cpp \
	src/literal.cpp \
	src/tabelaLiterala.cpp \
	src/relokacioniZapis.cpp \
	src/tabelaRelZapisa.cpp \
	src/mainLinker.cpp \

EMULATOR= src/emulator.cpp \
	src/mainEmulator.cpp \

all: tests/assembler flex bison tests/linker tests/emulator

bison: 
	bison -d misc/bison.y


flex: bison
	flex misc/flex.l

tests/linker:
	g++ -g -Iinc -o ${@} ${LINKER}

tests/emulator:
	g++ -g -Iinc -o ${@} ${EMULATOR}


tests/assembler: flex
	g++ -g -Iinc -o ${@} ${ASSEMBLER}


cleanAsembler:
	rm -f ./tests/assembler

cleanLinker:
	rm -f ./tests/linker

cleanEmulator:
	rm -f ./tests/emulator

cleanOfiles:
	rm -f ./tests/*.o

cleanProgHex:
	rm -f ./tests/*.hex