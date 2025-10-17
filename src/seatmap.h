#ifndef SEATMAP_H
#define SEATMAP_H

#include "movies.h"

void initialize_seats(Theatre *t);
void display_seats(Theatre *t);
int book_seat(Theatre *t, char row, int col);

#endif
