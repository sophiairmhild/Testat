#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   //für das Arbeitsverzeichnis
#include <string.h>   //Speicher Eingabe
#include <sys/wait.h> //Warteaufgabe
#include <signal.h>
#include <stdbool.h>

#define BEFEHLSZEILE_LAENGE 512
#define VERZEICHNIS_LAENGE 512
#define DEBUG 0

char letzterRückgabewert = 0;

/* Shell von Sophia Weber - beinhaltet die gewünschten Funktionalitäten
   wie z. B. die Anzeige der Pfade, die Nutzung klassicher Befehle, Pipe, Semikolon usw. */

// Teile die Eingabe in Befehl und Argumente auf
// Beispiel ls --all
// befehl1 = ls
// argv1 = [ls, -all]
void bearbeiteEingabe(char* eingabe, char** eingabeArgumente) {
    int i = 0;
    char* eingabeStart = eingabe;

    // lösche leer Zeichen am Start des Strings
    while (*eingabe == ' ') {
        eingabe++;
    }
    // kopiere Eingabe um die übergebenen Daten nicht zu verändern
    memcpy(eingabeStart, eingabe, strlen(eingabe)+1);
    // Teile Eingabe in mehrere Argumente       
    char *wort = strtok(eingabe, " ");
    while (wort && i < 14)
    {
        eingabeArgumente[i++] = wort;
#if DEBUG == 1
        printf("|%s|", wort);
#endif
        wort = strtok(NULL, " ");
    }
    eingabeArgumente[i] = NULL;
    
}

// Verarbeite eine Anforderung das Verzeichnis zu wechseln
// cd test
// cd ~/test
// cd /home/bla/blub
bool verarbeiteCD(char *befehl)
{
    char absoluterPfad[100] = {0};
    char *pfad = NULL;
    pfad = strtok(befehl, " ");
    if (pfad == NULL)
    {
        return false;
    }

    // Bei Null wird der Pointer vom vorherigen strtok genommen an der stelle wo das " " ersetzt wurde
    pfad = strtok(NULL, " "); 
    if (pfad == NULL)
    {
        return false;
    }
    // bearbeite home Pfad indem die ~ durch das home verzeichnis ersetzt wird
    if (pfad[0] == '~')
    {
        char *home = getenv("HOME");
        strcat(absoluterPfad, home);
        strcat(absoluterPfad, &pfad[1]);
    }
    // behandle relative pfade
    else if (pfad[0] != '/')
    {
        char curdir[100] = {0};
        getcwd(curdir, sizeof curdir);
        strcat(absoluterPfad, curdir);
        strcat(absoluterPfad, "/");
        strcat(absoluterPfad, pfad);
    }
    // behandle absolute pfade
    else
    {
        strcpy(absoluterPfad, pfad);
    }
    #if DEBUG == 1
    printf("Ändere Ordner: %s\n", absoluterPfad);
    #endif
    // Ändere Konsolenpfad
    chdir(absoluterPfad);
}

// Verarbeite einen Befehl mit einer Pipe im String
void verarbeitePipe(char *befehl)
{
    char *argv1[15] = {0};
    char *argv2[15] = {0};
    char *befehl1 = strtok(befehl,"|");
    char *befehl2 = strtok(NULL,"|");

    // Teile die Eingabe in Befehl und Argumente auf
    bearbeiteEingabe(befehl1, argv1);
    bearbeiteEingabe(befehl2, argv2);

    // öffne pipe
    int pipefd[2];
    pipe(pipefd); 

    // erstelle Kind 1 Prozess
    pid_t pid1 = fork(); 
    if (pid1 == 0) {
        // Kind 1
        close(pipefd[0]);                 // liest nicht
        dup2(pipefd[1], STDOUT_FILENO);  // stdout → pipe
        close(pipefd[1]);
        execvp(befehl1, argv1);
        perror("Fehler im Argument 1");
        exit(EXIT_FAILURE);                        // Ende Kind 1 Prozess
    }

    // erstelle Kind 2 Prozess
    pid_t pid2 = fork(); 
    if (pid2 == 0) {
        // Kind 2
        close(pipefd[1]);                // schreibt nicht
        dup2(pipefd[0], STDIN_FILENO);  // stdin ← pipe
        close(pipefd[0]);
        execvp(befehl2, argv2);
        perror("Fehler im Argument 2");
        exit(EXIT_FAILURE);                        // Ende Kind 2 Prozess
    }
    // beide Pipes im Elternprozess schließen
    close(pipefd[0]);
    close(pipefd[1]);

    // Warten auf beide Kinder im Elternprozess
    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    fflush(stdout);

    #if DEBUG == 1
    printf("%s Exit-Code: %d\n", befehl1, WEXITSTATUS(status1));
    printf("%s Exit-Code: %d\n", befehl2,  WEXITSTATUS(status2));
    #endif
    letzterRückgabewert = WEXITSTATUS(status2);
}

