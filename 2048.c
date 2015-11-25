#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "peli2048.h"

#define SOLUNLEVEYS 5

/*  Italuoma Sari
    Ohjelmoinnin tekniikka, kevat 2014
    Harjoitustyo: 2048
    
    Toteutettu 3/3 toiminnallisuuskokonaisuutta.
 */

 /* Pelitilanteen tallentamisesta tiedostoon
    Pelitilanne tallennetaan tiedostoon 2048.sav komennolla 'tallenna x', jossa x on pelille annettava nimi.
    
    
    Pelitilanne tallennetaan tiedostoon seuraavasti:
    
    aloitusmerkit ***
    tallennetun pelin nimi
    nykyinen pelitilanne
        kentta
        pisteet
        vapaat kentat
    
    siirtoja jaljella pysyy falsena -> peli paattynyt vaikka tehtais undo tai ladattais peli
    enta redo?
    
 */
 
 
 /* Laskee kokonaisluvun merkkien lukumaaran. */
int countDigits(int luku) {
	int i = 0;
    
    if(luku == 0) {
        return 1;
    }
        
	do {
		i++;
		luku = luku / 10;
	}
	while (luku > 0);
	
	return i;
}

char * lueRivi(FILE *syote, int *pituus) {
    
    int tila = 16;
    char *rivi = malloc(tila*sizeof(char));
    size_t luettu = 0;
    int pit = 0;
    *pituus = 0;
    rivi[luettu] = '\0';
    while(fgets(&rivi[luettu], tila-luettu, syote) != NULL) {
        
        pit = strlen(&rivi[luettu]);
       
        /* rivi luettiin loppuun asti */
        if(pit < tila - luettu - 1 || ( pit == tila - luettu - 1 && rivi[tila-2] == '\n')) {
            
            if(pit > 0 && rivi[luettu+pit-1] == '\n') {
                rivi[luettu+pit-1] = '\0';
            }   
            
            rivi = realloc(rivi, (strlen(rivi) + 1)*sizeof(char));
            
            *pituus = strlen(rivi);
            return rivi;
        }
        
        luettu = tila - 1;
        tila = tila*2;
        
        rivi = realloc(rivi, tila*sizeof(char));
        
    }
        
    return rivi;
}

/* Tulostaan pelikentan. */
void tulostaKentta(int kentta[SARAKKEET][RIVIT], int peliOnpaattynyt) {
    int x = 0;
    int y = 0;
    int digits = 0;
    while(y < RIVIT) {
        x = 0;
        while(x < SARAKKEET) {
            digits = countDigits(kentta[x][y]);
            while(SOLUNLEVEYS - digits > 0) {
                printf(" ");
                digits++;
            }
            if(kentta[x][y] == 0)
                printf(" ");
            else
                printf("%i", kentta[x][y]);
            x++;
        }
        printf("\n");
        y++;
    }
    if(peliOnpaattynyt)
        printf("Peli on päättynyt!\n");
}

/* etsitaan kentasta n:nnes vapaa ruutu ja palautetaan sen koordinaatit */
Koordinaatti etsiVapaaRuutu(int kentta[SARAKKEET][RIVIT], int n) {
    int eiValmis = 1;
    Koordinaatti k;
    
    
    
    k.y = 0;
    while(eiValmis && k.y < RIVIT) {
        
        k.x = 0;
        while(eiValmis && k.x < SARAKKEET) {
            
            if(kentta[k.x][k.y] == 0)
                n--;
            if(n == 0)
                goto ETSIVAPAARUUTULOPPU;
                
            
            k.x += 1;
        }
        k.y += 1;
    }
    
    ETSIVAPAARUUTULOPPU:
    
    return k;
}

