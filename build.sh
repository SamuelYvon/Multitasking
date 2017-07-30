#cleanup
mkdir ./build
mkdir ./out

rm ./build/*
rm ./out/*

#building assembler

nasm -f elf32 kernel.asm -o ./build/kasm.o
nasm -f elf32 hardware/hardware_io.asm -o ./build/hardware_io.o

#building C

gcc -fno-stack-protector -Wall -m32 -c kernel.c -o ./build/kc.o -g
gcc -fno-stack-protector -Wall -m32 -c ./lib/stdlib.c -o ./build/stdlib.o -g


#linking

ld -m elf_i386 -T link.ld -o ./out/kernel ./build/*.o