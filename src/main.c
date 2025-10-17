#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h> // For toupper

// --- Constants ---
#define MAX_ROWS 10 // A-J
#define MAX_COLS 15 // 1-15
#define MAX_SEATS_PER_BOOKING 10 // Maximum seats a user can book at once

// --- Seat Prices ---
#define PRICE_RECLINER 500 // Row J (index 9)
#define PRICE_PRIME 300    // Rows G-I (index 6-8)
#define PRICE_NORMAL 200   // Rows A-F (index 0-5)

// --- Struct Definitions ---
typedef struct {
    char name[50];
    char email[50];
    char mobile[11]; // Changed to 11 to safely hold 10 digits + '\0'
    char seat_labels[MAX_SEATS_PER_BOOKING][5]; // e.g., "A-15\0"
    int num_seats_booked;
    char movie_selected[50];
} Details;

typedef struct {
    char *movie_name;
    // [ROW][COL][String for "[ ]" or "[X]"]
    char seats[MAX_ROWS][MAX_COLS][4];
} Theatre;

// --- Global Variables ---
Details *dynamic_array = NULL;
int count = 0;

Theatre one, two, three, four, five;

// --- Function Prototypes ---
void InitializeTheatres();
void Cleanup();
void PrintMenu();
void InputDetails();
void ShowDetails();
void ReadCSVAndUpdateSeats(const char *filename, Theatre *theatre, const char *moviename);
void DisplaySeatMap(Theatre *theatre);
int GetSeatPrice(char row);
void Book();
void GenerateBill();

// --- Initialization & Cleanup ---

void InitializeTheatres() {
    one.movie_name = strdup("Dune 2");
    two.movie_name = strdup("Transformers One");
    three.movie_name = strdup("Oppenheimer");
    four.movie_name = strdup("Inception");
    five.movie_name = strdup("Tenet");

    const char *empty_seat = "[ ]";

    // Initialize all seats to empty for all theatres
    Theatre *theatres[] = {&one, &two, &three, &four, &five};
    for (int k = 0; k < 5; k++) {
        for (int i = 0; i < MAX_ROWS; i++) {
            for (int j = 0; j < MAX_COLS; j++) {
                strcpy(theatres[k]->seats[i][j], empty_seat);
            }
        }
    }
    
    // Allocate the initial dynamic array memory
    // Note: The original code used malloc(sizeof(Details)) before the loop, which is only for 1 element.
    // It's better to start with NULL and let InputDetails handle the first allocation with realloc.
    dynamic_array = NULL; 
}

void Cleanup() {
    // Free movie names
    free(one.movie_name);
    free(two.movie_name);
    free(three.movie_name);
    free(four.movie_name);
    free(five.movie_name);

    // Free the dynamic array for user details
    free(dynamic_array);
    dynamic_array = NULL;
    count = 0;
}

// --- Menu & Detail Functions (Minor corrections) ---

void PrintMenu()
{
    // ... (menu printing code remains the same)
    printf("\033[1;36m");
    printf("  ╔════════════════════════════════════════════════════════════╗\n");
    printf("  ║                                                            ║\n");
    printf("  ║                       BOOK YOUR SHOW                       ║\n");
    printf("  ║                                                            ║\n");
    printf("  ║ Please select an option from below:                        ║\n");
    printf("  ║                                                            ║\n");
    printf("  ║    (1) Enter Details                                       ║\n");
    printf("  ║    (2) Show Details                                        ║\n");
    printf("  ║    (3) Book Movies                                         ║\n");
    printf("  ║    (4) Generate Bill                                       ║\n");
    printf("  ║    (5) Exit                                                ║\n");
    printf("  ║                                                            ║\n");
    printf("  ╚════════════════════════════════════════════════════════════╝\n");
    printf("  > ");
    printf("\033[0m");
}

void InputDetails()
{
    // Fix: Realloc should handle the first allocation when dynamic_array is NULL
    Details *temp = realloc(dynamic_array, (count + 1) * sizeof(Details));
    if (temp == NULL)
    {
        printf("\n Memory allocation failed !\n");
        return;
    }
    dynamic_array = temp;

    // Clear the input buffer before reading strings
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    
    printf("  >>> Enter your name: ");
    scanf(" %[^\n]", dynamic_array[count].name);

    printf("  >>> Enter your email address: ");
    scanf("%s", dynamic_array[count].email);

    printf("  >>> Enter mobile number (max 10 digits): ");
    scanf("%s", dynamic_array[count].mobile);
    
    // Initialize new detail structure
    dynamic_array[count].num_seats_booked = 0;
    strcpy(dynamic_array[count].movie_selected, "None");

    count++;
    printf("  >>> Details saved successfully!\n");
}

