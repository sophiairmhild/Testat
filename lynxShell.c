#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //für das Arbeitsverzeichnis
#include <string.h> //Speicher Eingabe
#include <sys/wait.h> //Warteaufgabe

#define BEFEHLSZEILE_LAENGE 512
#define VERZEICHNIS_LAENGE 512

/* Shell von Sophia Weber - beinhaltet die gewünschten Funktionalitäten
   wie z. B. die Anzeige der Pfade, die PATH-Variablen-Verarbeitung usw. */

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

void holeInput(char *befehlszeile, size_t laenge)
{
    // Quelle Userinput: https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm
    if (fgets(befehlszeile, laenge, stdin) != NULL)
    {
        printf("Die Eingabe war: %s", befehlszeile);
    }
    else
    {
        printf("Fehler bei der Eingabe.");
        return;
    }
    befehlszeile[strlen(befehlszeile) - 1] = '\0'; // \n am ende weg

    if (strcmp(befehlszeile, "exit") == 0) //beenden ermöglichen
    {
        exit(0);
    }

    if (befehlszeile[0] == '\0')
    { // checken ob leer
        return;
    }
    //checke ob | vorhanden
    char *verbinderCount = strchr(befehlszeile, '|');
    int verbinderVorhanden = (verbinderCount != NULL); 
    
    if(verbinderVorhanden==1){
        *verbinderCount='\0';
        char *vorne = befehlszeile;
        char *hinten = verbinderCount+1;
        while (*hinten == ' ') {hinten++;}
    }

    //checke ob ; vorhanden
    char *befehlsteil = strtok(befehlszeile, ";");

    while (befehlsteil != NULL)
    {
        while (*befehlsteil == ' ')
        {
            befehlsteil++;
        }
        if (*befehlsteil == '\0')
        {
            befehlsteil = strtok(NULL, ";");
            continue;
        }

        char *aufgeteilterBefehl[15];
        int i = 0;
        char *einzelBefehl = strtok(befehlsteil, " ");

        while (einzelBefehl != NULL && i < 14)
        {
            aufgeteilterBefehl[i] = einzelBefehl;
            i++;
            einzelBefehl = strtok(NULL, " ");
        }
        aufgeteilterBefehl[i] = NULL;

        pid_t pid = fork();

        if (pid == -1)
        {
            printf("Fehler Child erstellen.\n");
            return;
        }

        if (pid == 0)
        {
            execvp(aufgeteilterBefehl[0], aufgeteilterBefehl);
            printf("Fehler Befehl ausfuehren.\n");
            exit(1);
        }
        else
        {
            // Shell wartet
            wait(NULL);
        }
        befehlsteil = strtok(NULL, ";");
    }
}

int main()
{

    while (1)
    {
        char verzeichnis[VERZEICHNIS_LAENGE];
        char befehlszeile[BEFEHLSZEILE_LAENGE];

        zeigePfad(verzeichnis);
        holeInput(befehlszeile, sizeof(befehlszeile));
    }
}