#include <stdio.h>
#include <string.h>
#include "details.h"

void input_details(Details *d) {
    printf("Enter your name: ");
    scanf(" %[^\n]", d->name);
    printf("Enter your email: ");
    scanf(" %[^\n]", d->email);
    printf("Enter your mobile: ");
    scanf(" %s", d->mobile);
}

void show_details(Details *d) {
    printf("\nName: %s\nEmail: %s\nMobile: %s\nMovie: %s\nSeat: %s%d\n",
           d->name, d->email, d->mobile,
           d->movie_selected, d->row, d->col);
}
