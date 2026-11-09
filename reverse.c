#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// yks rivi listassa
struct node {
    char *line;
    struct node *next;
};

int main(int argc, char *argv[]) {
    FILE *in = stdin;
    FILE *out = stdout;

    // liikaa argumentteja
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    // jos on annettu kaks tiedostoa, ni tarkistetaan ettei ne oo sama tiedosto
    if (argc == 3) {
        struct stat a, b;
        if (strcmp(argv[1], argv[2]) == 0) {
            fprintf(stderr, "Input and output file must differ\n");
            exit(1);
        }
        // stat nappaa myös tapauksen ./a.txt vs a.txt (sama inode)
        if (stat(argv[1], &a) == 0 && stat(argv[2], &b) == 0) {
            if (a.st_ino == b.st_ino && a.st_dev == b.st_dev) {
                fprintf(stderr, "Input and output file must differ\n");
                exit(1);
            }
        }
    }

    // avataan input jos se annettiin
    if (argc >= 2) {
        in = fopen(argv[1], "r");
        if (in == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }

    // avataan output jos se annettiin
    if (argc == 3) {
        out = fopen(argv[2], "w");
        if (out == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            exit(1);
        }
    }

    // luetaan rivit listaan niin et uusin rivi menee aina alkuun
    // näin lista on valmiiksi väärinpäin ja tulostus on helppo
    struct node *head = NULL;
    char *buf = NULL;
    size_t size = 0;

    // getline hoitaa pitkät rivit itse, ei tarvii arvailla pituutta
    while (getline(&buf, &size, in) != -1) {
        struct node *n = malloc(sizeof(struct node));
        if (n == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }

        // kopioidaan rivi, koska buf käytetään uudestaan seuraavalla kierroksella
        n->line = strdup(buf);
        if (n->line == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }

        n->next = head;
        head = n;
    }
    free(buf);

    // tulostetaan lista alusta loppuun, eli tiedosto takaperin
    struct node *cur = head;
    while (cur != NULL) {
        fprintf(out, "%s", cur->line);

        // jos viimeisellä rivillä ei ollu rivinvaihtoa, lisätään se
        // ettei rivit mene yhteen
        size_t len = strlen(cur->line);
        if (len > 0 && cur->line[len - 1] != '\n') {
            fprintf(out, "\n");
        }

        struct node *tmp = cur;
        cur = cur->next;
        free(tmp->line);
        free(tmp);
    }

    // siivotaan tiedostot kiinni
    if (in != stdin) {
        fclose(in);
    }
    if (out != stdout) {
        fclose(out);
    }

    return 0;
}