void ShowDetails()
{
    // Clear the input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("  >>> Enter first or last name to search: ");
    char search[50];
    scanf(" %49s", search); 
    
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strstr(dynamic_array[i].name, search) != NULL) {
            printf("\n  --------------------------------------\n");
            printf("  Name: %s\n  Mobile: %s\n  Email: %s\n", 
                   dynamic_array[i].name, dynamic_array[i].mobile, dynamic_array[i].email);
            printf("  Movie: %s\n", dynamic_array[i].movie_selected);
            if (dynamic_array[i].num_seats_booked > 0) {
                printf("  Seats Booked (%d): ", dynamic_array[i].num_seats_booked);
                for(int j = 0; j < dynamic_array[i].num_seats_booked; j++) {
                    printf("%s%s", dynamic_array[i].seat_labels[j], (j < dynamic_array[i].num_seats_booked - 1) ? ", " : "");
                }
                printf("\n");
            } else {
                printf("  Seats Booked: None\n");
            }
            printf("  --------------------------------------\n");
            found = 1;
        }
    }
    if (!found) {
        printf("  >>> No user found with name containing '%s'.\n", search);
    }
}

// --- Seat Management Functions ---

void ReadCSVAndUpdateSeats(const char *filename, Theatre *theatre, const char *moviename) {
    // Note: This function only reads and updates the map.
    // In a real application, you'd load all bookings once and initialize the maps.
    // The current implementation is inefficient as it re-reads the file every time a booking is made.
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("  [ERROR] Could not open file %s for reading. Seat status may be incorrect.\n", filename);
        return;
    }

    char line[256];
    char temp_line[256]; 
    
    // Reset seats for the theatre being updated, as we're re-reading from CSV
    const char *empty_seat = "[ ]";
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            strcpy(theatre->seats[i][j], empty_seat);
        }
    }

    while (fgets(line, sizeof(line), file)) {
        strcpy(temp_line, line); // Copy line for strtok since it modifies the string
        
        char *token;
        char movie[50] = {0}; 
        char row_char = 0;
        int col = 0;

        // Assuming CSV format: Name,Email,Mobile,Movie,Row,Col
        // Skip Name, Email, Mobile
        token = strtok(temp_line,","); // Name
        if (token) token = strtok(NULL,","); // Email
        if (token) token = strtok(NULL,","); // Mobile
        
        // Movie
        if (token) token = strtok(NULL, ",");
        if (token) {
            // Trim potential newline or carriage return from movie name
            size_t len = strlen(token);
            if (len > 0 && (token[len-1] == '\n' || token[len-1] == '\r')) token[len-1] = '\0';
            strcpy(movie, token);
        }

        // Row
        if (token) token = strtok(NULL, ",");
        if (token && token[0] != '\n') row_char = toupper(token[0]); 

        // Col
        if (token) token = strtok(NULL, ",");
        if (token) col = atoi(token);

        // Update the seat if the movie matches
        if (strcmp(movie, moviename) == 0) {
            int row_index = row_char - 'A';
            if (row_index >= 0 && row_index < MAX_ROWS && col >= 1 && col <= MAX_COLS) {
                strcpy(theatre->seats[row_index][col - 1], "[X]");
            }
        }
    }
    fclose(file);
}

void DisplaySeatMap(Theatre *theatre) {
    printf("\n\t\t ╔═══════════════════════════════════════╗\n");
    printf("\t\t ║               SCREEN                  ║\n");
    printf("\t\t ╚═══════════════════════════════════════╝\n");
    
    // Print Column Headers
    printf("\t\t   ");
    for(int j = 1; j <= MAX_COLS; j++) {
        printf("%3d", j);
    }
    printf("\n");

    // Print Seats
    for (int i = MAX_ROWS - 1; i >= 0; i--) {
        char row_char = 'A' + i;
        printf("\033[38;5;250m"); // Row label color
        printf("\t\t%c ", row_char);
        printf("\033[0m");

        // Price Category Label
        if (row_char == 'J')      printf("\tRecliner ($%d)", PRICE_RECLINER);
        else if (row_char == 'I') printf("\tPrime ($%d)", PRICE_PRIME);
        else if (row_char == 'F') printf("\tNormal ($%d)", PRICE_NORMAL);
        else printf("\t         "); // Alignment for other rows

        printf("\n"); // Newline for the price label

        // Reprint row for actual seats
        printf("\033[38;5;250m");
        printf("\t\t%c ", row_char);
        printf("\033[0m");
        
        for (int j = 0; j < MAX_COLS; j++) {
            if (strcmp(theatre->seats[i][j], "[ ]") == 0)
                printf("\033[1;32m"); // Green for empty
            else
                printf("\033[1;31m"); // Red for booked
            printf("%s", theatre->seats[i][j]);
            printf("\033[0m");
        }
        printf("\n");
    }
    printf("\n");
    printf("\t\t [ ] = Available | [X] = Booked\n");
}

