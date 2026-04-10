#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define program 1
#define id 2
#define pv 3
#define v 4
#define dp 5
#define integer 7
#define begin 9
#define end 10
#define iff 13
#define then 14
#define var 5
#define aff 12
#define oprel 25
#define oparith 24
#define nb 27
#define po 20
#define pf 21
#define readln 17
#define writeln 19
#define endif 18
#define finfile 100

#define T_ENTIER 1
#define T_INCONNU 0

typedef struct {
    int ul;
    int att;
} unilex;

typedef struct {
    char nom[20];
    int type;
} Entree_TS;

Entree_TS tab_iden[100];
int nbr_iden = 0;

unilex symbole;
char input[5000];
int pos = 0;

int ids_en_attente[20];
int nb_ids_attente = 0;

int erreur_detectee = 0;

int rangerid(char *ch) {
    int s = 0;
    while (s < nbr_iden && strcmp(tab_iden[s].nom, ch) != 0) {
        s++;
    }

    if (s < nbr_iden) {
        return s;
    } else {
        strcpy(tab_iden[nbr_iden].nom, ch);
        tab_iden[nbr_iden].type = T_INCONNU;
        nbr_iden++;
        return nbr_iden - 1;
    }
}

void ajouter_type(int type_donne) {
    for(int i = 0; i < nb_ids_attente; i++) {
        int index = ids_en_attente[i];
        if (tab_iden[index].type != T_INCONNU) {
            printf("Erreur Sémantique: Double déclaration de '%s'\n", tab_iden[index].nom);
            erreur_detectee = 1;
        } else {
            tab_iden[index].type = type_donne;
            printf("[Semantique] Variable '%s' typée comme ENTIER\n", tab_iden[index].nom);
        }
    }
    nb_ids_attente = 0;
}

void verifier_declaration(int index) {
    if (tab_iden[index].type == T_INCONNU) {
        printf("Erreur Sémantique: Variable '%s' non déclarée !\n", tab_iden[index].nom);
        erreur_detectee = 1;
    }
}

void vide(char *ch) { ch[0] = '\0'; }

unilex analex() {
    unilex t;
    char ch[100];
    int i = 0;
    char c;

    while (1) {
        c = input[pos++];

        while (c == ' ' || c == '\n' || c == '\r' || c == '\t') 
            c = input[pos++];

        if (c == '\0') {
            t.ul = finfile; t.att = 0;
            return t;
        }

        if (isalpha(c)) {
            vide(ch);
            i = 0;
            ch[i++] = c;
            while (isalpha(input[pos]) || isdigit(input[pos])) {
                ch[i++] = input[pos++];
            }
            ch[i] = '\0';

            if (strcmp(ch, "program") == 0) { t.ul = program; t.att = 0; return t; }
            if (strcmp(ch, "begin") == 0) { t.ul = begin; t.att = 0; return t; }
            if (strcmp(ch, "end") == 0) { t.ul = end; t.att = 0; return t; }
            if (strcmp(ch, "var") == 0) { t.ul = var; t.att = 0; return t; }
            if (strcmp(ch, "integer") == 0) { t.ul = integer; t.att = 0; return t; }
            if (strcmp(ch, "if") == 0) { t.ul = iff; t.att = 0; return t; }
            if (strcmp(ch, "then") == 0) { t.ul = then; t.att = 0; return t; }
            if (strcmp(ch, "endif") == 0) { t.ul = endif; t.att = 0; return t; }
            if (strcmp(ch, "writeln") == 0) { t.ul = writeln; t.att = 0; return t; }
            if (strcmp(ch, "readln") == 0) { t.ul = readln; t.att = 0; return t; }

            t.ul = id;
            t.att = rangerid(ch);
            return t;
        }

        else if (isdigit(c)) {
            vide(ch);
            i = 0;
            ch[i++] = c;
            while (isdigit(input[pos])) {
                ch[i++] = input[pos++];
            }
            ch[i] = '\0';
            t.ul = nb;
            t.att = atoi(ch);
            return t;
        }

        else {
            t.att = 0;
            if (c == '(') t.ul = po;
            else if (c == ')') t.ul = pf;
            else if (c == ';') t.ul = pv;
            else if (c == ',') t.ul = v;
            else if (c == '.') t.ul = finfile;
            else if (c == '+' || c == '-' || c == '*' || c == '/') t.ul = oparith;
            else if (c == '<' || c == '>') t.ul = oprel;
            else if (c == '=') t.ul = oprel;
            else if (c == ':') {
                if (input[pos] == '=') {
                    pos++;
                    t.ul = aff;
                } else {
                    t.ul = dp;
                }
            } else {
                printf("Erreur lexicale : '%c'\n", c);
                erreur_detectee = 1;
                t.ul = finfile;
            }
            return t;
        }
    }
}

