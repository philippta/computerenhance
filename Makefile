.PHONY: decode
decode:
	clang decode.c -o decode && ./decode listing38

.PHONY: nasm
nasm:
	nasm listing37.asm
	nasm listing38.asm
