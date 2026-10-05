/*
 * File:   main.c
 * Author: Beri
 * Projekat iz PE: Iks-oks igra
 * Created on January 23, 2021, 2:45 AM
 * Fixed on February 3, 2022
 * Finished on September 16, 2022
 */


//Ovaj kod koristi *pojednostavljeni* minimax algoritam kako bi AI mogao da
//skonta najbolji potez
//zbog toga sto je klasicni minimax algoritam prespor

#define _XTAL_FREQ 8000000

#include <xc.h>
#include <stdio.h>

#pragma config FOSC=HS,WDTE=OFF,PWRTE=OFF,MCLRE=ON,CP=OFF,CPD=OFF,BOREN=OFF,CLKOUTEN=OFF
#pragma config IESO=OFF,FCMEN=OFF,WRT=OFF,VCAPEN=OFF,PLLEN=OFF,STVREN=OFF,LVP=OFF

#define OUT_LED0 (LATDbits.LATD5)
#define OUT_LED1 (LATDbits.LATD6)
#define OUT_LED2 (LATDbits.LATD7)
#define OUT_LED3 (LATCbits.LATC6)
#define OUT_LED4 (LATCbits.LATC7)
#define OUT_LED5 (LATDbits.LATD4)
#define OUT_LED6 (LATDbits.LATD3)
#define OUT_LED7 (LATCbits.LATC4)
#define OUT_LED8 (LATCbits.LATC5)

#define OUT_LEDCOM (LATDbits.LATD2)

#define IN_0 (PORTAbits.RA0)
#define IN_1 (PORTAbits.RA1)
#define IN_2 (PORTAbits.RA3)
#define IN_3 (PORTCbits.RC2)
#define IN_4 (PORTAbits.RA2)
#define IN_5 (PORTAbits.RA4)
#define IN_6 (PORTCbits.RC1)
#define IN_7 (PORTCbits.RC0)
#define IN_8 (PORTAbits.RA5)

#define IN_PVP (PORTBbits.RB5)
#define IN_AIO (PORTBbits.RB4)
#define IN_PLAY (PORTBbits.RB3)

char tablica[9] = {}; // 0 - nista nije postavljeno, 1 - prvi igrac (X), 2 - drugi igrac(O)

char igramo = 0;
char stanjeZajednickogPina = 0;

// Pinovi za ispis, redom:
// RC0, RC1, RC2, RC3, RC4, RC5, RC6, RC7, RB7
// Zajednicki pin za ispis:
// RB6
// 9+1

// Pinovi za upis, redom:
// RA0, RA1, RA2, RA3, RA4, RA5, RB1, RB2, RB3
// 9

// Pinovi za PVP/PVE, AI X/O i PLAY, redom:
// RD0, RD1, RD2
// 1+1+1


// Problematicni/rezervisani pinovi:
// RA6, RA7, RE3


// Resetovati stanje tablica[] a stanje ledica ce se azurirati prilikom 
// iduceg prekida
void Reset() {
    for(char i = 0; i < 9; i++) {
        tablica[i] = 0;
    }
}

// Funkcija koja radi log2(broj)
//char Log2(char broj) {
//	if(broj & 1 << 7) return 7;
//	if(broj & 1 << 6) return 6;
//	if(broj & 1 << 5) return 5;
//	if(broj & 1 << 4) return 4;
//	if(broj & 1 << 3) return 3;
//	if(broj & 1 << 2) return 2;
//	if(broj & 1 << 1) return 1;
//	return 0;	
//}

// Ova funkcija gleda da li je ostalo poteza jos koji se mogu igrati (dakle,
// gleda ima li jos praznih mjesta)
// funkcija vraca 1 ako ima praznih mjesta, a 0 ako nema vise
char DaLiJeOstaloPoteza(void) {
    for(char i = 0; i < 9; i++)
        if(tablica[i] == 0) return 1;
    return 0;
}