void verarbeiteEinzelBefehl(char *befehl, char **arg)
{   
    // erstelle Kind Prozess
    pid_t pid = fork();
    if (pid == -1)
    {
        printf("Fehler beim Erzeugen des Kindprozesses.\n");
        return;
    }

    // Bereich des Kind Prozesses
    if (pid == 0)
    {
        execvp(befehl, arg);
        perror("Fehler beim Ausführen des Befehls");
        exit(EXIT_FAILURE);
    }
    // Bereich des Eltern Prozesses
    else
    {
        int status;
        waitpid(pid, &status, 0);
        letzterRückgabewert = WEXITSTATUS(status);
    }
}

// Signal-Handler für SIGHUP
void verarbeiteSighup(int signum) {
    printf("%i\n",letzterRückgabewert);
}

// Zeige momentanen Pfad der Konsole an
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

// hole eine neue Eingabe vom Nutzer
bool holeEingabe(char *befehlZeile, size_t laenge)
{
    if (fgets(befehlZeile, laenge, stdin) == NULL)
    {
        // Wenn etwas falsches oder EOF eingegeben wird
        perror("Fehler bei der Eingabe.\n");
        exit(EXIT_SUCCESS);
    }

    #if DEBUG == 1
    printf("Die Eingabe war: %s", befehlZeile);
    #endif

    size_t len = strlen(befehlZeile);
    // Ersetze \n durch ein string ende
    if (len > 0 && befehlZeile[len - 1] == '\n')
        befehlZeile[len - 1] = '\0';

    // schließe das Programm
    if (strcmp(befehlZeile, "exit") == 0)
        exit(EXIT_SUCCESS);

    // gebe letzten Rückgabewert des Kind Prozesses zurück
    if (strcmp(befehlZeile, "ret") == 0) {
        printf("%i\n",letzterRückgabewert);
        return false;
    }
        
    // kein String eingegeben
    if (befehlZeile[0] == '\0')
    {
        return false;
    }

    return true;
}

// Verarbeite die Eingabe vom Nutzer
void verarbeiteEingabe(char *befehlZeile)
{
    char *befehlTeil = strtok(befehlZeile, ";");
    char befehl[BEFEHLSZEILE_LAENGE] = {0};

    // Solange ein Abschnitt existiert, wird die while Schleife ausgeführt
    while (befehlTeil != NULL)
    {
        befehlZeile+=strlen(befehlTeil)+1;
        while (*befehlTeil == ' ')
            befehlTeil++;
        memcpy(befehl, befehlTeil, strlen(befehlTeil)+1);

        // Verarbeite einen Befehl mit einer Pipe im String
        if (strchr(befehl, '|'))
        {
            verarbeitePipe(befehl);
        }
        // Verarbeite eine Anforderung das Verzeichnis zu wechseln
        else if (strncmp(befehl, "cd", 2) == 0)
        {
            verarbeiteCD(befehl);
        }
        // Verarbeite alle sonstigen Befehle, welche in PATH bekannt sind oder manuell aufgerufen werden
        else
        {
            char *befehlAufgeteilt[15];
            // Teile die Eingabe in Befehl und Argumente auf
            bearbeiteEingabe(befehl, befehlAufgeteilt);
            verarbeiteEinzelBefehl(befehl, befehlAufgeteilt);
        }

        // Trenne Eingabe um eventuell den nächsten Abschitt zu bearbeiten
        befehlTeil = strtok(befehlZeile, ";");
    }
}

int main()
{
    char verzeichnis[VERZEICHNIS_LAENGE];
    char befehlszeile[BEFEHLSZEILE_LAENGE];
    char data[BEFEHLSZEILE_LAENGE] = "date +%s |wc -c";

    // Signal-Handler registrieren
    if (signal(SIGHUP, verarbeiteSighup) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        char verzeichnis[VERZEICHNIS_LAENGE] = {0};
        char befehlszeile[BEFEHLSZEILE_LAENGE] = {0};

        // Zeige momentanen Pfad der Konsole an
        zeigePfad(verzeichnis);
        // hole eine neue Eingabe vom Nutzer
        if (holeEingabe(befehlszeile, sizeof(befehlszeile)))
        {
            // Verarbeite die Eingabe vom Nutzer
            verarbeiteEingabe(befehlszeile);
        }
    }
}
