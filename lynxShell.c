#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   //für das Arbeitsverzeichnis
#include <string.h>   //Speicher Eingabe
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

void holeInput(char *befehlZeile, size_t laenge)
{
    if (fgets(befehlZeile, laenge, stdin) != NULL)
    {
        printf("Die Eingabe war: %s", befehlZeile);
    }
    else
    {
        printf("Fehler bei der Eingabe.\n");
        return;
    }
    befehlZeile[strlen(befehlZeile) - 1] = '\0'; // \n am Ende weg

    if (strcmp(befehlZeile, "exit") == 0)
    {
        exit(0);
    }

    if (befehlZeile[0] == '\0')
    {
        return;
    }

    // Semikolon 
    char *befehlTeil = strtok(befehlZeile, ";");

    while (befehlTeil != NULL)
    {
        while (*befehlTeil == ' ')
            befehlTeil++;

        if (*befehlTeil == '\0')
        {
            befehlTeil = strtok(NULL, ";");
            continue;
        }

        // Pipe
        char *positionPipe = strchr(befehlTeil, '|');
        int pipeVorhanden = (positionPipe != NULL);

        if (pipeVorhanden)
        {
            *positionPipe = '\0';
            char *befehlLinks = befehlTeil;
            char *befehlRechts = positionPipe + 1;

            while (*befehlRechts == ' ')
                befehlRechts++;

            char *befehlTeilLinks[15];
            int i = 0;
            char *wort = strtok(befehlLinks, " ");
            while (wort != NULL && i < 14)
            {
                befehlTeilLinks[i++] = wort;
                wort = strtok(NULL, " ");
            }
            befehlTeilLinks[i] = NULL;

            char *befehlTeilRechts[15];
            i = 0;
            wort = strtok(befehlRechts, " ");
            while (wort != NULL && i < 14)
            {
                befehlTeilRechts[i++] = wort;
                wort = strtok(NULL, " ");
            }
            befehlTeilRechts[i] = NULL;

            int dateiDeskriptoren[2];
            if (pipe(dateiDeskriptoren) == -1)
            {
                perror("Fehler bei pipe");
                return;
            }

            pid_t pidLinks = fork();
            if (pidLinks == 0)
            {
                dup2(dateiDeskriptoren[1], STDOUT_FILENO);
                close(dateiDeskriptoren[0]);
                close(dateiDeskriptoren[1]);
                execvp(befehlTeilLinks[0], befehlTeilLinks);
                perror("Fehler linker Teil");
                exit(1);
            }

            pid_t pidRechts = fork();
            if (pidRechts == 0)
            {
                dup2(dateiDeskriptoren[0], STDIN_FILENO);
                close(dateiDeskriptoren[1]);
                close(dateiDeskriptoren[0]);
                execvp(befehlTeilRechts[0], befehlTeilRechts);
                perror("Fehler rechter Teil");
                exit(1);
            }

            close(dateiDeskriptoren[0]);
            close(dateiDeskriptoren[1]);
            wait(NULL);
            wait(NULL);
        }
        else
        {
            char *befehlAufgeteilt[15];
            int i = 0;
            char *wort = strtok(befehlTeil, " ");
            while (wort != NULL && i < 14)
            {
                befehlAufgeteilt[i++] = wort;
                wort = strtok(NULL, " ");
            }
            befehlAufgeteilt[i] = NULL;

            pid_t pid = fork();
            if (pid == -1)
            {
                printf("Fehler Child.\n");
                return;
            }

            if (pid == 0)
            {
                printf("Starte Befehl: %s\n", befehlAufgeteilt[0]);
                execvp(befehlAufgeteilt[0], befehlAufgeteilt);
                perror("Fehler beim Ausführen des Befehls");
                exit(1);
            }
            else
            {
                wait(NULL);
            }
        }

        befehlTeil = strtok(NULL, ";");
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
