#include <stdio.h>
#include <ctype.h>
#include "seatmap.h"

void initialize_seats(Theatre *t) {
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 15; j++)
            snprintf(t->seats[i][j], sizeof(t->seats[i][j]), "[ ]");
}

void display_seats(Theatre *t) {
    printf("\n      ");
    for (int j = 0; j < 15; j++) printf("%2d ", j + 1);
    printf("\n");
    for (int i = 0; i < 10; i++) {
        printf("Row %c ", 'A' + i);
        for (int j = 0; j < 15; j++)
            printf("%s ", t->seats[i][j]);
        printf("\n");
    }
}

int book_seat(Theatre *t, char row, int col) {
    int i = toupper(row) - 'A';
    int j = col - 1;
    if (i < 0 || i >= 10 || j < 0 || j >= 15)
        return 0;
    if (t->seats[i][j][1] == 'X') return 0;
    snprintf(t->seats[i][j], sizeof(t->seats[i][j]), "[X]");
    return 1;
}
