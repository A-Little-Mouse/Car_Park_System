/**
 * Car Park System
 * 
 * A console-based application for managing a car parking facility.
 * This system allows tracking of vehicles, parking spots, and fees.
 * 
 * Features:
 * - Parking spot management (100 spots)
 * - Vehicle entry and exit tracking
 * - Fee calculation based on parking duration
 * - Search functionality by owner name or license plate
 * - Data persistence using text files
 */

#include <stdio.h>    // Standard input/output
#include <stdlib.h>   // Standard library functions
#include <string.h>   // String manipulation functions
#include <windows.h>  // Windows API functions
#include <conio.h>    // Console input/output
#include <time.h>     // Time-related functions
#include <ctype.h>    // Character type functions
#include <math.h>     // Mathematical functions

// Configure application to run as a Windows application
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

// Constants for system configuration
#define PARKING_SPOTS 100          // Total number of parking spots available
#define FILENAME_SPOTS "parking_spots.txt"    // File to store parking spot data
#define FILENAME_HISTORY "parking_history.txt"  // File to store parking history
#define RATE_PER_SECOND 0.03       // Parking fee rate per second (Rs.)

/**
 * Structure to store complete information about a car parking record
 * Used for maintaining the parking history and generating receipts
 */
typedef struct
{
    char name[50];      // Owner's name
    char plate[20];     // License plate number
    char phone[15];     // Contact phone number
    char address[100];  // Owner's address
    int spot;           // Assigned parking spot number
    time_t entry_time;  // Time when car entered the parking
    time_t exit_time;   // Time when car exited (0 if still parked)
    double fee;         // Calculated parking fee
} CarRecord;

/**
 * Structure to store information about individual parking spots
 * Used for tracking current parking status
 */
typedef struct
{
    int spot;           // Parking spot number (1-100)
    char plate[20];     // License plate of parked car (or "EMPTY")
    int occupied;       // Flag indicating if spot is occupied (1) or empty (0)
    time_t entry_time;  // Time when current car entered this spot
} ParkingSpot;

// Global variable for cursor positioning
COORD coord = {0, 0};

/**
 * Positions the cursor at specified coordinates in the console
 * 
 * @param x X-coordinate (column)
 * @param y Y-coordinate (row)
 */