int GetSeatPrice(char row) {
    row = toupper(row);
    if (row == 'J') return PRICE_RECLINER;
    if (row >= 'G' && row <= 'I') return PRICE_PRIME;
    if (row >= 'A' && row <= 'F') return PRICE_NORMAL;
    return 0; // Should not happen with validation
}

void Book()
{
    if (count == 0) {
        printf("  >>> Please enter your details first (Option 1).\n");
        return;
    }

    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("  >>> Enter your name to search for your booking: ");
    char search[50];
    scanf(" %49s", search);

    int user_index = -1;
    for (int i = 0; i < count; i++) {
        if (strstr(dynamic_array[i].name, search) != NULL) {
            user_index = i;
            break;
        }
    }

    if (user_index == -1) {
        printf("  >>> User not found. Please enter details first.\n");
        return;
    }

    // --- Movie Selection ---
    printf("\n  Which Movie would you like to watch:\n");
    printf("  \t(1) Dune 2\n");
    printf("  \t(2) Transformers One\n");
    printf("  \t(3) Oppenheimer\n");
    printf("  \t(4) Inception\n");
    printf("  \t(5) Tenet\n");
    printf("  >>> ");

    int movie_num;
    if (scanf("%d", &movie_num) != 1) {
        printf("  [ERROR] Invalid input. Please enter a number.\n");
        while ((c = getchar()) != '\n' && c != EOF); // Clear buffer
        return;
    }

    char *movie_chosen_name;
    Theatre *selected_theatre;

    switch(movie_num) {
        case 1: movie_chosen_name = "Dune 2"; selected_theatre = &one; break;
        case 2: movie_chosen_name = "Transformers One"; selected_theatre = &two; break;
        case 3: movie_chosen_name = "Oppenheimer"; selected_theatre = &three; break;
        case 4: movie_chosen_name = "Inception"; selected_theatre = &four; break;
        case 5: movie_chosen_name = "Tenet"; selected_theatre = &five; break;
        default:
            printf("  [ERROR] Invalid movie selection.\n");
            return;
    }

    strcpy(dynamic_array[user_index].movie_selected, movie_chosen_name);
    printf("  You have chosen the movie %s.\n", movie_chosen_name);

    // --- Update Seats from CSV and Display Map ---
    ReadCSVAndUpdateSeats("data.csv", selected_theatre, movie_chosen_name);
    DisplaySeatMap(selected_theatre);
    
    // Reset seats booked for the user before new booking
    dynamic_array[user_index].num_seats_booked = 0; 
    
    // --- Seat Selection Loop ---
    int num_seats;
    printf("  >>> How many seats would you like to book (max %d)? ", MAX_SEATS_PER_BOOKING);
    if (scanf("%d", &num_seats) != 1 || num_seats <= 0 || num_seats > MAX_SEATS_PER_BOOKING) {
        printf("  [ERROR] Invalid number of seats requested.\n");
        while ((c = getchar()) != '\n' && c != EOF); // Clear buffer
        return;
    }

    for (int k = 0; k < num_seats; k++) {
        char row_char;
        int col_num;
        int valid_seat = 0;

        while (!valid_seat) {
            printf("\n  >>> Select Seat %d of %d:\n", k + 1, num_seats);
            printf("  >>> Enter the row (A-%c): ", 'A' + MAX_ROWS - 1);
            if (scanf(" %c", &row_char) != 1) {
                 printf("  [ERROR] Invalid row input.\n");
                 while ((c = getchar()) != '\n' && c != EOF); 
                 continue;
            }
            row_char = toupper(row_char);

            printf("  >>> Enter the seat number (1-%d): ", MAX_COLS);
            if (scanf("%d", &col_num) != 1) {
                printf("  [ERROR] Invalid column input.\n");
                while ((c = getchar()) != '\n' && c != EOF); 
                continue;
            }
            
            int row_index = row_char - 'A';
            int col_index = col_num - 1;

            if (row_index < 0 || row_index >= MAX_ROWS || col_index < 0 || col_index >= MAX_COLS) {
                printf("  [ERROR] Invalid row or seat number. Row must be A-%c and seat must be 1-%d.\n", 
                       'A' + MAX_ROWS - 1, MAX_COLS);
            } else if (strcmp(selected_theatre->seats[row_index][col_index], "[X]") == 0) {
                printf("  [ERROR] Seat %c-%d is already booked. Please choose another.\n", row_char, col_num);
            } else {
                // Seat is valid and available
                const char *booked_seat = "[X]";
                strcpy(selected_theatre->seats[row_index][col_index], booked_seat);

                // Save seat label to user details
                snprintf(dynamic_array[user_index].seat_labels[k], 5, "%c-%d", row_char, col_num);
                dynamic_array[user_index].num_seats_booked++;
                
                // Write booking to CSV (In a simple file-based system, this is where you'd save it)
                FILE *file = fopen("data.csv", "a");
                if (file) {
                    fprintf(file, "%s,%s,%s,%s,%c,%d\n", 
                            dynamic_array[user_index].name, 
                            dynamic_array[user_index].email, 
                            dynamic_array[user_index].mobile, 
                            movie_chosen_name, 
                            row_char, 
                            col_num);
                    fclose(file);
                } else {
                    printf("  [WARNING] Could not open data.csv for writing. Booking may not be permanent.\n");
                }
                
                printf("  [SUCCESS] Seat %c-%d booked!\n", row_char, col_num);
                valid_seat = 1;
            }
        }
    }
    printf("\n  >>> Booking complete for %s. You booked %d seat(s).\n", 
           dynamic_array[user_index].name, dynamic_array[user_index].num_seats_booked);
}