int main () {
    
    int luku1, luku2, luku3, luku4, luku5, luku6;
    /* kentta sisaltaa taulukon luvut, 0 jos ruudussa ei ole mitaan */
    int kentta[SARAKKEET][RIVIT];
    char *komento = NULL;
    int pituus = 1;
    int pisteet = 0;
    /* alkutilanteessa kentassa on kaksi laattaa. */
    int vapaatKentat = (SARAKKEET * RIVIT) - 2;
    int siirtojaOnJaljella = 1;
    int laattojaSiirrettiin = 0;
    int vertailukohta = 0;
    
    /* taulukko johon tallennetaan pelitilanteet alusta asti. */
    Pelitilanne *tilanteet = malloc(sizeof(Pelitilanne));
    Pelitilanne tilanne;
    int nykyinenPelitilanne = 0;
    int tilanteetKoko = 1;
    
    /* tallennettavan pelin nimi. */
    char *pelinNimi = malloc(sizeof(char));
    char tiedosto[8] = "2048.sav";
    FILE *td = NULL;
    char *rivi = NULL;
    char luku[4] = "    ";
    char **tiedostonSisalto = NULL;
    
    /* erilaisia koordinaattimuuttujia */
    int x = 0;
    int y = 0;
    int x2 = 0;
    int y2 = 0;
    Koordinaatti koord;
    koord.x = 0;
    koord.y = 0;
    
    /* alustetaan kentta nollilla */
    while(x < SARAKKEET) {
        y = 0;
        while(y < RIVIT) {
            kentta[x][y] = 0;
            y++;
        }
        x++;
    }
    
    
    /* arvotaan kenttaan aluksi kaksi lukua */
    /* arvotaan aloituskohtien koordinaatit */
    srand(time(NULL));
    x = rand() % SARAKKEET;
    y = rand() % RIVIT;
    x2 = rand() % SARAKKEET;
    y2 = rand() % RIVIT;
    /* arvotaan uusia koordinaatteja kunnes parit eivat ole samat */
    while(x == x2 && y == y2) {
        x2 = rand() % SARAKKEET;
        y2 = rand() % RIVIT;
    }
    
    /* arvotaan mita numeroita aloituskoordinaatteihin laitetaan */
    if(rand() % 3) {
        kentta[x][y] = 2;
        kentta[x2][y2] = 4;
    }
    else if(rand() % 2) {
        kentta[x][y] = 4;
        kentta[x2][y2] = 4;
    }
    else {
        kentta[x][y] = 2;
        kentta[x2][y2] = 2;
    }
    
    /* kopioidaan aloitustilanne tilanteet-taulukon alkuun. */
    y = 0;
    while(y < RIVIT) {
        x = 0;
        while(x < SARAKKEET) {
            tilanne.laatat[x][y] = kentta[x][y];
            x++;
        }
        y++;
    }
    tilanne.pisteet = 0;
    tilanne.vapaatkentat = (SARAKKEET * RIVIT) - 2;
    
    tilanteet[0] = tilanne;
    nykyinenPelitilanne = 0;
    tilanteetKoko = 1;
    
    
    /* luetaan kayttajalta komentoja kunnes annetaan komento 'lopeta'. */
    while(1) {
        
        komento = lueRivi(stdin, &pituus);
        pelinNimi = realloc(pelinNimi, (pituus+1) * sizeof(char));
        
        /* muuttuja pitaa kirjaa siita siirsiko annettu komento laattoja. */
        laattojaSiirrettiin = 0;
        
        if(strcmp(komento, "resetoi") == 0) {
            vapaatKentat = (SARAKKEET * RIVIT) - 2;
            pisteet = 0;
            x = 0;
            while(x < SARAKKEET) {
                y = 0;
                while(y < RIVIT) {
                    kentta[x][y] = 0;
                    y++;
                }
                x++;
            }
            /* kaksi aloituslukua satunnaisiin kohtiin */
            /* arvotaan aloituskohtien koordinaatit */
            srand(time(NULL));
            
            x = rand() % SARAKKEET;
            y = rand() % RIVIT;
            x2 = rand() % SARAKKEET;
            y2 = rand() % RIVIT;
            
            /* arvotaan uusia koordinaatteja kunnes parit eivat ole samat */
            while(x == x2 && y == y2) {
                x2 = rand() % SARAKKEET;
                y2 = rand() % RIVIT;
            }
            
            /* arvotaan mita numeroita aloituskoordinaatteihin laitetaan */
            if(rand() % 3) {
                kentta[x][y] = 2;
                kentta[x2][y2] = 4;
            }
            else if(rand() % 2) {
                kentta[x][y] = 4;
                kentta[x2][y2] = 4;
            }
            else {
                kentta[x][y] = 2;
                kentta[x2][y2] = 2;
            }
            
            /* kopioidaan aloitustilanne tilanteet-taulukon alkuun. */
            /* Taulukon lopun ylimaaraiset tilanteet tuhoutuvat kun ensimmainen siirto tehdaan. */
            y = 0;
            while(y < RIVIT) {
                x = 0;
                while(x < SARAKKEET) {
                    tilanne.laatat[x][y] = kentta[x][y];
                    x++;
                }
                y++;
            }
            tilanne.pisteet = 0;
            tilanne.vapaatkentat = (SARAKKEET*RIVIT) - 2;
            
            tilanteet[0] = tilanne;
            nykyinenPelitilanne = 0;
            tilanteetKoko = 1;
            siirtojaOnJaljella = 1;
            
        }   
        else if(sscanf(komento, "resetoi %d %d %d %d %d %d", &luku1, &luku2, &luku3, &luku4, &luku5, &luku6) == 6) {
            pisteet = 0;
            vapaatKentat = (SARAKKEET * RIVIT) - 2;
            x = 0;
            while(x < SARAKKEET) {
                y = 0;
                while(y < RIVIT) {
                    kentta[x][y] = 0;
                    y++;
                }
                x++;
            }
            kentta[luku2][luku1] = luku3;
            kentta[luku5][luku4] = luku6;
            
            /* kopioidaan aloitustilanne tilanteet-taulukon alkuun. */
            /* Taulukon lopun ylimaaraiset tilanteet tuhoutuvat kun ensimmainen siirto tehdaan. */
            y = 0;
            while(y < RIVIT) {
                x = 0;
                while(x < SARAKKEET) {
                    tilanne.laatat[x][y] = kentta[x][y];
                    x++;
                }
                y++;
            }
            tilanne.pisteet = 0;
            tilanne.vapaatkentat = (SARAKKEET*RIVIT) - 2;
            
            tilanteet[0] = tilanne;
            nykyinenPelitilanne = 0;
            tilanteetKoko = 1;
            siirtojaOnJaljella = 1;
            
        }
        else if(strcmp(komento, "tilanne") == 0) {
            tulostaKentta(kentta, !siirtojaOnJaljella);
        }
        else if(strcmp(komento, "undo") == 0) {
            if(nykyinenPelitilanne > 0) {
                /* kopioidaan edellinen pelitilanne kenttaan. */
                nykyinenPelitilanne--;
                
                y = 0;
                while(y < RIVIT) {
                    x = 0;
                    while(x < SARAKKEET) {
                        kentta[x][y] = tilanteet[nykyinenPelitilanne].laatat[x][y];
                        x++;
                    }
                    y++;
                }
                
                vapaatKentat = tilanteet[nykyinenPelitilanne].vapaatkentat;
                pisteet = tilanteet[nykyinenPelitilanne].pisteet;
                
                siirtojaOnJaljella = 1;
            }
        }
        else if(strcmp(komento, "redo") == 0) {
            if((nykyinenPelitilanne + 1) < tilanteetKoko) {
                /* kopioidaan edellinen pelitilanne kenttaan. */
                nykyinenPelitilanne++;
                
                y = 0;
                while(y < RIVIT) {
                    x = 0;
                    while(x < SARAKKEET) {
                        kentta[x][y] = tilanteet[nykyinenPelitilanne].laatat[x][y];
                        x++;
                    }
                    y++;
                }
                
                vapaatKentat = tilanteet[nykyinenPelitilanne].vapaatkentat;
                pisteet = tilanteet[nykyinenPelitilanne].pisteet;
            }
            
        }
        else if(strcmp(komento, "ylös") == 0) {
            /* kaydaan ruudukkoa lapi sarakkeittain vasemmalta oikealle */
            x = 0;
            while(x < SARAKKEET) {
                y = 0;
                y2 = 1;
                vertailukohta = 0;
                
                /* siirretaan laatat vasempaan reunaan ja ynnaillaan, y2 on seuraava siirrettava laatta ja y on mihin siirretaan. */
                while(y2 < RIVIT) {
                    /* jos ruutu johon ollaan siirtamassa ei ole tyhja, tutkitaan yhdistetaanko laattoja. */
                    if(kentta[x][y] != 0) {
                        /* verrataan laattoja */
                        if(kentta[x][vertailukohta] == kentta[x][vertailukohta+1]) {
                            
                            kentta[x][vertailukohta] = kentta[x][vertailukohta] + kentta[x][vertailukohta];
                            kentta[x][vertailukohta+1] = 0;
                            
                            pisteet += kentta[x][vertailukohta];
                            vapaatKentat++;
                            laattojaSiirrettiin = 1;
                            
                            vertailukohta++;
                        }
                        /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                        else if(kentta[x][vertailukohta+1] != 0) {
                            vertailukohta++;
                        }
                        y++;
                        
                    }
                    /* jos siirrettavan laatan ruutu y2 on epatyhja (ja y on tyhja), siirretaan laattaa ja vertaillaan */
                    else if(kentta[x][y2] != 0) {
                        kentta[x][y] = kentta[x][y2];
                        kentta[x][y2] = 0;
                        
                        laattojaSiirrettiin = 1;
                        
                        /* tutkitaan yhdistetaanko laattoja */
                        if(kentta[x][vertailukohta] == kentta[x][vertailukohta+1]) {
                            
                            kentta[x][vertailukohta] = kentta[x][vertailukohta] + kentta[x][vertailukohta];
                            kentta[x][vertailukohta+1] = 0;
                            
                            pisteet += kentta[x][vertailukohta];
                            vapaatKentat++;
                            
                            vertailukohta++;
                            y = y2 = vertailukohta;
                        }
                        else {
                            /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                            if(kentta[x][vertailukohta+1] != 0) {
                                vertailukohta++;
                            }
                            y++;
                        }
                    }
                    y2++;
                }
                x++;
            }
            
            /* uusi laatta arvotaan vain jos jokin laatta siirtyi. */
            if(laattojaSiirrettiin) {
                /* arvotaan kuinka monenteen vapaaseen kenttaan uusi numerolaatta tulee. */
                koord = etsiVapaaRuutu(kentta, (rand() % vapaatKentat) + 1);
                /* arvotaan tuleeko uuteen ruutuun 2 vai 4 */
                if(rand() % 2 == 0)
                    kentta[koord.x][koord.y] = 2;
                else 
                    kentta[koord.x][koord.y] = 4;
                
                vapaatKentat--;
                
                
            }
        }
        else if(strcmp(komento, "vasemmalle") == 0) {
                        /* kaydaan ruudukkoa lapi riveittain ylhaalta alas */
            y = 0;
            while(y < RIVIT) {
                x = 0;
                x2 = 1;
                vertailukohta = 0;
                
                /* siirretaan ruudut vasempaan reunaan ja ynnaillaan, y2 on seuraava siirrettava ruutu ja y on mihin siirretaan. */
                while(x2 < SARAKKEET) {
                    /* jos ruutu johon ollaan siirtamassa ei ole tyhja siirrytaan seuraavaan ruutuun */
                    if(kentta[x][y] != 0) {
                        /* verrataan ruutuja */
                        if(kentta[vertailukohta][y] == kentta[vertailukohta+1][y]) {
                            
                            kentta[vertailukohta][y] = kentta[vertailukohta][y] + kentta[vertailukohta][y];
                            kentta[vertailukohta+1][y] = 0;
                            
                            pisteet += kentta[vertailukohta][y];
                            vapaatKentat++;
                            laattojaSiirrettiin = 1;
                            
                            vertailukohta++;
                        }
                        /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                        else if(kentta[vertailukohta+1][y] != 0) {
                            vertailukohta++;
                        }
                        x++;
                        
                    }
                    /* jos siirrettava ruutu ei ole tyhja, siirretaan laattaa ja vertaillaan */
                    else if(kentta[x2][y] != 0) {
                        kentta[x][y] = kentta[x2][y];
                        kentta[x2][y] = 0;
                        
                        laattojaSiirrettiin = 1;
                        
                        if(kentta[vertailukohta][y] == kentta[vertailukohta+1][y]) {
                            
                            kentta[vertailukohta][y] = kentta[vertailukohta][y] + kentta[vertailukohta][y];
                            kentta[vertailukohta+1][y] = 0;
                            
                            pisteet += kentta[vertailukohta][y];
                            vapaatKentat++;
                            
                            vertailukohta++;
                            x = x2 = vertailukohta;
                        }
                        else {
                            /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                            if(kentta[vertailukohta+1][y] != 0) {
                                vertailukohta++;
                            }
                            x++;
                        }
                    }
                    x2++;
                }
                y++;
            }
            
            /* uusi laatta arvotaan vain jos jokin laatta siirtyi. */
            if(laattojaSiirrettiin) {
                /* arvotaan kuinka monenteen vapaaseen kenttaan uusi numerolaatta tulee. */
                koord = etsiVapaaRuutu(kentta, (rand() % vapaatKentat) + 1);
                /* arvotaan tuleeko uuteen ruutuun 2 vai 4 */
                if(rand() % 2 == 0)
                    kentta[koord.x][koord.y] = 2;
                else 
                    kentta[koord.x][koord.y] = 4;
                
                vapaatKentat--;
            }
                
        }
        else if(strcmp(komento, "alas") == 0) {
            /* kaydaan ruudukkoa lapi sarakkeittain oikealta vasemmalle */
            x = SARAKKEET - 1;
            while(x >= 0) {
                y = RIVIT - 1;
                y2 = y - 1;
                vertailukohta = y;
                
                /* siirretaan ruudut vasempaan reunaan ja ynnaillaan, y2 on seuraava siirrettava ruutu ja y on mihin siirretaan. */
                while(y2 >= 0) {
                    /* jos ruutu johon ollaan siirtamassa ei ole tyhja siirrytaan seuraavaan ruutuun */
                    if(kentta[x][y] != 0) {
                        /* verrataan ruutuja */
                        if(kentta[x][vertailukohta] == kentta[x][vertailukohta-1]) {
                            kentta[x][vertailukohta] = kentta[x][vertailukohta] + kentta[x][vertailukohta];
                            kentta[x][vertailukohta-1] = 0;
                            
                            pisteet += kentta[x][vertailukohta];
                            vapaatKentat++;
                            laattojaSiirrettiin = 1;
                            
                            vertailukohta--;
                        }
                        /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                        else if(kentta[x][vertailukohta-1] != 0) {
                            vertailukohta--;
                        }
                        y--;
                        
                    }
                    /* jos siirrettava ruutu ei ole tyhja, siirretaan laattaa ja vertaillaan */
                    else if(kentta[x][y2] != 0) {
                        kentta[x][y] = kentta[x][y2];
                        kentta[x][y2] = 0;
                        
                        laattojaSiirrettiin = 1;
                        
                        if(kentta[x][vertailukohta] == kentta[x][vertailukohta-1]) {
                            
                            kentta[x][vertailukohta] = kentta[x][vertailukohta] + kentta[x][vertailukohta];
                            kentta[x][vertailukohta-1] = 0;
                            
                            pisteet += kentta[x][vertailukohta];
                            vapaatKentat++;
                            
                            vertailukohta--;
                            y = y2 = vertailukohta;
                        }
                        else {
                            /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                            if(kentta[x][vertailukohta-1] != 0) {
                                vertailukohta--;
                            }
                            y--;
                        }
                    }
                    y2--;
                }
                x--;
            }
            
            /* uusi laatta arvotaan vain jos jokin laatta siirtyi. */
            if(laattojaSiirrettiin) {
                /* arvotaan kuinka monenteen vapaaseen kenttaan uusi numerolaatta tulee. */
                koord = etsiVapaaRuutu(kentta, (rand() % vapaatKentat) + 1);
                /* arvotaan tuleeko uuteen ruutuun 2 vai 4 */
                if(rand() % 2 == 0)
                    kentta[koord.x][koord.y] = 2;
                else 
                    kentta[koord.x][koord.y] = 4;
                
                vapaatKentat--;
            }
                
        }
        else if(strcmp(komento, "oikealle") == 0) {
            /* kaydaan ruudukkoa lapi riveittain alhaalta ylös */
            y = RIVIT - 1;
            while(y >= 0) {
                x = SARAKKEET - 1;
                x2 = x - 1;
                vertailukohta = x;
                
                /* siirretaan ruudut vasempaan reunaan ja ynnaillaan, y2 on seuraava siirrettava ruutu ja y on mihin siirretaan. */
                while(x2 >= 0) {
                    /* jos ruutu johon ollaan siirtamassa ei ole tyhja siirrytaan seuraavaan ruutuun */
                    if(kentta[x][y] != 0) {
                        /* verrataan ruutuja */
                        if(kentta[vertailukohta][y] == kentta[vertailukohta-1][y]) {
                            kentta[vertailukohta][y] = kentta[vertailukohta][y] + kentta[vertailukohta][y];
                            kentta[vertailukohta-1][y] = 0;
                            
                            pisteet += kentta[vertailukohta][y];
                            vapaatKentat++;
                            laattojaSiirrettiin = 1;
                            
                            vertailukohta--;
                        }
                        /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                        else if(kentta[vertailukohta-1][y] != 0) {
                            vertailukohta--;
                        }
                        x--;
                        
                    }
                    /* jos siirrettava ruutu ei ole tyhja, siirretaan laattaa ja vertaillaan */
                    else if(kentta[x2][y] != 0) {
                        kentta[x][y] = kentta[x2][y];
                        kentta[x2][y] = 0;
                        
                        laattojaSiirrettiin = 1;
                        
                        if(kentta[vertailukohta][y] == kentta[vertailukohta-1][y]) {
                            
                            kentta[vertailukohta][y] = kentta[vertailukohta][y] + kentta[vertailukohta][y];
                            kentta[vertailukohta-1][y] = 0;
                            
                            pisteet += kentta[vertailukohta][y];
                            vapaatKentat++;
                            
                            vertailukohta--;
                            x = x2 = vertailukohta;
                        }
                        else {
                            /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                            if(kentta[vertailukohta-1][y] != 0) {
                                vertailukohta--;
                            }
                            x--;
                        }
                    }
                    x2--;
                }
                y--;
            }
            
            /* uusi laatta arvotaan vain jos jokin laatta siirtyi. */
            if(laattojaSiirrettiin) {
                /* arvotaan kuinka monenteen vapaaseen kenttaan uusi numerolaatta tulee. */
                koord = etsiVapaaRuutu(kentta, (rand() % vapaatKentat) + 1);
                /* arvotaan tuleeko uuteen ruutuun 2 vai 4 */
                if(rand() % 2 == 0)
                    kentta[koord.x][koord.y] = 2;
                else 
                    kentta[koord.x][koord.y] = 4;
                
                vapaatKentat--;
            }
                
            
        }
        else if(sscanf(komento, "ylös %d %d %d", &luku1, &luku2, &luku3) == 3) {
            /* kaydaan ruudukkoa lapi sarakkeittain vasemmalta oikealle */
            x = 0;
            while(x < SARAKKEET) {
                y = 0;
                y2 = 1;
                vertailukohta = 0;
                
                /* siirretaan ruudut vasempaan reunaan ja ynnaillaan, y2 on seuraava siirrettava ruutu ja y on mihin siirretaan. */
                while(y2 < RIVIT) {
                    /* jos ruutu johon ollaan siirtamassa ei ole tyhja siirrytaan seuraavaan ruutuun */
                    if(kentta[x][y] != 0) {
                        /* verrataan ruutuja */
                        if(kentta[x][vertailukohta] == kentta[x][vertailukohta+1]) {
                            kentta[x][vertailukohta] = kentta[x][vertailukohta] + kentta[x][vertailukohta];
                            kentta[x][vertailukohta+1] = 0;
                            
                            laattojaSiirrettiin = 1;
                            
                            pisteet += kentta[x][vertailukohta];
                            vapaatKentat++;
                            
                            vertailukohta++;
                        }
                        /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                        else if(kentta[x][vertailukohta+1] != 0) {
                            vertailukohta++;
                        }
                        y++;
                        
                    }
                    /* jos siirrettava ruutu ei ole tyhja, siirretaan laattaa ja vertaillaan */
                    else if(kentta[x][y2] != 0) {
                        kentta[x][y] = kentta[x][y2];
                        kentta[x][y2] = 0;
                        
                        laattojaSiirrettiin = 1;
                        
                        if(kentta[x][vertailukohta] == kentta[x][vertailukohta+1]) {
                            
                            kentta[x][vertailukohta] = kentta[x][vertailukohta] + kentta[x][vertailukohta];
                            kentta[x][vertailukohta+1] = 0;
                            
                            pisteet += kentta[x][vertailukohta];
                            vapaatKentat++;
                            
                            vertailukohta++;
                            y = y2 = vertailukohta;
                        }
                        else {
                            /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                            if(kentta[x][vertailukohta+1] != 0) {
                                vertailukohta++;
                            }
                            y++;
                        }
                    }
                    y2++;
                }
                x++;
            }
            
            if(laattojaSiirrettiin) { 
                /* asetetaan parametreina saatu uusi ruutu */
                kentta[luku2][luku1] = luku3;
                vapaatKentat--;
            }
            
        }
        else if(sscanf(komento, "vasemmalle %d %d %d", &luku1, &luku2, &luku3) == 3) {
            /* kaydaan ruudukkoa lapi riveittain ylhaalta alas */
            y = 0;
            while(y < RIVIT) {
                x = 0;
                x2 = 1;
                vertailukohta = 0;
                
                /* siirretaan ruudut vasempaan reunaan ja ynnaillaan, y2 on seuraava siirrettava ruutu ja y on mihin siirretaan. */
                while(x2 < SARAKKEET) {
                    /* jos ruutu johon ollaan siirtamassa ei ole tyhja siirrytaan seuraavaan ruutuun */
                    if(kentta[x][y] != 0) {
                        /* verrataan ruutuja */
                        if(kentta[vertailukohta][y] == kentta[vertailukohta+1][y]) {
                            kentta[vertailukohta][y] = kentta[vertailukohta][y] + kentta[vertailukohta][y];
                            kentta[vertailukohta+1][y] = 0;
                            
                            laattojaSiirrettiin = 1;
                            
                            pisteet += kentta[vertailukohta][y];
                            vapaatKentat++;
                            
                            vertailukohta++;
                        }
                        /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                        else if(kentta[vertailukohta+1][y] != 0) {
                            vertailukohta++;
                        }
                        x++;
                        
                    }
                    /* jos siirrettava ruutu ei ole tyhja, siirretaan laattaa ja vertaillaan */
                    else if(kentta[x2][y] != 0) {
                        kentta[x][y] = kentta[x2][y];
                        kentta[x2][y] = 0;
                        laattojaSiirrettiin = 1;
                        
                        if(kentta[vertailukohta][y] == kentta[vertailukohta+1][y]) {
                            
                            kentta[vertailukohta][y] = kentta[vertailukohta][y] + kentta[vertailukohta][y];
                            kentta[vertailukohta+1][y] = 0;
                            
                            
                            pisteet += kentta[vertailukohta][y];
                            vapaatKentat++;
                            
                            vertailukohta++;
                            x = x2 = vertailukohta;
                        }
                        else {
                            /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                            if(kentta[vertailukohta+1][y] != 0) {
                                vertailukohta++;
                            }
                            x++;
                        }
                    }
                    x2++;
                }
                y++;
            }
            
            if(laattojaSiirrettiin) {
                /* asetetaan parametreina saatu uusi ruutu */
                kentta[luku2][luku1] = luku3;
                vapaatKentat--;
            }
        }
        else if(sscanf(komento, "alas %d %d %d", &luku1, &luku2, &luku3) == 3) {
                        /* kaydaan ruudukkoa lapi sarakkeittain oikealta vasemmalle */
            x = SARAKKEET - 1;
            while(x >= 0) {
                y = RIVIT - 1;
                y2 = y - 1;
                vertailukohta = y;
                
                /* siirretaan ruudut vasempaan reunaan ja ynnaillaan, y2 on seuraava siirrettava ruutu ja y on mihin siirretaan. */
                while(y2 >= 0) {
                    /* jos ruutu johon ollaan siirtamassa ei ole tyhja siirrytaan seuraavaan ruutuun */
                    if(kentta[x][y] != 0) {
                        /* verrataan ruutuja */
                        if(kentta[x][vertailukohta] == kentta[x][vertailukohta-1]) {
                            kentta[x][vertailukohta] = kentta[x][vertailukohta] + kentta[x][vertailukohta];
                            kentta[x][vertailukohta-1] = 0;
                            
                            laattojaSiirrettiin = 1;
                            
                            pisteet += kentta[x][vertailukohta];
                            vapaatKentat++;
                            
                            vertailukohta--;
                        }
                        /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                        else if(kentta[x][vertailukohta-1] != 0) {
                            vertailukohta--;
                        }
                        y--;
                        
                    }
                    /* jos siirrettava ruutu ei ole tyhja, siirretaan laattaa ja vertaillaan */
                    else if(kentta[x][y2] != 0) {
                        kentta[x][y] = kentta[x][y2];
                        kentta[x][y2] = 0;
                        laattojaSiirrettiin = 1;
                        
                        if(kentta[x][vertailukohta] == kentta[x][vertailukohta-1]) {
                            
                            kentta[x][vertailukohta] = kentta[x][vertailukohta] + kentta[x][vertailukohta];
                            kentta[x][vertailukohta-1] = 0;
                            
                            
                            pisteet += kentta[x][vertailukohta];
                            vapaatKentat++;
                            
                            vertailukohta--;
                            y = y2 = vertailukohta;
                        }
                        else {
                            /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                            if(kentta[x][vertailukohta-1] != 0) {
                                vertailukohta--;
                            }
                            y--;
                        }
                    }
                    y2--;
                }
                x--;
            }
            
            if(laattojaSiirrettiin) {
                /* asetetaan parametreina saatu uusi ruutu */
                kentta[luku2][luku1] = luku3;
                vapaatKentat--;
            }
            
        }
        else if(sscanf(komento, "oikealle %d %d %d", &luku1, &luku2, &luku3) == 3) {
            /* kaydaan ruudukkoa lapi riveittain alhaalta ylös */
            y = RIVIT - 1;
            while(y >= 0) {
                x = SARAKKEET - 1;
                x2 = x - 1;
                vertailukohta = x;
                
                /* siirretaan ruudut vasempaan reunaan ja ynnaillaan, y2 on seuraava siirrettava ruutu ja y on mihin siirretaan. */
                while(x2 >= 0) {
                    /* jos ruutu johon ollaan siirtamassa ei ole tyhja siirrytaan seuraavaan ruutuun */
                    if(kentta[x][y] != 0) {
                        /* verrataan ruutuja */
                        if(kentta[vertailukohta][y] == kentta[vertailukohta-1][y]) {
                            kentta[vertailukohta][y] = kentta[vertailukohta][y] + kentta[vertailukohta][y];
                            kentta[vertailukohta-1][y] = 0;
                            
                            laattojaSiirrettiin = 1;
                            
                            pisteet += kentta[vertailukohta][y];
                            vapaatKentat++;
                            
                            vertailukohta--;
                        }
                        /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                        else if(kentta[vertailukohta-1][y] != 0) {
                            vertailukohta--;
                        }
                        x--;
                        
                    }
                    /* jos siirrettava ruutu ei ole tyhja, siirretaan laattaa ja vertaillaan */
                    else if(kentta[x2][y] != 0) {
                        kentta[x][y] = kentta[x2][y];
                        kentta[x2][y] = 0;
                        laattojaSiirrettiin = 1;
                        
                        if(kentta[vertailukohta][y] == kentta[vertailukohta-1][y]) {
                            
                            kentta[vertailukohta][y] = kentta[vertailukohta][y] + kentta[vertailukohta][y];
                            kentta[vertailukohta-1][y] = 0;
                            
                            pisteet += kentta[vertailukohta][y];
                            vapaatKentat++;
                            
                            
                            vertailukohta--;
                            x = x2 = vertailukohta;
                        }
                        else {
                            /* vertailukohtaa siirretaan yhdella myos jos vertailukohdasta seuraava on epatyhja */
                            if(kentta[vertailukohta-1][y] != 0) {
                                vertailukohta--;
                            }
                            x--;
                        }
                    }
                    x2--;
                }
                y--;
            }
            
            if(laattojaSiirrettiin) {
                /* asetetaan parametreina saatu uusi ruutu */
                kentta[luku2][luku1] = luku3;
                vapaatKentat--;
            }
        }
        else if(strcmp(komento, "pisteet") == 0) {
            printf("%d\n", pisteet);
        }
        else if(sscanf(komento, "tallenna %s", pelinNimi) == 1) {
            /* pelille annettavan nimen todellinen pituus on rivin pituus miinus 'tallenna ' eli 9 merkkia. */
            pelinNimi = realloc(pelinNimi, (pituus - 8) * sizeof(char));
            
            td = fopen(tiedosto, "a+");
            
            /* kaytetan x:aa lippumuuttujana */
            x = 1;
            
            /* tarkistetaan onko samannimista pelia olemassa. */
            do {
                rivi = lueRivi(td, &pituus);
                if(strcmp(rivi, "***") == 0) {
                    rivi = lueRivi(td, &pituus);
                    if(strcmp(rivi, pelinNimi) == 0) {
                        printf("Peli %s on jo olemassa\n", pelinNimi);
                        x = 0;
                    }
                }
            }
            while(pituus > 0);
            
            /* kirjoitetaan nykyinen pelitilanne tiedostoon, jos tiedostossa ei viela ollut samannimista pelia. */
            /*
            aloitusmerkit ***
            tallennetun pelin nimi
            nykyinen pelitilanne
                kentta
                pisteet
                vapaat kentat
            */
            if(x) {
                /* aloitusmerkki */
                fprintf(td, "***\n");
                /* tallennettavan pelin nimi */
                fprintf(td, "%s\n", pelinNimi);
                /* nykyinen pelitilanne */
                y = 0;
                while(y < RIVIT) {
                    x = 0;
                    while(x < SARAKKEET) {
                        fprintf(td, "%i\n", kentta[x][y]);
                        x++;
                    }
                    y++;
                }
                /* pisteet */
                fprintf(td, "%i\n", pisteet);
                /* vapaat kentat */
                fprintf(td, "%i\n", vapaatKentat);
                
                
            }
            fclose(td);
        }
        
        else if(sscanf(komento, "lataa %s", pelinNimi) == 1) {
            /* ladattavan pelin nimen todellinen pituus on rivin pituus miinus 'lataa ' eli 6 merkkia. */
            pelinNimi = realloc(pelinNimi, (pituus - 5) * sizeof(char));
            td = fopen(tiedosto, "r");
            
            /* kaytetan x:aa lippumuuttujana */
            x = 0;
            
            /* etsitaan peli tiedostosta. Etsitaan kunnes rivi loytyy (x <- 1) tai tiedosto loppuu. */
            do {
                rivi = lueRivi(td, &pituus);
                if(strcmp(rivi, "***") == 0) {
                    rivi = lueRivi(td, &pituus);
                    if(strcmp(rivi, pelinNimi) == 0) {
                        x = 1;
                    }
                }
            }
            while(pituus > 0 && !x);
            
            /* luetaan pelitilanne tiedostosta, jos peli loytyi. */
            /*
            aloitusmerkit ***
            tallennetun pelin nimi
            nykyinen pelitilanne
                kentta
                pisteet
                vapaat kentat
            */
            if(x) {
                
                /* luetaan pelitilanne */
                y = 0;
                x2 = 0;
                while(y < RIVIT) {
                    
                    x = 0;
                    while(x < SARAKKEET) {
                        /* luetaan rivin sisalto ja sijoitetaan kokonaisluvuksi muunnettuna kenttaan. */
                        
                        sscanf(lueRivi(td, &pituus), "%s", luku);
                        
                        kentta[x][y] = atoi(luku);
                        x++;
                        
                    }
                    y++;
                }
                /* pisteet */
                sscanf(lueRivi(td, &pituus), "%s", luku);
                pisteet = atoi(luku);
                /* vapaat kentat */
                sscanf(lueRivi(td, &pituus), "%s", luku);
                vapaatKentat = atoi(luku);
                
                
                /* nollataan undo-toiminnon historia. */
                /* kopioidaan aloitustilanne tilanteet-taulukon alkuun. */
                y = 0;
                while(y < RIVIT) {
                    x = 0;
                    while(x < SARAKKEET) {
                        tilanne.laatat[x][y] = kentta[x][y];
                        x++;
                    }
                    y++;
                }
                tilanne.pisteet = pisteet;
                tilanne.vapaatkentat = vapaatKentat;
                
                tilanteet[0] = tilanne;
                nykyinenPelitilanne = 0;
                tilanteetKoko = 1;
                siirtojaOnJaljella = 1;
                
            }
            else {
                printf("Peliä %s ei ole olemassa\n", pelinNimi);
            }
            
            
            fclose(td);
        }
        else if(strcmp(komento, "listaa") == 0) {
            td = fopen(tiedosto, "r");
            
            /* luetaan tiedostoa rivi kerrallaan. Pelin nimi on aloitusmerkintaa *** seuraavalla rivilla. */
            do {
                rivi = lueRivi(td, &pituus);
                if(strcmp(rivi, "***") == 0) {
                    rivi = lueRivi(td, &pituus);
                    printf("%s\n", rivi);
                }
            }
            while(pituus > 0);
            
            fclose(td);
        }
        else if(sscanf(komento, "poista %s", pelinNimi) == 1) {
            /* poistettavan pelin nimen todellinen pituus on rivin pituus miinus 'poista ' eli 7 merkkia. */
            pelinNimi = realloc(pelinNimi, (pituus - 6) * sizeof(char));
            td = fopen(tiedosto, "r");
            
            /* Etsitaan poistettavaa pelia tiedostosta ja lasketaan tiedoston rivien lukumaara muuttujaan y2. */
            y2 = -1;
            x = 0;
            do {
                rivi = lueRivi(td, &pituus);
                y2++;
                if(strcmp(rivi, "***") == 0) {
                    rivi = lueRivi(td, &pituus);
                    y2++;
                    if(strcmp(rivi, pelinNimi) == 0) {
                        x = 1;
                    }
                }
            }
            while(pituus > 0);
            
            /* poistetaan pelin tiedot tiedostosta. */
            if(x) {
                /* jos poistettava peli on ainoa, tyhjennetaan tiedosto. */
                if(y2 <= (4 + (RIVIT * SARAKKEET))) {
                    
                    fclose(td);
                    td = fopen(tiedosto, "w");
                    fclose(td);
                }
                else {
                    
                    /* palataan tiedoston alkuun ja kopioidaan sisalto ilman poistettavaa kohtaa aputaulukkoon ja aputaulukosta takaisin tiedostoon. */ 
                    rewind(td);
                    
                    
                    /* varataan tilaa yhden pelin tallentamiseen. y2 tiedoston rivien maara. 
                     * Taulukon ylimman tason pituus on rivien maara vahennettyna yhden pelin rivien maaralla. 
                     */
                    y2 -= (4 + (SARAKKEET * RIVIT));
                    
                    tiedostonSisalto = malloc(y2 * sizeof(char*));
                    
                    
                    y = 0;
                    while(y < y2) {
                        
                        rivi = lueRivi(td, &pituus);
                        
                        /* kun tullaan poistettavan pelin kohdalle, hypataan sen sisaltavat rivit yli ja ylikirjoitetaan tiedostonSisalto[y] 
                         * silla se sisaltaa jo kirjoitetun aloitusmerkin ***. 
                         */
                        if(strcmp(rivi, pelinNimi) == 0) {
                            x = 0;
                            while(x < (3 + (RIVIT * SARAKKEET))) {
                                rivi = lueRivi(td, &pituus);
                                x++;
                            }
                            
                        }
                        else {
                            tiedostonSisalto[y] = malloc(((pituus + 1) * sizeof(char)));
                            strcpy(tiedostonSisalto[y], rivi);
                            y++;
                        }
                    }
                    
                   
                    
                    fclose(td);
                    td = fopen(tiedosto, "w");
                    
                    /* kopioidaan taulukon sisalto takaisin tiedostoon. */
                    y = 0;
                    while(y < y2) {
                        
                        fprintf(td, "%s\n", tiedostonSisalto[y]);
                        y++;
                    }
                    
                    
                    /* vapautetaan taulukko */
                    y = 0;
                    while(y < y2) {
                        free(tiedostonSisalto[y]);
                        y++;
                    }
                    free(tiedostonSisalto);
                }
            }
            else {
                printf("Peliä %s ei ole olemassa\n", pelinNimi);
            }
            
            fclose(td);
        }
        else if(strcmp(komento, "lopeta") == 0) {
            goto LOPPU;
        }
        else {
            printf("Tuntematon komento!\n");
        }
        
        /* jos laattoja siirrettiin, tallennetaan nykyinen kentta tilanteet-taulukon loppuun. */
        if(laattojaSiirrettiin) {
            y = 0;
            while(y < RIVIT) {
                x = 0;
                while(x < SARAKKEET) {
                    tilanne.laatat[x][y] = kentta[x][y];
                    x++;
                }
                y++;
            }
            tilanne.pisteet = pisteet;
            tilanne.vapaatkentat = vapaatKentat;

            
            
            nykyinenPelitilanne++;
            
            /* jos uusin siirto tehtiin peruutuskomennon jalkeen, tallennettujen tilanteiden maara vahenee */
            if(tilanteetKoko >= (nykyinenPelitilanne + 1))
                tilanteetKoko = nykyinenPelitilanne + 1;
            else
                tilanteetKoko++;
                
            
            /* varataan tilaa yhta isommalle maaralle pelitilanteita */
            tilanteet = realloc(tilanteet, tilanteetKoko*sizeof(Pelitilanne));
            tilanteet[nykyinenPelitilanne] = tilanne;
        }
            
        
        /* tutkitaan komennon jalkeen tuliko kentta tayteen. Jos kentta tayttyi, 
         * tutkitaan onko loppuiko peli ts onko mahdollisia siirtoja. 
         */
        if(vapaatKentat == 0) {
            /* onko kentassa kahta vierekkaista saman numeron laattaa */
            y = x = 0;
            siirtojaOnJaljella = 0;
            
            while(!siirtojaOnJaljella && y < RIVIT) {
                x = 0;
                while(!siirtojaOnJaljella && x < SARAKKEET - 1) {
                    if(kentta[x][y] == kentta[x+1][y])
                        siirtojaOnJaljella = 1;
                    x++;
                }
                y++;
            }
            
            
            x = y = 0;
            /* onko kentassa kahta paallekkaista saman numeron laattaa */
            while(!siirtojaOnJaljella && x < SARAKKEET) {
                y = 0;
                while(!siirtojaOnJaljella && y < RIVIT - 1) {
                    if(kentta[x][y] == kentta[x][y+1])
                        siirtojaOnJaljella = 1;
                    y++;
                }
                x++;
            }
            
            
        }
        
        
        
    }
    
    LOPPU:
    free(komento);
    free(tilanteet);
    free(pelinNimi);
    
    return 0;
}