void gotoxy(int x, int y)
{
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/**
 * Sets the text color for console output
 * 
 * @param color Color code (Windows console color attribute)
 *              Common values: 10=green, 11=cyan, 12=red, 15=white
 */
void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/**
 * Draws a rectangular border using ASCII extended characters
 * 
 * @param width Width of the border in characters
 * @param height Height of the border in characters
 * @param x X-coordinate of the top-left corner
 * @param y Y-coordinate of the top-left corner
 */
void drawBorder(int width, int height, int x, int y)
{
    // Draw top border with corners
    gotoxy(x, y);
    printf("%c", 201);  // Top-left corner
    for (int i = 0; i < width - 2; i++)
        printf("%c", 205);  // Horizontal line
    printf("%c", 187);  // Top-right corner

    // Draw vertical borders
    for (int i = 1; i < height - 1; i++)
    {
        gotoxy(x, y + i);
        printf("%c", 186);  // Left vertical line
        gotoxy(x + width - 1, y + i);
        printf("%c", 186);  // Right vertical line
    }

    // Draw bottom border with corners
    gotoxy(x, y + height - 1);
    printf("%c", 200);  // Bottom-left corner
    for (int i = 0; i < width - 2; i++)
        printf("%c", 205);  // Horizontal line
    printf("%c", 188);  // Bottom-right corner
}

/**
 * Initializes the parking spots data file
 * 
 * Creates a new parking spots file if it doesn't exist.
 * Each line in the file represents one parking spot with format:
 * [spot_number] [license_plate] [occupied_flag] [entry_time]
 */
void initializeParkingSpots()
{
    FILE *file = fopen(FILENAME_SPOTS, "r");
    if (file == NULL)
    {
        // File doesn't exist, create it with all spots empty
        file = fopen(FILENAME_SPOTS, "w");
        for (int i = 0; i < PARKING_SPOTS; i++)
        {
            fprintf(file, "%d EMPTY 0 0\n", i + 1);
        }
        fclose(file);
    }
    else
    {
        // File exists, no need to initialize
        fclose(file);
    }
}

/**
 * Displays the welcome screen with loading animation
 * 
 * Shows a welcome message and animated loading bar when the application starts
 */
void welcomeScreen()
{
    system("cls");  // Clear the screen
    setColor(11);   // Set text color to cyan
    drawBorder(60, 10, 10, 5);  // Draw border for welcome message

    // Display welcome messages
    gotoxy(25, 8);
    printf("WELCOME TO CAR PARK SYSTEM");
    gotoxy(22, 10);
    printf("Loading Parking Management System");

    // Display animated loading bar
    gotoxy(15, 13);
    for (int i = 0; i < 50; i++)
    {
        printf("%c", 177);  // Block character for loading bar
        Sleep(50);          // Delay for animation effect
    }
    Sleep(1000);  // Pause before proceeding to main menu
}


/**
 * Displays the exit screen with animation
 * 
 * Shows a goodbye message and animated "Exiting" text when the application closes
 */
void exitScreen()
{
    system("cls");  // Clear the screen
    setColor(12);   // Set text color to red
    drawBorder(60, 10, 10, 5);  // Draw border for exit message

    // Display credits
    gotoxy(30, 8);
    printf("Made by A Little Mouse");
    
    // Animated exit message
    gotoxy(35, 12);
    printf("Exiting.");
    Sleep(2000);
    gotoxy(35, 12);
    printf("Exiting..");
    Sleep(2000);
    gotoxy(35, 12);
    printf("Exiting...");
    Sleep(2000);

    system("cls");  // Clear screen before exit
}

/**
 * Displays ASCII art of a car at specified coordinates
 * 
 * @param x X-coordinate for the top-left corner of the car art
 * @param y Y-coordinate for the top-left corner of the car art
 */
void displayCarArt(int x, int y)
{
    gotoxy(x, y++);
    printf("   ______");        // Car roof
    gotoxy(x, y++);
    printf("  /|_||_\\`.__");    // Car windows and top
    gotoxy(x, y++);
    printf(" (   _    _ _\\");   // Car body
    gotoxy(x, y++);
    printf("=`-(_)--(_)-'");    // Car wheels
}

/**
 * Counts the number of currently parked cars
 * 
 * Reads the parking spots file and counts occupied spots
 * 
 * @return Number of currently parked cars
 */
int countParkedCars()
{
    FILE *file = fopen(FILENAME_SPOTS, "r");
    if (file == NULL)
        return 0;  // Return 0 if file cannot be opened
    
    ParkingSpot spot;
    int count = 0;
    // Read each spot record and count occupied spots
    while (fscanf(file, "%d %19s %d %ld", &spot.spot, spot.plate, &spot.occupied, &spot.entry_time) == 4)
    {
        if (spot.occupied)
            count++;
    }
    fclose(file);
    return count;
}

/**
 * Displays the main menu with all available options
 * 
 * Shows the main menu interface with car art and current parking status
 */
void mainMenu()
{
    system("cls");  // Clear the screen
    setColor(10);   // Set text color to green
    drawBorder(60, 20, 10, 3);  // Draw border for menu
    displayCarArt(20, 5);  // Display car ASCII art
    
    // Display menu title and options
    gotoxy(30, 8);
    printf("MAIN MENU");
    gotoxy(25, 10);
    printf("1. Display Parking Status");
    gotoxy(25, 11);
    printf("2. Add Car");
    gotoxy(25, 12);
    printf("3. Remove Car");
    gotoxy(25, 13);
    printf("4. Search Records");
    gotoxy(25, 14);
    printf("5. Exit System");

    // Prompt for user input
    gotoxy(25, 16);
    printf("Enter your choice (1-5): ");
    
    // Display current parking status
    gotoxy(25, 20);
    printf("Cars Parked: %d", countParkedCars());
    
    // Position cursor at input position
    gotoxy(50,16);
}

/**
 * Displays the current status of all parking spots
 * 
 * Shows a visual grid of all parking spots with their status (occupied or available)
 * Occupied spots are shown in red with [X], available spots show their spot number in green
 */
void displayParkingStatus()
{
    system("cls");  // Clear the screen
    setColor(15);   // Set text color to white
    drawBorder(82, 27, 5, 2);  // Draw border for parking display
    
    // Ensure parking spots file exists
    initializeParkingSpots();
    
    // Open parking spots file
    FILE *file = fopen(FILENAME_SPOTS, "r");
    if (file == NULL)
    {
        printf("Error loading parking data!");
        return;
    }

    // Display title
    gotoxy(40, 4);
    printf("PARKING STATUS");
    gotoxy(12, 5);

    // Load all parking spots data into memory
    ParkingSpot spots[PARKING_SPOTS];
    for (int i = 0; i < PARKING_SPOTS; i++)
    {
        fscanf(file, "%d %19s %d %ld", &spots[i].spot, spots[i].plate,
               &spots[i].occupied, &spots[i].entry_time);
    }
    fclose(file);

    // Display parking spots in a grid layout (10x10)
    for (int i = 0; i < PARKING_SPOTS; i++)
    {
        // Calculate position in grid (10 columns)
        int row = 8 + (i / 10) * 2;  // New row every 10 spots, with spacing
        int col = 10 + (i % 10) * 7; // 7 characters width per spot

        gotoxy(col, row);
        if (spots[i].occupied)
        {
            setColor(12);  // Red for occupied spots
            printf("[ X ]");
        }
        else
        {
            setColor(10);  // Green for available spots
            printf("[%3d]", spots[i].spot);
        }
    }

    // Prompt to return to main menu
    setColor(15);  // White text
    gotoxy(10, 27);
    printf("Press any key to return to main menu...");
    getch();  // Wait for key press
}

/**
 * Adds a new car to the parking system
 * 
 * Collects car and owner information, validates input data,
 * assigns a parking spot, and updates both the parking spots
 * and history files
 */
void addCar()
{
    system("cls");  // Clear the screen
    drawBorder(60, 20, 10, 3);  // Draw border for input form
    displayCarArt(20, 5);  // Display car ASCII art
    
    CarRecord newCar;  // Structure to store new car information
    time_t now = time(NULL);  // Current timestamp
    gotoxy(25, 8);
    printf("ADD NEW CAR ENTRY");

    // Get Owner Name
    do
    {
        gotoxy(20, 10);
        printf("Owner Name:            ");
        gotoxy(33, 10);
        fflush(stdin);
        fgets(newCar.name, 50, stdin);
        newCar.name[strcspn(newCar.name, "\n")] = 0;
        if (strlen(newCar.name) == 0)
        {
            gotoxy(20, 16);
            setColor(12);
            printf("Name cannot be empty!");
            Sleep(1000);
            gotoxy(20, 16);
            printf("                         ");
        }
    } while (strlen(newCar.name) == 0);

    // Check for existing plates
    ParkingSpot spots[PARKING_SPOTS];
    FILE *file = fopen(FILENAME_SPOTS, "r");
    for (int i = 0; i < PARKING_SPOTS; i++)
    {
        fscanf(file, "%d %19s %d %ld", &spots[i].spot, spots[i].plate,
               &spots[i].occupied, &spots[i].entry_time);
    }
    fclose(file);

    // Get License Plate
    int plateExists = 0;
    do
    {
        plateExists = 0;
        gotoxy(20, 11);
        printf("License Plate:          ");
        gotoxy(35, 11);
        fflush(stdin);
        fgets(newCar.plate, 20, stdin);
        newCar.plate[strcspn(newCar.plate, "\n")] = 0;

        for (int i = 0; i < PARKING_SPOTS; i++)
        {
            if (stricmp(spots[i].plate, newCar.plate) == 0 && spots[i].occupied)
            {
                plateExists = 1;
                break;
            }
        }

        if (plateExists)
        {
            gotoxy(20, 16);
            setColor(12);
            printf("Car already parked! Exiting...");
            Sleep(2000);
            return;
        }
        else if (strlen(newCar.plate) == 0)
        {
            gotoxy(20, 16);
            setColor(12);
            printf("Plate cannot be empty!");
            Sleep(1000);
            gotoxy(20, 16);
            printf("                         ");
        }
    } while (strlen(newCar.plate) == 0);

    // Get Phone Number
    int validPhone = 0;
    setColor(10);
    do
    {
        gotoxy(20, 12);
        printf("Phone Number:          ");
        gotoxy(35, 12);
        fflush(stdin);
        fgets(newCar.phone, 15, stdin);
        newCar.phone[strcspn(newCar.phone, "\n")] = 0;

        validPhone = 1;
        if (strlen(newCar.phone) != 10)
            validPhone = 0;
        for (int i = 0; i < strlen(newCar.phone); i++)
        {
            if (!isdigit(newCar.phone[i]))
            {
                validPhone = 0;
                break;
            }
        }

        if (!validPhone)
        {
            gotoxy(20, 16);
            setColor(12);
            printf("Invalid phone! 10 digits required!");
            Sleep(1000);
            gotoxy(20, 16);
            printf("                                    ");
        }
    } while (!validPhone);

    // Get Address
    setColor(10);
    do
    {
        gotoxy(20, 13);
        printf("Address:             ");
        gotoxy(30, 13);
        fflush(stdin);
        fgets(newCar.address, 100, stdin);
        newCar.address[strcspn(newCar.address, "\n")] = 0;
        if (strlen(newCar.address) == 0)
        {
            gotoxy(20, 16);
            setColor(12);
            printf("Address cannot be empty!");
            Sleep(1000);
            gotoxy(20, 16);
            printf("                       ");
        }
    } while (strlen(newCar.address) == 0);

    // Get Parking Spot
    int valid = 0;
    char input[10];
    setColor(10);
    do
    {
        gotoxy(20, 14);
        printf("Parking Spot (1-100): ");
        gotoxy(41, 14);
        printf("      ");
        gotoxy(41, 14);
        fflush(stdin);
        fgets(input, 10, stdin);
        if (sscanf(input, "%d", &newCar.spot) != 1)
        {
            gotoxy(20, 16);
            setColor(12);
            printf("Invalid number input!");
            Sleep(1000);
            gotoxy(20, 16);
            printf("                        ");
            continue;
        }

        valid = 0;
        for (int i = 0; i < PARKING_SPOTS; i++)
        {
            if (spots[i].spot == newCar.spot && !spots[i].occupied)
            {
                valid = 1;
                break;
            }
        }

        if (!valid)
        {
            gotoxy(20, 16);
            setColor(12);
            printf("Invalid or occupied spot!");
            Sleep(1000);
            gotoxy(20, 16);
            printf("                             ");
        }
    } while (!valid);

    // Update parking spots
    for (int i = 0; i < PARKING_SPOTS; i++)
    {
        if (spots[i].spot == newCar.spot)
        {
            spots[i].occupied = 1;
            strcpy(spots[i].plate, newCar.plate);
            spots[i].entry_time = now;
            break;
        }
    }

    file = fopen(FILENAME_SPOTS, "w");
    for (int i = 0; i < PARKING_SPOTS; i++)
    {
        fprintf(file, "%d %s %d %ld\n", spots[i].spot, spots[i].plate,
                spots[i].occupied, spots[i].entry_time);
    }
    fclose(file);

    // Update history
    file = fopen(FILENAME_HISTORY, "a");
    newCar.entry_time = now;
    newCar.exit_time = 0;
    newCar.fee = 0.0;
    fprintf(file, "%s,%s,%s,%s,%d,%ld,%ld,%.2f\n",
            newCar.name, newCar.plate, newCar.phone,
            newCar.address, newCar.spot, newCar.entry_time,
            newCar.exit_time, newCar.fee);
    fclose(file);

    gotoxy(20, 16);
    setColor(10);
    printf("Entry added successfully!");
    gotoxy(20, 17);
    printf("Press any key to return...");
    getch();
}

/**
 * Removes a car from the parking system
 * 
 * Gets the license plate of the car to remove, calculates parking fee,
 * updates parking spots and history files, and displays a receipt
 */
void removeCar()
{
    system("cls");  // Clear the screen
    setColor(15);   // Set text color to white
    drawBorder(60, 20, 10, 3);  // Draw border for form
    displayCarArt(20, 5);  // Display car ASCII art

    // Check if there are any parked cars
    int parkedCars = countParkedCars();
    if (parkedCars == 0)
    {
        gotoxy(20, 10);
        setColor(12);  // Red text for error
        printf("No cars parked!");
        Sleep(1500);
        return;
    }

    char plate[20];
    time_t exit_time = time(NULL);
    gotoxy(25, 8);
    printf("REMOVE CAR FROM PARKING");

    // Get license plate
    do
    {
        gotoxy(20, 10);
        printf("Enter License Plate: ");
        gotoxy(40, 10);
        printf("                    ");
        gotoxy(40, 10);
        fflush(stdin);
        fgets(plate, 20, stdin);
        plate[strcspn(plate, "\n")] = 0;
        if (strlen(plate) == 0)
        {
            gotoxy(20, 12);
            setColor(12);
            printf("Plate cannot be empty!");
            Sleep(1000);
            gotoxy(20, 12);
            printf("                        ");
        }
    } while (strlen(plate) == 0);

    // Load parking spots
    ParkingSpot spots[PARKING_SPOTS];
    FILE *file = fopen(FILENAME_SPOTS, "r");
    if (file == NULL)
    {
        gotoxy(20, 12);
        printf("Error loading parking data!");
        getch();
        return;
    }

    for (int i = 0; i < PARKING_SPOTS; i++)
    {
        fscanf(file, "%d %19s %d %ld", &spots[i].spot, spots[i].plate,
               &spots[i].occupied, &spots[i].entry_time);
    }
    fclose(file);

    // Find and remove car
    int found = 0;
    double fee = 0.0;
    time_t entry_time = 0;

    for (int i = 0; i < PARKING_SPOTS; i++)
    {
        if (stricmp(spots[i].plate, plate) == 0 && spots[i].occupied)
        {
            entry_time = spots[i].entry_time;
            time_t duration = exit_time - entry_time;
            fee = duration * RATE_PER_SECOND;

            spots[i].occupied = 0;
            strcpy(spots[i].plate, "EMPTY");
            spots[i].entry_time = 0;
            found = 1;
            break;
        }
    }

    if (!found)
    {
        gotoxy(20, 12);
        setColor(12);
        printf("Invalid car entry!");
        Sleep(3000);
        return;
    }

    // Save updated spots
    file = fopen(FILENAME_SPOTS, "w");
    for (int i = 0; i < PARKING_SPOTS; i++)
    {
        fprintf(file, "%d %s %d %ld\n", spots[i].spot, spots[i].plate,
                spots[i].occupied, spots[i].entry_time);
    }
    fclose(file);

    // Update history
    FILE *histFile = fopen(FILENAME_HISTORY, "r+");
    if (histFile != NULL)
    {
        char line[256];
        long pos = 0;
        while (fgets(line, sizeof(line), histFile))
        {
            CarRecord record;
            sscanf(line, "%[^,],%[^,],%[^,],%[^,],%d,%ld,%ld,%lf",
                   record.name, record.plate, record.phone,
                   record.address, &record.spot, &record.entry_time,
                   &record.exit_time, &record.fee);

            if (strcmp(record.plate, plate) == 0 && record.exit_time == 0)
            {
                record.exit_time = exit_time;
                record.fee = fee;

                fseek(histFile, pos, SEEK_SET);
                fprintf(histFile, "%s,%s,%s,%s,%d,%ld,%ld,%.2f\n",
                        record.name, record.plate, record.phone,
                        record.address, record.spot, record.entry_time,
                        record.exit_time, record.fee);
                break;
            }
            pos = ftell(histFile);
        }
        fclose(histFile);
    }

    // Display receipt
    gotoxy(20, 12);
    setColor(10);
    printf("Car removed successfully!");
    gotoxy(20, 13);
    printf("Entry Time:  %s", ctime(&entry_time));
    gotoxy(20, 14);
    printf("Exit Time:   %s", ctime(&exit_time));
    gotoxy(20, 15);
    printf("Parking Duration: %lld seconds", (long long)(exit_time - entry_time));
    gotoxy(20, 16);
    printf("Total Fee:   Rs,%.2f (Rs,0.03/sec)", fee);
    gotoxy(20, 18);
    printf("Press any key to return...");
    getch();
}

/**
 * Searches parking history by owner name
 * 
 * Finds all parking records for a specific owner and displays
 * summary information including total entries and unique vehicles
 */
void searchByName()
{
    system("cls");  // Clear the screen
    char name[50];  // Buffer for owner name input
    drawBorder(60, 25, 10, 3);  // Draw border for search form
    displayCarArt(20, 5);  // Display car ASCII art

    gotoxy(25, 8);
    printf("SEARCH BY OWNER NAME");

    // Get name with validation
    do
    {
        gotoxy(20, 10);
        printf("Enter Owner Name: ");
        gotoxy(38, 10);
        printf("                    ");
        gotoxy(38, 10);
        fflush(stdin);
        fgets(name, 50, stdin);
        name[strcspn(name, "\n")] = 0;
        if (strlen(name) == 0)
        {
            gotoxy(20, 12);
            setColor(12);
            printf("Name cannot be empty!");
            Sleep(1000);
            gotoxy(20, 12);
            printf("                        ");
        }
    } while (strlen(name) == 0);

    FILE *file = fopen(FILENAME_HISTORY, "r");
    if (file == NULL)
    {
        gotoxy(20, 12);
        printf("No history records found!");
        getch();
        return;
    }

    int totalEntries = 0;
    char plates[10][20] = {0};
    int plateCount = 0;
    char line[256];

    while (fgets(line, sizeof(line), file))
    {
        CarRecord record;
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%d,%ld,%ld",
               record.name, record.plate, record.phone,
               record.address, &record.spot, &record.entry_time, &record.exit_time);

        if (stricmp(record.name, name) == 0)
        {
            totalEntries++;

            int isNew = 1;
            for (int i = 0; i < plateCount; i++)
            {
                if (stricmp(plates[i], record.plate) == 0)
                {
                    isNew = 0;
                    break;
                }
            }

            if (isNew && plateCount < 10)
            {
                strcpy(plates[plateCount++], record.plate);
            }
        }
    }
    fclose(file);

    gotoxy(20, 12);
    printf("Parking History for: %s", name);
    gotoxy(20, 13);
    printf("Total Times Parked: %d", totalEntries);
    gotoxy(20, 14);
    printf("Unique Vehicles: %d", plateCount);

    gotoxy(20, 16);
    printf("License Plates Used:");
    for (int i = 0; i < plateCount; i++)
    {
        gotoxy(22, 17 + i);
        printf("%d. %s", i + 1, plates[i]);
    }

    gotoxy(20, 22);
    printf("Press any key to return...");
    getch();
}