// Ova funkcija provjerava da li je neko od igraca pobijedio
// vratice 0 ukoliko nije niko pobijedio, 10 ako je pobijedio player1 i 
// -10 ako je pobijedio player2/AI
int ProvjeraPobjede(void) {
    //+10 oznacava da je pobijedio X, a 20 oznacava da je pobijedio O
    //provjerava prvo redove
    for(char i = 0; i < 9; i += 3) {
        if(tablica[i] == tablica[i+1] && tablica[i] == tablica[i+2]) {
            if(tablica[i] == 1) return 10;
            else if(tablica[i] == 2) return 20; 
        }
    }
    //onda provjerava kolone
    for(char i = 0; i < 3; i++) {
        if(tablica[i] == tablica[i+3] && tablica[i] == tablica[i+6]) {
            if(tablica[i] == 1) return 10;
            else if(tablica[i] == 2) return 20;
        }
    }
    //sada dijagonalno provjeravamo
    //prva dijagonala:
    if(tablica[0] == tablica[4] && tablica[0] == tablica[8]) {
        if(tablica[0] == 1) return 10;
        else if(tablica[0] == 2) return 20;
    }
    //druga dijagonala
    if(tablica[2] == tablica[4] && tablica[2] == tablica[6]) {
        if(tablica[2] == 1) return 10;
        else if(tablica[2] == 2) return 20;
    }
    //ukoliko niko nije pobijedio, vracamo 0
    return 0;
}

// Pomocna funkcija za interrupt
void PromjenaPraznih(void) {
	if(tablica[0] == 0) OUT_LED0 = OUT_LEDCOM;
    if(tablica[1] == 0) OUT_LED1 = OUT_LEDCOM;
	if(tablica[2] == 0) OUT_LED2 = OUT_LEDCOM;
	if(tablica[3] == 0) OUT_LED3 = OUT_LEDCOM;
	if(tablica[4] == 0) OUT_LED4 = OUT_LEDCOM;
	if(tablica[5] == 0) OUT_LED5 = OUT_LEDCOM;
	if(tablica[6] == 0) OUT_LED6 = OUT_LEDCOM;
	if(tablica[7] == 0) OUT_LED7 = OUT_LEDCOM;
	if(tablica[8] == 0) OUT_LED8 = OUT_LEDCOM;
    
    // Za sva polja koja ne trebaju svijetliti pobrinuti se da su anode i 
    // katode na istim potencijalima
}

// Funkcija koja ispisuje na ledice stanje ploce
void IspisNaLedice(void) { 
	// Zelena boja je x, a crvena je o
    if(tablica[0] == 1) OUT_LED0 = 1;
    if(tablica[0] == 2) OUT_LED0 = 0;
    if(tablica[1] == 1) OUT_LED1 = 1;
    if(tablica[1] == 2) OUT_LED1 = 0;
    if(tablica[2] == 1) OUT_LED2 = 1;
    if(tablica[2] == 2) OUT_LED2 = 0;
    if(tablica[3] == 1) OUT_LED3 = 1;
    if(tablica[3] == 2) OUT_LED3 = 0;
    if(tablica[4] == 1) OUT_LED4 = 1;
    if(tablica[4] == 2) OUT_LED4 = 0;
    if(tablica[5] == 1) OUT_LED5 = 1;
    if(tablica[5] == 2) OUT_LED5 = 0;
    if(tablica[6] == 1) OUT_LED6 = 1;
    if(tablica[6] == 2) OUT_LED6 = 0;
    if(tablica[7] == 1) OUT_LED7 = 1;
    if(tablica[7] == 2) OUT_LED7 = 0;
    if(tablica[8] == 1) OUT_LED8 = 1;
    if(tablica[8] == 2) OUT_LED8 = 0;
    
    // Ova funkcija ne radi nista sa poljima koja trebaju biti ugasena
    // Njima se bavi funkcija PromjenaPraznih())
}




// Ova funkcija provjerava da li je player uradio validan potez
// Ako jeste validan potez vraca 1, ako nije onda vraca 0
char ProvjeraValidnostiPoteza(int indeks) {
    if(tablica[indeks] == 0) return 1;
    return 0;
}

// Pomocna funkcija za unos poteza
void Unos(char kojiJeIgrac, int lokacija) { // kojiJeIgrac == 1 -> prvi igrac (X/zeleni), kojiJeIgrac == 2 -> drugi igrac (O/crveni)
	tablica[lokacija] = kojiJeIgrac;
}

