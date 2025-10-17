// movies.h
#ifndef MOVIES_H
#define MOVIES_H

typedef struct {
    char name[50];
    char email[50];
    char mobile[11];
    char *row;
    int col;
    char movie_selected[50];
} Details;

typedef struct {
    char *movie_name;
    char seats[10][15][4];
} Theatre;

#endif