/**
 * Searches parking history by license plate
 * 
 * Finds all parking records for a specific vehicle and displays
 * summary information including total entries and all registered owners
 */
void searchByPlate()
{
    system("cls");  // Clear the screen
    char plate[20]; // Buffer for license plate input
    drawBorder(60, 25, 10, 3);  // Draw border for search form
    displayCarArt(20, 5);  // Display car ASCII art

    gotoxy(25, 8);
    printf("SEARCH BY LICENSE PLATE");

    // Get plate with validation
    do
    {
        gotoxy(20, 10);
        printf("Enter License Plate: ");
        gotoxy(40, 10);
        printf("                    ");
        gotoxy(40, 10);
        fflush(stdin);
        fgets(plate, 20, stdin);
        plate[strcspn(plate, "\n")] = 0;
        if (strlen(plate) == 0)
        {
            gotoxy(20, 12);
            setColor(12);
            printf("Plate cannot be empty!");
            Sleep(1000);
            gotoxy(20, 12);
            printf("                        ");
        }
    } while (strlen(plate) == 0);

    FILE *file = fopen(FILENAME_HISTORY, "r");
    if (file == NULL)
    {
        gotoxy(20, 12);
        printf("No history records found!");
        getch();
        return;
    }

    int totalEntries = 0;
    char names[10][50] = {0};
    int nameCount = 0;
    char line[256];

    while (fgets(line, sizeof(line), file))
    {
        CarRecord record;
        sscanf(line, "%[^,],%[^,],%[^,],%[^,],%d,%ld,%ld",
               record.name, record.plate, record.phone,
               record.address, &record.spot, &record.entry_time, &record.exit_time);

        if (stricmp(record.plate, plate) == 0)
        {
            totalEntries++;

            int isNew = 1;
            for (int i = 0; i < nameCount; i++)
            {
                if (stricmp(names[i], record.name) == 0)
                {
                    isNew = 0;
                    break;
                }
            }

            if (isNew && nameCount < 10)
            {
                strcpy(names[nameCount++], record.name);
            }
        }
    }
    fclose(file);

    gotoxy(20, 12);
    printf("Parking History for: %s", plate);
    gotoxy(20, 13);
    printf("Total Entries: %d", totalEntries);

    gotoxy(20, 15);
    printf("Registered Owners:");
    for (int i = 0; i < nameCount; i++)
    {
        gotoxy(22, 16 + i);
        printf("%d. %s", i + 1, names[i]);
    }

    gotoxy(20, 22);
    printf("Press any key to return...");
    getch();
}

