#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //für das Arbeitsverzeichnis
#include <string.h> //Speicher Eingabe

#define BEFEHLSZEILE_LAENGE 64
#define VERZEICHNIS_LAENGE 512

/* Shell von Sophia Weber - beinhaltet die gewünschten Funktionalitäten
   wie z. B. die Anzeige der Pfade, die PATH-Variablen-Verarbeitung usw. */

   void liesInput(char *befehlszeile)
   {
       return;
       
   }


void zeigePfad(char *verzeichnis)
{
    // Quelle für Arbeitsverzeichnis anzeigen: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
    if (getcwd(verzeichnis, VERZEICHNIS_LAENGE) == NULL)
    {
        perror("Fehler beim Anzeigen des Arbeitsverzeichnisses.");
        return;
    }

    printf("%s> ", verzeichnis);
    fflush(stdout);
}

void holeInput(char *befehlszeile, size_t laenge){
    // Quelle Userinput: https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm
    if (fgets(befehlszeile, laenge, stdin) != NULL)
    {
        printf("Die Eingabe war: %s", befehlszeile);
        liesInput(befehlszeile);
    }
    else
    {
        printf("Fehler bei der Eingabe.");
        return;
    }
    befehlszeile[strlen(befehlszeile)-1] = '\0'; // \n am ende weg

}




int main()
{

    while (1)
    {
        char verzeichnis[VERZEICHNIS_LAENGE];
        char befehlszeile[BEFEHLSZEILE_LAENGE];


        zeigePfad(verzeichnis);
        holeInput(befehlszeile, sizeof(befehlszeile));


        if (strcmp(befehlszeile, "exit\n") == 0)
        {
            break;
        }
    }
}