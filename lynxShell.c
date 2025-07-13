#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   //für das Arbeitsverzeichnis
#include <string.h>   //Speicher Eingabe
#include <sys/wait.h> //Warteaufgabe
#include <signal.h>
#include <stdbool.h>

#define BEFEHLSZEILE_LAENGE 512
#define VERZEICHNIS_LAENGE 512

char letzterRückgabewert = 0;

/* Shell von Sophia Weber - beinhaltet die gewünschten Funktionalitäten
   wie z. B. die Anzeige der Pfade, die Nutzung klassicher Befehle, Pipe, Semikolon usw. */

void bearbeiteInput(char* input, char** inputArgs) {
    int i = 0;
    char* inputStart = input;
    while (*input == ' ') {
        input++;
    }
    memcpy(inputStart, input, strlen(input)+1);        
    char *wort = strtok(input, " ");
    while (wort && i < 14)
    {
        inputArgs[i++] = wort;
        // printf("|%s|", wort);
        wort = strtok(NULL, " ");
    }
    // printf("\n");
    inputArgs[i] = NULL;
    
}

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
    pfad = strtok(NULL, " "); // Bei Null wird der Pointer vom vorherigen strtok genommen an der stelle wo das " " ersetzt wurde
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
    // printf("Ändere Ordner: %s\n", absoluterPfad);
    chdir(absoluterPfad);
}

void verarbeitePipe(char *befehl)
{
    char *argv1[15] = {0};
    char *argv2[15] = {0};
    char *befehl1 = strtok(befehl,"|");
    char *befehl2 = strtok(NULL,"|");

    bearbeiteInput(befehl1, argv1);
    bearbeiteInput(befehl2, argv2);

    int pipefd[2];
    pipe(pipefd);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Kind 1: false
        close(pipefd[0]);                 // liest nicht
        dup2(pipefd[1], STDOUT_FILENO);  // stdout → pipe
        close(pipefd[1]);
        execvp(befehl1, argv1);
        perror("Fehler im Argument 1");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Kind 2: wc
        close(pipefd[1]);                // schreibt nicht
        dup2(pipefd[0], STDIN_FILENO);  // stdin ← pipe
        close(pipefd[0]);
        execvp(befehl2, argv2);
        perror("Fehler im Argument 2");
        exit(1);
    }
    // Elternprozess: beide Seiten schließen
    close(pipefd[0]);
    close(pipefd[1]);

    // Warten auf beide Kinder
    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    fflush(stdout);
    
    // printf("%s Exit-Code: %d\n", befehl1, WEXITSTATUS(status1));
    // printf("%s Exit-Code: %d\n", befehl2,  WEXITSTATUS(status2));
}

void verarbeiteEinzelBefehl(char *befehl, char **arg)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        printf("Fehler beim Erzeugen des Kindprozesses.\n");
        return;
    }

    if (pid == 0)
    {
        execvp(befehl, arg);
        perror("Fehler beim Ausführen des Befehls");
        exit(EXIT_FAILURE);
    }
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

    // printf("Die Eingabe war: %s", befehlZeile);

    size_t len = strlen(befehlZeile);
    if (len > 0 && befehlZeile[len - 1] == '\n')
        befehlZeile[len - 1] = '\0';

    if (strcmp(befehlZeile, "exit") == 0)
        exit(0);

    if (strcmp(befehlZeile, "ret") == 0) {
        printf("%i\n",letzterRückgabewert);
        return false;
    }
        

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
    char befehl[BEFEHLSZEILE_LAENGE] = {0};

    while (befehlTeil != NULL)
    {
        befehlZeile+=strlen(befehlTeil)+1;
        while (*befehlTeil == ' ')
            befehlTeil++;
        memcpy(befehl, befehlTeil, strlen(befehlTeil)+1);

        if (strchr(befehl, '|'))
        {
            verarbeitePipe(befehl);
        }
        else if (strncmp(befehl, "cd", 2) == 0)
        {
            verarbeiteCD(befehl);
        }
        else
        {
            char *befehlAufgeteilt[15];
            bearbeiteInput(befehl, befehlAufgeteilt);
            verarbeiteEinzelBefehl(befehl, befehlAufgeteilt);
        }

        befehlTeil = strtok(befehlZeile, ";");
    }
}

int main()
{
    char verzeichnis[VERZEICHNIS_LAENGE];
    char befehlszeile[BEFEHLSZEILE_LAENGE];
    char data[BEFEHLSZEILE_LAENGE] = "date +%s |wc -c";
    // verarbeiteInput(data);
    // return;

    // Signal-Handler registrieren
    if (signal(SIGHUP, verarbeiteSighup) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
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