char UpisaniIndeks() {
    if(IN_0) return 0;
    if(IN_1) return 1;
    if(IN_2) return 2;
    if(IN_3) return 3;
    if(IN_4) return 4;
    if(IN_5) return 5;
    if(IN_6) return 6;
    if(IN_7) return 7;
    if(IN_8) return 8;
    return 9;
}

// Funkcija koja unosi potez
void Player(char kojiJeIgrac) { // kojiJeIgrac == 1 -> prvi igrac (X), kojiJeIgrac == 2 -> drugi igrac (O)
	while(1) {
		while(UpisaniIndeks() == 9) {
            if(!igramo) return;
        } // Nek vrti dok player nije nista unio
        
        char indeks = UpisaniIndeks();
        if(indeks == 9) continue; // Moguce je da je u medjuvremenu iskljucen
        // switch
        char validan = ProvjeraValidnostiPoteza(indeks);
        
        if(validan) {
			Unos(kojiJeIgrac, indeks);
			break;
		}
	}
}

// Nova verzija pvp funkcije
// Pozove se na pocetku igre izmedju dvije osobe
void PvP(char kojiJeIgrac) {
	while(ProvjeraPobjede() == 0 && DaLiJeOstaloPoteza() == 1) {
        if(!igramo) {
            Reset();
            return;
        }
		Player(kojiJeIgrac);
        
		kojiJeIgrac = 3 - kojiJeIgrac; // 1->2 i 2->1
	}
}

// Funkcija koja odredjuje da li jedan igrac moze pobijediti na svom iducem potezu
char PobjednickiPotez(char kojiJeIgrac) {
    // Vraca indeks polja (0-8) ako postoji pobjednicki potez za ovog igraca
    // Vraca 9 ako ne postoji
    
    // Za svako polje provjeri pojedinacno da li u ostala dva polja tog reda,
    // kolone ili jedne od dijagonala su znakovi istog igraca
    if(tablica[0] == 0) {
        if(tablica[1] == kojiJeIgrac && tablica[2] == kojiJeIgrac) return 0;
        if(tablica[4] == kojiJeIgrac && tablica[8] == kojiJeIgrac) return 0;
        if(tablica[3] == kojiJeIgrac && tablica[6] == kojiJeIgrac) return 0;
    }
    if(tablica[1] == 0) {
        if(tablica[0] == kojiJeIgrac && tablica[2] == kojiJeIgrac) return 1;
        if(tablica[4] == kojiJeIgrac && tablica[7] == kojiJeIgrac) return 1;
    }
    if(tablica[2] == 0) {
        if(tablica[0] == kojiJeIgrac && tablica[1] == kojiJeIgrac) return 2;
        if(tablica[4] == kojiJeIgrac && tablica[6] == kojiJeIgrac) return 2;
        if(tablica[5] == kojiJeIgrac && tablica[8] == kojiJeIgrac) return 2;
    }
    if(tablica[3] == 0) {
        if(tablica[0] == kojiJeIgrac && tablica[6] == kojiJeIgrac) return 3;
        if(tablica[4] == kojiJeIgrac && tablica[5] == kojiJeIgrac) return 3;
    }
    if(tablica[4] == 0) {
        if(tablica[0] == kojiJeIgrac && tablica[8] == kojiJeIgrac) return 4;
        if(tablica[1] == kojiJeIgrac && tablica[7] == kojiJeIgrac) return 4;
        if(tablica[2] == kojiJeIgrac && tablica[6] == kojiJeIgrac) return 4;
        if(tablica[3] == kojiJeIgrac && tablica[5] == kojiJeIgrac) return 4;
    }
    if(tablica[5] == 0) {
        if(tablica[2] == kojiJeIgrac && tablica[8] == kojiJeIgrac) return 5;
        if(tablica[3] == kojiJeIgrac && tablica[4] == kojiJeIgrac) return 5;
    }
    if(tablica[6] == 0) {
        if(tablica[0] == kojiJeIgrac && tablica[3] == kojiJeIgrac) return 6;
        if(tablica[2] == kojiJeIgrac && tablica[4] == kojiJeIgrac) return 6;
        if(tablica[7] == kojiJeIgrac && tablica[8] == kojiJeIgrac) return 6;
    }
    if(tablica[7] == 0) {
        if(tablica[1] == kojiJeIgrac && tablica[4] == kojiJeIgrac) return 7;
        if(tablica[6] == kojiJeIgrac && tablica[8] == kojiJeIgrac) return 7;
    }
    if(tablica[8] == 0) {
        if(tablica[0] == kojiJeIgrac && tablica[4] == kojiJeIgrac) return 8;
        if(tablica[2] == kojiJeIgrac && tablica[5] == kojiJeIgrac) return 8;
        if(tablica[6] == kojiJeIgrac && tablica[7] == kojiJeIgrac) return 8;
    }
    
    return 9; // Nema pobjednickog poteza za ovog igraca
}

