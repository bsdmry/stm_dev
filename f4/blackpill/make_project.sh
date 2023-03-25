#!/bin/sh
mkdir $1
echo "BINARY = "$1 > $1/Makefile
echo "LDSCRIPT = ../blackpill.ld" >> $1/Makefile
echo "include ../../Makefile.include" >> $1/Makefile
touch $1/$1.c
touch $1/$1.h

echo "#include <libopencm3/stm32/rcc.h>" >> $1/$1.c
echo "#include \""$1".h\"" >> $1/$1.c
echo "" >> $1/$1.c
echo "static void clock_setup(void)" >> $1/$1.c
echo "{" >> $1/$1.c
echo "rcc_clock_setup_pll(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_96MHZ]);" >> $1/$1.c
echo "rcc_periph_clock_enable(RCC_GPIOA);" >> $1/$1.c
echo "}" >> $1/$1.c
echo "" >> $1/$1.c
echo "int main(void)" >> $1/$1.c
echo "{" >> $1/$1.c
echo "clock_setup();" >> $1/$1.c
echo "while(1);" >> $1/$1.c
echo "}" >> $1/$1.c
