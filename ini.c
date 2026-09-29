#include "ini.h"
#include "config.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    return s;
}

void parse_ini(const char *filename, Config *dst) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        char msg[256];
        snprintf(msg, sizeof(msg),
                 "nao foi possivel abrir o arquivo de configuracao '%s': %s",
                 filename, strerror(errno));
        die(msg);
    }

    char line[256];
    int  linenum = 0;

    while (fgets(line, sizeof(line), f)) {
        linenum++;
        char *s = trim(line);

        if (*s == '\0' || *s == '#') continue;

        char *eq = strchr(s, '=');
        if (!eq) {
            char source[128];
            snprintf(source, sizeof(source), "%s:%d", filename, linenum);
            die_at(source, NULL, "formato esperado: 'chave = valor'");
        }

        *eq        = '\0';
        char *key   = trim(s);
        char *value = trim(eq + 1);

        char source[128];
        snprintf(source, sizeof(source), "%s:%d", filename, linenum);

        apply_field(source, key, value, dst);
    }

    fclose(f);
}
