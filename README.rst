.. vim: set ft=rst noexpandtab fileencoding=utf-8 nomodified   wrap textwidth=0 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list tabstop=8 noexpandtab nosmarttab softtabstop=0 shiftwidth=0 linebreak showbreak=»\

.. |Ohm| raw:: html

	Ω

.. |kOhm| raw:: html

	kΩ


.. image:: MemxFORTHChipandColorfulStack.png
	:width: 250
	:target: MemxFORTHChipandColorfulStack.png

- `memxFORTH-core <#memxforth-core>`__
	- `Interesting words <#interesting-words>`__
	- `Classical words <#classical-words>`__

Based on `memxFORTH-core <https://github.com/githubgilhad/memxFORTH-core>`__ and `NanoHomeComputer <https://github.com/githubgilhad/NanoHomeComputer>`__

NanoHomeFORTH
=============

Small home computer with monitor and keyboard and FORTH as system

It started as merge of **memxFORTH-core** and **NanoHomeComputer** projects

WARNING
========

does not fully work yet. There are probabely some memory/stack issues, on Nano it sometimes fails without visible reason (program goes wild, screen flyes, ... probabelly nasal demons involved)

Looks like with pushing 7+ items into stack and then printing it out with . makes real system stack hit VRAM (or something like that - binary goo flows from bottom of screeen up - so from higher to lower adresses)

memxFORTH-core
==============

A minimal Forth-like core for AVR (ATmega328P), designed to experiment with unified RAM and FLASH word handling
using `__memx` pointers in C. This project should result in small tool for pin manipulation on both atmega328P and atmega2560.
Based on `memxFORTH-init <https://github.com/githubgilhad/memxFORTH-init>`__

Project Goals
-------------
- Unified handling of dictionary words in both RAM and FLASH
- Explore low-footprint interpreter design in C
- Use `__memx` for flexible word list chaining and execution
- Small tool for HW testing - mainly pin manipulation
- Targets: atmega328P and atmega2560


Current Features
----------------
- Dictionary traversal using `__memx` pointers / uint32_t
- Execution of basic hardcoded words in FLASH and RAM
- Basic words for building new words ( : ; CREATE )
- Basic structures ( IF ELSE THEN FI )
- debugging tools ( dump show noinfo nodebug )
- PORTx, PINx, DDRx for all x on platform
- Modular, small codebase suitable for embedded hardware debugging
- Uses C and AVR-GCC (also Arduino IDE compatible)

Build
-----

Compile as usual with `avr-gcc`, tested on ATmega328P.

**Arduino IDE:**

- Open `memxFORTH-core.ino`
- Select "Arduino Nano" with ATmega328P
- Upload as usual

Usage
-----
- Developed for ANSI terminal on a wide monitor
- Serial speed: 115200 baud
- Colors can be disabled in **colors.h**
- After upload, the program:
  - Show some hints for copy-paste
  - Runs internal test on DOUBLE and show WORDS
  - Run classical loop on INTERPRETER

- This project is attempt to build small usable tool

Interesting words
-----------------
- **show** (xt -- ) "disasseble" a word - ' DOUBLE show
- **dump** (Daddr -- ) dump some memory around given address - LATEST @ 20 - dump
- **ff** ( -- 0xFF ) - constant
- **aa** ( -- 0xAA ) - constant
- **PORTx** **PINx** **DDRx** - A..H on atmega2560 A..C on atmega328P - ff DDRD !C aa PORTD !C ff PINC !C 
- **nodebug** (bool -- ) set debugging prints on(true)/off(false)
- **noinfo** (bool -- ) set info level on(true)/off(false)
- **cw2h** (Dcw -- Dh) convert pointer to codeword to pointer to head
- **bin** **dec** **hex** (--) set BASE to 2 10 16 respectively