void P(); void Dcl(); void Liste_id(); void L1(); void Liste_inst();
void I1(); void I(); void C(); void Exp(); void E1();

void accepter(int t) {
    if (symbole.ul == t) {
        symbole = analex();
    } else {
        printf("Erreur Syntaxique : symbole %d attendu, trouvé %d\n", t, symbole.ul);
        exit(1);
    }
}

void P() {
    if (symbole.ul == program) {
        accepter(program);
        accepter(id);
        accepter(pv);
        Dcl();
        accepter(begin);
        Liste_inst();
        accepter(end);
    } else printf("Erreur : program manquant\n");
}

void Dcl() {
    if (symbole.ul == var) {
        accepter(var);

        nb_ids_attente = 0;

        Liste_id();

        accepter(dp);

        if (symbole.ul == integer) {
            ajouter_type(T_ENTIER); 
            accepter(integer);
        } else {
            printf("Erreur Syntaxique : Type attendu (integer)\n");
            exit(1);
        }

        accepter(pv);
    }
}

void Liste_id() {
    if (symbole.ul == id) {
        ids_en_attente[nb_ids_attente++] = symbole.att; 

        accepter(id);
        L1();
    }
}

void L1() {
    if (symbole.ul == v) {
        accepter(v);
        Liste_id();
    }
}

void Liste_inst() {
    I();
    I1();
}

void I1() {
    if (symbole.ul == id || symbole.ul == writeln || symbole.ul == readln || symbole.ul == iff)
        Liste_inst();
}

void I() {
    if (symbole.ul == id) {
        int index_id = symbole.att;
        verifier_declaration(index_id);

        accepter(id);
        accepter(aff);
        Exp();

        accepter(pv);
    } 
    else if (symbole.ul == writeln) {
        accepter(writeln);
        accepter(po);

        if (symbole.ul == id) verifier_declaration(symbole.att);
        accepter(id);

        accepter(pf);
        accepter(pv);
    } 
    else if (symbole.ul == readln) {
        accepter(readln);
        accepter(po);

        if (symbole.ul == id) verifier_declaration(symbole.att);
        accepter(id);

        accepter(pf);
        accepter(pv);
    } 
    else if (symbole.ul == iff) {
        accepter(iff);
        C();
        accepter(then);
        Liste_inst();
        accepter(endif);
        accepter(pv);
    }
}

void C() {
    Exp();
    accepter(oprel);
    Exp();
}

void Exp() {
    if (symbole.ul == id) {
        verifier_declaration(symbole.att);
        accepter(id);
        E1();
    }
    else if (symbole.ul == nb) {
        accepter(nb);
        E1();
    }
    else if (symbole.ul == po) {
        accepter(po);
        Exp();
        accepter(pf);
        E1();
    }
}

void E1() {
    if (symbole.ul == oparith) {
        accepter(oparith);
        Exp();
        E1();
    }
}

int main() {
    FILE *fichier;
    char nom_fichier[] = "program.txt";
    char ligne[500];

    printf("Ouverture du fichier '%s'...\n", nom_fichier);

    fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("ERREUR: Impossible d'ouvrir le fichier %s\n", nom_fichier);
        printf("Veuillez creer un fichier 'program.txt' avec votre code source.\n");
        return 1;
    }

    input[0] = '\0';
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (strlen(input) + strlen(ligne) < 5000) {
            strcat(input, ligne);
        } else {
            printf("Erreur: Le fichier est trop volumineux pour le buffer.\n");
            break;
        }
    }

    fclose(fichier);

    printf("Contenu lu : \n%s\n----------------------------------\n", input);

    pos = 0;
    symbole = analex();

    P();

    if (erreur_detectee == 0) {
        printf("\nAnalyse Lexicale, Syntaxique et Sémantique terminée avec succès.\n");
    } else {
        printf("\nLe programme comporte des erreurs (voir ci-dessus).\n");
    }

    return 0;
}