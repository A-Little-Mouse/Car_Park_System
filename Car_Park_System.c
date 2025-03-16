#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

#define PARKING_SPOTS 100
#define FILENAME_SPOTS "parking_spots.txt"
#define FILENAME_HISTORY "parking_history.txt"
#define RATE_PER_SECOND 0.03

typedef struct
{
    char name[50];
    char plate[20];
    char phone[15];
    char address[100];
    int spot;
    time_t entry_time;
    time_t exit_time;
    double fee;
} CarRecord;

typedef struct
{
    int spot;
    char plate[20];
    int occupied;
    time_t entry_time;
} ParkingSpot;

COORD coord = {0, 0};

void gotoxy(int x, int y)
{
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void drawBorder(int width, int height, int x, int y)
{
    gotoxy(x, y);
    printf("%c", 201);
    for (int i = 0; i < width - 2; i++)
        printf("%c", 205);
    printf("%c", 187);

    for (int i = 1; i < height - 1; i++)
    {
        gotoxy(x, y + i);
        printf("%c", 186);
        gotoxy(x + width - 1, y + i);
        printf("%c", 186);
    }

    gotoxy(x, y + height - 1);
    printf("%c", 200);
    for (int i = 0; i < width - 2; i++)
        printf("%c", 205);
    printf("%c", 188);
}

void initializeParkingSpots()
{
    FILE *file = fopen(FILENAME_SPOTS, "r");
    if (file == NULL)
    {
        file = fopen(FILENAME_SPOTS, "w");
        for (int i = 0; i < PARKING_SPOTS; i++)
        {
            fprintf(file, "%d EMPTY 0 0\n", i + 1);
        }
        fclose(file);
    }
    else
    {
        fclose(file);
    }
}

void welcomeScreen()
{
    system("cls");
    setColor(11);
    drawBorder(60, 10, 10, 5);

    gotoxy(25, 8);
    printf("WELCOME TO CAR PARK SYSTEM");
    gotoxy(22, 10);
    printf("Loading Parking Management System");

    gotoxy(15, 13);
    for (int i = 0; i < 50; i++)
    {
        printf("%c", 177);
        Sleep(50);
    }
    Sleep(1000);
}


void exitScreen()
{

    system("cls");
    setColor(12);
    drawBorder(60, 10, 10, 5);

    gotoxy(30, 8);
    printf("Made by A Little Mouse");
    gotoxy(35, 12);
    printf("Exiting.");
    Sleep(2000);
    gotoxy(35, 12);
    printf("Exiting..");
    Sleep(2000);
    gotoxy(35, 12);
    printf("Exiting...");
    Sleep(2000);

    system("cls");
}

void displayCarArt(int x, int y)
{
    gotoxy(x, y++);
    printf("   ______");
    gotoxy(x, y++);
    printf("  /|_||_\\`.__");
    gotoxy(x, y++);
    printf(" (   _    _ _\\");
    gotoxy(x, y++);
    printf("=`-(_)--(_)-'");
}

int countParkedCars()
{
    FILE *file = fopen(FILENAME_SPOTS, "r");
    if (file == NULL)
        return 0;
    
    ParkingSpot spot;
    int count = 0;
    while (fscanf(file, "%d %19s %d %ld", &spot.spot, spot.plate, &spot.occupied, &spot.entry_time) == 4)
    {
        if (spot.occupied)
            count++;
    }
    fclose(file);
    return count;
}

void mainMenu()
{
    system("cls");
    setColor(10);
    drawBorder(60, 20, 10, 3);
    displayCarArt(20, 5);

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

    gotoxy(25, 16);
    printf("Enter your choice (1-5): ");
    gotoxy(25, 20);
    printf("Cars Parked: %d", countParkedCars()); // Display the car counter
    gotoxy(50,16);
}

void displayParkingStatus()
{
    system("cls");
    setColor(15);
    drawBorder(82, 27, 5, 2);

    initializeParkingSpots();
    FILE *file = fopen(FILENAME_SPOTS, "r");
    if (file == NULL)
    {
        printf("Error loading parking data!");
        return;
    }

    gotoxy(40, 4);
    printf("PARKING STATUS");
    gotoxy(12, 5);

    ParkingSpot spots[PARKING_SPOTS];
    for (int i = 0; i < PARKING_SPOTS; i++)
    {
        fscanf(file, "%d %19s %d %ld", &spots[i].spot, spots[i].plate,
               &spots[i].occupied, &spots[i].entry_time);
    }
    fclose(file);

    for (int i = 0; i < PARKING_SPOTS; i++)
    {
        int row = 8 + (i / 10) * 2;
        int col = 10 + (i % 10) * 7;

        gotoxy(col, row);
        if (spots[i].occupied)
        {
            setColor(12);
            printf("[ X ]");
        }
        else
        {
            setColor(10);
            printf("[%3d]", spots[i].spot);
        }
    }

    setColor(15);
    gotoxy(10, 27);
    printf("Press any key to return to main menu...");
    getch();
}

void addCar()
{
    system("cls");
    drawBorder(60, 20, 10, 3);
    displayCarArt(20, 5);

    CarRecord newCar;
    time_t now = time(NULL);
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

void removeCar()
{

    system("cls");
    setColor(15);
    drawBorder(60, 20, 10, 3);
    displayCarArt(20, 5);

    int parkedCars = countParkedCars();
    if (parkedCars == 0)
    {
        gotoxy(20, 10);
        setColor(12);
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

void searchByName()
{
    system("cls");
    char name[50];
    drawBorder(60, 25, 10, 3);
    displayCarArt(20, 5);

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

void searchByPlate()
{
    system("cls");
    char plate[20];
    drawBorder(60, 25, 10, 3);
    displayCarArt(20, 5);

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

void searchMenu()
{
    char choice;
    do
    {
        system("cls");
        setColor(11);
        drawBorder(60, 20, 10, 3);
        displayCarArt(20, 5);

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

        choice = getche();
        system("cls");

        switch (choice)
        {
        case '1':
            searchByName();
            break;
        case '2':
            searchByPlate();
            break;
        case '3':
            return;
        default:
            gotoxy(25, 16);
            printf("Invalid choice!");
            Sleep(1000);
        }
    } while (choice != '3');
}

int main()
{
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

    // Set console title
    SetConsoleTitleA("Car Park System");
    system("title Car Park System");

    // Set console size
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD size = {90, 30};
    SetConsoleScreenBufferSize(hConsole, size);

    SMALL_RECT rect = {0, 0, 89, 29};
    SetConsoleWindowInfo(hConsole, TRUE, &rect);

    // Initialize and run program
    initializeParkingSpots();
    welcomeScreen();

    int running = 1;
    while (running)
    {
        mainMenu();
        char choice = getche();

        switch (choice)
        {
        case '1':
            displayParkingStatus();
            break;
        case '2':
            addCar();
            break;
        case '3':
            removeCar();
            break;
        case '4':
            searchMenu();
            break;
        case '5':
            running = 0;
            break;
        default:
            gotoxy(25, 18);
            setColor(12);
            printf("Invalid choice!");
            Sleep(1000);
        }
    }

    exitScreen();
    return 0;
}