/**
 * Displays the search menu with options to search by name or license plate
 * 
 * Provides a submenu for different search options and handles user input
 */
void searchMenu()
{
    char choice;
    do
    {
        system("cls");  // Clear the screen
        setColor(11);   // Set text color to cyan
        drawBorder(60, 20, 10, 3);  // Draw border for menu
        displayCarArt(20, 5);  // Display car ASCII art

        // Display search menu options
        gotoxy(30, 8);
        printf("SEARCH MENU");
        gotoxy(25, 10);
        printf("1. Search by Owner Name");
        gotoxy(25, 11);
        printf("2. Search by License Plate");
        gotoxy(25, 12);
        printf("3. Return to Main Menu");
        gotoxy(25, 14);
        printf("Enter your choice (1-3): ");

        // Get user choice and process it
        choice = getche();  // Get character without waiting for Enter
        system("cls");

        // Handle user selection
        switch (choice)
        {
        case '1':
            searchByName();  // Search records by owner name
            break;
        case '2':
            searchByPlate(); // Search records by license plate
            break;
        case '3':
            return;  // Return to main menu
        default:
            gotoxy(25, 16);
            printf("Invalid choice!");
            Sleep(1000);
        }
    } while (choice != '3');  // Continue until user chooses to return
}

/**
 * Main function - Entry point of the application
 * 
 * Sets up the console environment, initializes the system,
 * and handles the main program loop
 * 
 * @return 0 on successful execution
 */