// Funkcija koja predlaze potez za AI/umjetnu inteligenciju
void PotezAI(char kojiJeAI, char tezina) {
    if(tezina == 1) {
        // U ovom rezimu (koji je onemogu?en) umjetna inteligencija samo
        // ima uredjenu listu polja, redom kojim preferira da ih igra
        char redoslijed[9] = {0, 8, 4, 2, 6, 1, 3, 5};
        for(char i=0;i<9;i++) {
            if(tablica[redoslijed[i]] == 0) {
                Unos(kojiJeAI, redoslijed[i]);
                break;
            }
        }
        
        return;
    }
    
    if(tezina == 2) {
        char kojiJeIgrac = 3 - kojiJeAI;
        
        // Ukoliko moze pobijediti ovaj potez, neka to i uradi
        
        char odigrati;
        odigrati = PobjednickiPotez(kojiJeAI);
        
        if(odigrati < 9) {
            Unos(kojiJeAI, odigrati);
            return;
        }
        
        // Ukoliko ne moze pobijediti ovaj potez, ali protivnik moze, onda
        // protivnikovu pobjedu pokusati blokirati
        
        odigrati = PobjednickiPotez(kojiJeIgrac);
        
        if(odigrati < 9) {
            Unos(kojiJeAI, odigrati);
            return;
        }
        
        // Ukoliko niko nece pobijediti iduci potez onda pratiti (napredniju)
        // listu preferiranih poteza, slicno strategiji u slucaju da 
        // tezina == 1
        
        char redoslijed[9] = {4, 1, 3, 0, 2, 6, 8, 5, 7};
        for(char i=0;i<9;i++) {
            if(tablica[redoslijed[i]] == 0) {
                Unos(kojiJeAI, redoslijed[i]);
                return;
            }
        }
    }
}

// Funkcija koja se pozove na pocetku igre izmedju covjeka i racunara
void PvE(char prviAI, char tezina) {
    char kojiJeIgrac = 1 + prviAI; // Pri pozivu funkcije 
    // 0 se odnosi na X, 1 na O, ali u ostatku koda 1 se odnosi na X, a 2 na O
    char kojiJeAI = 2 - prviAI;  
    
    if(prviAI) PotezAI(kojiJeAI, tezina); // Obezbjedjujemo korektan redoslijed
    // igranja
    
    while(ProvjeraPobjede() == 0 && DaLiJeOstaloPoteza() == 1) {
        if(!igramo) {
            Reset();
            return;
        }
		Player(kojiJeIgrac);
        IspisNaLedice();
        if(ProvjeraPobjede() != 0 || DaLiJeOstaloPoteza() == 0) return;
        // Obratiti paznju da ProvjeraPobjede ne vraca 1 ukoliko je neko 
        // pobijedio, vec 10 ili 20
        if(!igramo) { // Ukoliko je iskljucena igra potrebno je brzo reagovati
            Reset();
            return;
        }
		PotezAI(kojiJeAI, tezina);
        IspisNaLedice();
	}
    
    return;
}

