#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"

void read_csv_update(Theatre theatres[], int count) {
    FILE *fp = fopen("data.csv", "r");
    if (!fp) return;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char movie[50], row;
        int col;
        if (sscanf(line, "%*[^,],%*[^,],%*[^,],%49[^,],%c,%d", movie, &row, &col) == 3) {
            for (int k = 0; k < count; k++) {
                if (strcmp(theatres[k].movie_name, movie) == 0) {
                    int i = row - 'A';
                    int j = col - 1;
                    snprintf(theatres[k].seats[i][j], 4, "[X]");
                }
            }
        }
    }
    fclose(fp);
}

void write_booking_to_csv(Details *d) {
    FILE *fp = fopen("data.csv", "a");
    if (!fp) return;
    fprintf(fp, "%s,%s,%s,%s,%s,%d\n",
            d->name, d->email, d->mobile,
            d->movie_selected, d->row, d->col);
    fclose(fp);
}
