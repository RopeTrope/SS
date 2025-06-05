ASSEMBLER=./tests/assembler
LINKER=./tests/linker
EMULATOR=./tests/emulator

TESTS=./tests

make cleanAsembler
make cleanLinker
make cleanEmulator
make cleanOfiles
make cleanProgHex
make all

${ASSEMBLER} -o ${TESTS}/main.o ${TESTS}/main.s
${ASSEMBLER} -o ${TESTS}/math.o ${TESTS}/math.s
${ASSEMBLER} -o ${TESTS}/handler.o ${TESTS}/handler.s
${ASSEMBLER} -o ${TESTS}/isr_timer.o ${TESTS}/isr_timer.s
${ASSEMBLER} -o ${TESTS}/isr_terminal.o ${TESTS}/isr_terminal.s
${ASSEMBLER} -o ${TESTS}/isr_software.o ${TESTS}/isr_software.s
${LINKER} -hex \
  -place=my_code@0x40000000 -place=math@0xf0000000  \
  -o ${TESTS}/program.hex \
  ${TESTS}/handler.o ${TESTS}/math.o ${TESTS}/main.o ${TESTS}/isr_terminal.o ${TESTS}/isr_timer.o ${TESTS}/isr_software.o
${EMULATOR} ${TESTS}/program.hex