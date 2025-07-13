#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   //für das Arbeitsverzeichnis
#include <string.h>   //Speicher Eingabe
#include <sys/wait.h> //Warteaufgabe
#include <signal.h>

#define BEFEHLSZEILE_LAENGE 512
#define VERZEICHNIS_LAENGE 512

char letzterBefehl[BEFEHLSZEILE_LAENGE] = {0};

/* Shell von Sophia Weber - beinhaltet die gewünschten Funktionalitäten
   wie z. B. die Anzeige der Pfade, die Nutzung klassicher Befehle, Pipe, Semikolon usw. */

// Signal-Handler für SIGHUP
void handle_sighup(int signum) {
    printf("%s\n", letzterBefehl);
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
void bearbeiteInput(char* input, char** inputArgs) {
    int i = 0;
    while (*input == ' ') {
        input++;
    }         
    char *wort = strtok(input, " ");
    while (wort && i < 14)
    {
        inputArgs[i++] = wort;
        wort = strtok(NULL, " ");
    }
    inputArgs[i] = 0;

}

void holeInput(char *befehlZeile, size_t laenge)
{
    if (fgets(befehlZeile, laenge, stdin) == NULL)
    {
        printf("Fehler bei der Eingabe.\n");
        return;
    }

    printf("Die Eingabe war: %s", befehlZeile);

    size_t len = strlen(befehlZeile);
    if (len > 0 && befehlZeile[len - 1] == '\n')
        befehlZeile[len - 1] = '\0';

    if (strcmp(befehlZeile, "exit") == 0)
        exit(0);

    if (strcmp(befehlZeile, "ret") == 0) {
        printf("Letzter Befehl: %s", letzterBefehl);
        return;
    }
        

    if (befehlZeile[0] == '\0')
        return;

    // toDo einzelne funktionen wie trennung ; pipe usw in einzelne Funktionen trennen
    // die können dann wiederverwendet werden ;-)
    memcpy(letzterBefehl, befehlZeile, BEFEHLSZEILE_LAENGE);
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
            char *argvLinks[15] = {0};
            bearbeiteInput(befehlTeil, argvLinks);

            char *argvRechts[15] = {0};
            bearbeiteInput(positionPipe, argvRechts);

            int datenStrom[2];
            if (pipe(datenStrom) == -1)
            {
                perror("Fehler bei pipe()");
                return;
            }

            // vielleicht eigene funktion mit datenstrom und argv als übergabeparameter
            pid_t pidLinks = fork();
            if (pidLinks == 0)
            {
                printf("Ich bin das Kind");
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
                printf("Ich bin das Kind");
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
        else
        {
            char * befehlAufgeteilt[15] = {0};
            bearbeiteInput(befehlTeil, befehlAufgeteilt);
            pid_t pid = fork();
            if (pid == -1)
            {
                printf("Fehler beim Erzeugen des Kindprozesses.\n");
                return;
            }

            if (pid == 0)
            {
                printf("Ich bin das Kind");
                execvp(befehlTeil, befehlAufgeteilt);
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

        // Signal-Handler registrieren
        if (signal(SIGHUP, handle_sighup) == SIG_ERR) {
            perror("signal");
            exit(1);
        }

        zeigePfad(verzeichnis);
        holeInput(befehlszeile, sizeof(befehlszeile));
    }
}
