# Tic-Tac-Toe with AI on a PIC16F1939

A hardware tic-tac-toe game built around the PIC16F1939 microcontroller, playable either between two people or against a built-in computer opponent. The board is a 3×3 grid of bi-color LEDs driven by timer-interrupt multiplexing, and the firmware is written in C for the XC8 compiler. The full circuit and PCB were designed and simulated in Proteus.

> **Note:** Apart from this summary, everything in this project is written in Bosnian: the rest of this README, the source code, the comments and the technical documentation.

> **Napomena:** Izvorni kod, komentari i tehnička dokumentacija za ovaj projekat su napisani na bosanskom jeziku.

## O projektu

Projekat je rađen u okviru predmeta **Praktikum elektronike** na Elektrotehničkom fakultetu Univerziteta u Sarajevu (završen u septembru 2022. godine).

Iks-oks je igra pune informacije za dva igrača na tabli 3×3. Igra je riješena, što znači da se pri optimalnoj igri oba igrača uvijek završava neriješeno. Implementirano rješenje omogućava:

- **igru dva igrača** (PvP);
- **igru protiv računara** (PvE), pri čemu se može izabrati da li računar igra prvi.

Stanje table prikazuje se na mreži dvobojnih svijetlećih dioda: **zelena boja označava X, a crvena O**. Potezi se unose tasterima, po jedan za svako polje, a dodatni tasteri služe za izbor režima i početak igre.

## Metodologija

### Prikaz stanja table

Izlazi mikrokontrolera multipleksiraju se pomoću prekida tajmera **TMR0**. Uz oscilator od 8 MHz i preskaler 128, prekid se javlja otprilike svakih 16 ms. Pri svakom prekidu mijenja se stanje zajedničkog pina dioda, pa se naizmjenično prikazuju zelene i crvene diode. Puni ciklus od dva prekida (oko 32 ms) dovoljno je brz da ljudsko oko ne primjećuje treperenje.

### Umjetna inteligencija

Računar bira potez prema sljedećim prioritetima:

1. ako može pobijediti u ovom potezu, igra pobjednički potez;
2. ako protivnik može pobijediti u svom idućem potezu, blokira ga;
3. u suprotnom igra prema unaprijed definisanoj listi preferiranih polja.

Ovakav algoritam nije savršen, ali je dovoljno brz za mikrokontroler s ograničenim resursima. Razvijen je i savršen algoritam zasnovan na minimax pristupu, koji bi uvijek završavao pobjedom ili neriješenim rezultatom. On nije uključen u konačnu verziju jer je po prirodi rekurzivan, a kompajler XC8 ne podržava rekurziju zbog ograničenog steka poziva funkcija na ovom mikrokontroleru.

### Sklopovlje

Kompletna shema i štampana pločica (PCB) dizajnirani su u programskom paketu **Proteus**, uz 3D prikaz gornje i donje strane pločice. Rad sistema je provjeren simulacijom.

## Struktura repozitorija

```
.
├── MPLAB X/
│   ├── novaverzija1.c              # Izvorni kod firmvera (C, XC8)
│   └── Berina.X.production.hex     # Prevedeni firmver za PIC16F1939
├── Proteus/
│   ├── PE_Projekat_Simulacija.pdsprj   # Shema za simulaciju
│   └── PE_Projekat_PCB.pdsprj          # Dizajn štampane pločice
├── Izvještaj/                      # LaTeX izvorni kod rada, slike i finalni PDF (PE_seminarski.pdf)
└── Prezentacija/                   # Prezentacija projekta (PPTX i PDF)
```

## Pokretanje

### Prevođenje firmvera

**Preduslovi:** MPLAB X IDE i kompajler XC8.

1. U MPLAB X-u kreirati novi projekat (*Standalone Project*) za mikrokontroler **PIC16F1939**.
2. Dodati datoteku `MPLAB X/novaverzija1.c` u *Source Files*.
3. Prevesti projekat (*Production → Build Main Project*). Rezultat je `.hex` datoteka.

Konfiguracijski biti (HS oscilator, isključen watchdog i drugo) postavljeni su direktno u kodu pomoću `#pragma config`. Očekuje se kristal od 8 MHz.

### Simulacija u Proteusu

1. Otvoriti `Proteus/PE_Projekat_Simulacija.pdsprj`.
2. U svojstvima mikrokontrolera (*Edit Properties → Program File*) izabrati `MPLAB X/Berina.X.production.hex` ili novoprevedenu `.hex` datoteku.
3. Pokrenuti simulaciju i unositi poteze tasterima.

### Prevođenje izvještaja

```bash
cd Izvještaj
pdflatex bare_jrnl.tex
bibtex bare_jrnl
pdflatex bare_jrnl.tex
pdflatex bare_jrnl.tex
```

## Autor

- **Student:** Berina Biberović
- **Predmetni profesor:** red. prof. dr Abdulah Akšamović
- **Ustanova:** Elektrotehnički fakultet, Univerzitet u Sarajevu
