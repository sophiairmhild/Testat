#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   //für das Arbeitsverzeichnis
#include <string.h>   //Speicher Eingabe
#include <sys/wait.h> //Warteaufgabe
#include <stdbool.h>

#define BEFEHLSZEILE_LAENGE 512
#define VERZEICHNIS_LAENGE 512

/* Shell von Sophia Weber - beinhaltet die gewünschten Funktionalitäten
   wie z. B. die Anzeige der Pfade, die Nutzung klassicher Befehle, Pipe, Semikolon usw. */
void verarbeitePipe(char *positionPipe, char *befehlTeil)
{
    char *befehlLinks = befehlTeil;
    char *befehlRechts = positionPipe + 1;

    while (*befehlRechts == ' ')
        befehlRechts++;

    char *argvLinks[15];
    int i = 0;
    char *wort = strtok(befehlLinks, " ");
    while (wort && i < 14)
    {
        argvLinks[i++] = wort;
        wort = strtok(NULL, " ");
    }
    argvLinks[i] = NULL;

    char *argvRechts[15];
    i = 0;
    wort = strtok(befehlRechts, " ");
    while (wort && i < 14)
    {
        argvRechts[i++] = wort;
        wort = strtok(NULL, " ");
    }
    argvRechts[i] = NULL;

    int datenStrom[2];
    if (pipe(datenStrom) == -1)
    {
        perror("Fehler bei pipe()");
        return;

        // vielleicht eigene funktion mit datenstrom und argv als übergabeparameter
        pid_t pidLinks = fork();
        if (pidLinks == 0)
        {
            dup2(datenStrom[1], STDOUT_FILENO);
            close(datenStrom[0]);
            close(datenStrom[1]);
            execvp(argvLinks[0], argvLinks);
            perror("Fehler linker Teil");
            exit(1);
        }

        pid_t pidRechts = fork();
        if (pidRechts == 0)
        {
            dup2(datenStrom[0], STDIN_FILENO);
            close(datenStrom[1]);
            close(datenStrom[0]);
            execvp(argvRechts[0], argvRechts);
            perror("Fehler rechter Teil");
            exit(1);
        }

        // unnötig da nicht offen
        close(datenStrom[0]);
        close(datenStrom[1]);

        waitpid(pidLinks, NULL, 0);
        waitpid(pidRechts, NULL, 0);
    }
}

void verarbeiteSemikolon()
{
}

void verarbeiteEinzelBefehl()
{
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

bool holeInput(char *befehlZeile, size_t laenge)
{
    if (fgets(befehlZeile, laenge, stdin) == NULL)
    {
        printf("Fehler bei der Eingabe.\n");
        return false;
    }

    printf("Die Eingabe war: %s", befehlZeile);

    size_t len = strlen(befehlZeile);
    if (len > 0 && befehlZeile[len - 1] == '\n')
        befehlZeile[len - 1] = '\0';

    if (strcmp(befehlZeile, "exit") == 0)
        exit(0);

    if (befehlZeile[0] == '\0')
    {
        return false;
    }

    return true;
}

void verarbeiteInput(char *befehlZeile)
{
    // toDo einzelne funktionen wie trennung ; pipe usw in einzelne Funktionen trennen
    // die können dann wiederverwendet werden ;-)
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

        char *positionPipe = strchr(befehlTeil, '|');
        if (positionPipe)
        {
            *positionPipe = '\0';
            verarbeitePipe(positionPipe, befehlTeil);
        }
        else
        {
            char *befehlAufgeteilt[15];
            int i = 0;
            char *wort = strtok(befehlTeil, " ");
            while (wort && i < 14)
            {
                befehlAufgeteilt[i++] = wort;
                wort = strtok(NULL, " ");
            }
            befehlAufgeteilt[i] = NULL;

            pid_t pid = fork();
            if (pid == -1)
            {
                printf("Fehler beim Erzeugen des Kindprozesses.\n");
                return;
            }

            if (pid == 0)
            {
                execvp(befehlAufgeteilt[0], befehlAufgeteilt);
                perror("Fehler beim Ausführen des Befehls");
                exit(1);
            }
            else
            {
                waitpid(pid, NULL, 0);
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
        if (holeInput(befehlszeile, sizeof(befehlszeile)))
        {
            verarbeiteInput(befehlszeile);
        }
    }
}
