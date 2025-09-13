#ifndef FONTBITMAP20_H
#define FONTBITMAP20_H

#include <Arduino.h>  // biar PROGMEM dikenali
#include <ESP32-HUB75-VirtualMatrixPanel_T.hpp>


#define PANEL_CHAIN_TYPE 	CHAIN_BOTTOM_RIGHT_UP_ZZ
extern VirtualMatrixPanel_T<PANEL_CHAIN_TYPE>* virtualDisp;
extern int l;

// Deklarasi bitmap font
extern const unsigned char angka_1_bits[] PROGMEM;
extern const unsigned char angka_4_bits[] PROGMEM;
extern const unsigned char angka_7_bits[] PROGMEM;
extern const unsigned char angka_K_bits[] PROGMEM;
extern const unsigned char angka_N_bits[] PROGMEM;
extern const unsigned char angka_M_bits[] PROGMEM;
extern const unsigned char angka_T_bits[] PROGMEM;
extern const unsigned char angka_Z_bits[] PROGMEM;
extern const unsigned char angka_V_bits[] PROGMEM;
extern const unsigned char angka_W_bits[] PROGMEM;
extern const unsigned char angka_per_bits[] PROGMEM;

int getCharWidth20(char c);
int getTextWidth20(const char* input);
void drawCharCentered20(const char* input, uint16_t color, int x, int y);

#endif