Classical words
---------------
- **BRANCH** 0BRANCH - branch, branch if zero - next field is offset in pointer increases - -2=prev instruction, -1=loop(self), 0=crash (jump inside instruction),  +1=nop(next instruction), +2=skip next instruction
- **WORDS2** - WORDS but in RAM
- **ELSE** **THEN** **FI** **IF**  -  THEN is synonymum for FI
- **:** ; 
- **HIDE** (--) \ HIDE WORD hide given word
- **HIDDEN** (Daddr -- ) hide/unhide word at address
- **QUIT** - loops INTERPRETER
- **<=0** <0 >=0 >0 !=0 ==0 - tests
- **IMMEDIATE** (Daddr -- ) make word at address IMMEDIATE
- **'** - "TICK" read next word and push address of its codeword
- **FIND** (str n -- Daddr) find word in vocabulary
- **]** (--) STATE=st_compiling
- **[** (--) STATE=st_executing
- **CREATE** (str n --) create head from WORD result
- **WORDS** (--) print all words. Immediate words have red background, hidden words have grey text.
- **INTERPRET** - read word from input and execute it
- **.** (n -- ) print number
- **NUMBER** (str n -- num bad) decode string for number, return decoded number and count of unconverted chars
- **,** (Daddr--) - "COMMA" put address to \*HERE and increments HERE
- **WORD** (-- str n) read word from input to data stack
- **KEY** (-- c) read character from input
- **EXIT** end words definition


- **/4D** (D -- D/4)
- **/2D** (D -- D/2)
- **/D** (D1 D2 -- D1/D2)
- **\*D** (D1 D2 -- D1*D2)
- **-D** (D1 D2 -- D1-D2)
- **+D** (D1 D2 -- D1+D2)
- **SWAP2** (D1 D2 -- D2 D1)
- **DUP2** (D1 -- D1 D1)
- **/4** (n -- n/4)
- **/2** (n -- n/2)
- **/** (n1 n2 -- n1/n2)
- **\*** (n1 n2 -- n1*n2)
- **-** (n1 n2 -- n1-n2)
- **+** (n1 n2 -- n1+n2) 
- **SWAP**  (n1 n2 -- n2 n1) 
- **DUP**  (n1 -- n1 n1) 
- **D@** (Daddr -- D) "DOUBLE AT"
- **C@** (Daddr -- c) "Char AT"
- **@** (Daddr -- n) "AT"
- **!D** (D Daddr --) "SET DOUBLE"
- **!C** (c Daddr --) "SET Char"
- **!** (n Daddr --) "SET"
- **LIT2** (-- D) push next pointer to datastack as DOUBLE
- **LIT** (-- n) push next pointer to datastack as single CELL
- **DOCOL** (-- &f_docol) constant = codeword for words
- **BASE** (-- Daddr) variable = numerical base 
- **STATE** (-- Daddr) variable = STATE
- **LAST** (-- Daddr) variable = start of latest head
- **HERE** (-- Daddr) variable = first unused character in RAM
- **PORTx** **DDRx** **PINx** - addreses for pin manipulation (x=A..L for atmega2560, x=B..C for atmega328P)

Hardware
*********

VGA
++++

Prvním krokem bylo nějak zajistit fyzické připojení konektorů pro VGA monitor, kompozitní monitor (RCA) a PS/2 klávesnici - to bylo zrealizováno na univerzální destičce a ta byla vybavena i piny pro snadné osazení na breadboard. Přepínače napájení ji umožňují připojit bez ohledu na jeho polaritu - ta jde nastavit na dvou jumperech u breadboardu. Že je připojena správně se pozná podle LED na ní umístěné.


|conectors.jpg| |PCB.png| |conectorsSchema.png|

Výstup na VGA je řízen pomocí pinů Hsync, Vsync a VGA Signál jako "jednoduchý" černobílý obraz.
Pro ovládání barev na VGA slouží vstupy Red, Green, Blue a Gray, které nastavují, jakou barvu má popředí, ale z výkonových důvodů je atmega328P nastavuje jen na začátku řádky znaků, takže všechny znaky na řádce mají stejnou barvu.
Pro správnou funkci je potřeba během každých 8 cyklů procesoru vystavit dalších 8bitů obrazu, takže moc volnosti k vylepšování už není.

Zde je schema breadboardu kde je vidět, že signál jde paralelně jako byte přez celý port B a je následně převeden do sériové podoby pomocí shift-registru **74HC165** (šlo by použít i 74HC166, ale ten jsem doma neměl). Jako zdroj hodin slouží pin D8, na který je přiveden hodinový signál krystalu pomocí správně nastavených fuses.

|breadboard-008-PS2-RCA-VGA-Nano.png|

RCA
++++

Po přepnutí na RCA se naopak signál generuje pomocí vnitřního UART taktovaného na maximální rychlost, tedy na polovinu frekvence procesoru.
Pro dosažní patřičných napěťových úrovní (0V pro synchronizaci, 0.3V pro černou barvu a 1.0V pro barvu bílou) byl navržen oporový dělič R2 (440 |Ohm| ) R3 (1 |kOhm|) a odpor vlastního vstupu (standardních 75 |Ohm|). Tím se pro logickou nulu na výstupu dostanou 0.3V a pro logickou jedničku 1.0V. Pro dosažení 0V je použit tranzistor Q1.
Protože UART vysílá na začátku jeden pulz a po skončení výstup odpojí (a tedy na něm není žádný signál), bylo nutno potlačit výstup pomocí hradla AND realizovaného 1/2 **74HC00**.


PS/2
+++++

Z důvodu relativně pozvolných hran PS/2 signálu je tento veden přez Schmitt hradla **74HC14** a zároveň je z něj odvozena obálka hodin přivedená na D13 - mimo tuto obálku nenní třeba data sledovat vůbec. Také může být použita pro zajištění, že pokud nějaký hodinový pulz není rozpoznán, tak se tato chyba nebude převádět do dalších datových rámců.


8bitový počítač za cenu oběda
******************************

* **Procesor**:
	* **ATmega328P** - **Arduino Nano**, cena okolo 120,- kč
	* **16MHz** (asi 90% zabere generování obrazu a 10% zbývá pro uživatelský program)
	* **32kB FLASH** (z toho asi 1/2 - 3/4 zbývají pro uživatelské programy a fonty)
	* **2kB RAM** (1kB videoRAM pro text 37x25 a barvy řádků, zbytek pro uživatele)
	* obraz je generován **on-the-fly** ze znaků ve VideoRAM a fontů ve Flash (kompletní font zabere 2.25kB)
* **konektory**:
	* **VGA** pro běžné monitory, projektory a podobná zařízení, přez běžně dostupné konverze lze připojit i např. HDMI
	* **RCA** pro malé "couvací" monitory z aliexpresu a starší zařízení s kompozitním vstupem
	* výstup lze přepínat za běhu, program nic nepozná, pořád píše do stejné VideoRAM
	* **PS/2** pro klávesnici. Ne, není to USB. Ano, běžně jsou k sehnání nové PS/2 klávesnice a značná část počítačů má pro ně vyvedený konektor, takže tu budou ještě dlouho.
	* (**USB** pro nahrávání programů z PC a napájení. **Nelze** použít pro komunikaci za běhu, protože příslušné RxTx piny jsou využity jinak.)
* **další součástky**:
	* **74HC165** posuvný registr pro VGA výstup (74HC166 by šel taky použít)
	* **74HC14** (6*Schmitt NOT) pro převod pomalých hran PS/2 na signál bez zákmitů (plus jako invertor pro RCA)
	* **74HC08** (4*AND) pro skládání VGA barev se signálem
	* **74HC00** (4*NotAND) pro RCA potlačení nežádoucích pulzů a čistýý signál
	* **S8050** tranzistor pro přitažení RCA synchronizace k zemi (jakýkoli rozumný tranzistor stačí)
	* trocha odporů a podobné drobotiny
	* univerzální destička a **breadboard**, drátky

Na předváděné sestavě je současně **nainstalováno**:
	* **F12** přepíná výstup mezi **VGA** a **RCA**
	* **F4** hra **Tetris** - ovládání šipkami, doleva, doprava posun, nahoru rotace, dolu zrychlení pádu. Klasicky při zaplnění celé řádky řádka zmizí, cílem je vydržet co nejdéle. S přibývajícím časem se hra zrychluje.
	* **F6** hra **Had** (lze hrát jak v **ASCII**, tak **pseudografice** s příslušným fontem), šipkami se řídí směr hada, nesmí narazit do zdi, ani do sebe, po sežrání ovoce se tělo prodlouží. Cílem je vydržet co nejdéle a sníst co nejvíc ovoce. S přibývajícím časem se hra zrychluje.
	* **F5** **Matrix** screen saver (taky se spustí po nějaké době sám). Mezerníkem (nebo jiným písmenem) se ukončí
	* **F7** zobrazování nainstalovaných **znakových sad**. Tečkou se přepíná na další sadu. Teď je nainstalována sada klasická a modifikace pro Hada
	* **F1** zápis **10.000** znaků z **C**\čka na souřadnice ve **VideoRAM**
	* **F2** 1.000x přepis celé obrazovky znakovou sadou (čili asi **1.000.000** znaků ve **strojáku**) (zapisuje dokola znaky 00..FF)
	* **F3** 1.000x přepis celé obrazovky dalším jedním znakem (čili asi 1.000.000 znaků ve strojáku) (zapisuje stejný znak na celou obrazovku, pak přejde k dalšímu 00..FF a dokola)
	* normální písmena se zobrazují a přepisují obsah, šipky pohybují kurzorem, <Esc> přepíše obrazovku předchozím znakem
	* hry pro zpomalení na lidskou rychlost používají čekací rutinu (interně vázanou na začátek obrazovky, ale to je jedno, jen to zajišťuje pravidelnost)
	* programy běží na **plný výkon**, nestarají se o zobrazování, píší do **VideoRAM** jak je napadne, zobrazovací rutiny používají **přerušení** a zajišťují zobrazování **transparentně** pro uživatelský program.
	* obrazovka je v tuto chvíli **37x25**, kvůli problémům se stabilitou při čtení PS/2, výhledově **40x25** (při rozlišení **320x200** bodů)


Program a schémata k nalezení například na  `<http://mix.gilhad.cz//HW/PS2-RCA-VGA/MakeFair2025.html>`__ a `<https://github.com/githubgilhad/NanoHomeComputer.git>`__

Založeno na sloučení a rozšíření projektů  `Squeezing Water from Stone 3: Arduino Nano + 1(!) Logic IC = Computer with VGA and PS/2 <https://github.com/slu4coder/YouTube>`__ a `Composite video from Arduino UNO <https://www.youtube.com/watch?v=Th18tLP86WQ>`__ 

.. |breadboard-008-PS2-RCA-VGA-Nano.png| image:: breadboard-008-PS2-RCA-VGA-Nano.png
	:width: 250
	:align: top
	:target: breadboard-008-PS2-RCA-VGA-Nano.png

.. |conectors.jpg| image:: conectors.jpg
	:width: 250
	:align: top
	:target: conectors.jpg

.. |conectorsSchema.png| image:: conectorsSchema.png
	:width: 250
	:align: top
	:target: conectorsSchema.png


License
-------
GPL 2 or GPL 3 - choose the one that suits your needs.

Author
------
Gilhad - 2025