// Frekvencija oscilatora je 8 Mhz
// Zelimo mijenjati LEDice sa periodom od ispod 50 ms (2 interrrupta)
// Ako se TMR0 postavlja na 0, a prescaler je na 128, to daje period od
// 256*128/2000000 = priblizno 16 ms
// 2 prekida su onda 32 ms, sto je prihvatljivo
void PripremiZaInterrupte() {
    OPTION_REGbits.TMR0CS = 0;
    
    OPTION_REGbits.PSA = 0; // Koristi prescaler
    OPTION_REGbits.PS2 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 0; // Prescaler postavljen na 128
    
    TMR0 = 0;
    TMR0IF = 0;
    TMR0IE = 1;
    GIE=1;
}

// Interrupt funkcija
void __interrupt() ISR(void) { //WOW
     if(INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        stanjeZajednickogPina = !stanjeZajednickogPina;
        OUT_LEDCOM = stanjeZajednickogPina; // Vezano za ispis na diodama
		PromjenaPraznih();
        IspisNaLedice();
        igramo = IN_PLAY;
        TMR0 = 0;
        INTCONbits.TMR0IF = 0;
    }
}


void main(void) {
    // Setup za timer
    PripremiZaInterrupte();
    
    //**************************************************************************
    // Ovaj program treba nakon svakog unesenog player poteza
    // da uradi neki potez
    //**************************************************************************
    // Potrebno je sada da standardno proglasim koji su mi portovi izlazni
    // a koji ulazni... stsndardna procedura
    // posto sam prije ovoga napravila generalnu shemu, to mi je onda isplanirano
    //**************************************************************************
    // U nastavku idu svi output pinovi
    // koji ce prikazivati stanje tablice
    // PORTA  digitalni
    ANSELA = 0x00; // Digitalni
    TRISA = 0xFF; // Ulazni
    // PORTB 
    ANSELB = 0x00; // Digitalni
    TRISB = 0xFF;
    // Odmah stavljam RB6 da bude 0:
    OUT_LEDCOM = 0;
    // PORTC 
//    ANSELC = 0x00;
    TRISC = 0x0F; // Izlazni
//    LATC = 0xFF;
    // PORTD nek bude cijeli ulazni
    ANSELD = 0x00; // Jer necu da mi cita dig ulaze kao log 0
    TRISD = 0x03; // 
    // PORTE takodjer osposobljavamo
//    ANSELE = 0x00;
//    TRISE = 0x07;
//    PORTEbits.RE0 = 0;
//    PORTEbits.RE1 = 0;
//    PORTEbits.RE2 = 0;
    //**************************************************************************
    // Za prazno mjesto koristim 0, za X (player1\AI) koristim 1, a za O
    // (player2/AI)koristim 2
    // tabla je na pocetku prazna:
    Reset();
    
    // Ovdje sam koristila niz umjesto matrice
    // jer mislim da mi je tako bolje zbog memorije
    // iako je praksa inace za iks oks da se koristi matrica
    //**************************************************************************
    // Na pocetku se bira pvp ili pve namjestanjem switcha
    // ako je na RD0 1 to je pvp, a ako je 0 to je pve
    // onda se bira koji player ide prvi, player 1 ili player2/AI
    // ako je RD1 1, onda player1 ide prvi, a ako je RD1 0, onda ide player2 prvi
    // START sluzi da procita sta smo namjestili na RD0 i RD1 - ako je START na
    // nuli, onda nista ne citamo, a ako je na 1, eh onda treba da krenemo sa
    // igricom
    // i START je preko switcha stavljen, na RD2
    //**************************************************************************
    //implementacija gore objasnjenog
    while(1) {
        while(igramo == 0) { // Nek vrti dokle god je 0 na START
            Reset();
        }
        // Sada ide implementacija koda za pve
        if(!IN_PVP) PvE(IN_AIO, 2); // Ako je RD1 jednak 0 onda AI igra prvi
        else PvP(1); // Prosllijedjuje se 1 jer uvijek X igra prvi
        
        while(igramo == 1); // Zavrsila je partija, nemoj nista raditi dok se ne iskljuci
    }
    
    //**************************************************************************
    return;
}

// RA6 se ne moze aktivirati iz nekog razloga u MPLAB X okruzenju
// PORTE ne mozemo koristiti kao ulazni