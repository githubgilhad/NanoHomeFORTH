
# Připojení k simavr (běžící na localhost:1234)
target remote localhost:1234

# Přepnutí na assemblerový pohled (v TUI režimu)
layout asm
layout regs

# Pokud není main, nastav ručně startovní adresu
# set $pc = 0x214dc

# Automaticky nastavíme breakpoint na main
break my_setup
break f_next

# Zobrazíme registry a instrukce kolem PC
display/i $pc
# display $r0
# display $r16
# display $r24

# SRAM ukázka
display/3bx &IP
display/3xb &LAST
display/3xb LAST

# Spustit program
continue