int main()
{
    // Set up console for Windows GUI application
    AllocConsole();
    freopen("CONIN$", "r", stdin);     // Redirect standard input
    freopen("CONOUT$", "w", stdout);   // Redirect standard output
    freopen("CONOUT$", "w", stderr);   // Redirect standard error

    // Set console title
    SetConsoleTitleA("Car Park System");
    system("title Car Park System");

    // Set console size and buffer
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD size = {90, 30};  // Width and height of console buffer
    SetConsoleScreenBufferSize(hConsole, size);

    // Set console window size
    SMALL_RECT rect = {0, 0, 89, 29};  // Left, top, right, bottom coordinates
    SetConsoleWindowInfo(hConsole, TRUE, &rect);

    // Initialize system and display welcome screen
    initializeParkingSpots();  // Create or verify parking spots file
    welcomeScreen();           // Show welcome animation

    // Main program loop
    int running = 1;
    while (running)
    {
        mainMenu();  // Display main menu options
        char choice = getche();  // Get user selection without waiting for Enter

        // Process user selection
        switch (choice)
        {
        case '1':
            displayParkingStatus();  // Show parking spot grid
            break;
        case '2':
            addCar();  // Add a new car to parking
            break;
        case '3':
            removeCar();  // Remove a car from parking
            break;
        case '4':
            searchMenu();  // Show search submenu
            break;
        case '5':
            running = 0;  // Exit the program
            break;
        default:
            // Handle invalid input
            gotoxy(25, 18);
            setColor(12);  // Red text for error
            printf("Invalid choice!");
            Sleep(1000);
        }
    }

    // Show exit screen and terminate
    exitScreen();
    return 0;
}