void GenerateBill()
{
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("  >>> Enter your name to generate the bill: ");
    char search[50];
    scanf(" %49s", search);
    printf("\033[0;31m");
    
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strstr(dynamic_array[i].name, search) != NULL)
        {
            found = 1;
            int total_price = 0;
            
            printf("\n\n\t\t *************************************\n");
            printf("\t\t     * MOVIE TICKET RECEIPT\n");
            printf("\t\t     *************************************\n");
            printf("\t\t     * Name: %s\n", dynamic_array[i].name);
            printf("\t\t     * Email: %s\n", dynamic_array[i].email);
            printf("\t\t     * Mobile No: %s\n", dynamic_array[i].mobile);
            
            time_t currentTime;
            struct tm *localTime;
            currentTime = time(NULL);
            localTime = localtime(&currentTime);
            printf("\t\t     * Date/Time: %s", asctime(localTime));
            
            printf("\t\t     * Movie Selected: %s\n", dynamic_array[i].movie_selected);
            printf("\t\t     *-----------------------------------\n");
            printf("\t\t     * Seat Details:\n");

            for(int j = 0; j < dynamic_array[i].num_seats_booked; j++) {
                char *seat_label = dynamic_array[i].seat_labels[j];
                char row = seat_label[0];
                int price = GetSeatPrice(row);
                total_price += price;
                printf("\t\t     * Seat %s (Row %c): $%d\n", seat_label, row, price);
            }

            printf("\t\t     *-----------------------------------\n");
            printf("\t\t     * Total Price: $%d\n", total_price);
            printf("\t\t     *************************************\n\n");
            break;
        }
    }
    
    if (!found) {
        printf("  >>> No user found with name containing '%s'.\n", search);
    }
    printf("\033[0m");
}

int main()
{
    InitializeTheatres();

    int flag = 0;
    int n;

    // The original code allocated 1 'Details' struct here, which is immediately lost 
    // when 'InputDetails' calls 'realloc'. The fix is to let 'InputDetails' handle 
    // the first allocation when 'dynamic_array' is NULL (done in InitializeTheatres).
    // The previous 'malloc' block is now removed.

    while (flag != 1)
    {
        PrintMenu();
        if (scanf("%d", &n) != 1) {
            printf("  [ERROR] Invalid input. Please enter a number.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF); // Clear buffer
            continue;
        }
        
        switch (n)
        {
        case 1:
            InputDetails();
            break;
        case 2:
            ShowDetails();
            break;
        case 3:
            Book();
            break;
        case 4:
            GenerateBill();
            break;
        case 5:
            flag = 1;
            break;
        default:
            printf("  [ERROR] Invalid entry!\n");
            break;
        }
    }

    Cleanup();
    return 0;
}