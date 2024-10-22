#include <stdbool.h>
#include <stdio.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)                                                   \
    ((byte) & 0x80 ? '1' : '0'), ((byte) & 0x40 ? '1' : '0'),                  \
        ((byte) & 0x20 ? '1' : '0'), ((byte) & 0x10 ? '1' : '0'),              \
        ((byte) & 0x08 ? '1' : '0'), ((byte) & 0x04 ? '1' : '0'),              \
        ((byte) & 0x02 ? '1' : '0'), ((byte) & 0x01 ? '1' : '0')

void disassemble(const char *s, size_t size);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        perror("error opening file");
        return 1;
    }

    char buf[512] = {0};
    size_t n = fread(buf, 1, sizeof(buf), f);
    fclose(f);

    disassemble(buf, n);
}

void disassemble(const char *s, size_t size) {
    char *regs_byte[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
    char *regs_word[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

    printf("bits 16\n\n");

    size_t cursor = 0;
    while (cursor < size) {
        if ((s[cursor] & 0b11111100) == 0b10001000) {
            fprintf(stderr, ";  inst: mov\n");

            bool src_in_reg = (s[cursor] & 0b00000010) == 0;
            bool op_on_byte = (s[cursor] & 0b00000001) == 0;
            fprintf(stderr, ";  src in reg: %d\n", src_in_reg);
            fprintf(stderr, ";  op on byte: %d\n", op_on_byte);

            cursor++;
            
            char mode = (s[cursor] & 0b11000000) >> 6;
            char reg  = (s[cursor] & 0b00111000) >> 3;
            char rm   = (s[cursor] & 0b00000111);
            fprintf(stderr, ";  mode:       "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(mode));
            fprintf(stderr, ";  reg:        "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(reg));
            fprintf(stderr, ";  r/m:        "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(rm));

            printf("mov "); 
            if (src_in_reg) {
                if (op_on_byte) {
                    printf("%s, ", regs_byte[rm&0b111]);
                    printf("%s",   regs_byte[reg&0b111]);
                } else {
                    printf("%s, ", regs_word[rm&0b111]);
                    printf("%s",   regs_word[reg&0b111]);
                }
            } else {
                if (op_on_byte) {
                    printf("%s, ", regs_byte[reg&0b111]);
                    printf("%s",   regs_byte[reg&0b111]);
                } else {
                    printf("%s, ", regs_word[reg&0b111]);
                    printf("%s",   regs_word[rm&0b111]);
                }
            }

            printf("\n");
        }

        cursor++;
    }
}
