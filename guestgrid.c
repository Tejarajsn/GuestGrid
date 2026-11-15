#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_GUESTS 500
#define TOTAL_TABLES 10
#define SEATS_PER_TABLE 8

typedef struct {
    int id;
    char name[50];
    char phone[15];
    char group[50];
    int tableNo;     // 0 = Unassigned, 1..10 = Assigned Table
    int checkedIn;   // 0 = Pending, 1 = Arrived
} Guest;

Guest guests[MAX_GUESTS];
int guestCount = 0;
int nextID = 1;

// Clear leftover characters from input buffer
void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Save all guest records to guests.txt
void saveGuests() {
    FILE *fp = fopen("guests.txt", "w");
    if (fp == NULL) {
        printf("Error saving guest data!\n");
        return;
    }

    for (int i = 0; i < guestCount; i++) {
        fprintf(fp, "%d|%s|%s|%s|%d|%d\n",
                guests[i].id,
                guests[i].name,
                guests[i].phone,
                guests[i].group,
                guests[i].tableNo,
                guests[i].checkedIn);
    }
    fclose(fp);
}

// Load guest records from guests.txt (supports both old 4-field and new 6-field formats)
void loadGuests() {
    FILE *fp = fopen("guests.txt", "r");
    if (fp == NULL) return;

    char line[256];
    guestCount = 0;

    while (guestCount < MAX_GUESTS && fgets(line, sizeof(line), fp) != NULL) {
        int id, tableNo = 0, checkedIn = 0;
        char name[50], phone[15], group[50];

        int fields = sscanf(line, "%d|%49[^|]|%14[^|]|%49[^|\n]|%d|%d",
                            &id, name, phone, group, &tableNo, &checkedIn);

        if (fields >= 4) {
            guests[guestCount].id = id;
            strcpy(guests[guestCount].name, name);
            strcpy(guests[guestCount].phone, phone);
            strcpy(guests[guestCount].group, group);
            guests[guestCount].tableNo = (fields >= 5) ? tableNo : 0;
            guests[guestCount].checkedIn = (fields >= 6) ? checkedIn : 0;

            if (id >= nextID) {
                nextID = id + 1;
            }
            guestCount++;
        }
    }
    fclose(fp);
}

// Check duplicate phone number
int checkDuplicate(const char phone[], int ignoreIndex) {
    for (int i = 0; i < guestCount; i++) {
        if (i != ignoreIndex && strcmp(guests[i].phone, phone) == 0) {
            return 1;
        }
    }
    return 0;
}

// Find guest index by numeric ID
int findGuestIndexByID(int id) {
    for (int i = 0; i < guestCount; i++) {
        if (guests[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Count occupied seats at a given table (1..10)
int getTableOccupancy(int tableNo) {
    int count = 0;
    for (int i = 0; i < guestCount; i++) {
        if (guests[i].tableNo == tableNo) {
            count++;
        }
    }
    return count;
}

// 1. Add new guest
void addGuest() {
    if (guestCount >= MAX_GUESTS) {
        printf("\nGuest storage is full!\n");
        return;
    }

    printf("\n========== GUEST REGISTRATION ==========\n");

    printf("Enter Guest Name: ");
    scanf(" %49[^\n]", guests[guestCount].name);
    clearBuffer();

    printf("Enter Phone Number: ");
    scanf(" %14s", guests[guestCount].phone);
    clearBuffer();

    if (checkDuplicate(guests[guestCount].phone, -1)) {
        printf("\nDuplicate Guest Detected! Phone number already registered.\n");
        return;
    }

    printf("Enter Family/Group: ");
    scanf(" %49[^\n]", guests[guestCount].group);
    clearBuffer();

    guests[guestCount].id = nextID++;
    guests[guestCount].tableNo = 0;
    guests[guestCount].checkedIn = 0;

    printf("\nRegistration Successful!\n");
    printf("Guest ID   : G%03d\n", guests[guestCount].id);
    printf("Guest Name : %s\n", guests[guestCount].name);
    printf("Phone      : %s\n", guests[guestCount].phone);
    printf("Group      : %s\n", guests[guestCount].group);

    guestCount++;
    saveGuests();
    printf("Guest information saved successfully!\n");
}

// 2. View all guests
void viewGuests() {
    printf("\n==================== REGISTERED GUESTS ====================\n");
    if (guestCount == 0) {
        printf("No guests registered yet.\n");
        return;
    }

    printf("\n%-7s %-20s %-14s %-16s %-8s %-10s\n",
           "ID", "NAME", "PHONE", "GROUP", "TABLE", "STATUS");
    printf("-------------------------------------------------------------------------------\n");

    for (int i = 0; i < guestCount; i++) {
        char tableStr[12];
        if (guests[i].tableNo == 0)
            strcpy(tableStr, "None");
        else
            sprintf(tableStr, "T%02d", guests[i].tableNo);

        printf("G%03d    %-20s %-14s %-16s %-8s %-10s\n",
               guests[i].id,
               guests[i].name,
               guests[i].phone,
               guests[i].group,
               tableStr,
               guests[i].checkedIn ? "Arrived" : "Pending");
    }
    printf("\nTotal Registered Guests: %d\n", guestCount);
}

// 3. Search guest by ID or Name
void searchGuest() {
    if (guestCount == 0) {
        printf("\nNo guests registered yet.\n");
        return;
    }

    int searchChoice;
    printf("\n========== SEARCH GUEST ==========\n");
    printf("1. Search by Guest ID (numeric, e.g., 1 for G001)\n");
    printf("2. Search by Name\n");
    printf("Enter choice: ");
    if (scanf("%d", &searchChoice) != 1) {
        clearBuffer();
        printf("Invalid input!\n");
        return;
    }
    clearBuffer();

    if (searchChoice == 1) {
        int id;
        printf("Enter Guest ID number: ");
        if (scanf("%d", &id) != 1) {
            clearBuffer();
            printf("Invalid ID!\n");
            return;
        }
        clearBuffer();

        int idx = findGuestIndexByID(id);
        if (idx == -1) {
            printf("\nGuest G%03d not found.\n", id);
        } else {
            printf("\nGuest Found:\n");
            printf("ID     : G%03d\n", guests[idx].id);
            printf("Name   : %s\n", guests[idx].name);
            printf("Phone  : %s\n", guests[idx].phone);
            printf("Group  : %s\n", guests[idx].group);
            if (guests[idx].tableNo == 0)
                printf("Table  : Unassigned\n");
            else
                printf("Table  : Table %d\n", guests[idx].tableNo);
            printf("Status : %s\n", guests[idx].checkedIn ? "Arrived" : "Pending");
        }
    } else if (searchChoice == 2) {
        char query[50];
        int found = 0;
        printf("Enter Name (or part of name): ");
        scanf(" %49[^\n]", query);
        clearBuffer();

        printf("\n%-7s %-20s %-14s %-16s %-8s %-10s\n",
               "ID", "NAME", "PHONE", "GROUP", "TABLE", "STATUS");
        printf("-------------------------------------------------------------------------------\n");

        for (int i = 0; i < guestCount; i++) {
            if (strstr(guests[i].name, query) != NULL) {
                char tableStr[12];
                if (guests[i].tableNo == 0)
                    strcpy(tableStr, "None");
                else
                    sprintf(tableStr, "T%02d", guests[i].tableNo);

                printf("G%03d    %-20s %-14s %-16s %-8s %-10s\n",
                       guests[i].id, guests[i].name, guests[i].phone,
                       guests[i].group, tableStr,
                       guests[i].checkedIn ? "Arrived" : "Pending");
                found++;
            }
        }
        if (found == 0) printf("No matching guests found.\n");
    } else {
        printf("Invalid choice!\n");
    }
}

// 4. Update guest details
void updateGuest() {
    if (guestCount == 0) {
        printf("\nNo guests registered yet.\n");
        return;
    }

    int id;
    printf("\n========== UPDATE GUEST ==========\n");
    printf("Enter Guest ID number to update (e.g., 1 for G001): ");
    if (scanf("%d", &id) != 1) {
        clearBuffer();
        printf("Invalid ID!\n");
        return;
    }
    clearBuffer();

    int idx = findGuestIndexByID(id);
    if (idx == -1) {
        printf("Guest G%03d not found.\n", id);
        return;
    }

    char newName[50], newPhone[15], newGroup[50];

    printf("Current Name (%s) - Enter New Name: ", guests[idx].name);
    scanf(" %49[^\n]", newName);
    clearBuffer();

    printf("Current Phone (%s) - Enter New Phone: ", guests[idx].phone);
    scanf(" %14s", newPhone);
    clearBuffer();

    if (checkDuplicate(newPhone, idx)) {
        printf("\nUpdate failed! Another guest already uses phone %s.\n", newPhone);
        return;
    }

    printf("Current Group (%s) - Enter New Group: ", guests[idx].group);
    scanf(" %49[^\n]", newGroup);
    clearBuffer();

    strcpy(guests[idx].name, newName);
    strcpy(guests[idx].phone, newPhone);
    strcpy(guests[idx].group, newGroup);

    saveGuests();
    printf("\nGuest G%03d updated successfully!\n", id);
}

// 5. Delete guest record
void deleteGuest() {
    if (guestCount == 0) {
        printf("\nNo guests registered yet.\n");
        return;
    }

    int id;
    printf("\n========== DELETE GUEST ==========\n");
    printf("Enter Guest ID number to delete (e.g., 1 for G001): ");
    if (scanf("%d", &id) != 1) {
        clearBuffer();
        printf("Invalid ID!\n");
        return;
    }
    clearBuffer();

    int idx = findGuestIndexByID(id);
    if (idx == -1) {
        printf("Guest G%03d not found.\n", id);
        return;
    }

    for (int i = idx; i < guestCount - 1; i++) {
        guests[i] = guests[i + 1];
    }
    guestCount--;
    saveGuests();
    printf("Guest G%03d deleted successfully!\n", id);
}

// 6. Assign Table (Manual or Auto-Assign keeping Family/Group together)
void assignTable() {
    if (guestCount == 0) {
        printf("\nNo guests registered yet.\n");
        return;
    }

    int mode;
    printf("\n========== ASSIGN TABLE ==========\n");
    printf("1. Assign Specific Guest to a Table\n");
    printf("2. Auto-Assign All Unassigned Guests (Keep Groups Together)\n");
    printf("Enter choice: ");
    if (scanf("%d", &mode) != 1) {
        clearBuffer();
        printf("Invalid input!\n");
        return;
    }
    clearBuffer();

    if (mode == 1) {
        int id, tableNo;
        printf("Enter Guest ID number (e.g., 1 for G001): ");
        if (scanf("%d", &id) != 1) {
            clearBuffer();
            printf("Invalid ID!\n");
            return;
        }
        clearBuffer();

        int idx = findGuestIndexByID(id);
        if (idx == -1) {
            printf("Guest G%03d not found.\n", id);
            return;
        }

        // Suggest table if family/group members are already seated
        for (int i = 0; i < guestCount; i++) {
            if (i != idx && guests[i].tableNo > 0 &&
                strcmp(guests[i].group, guests[idx].group) == 0) {
                int occ = getTableOccupancy(guests[i].tableNo);
                if (occ < SEATS_PER_TABLE) {
                    printf("Tip: Group '%s' is seated at Table %d (%d/%d seats used).\n",
                           guests[idx].group, guests[i].tableNo, occ, SEATS_PER_TABLE);
                    break;
                }
            }
        }

        printf("Enter Table Number (1 - %d): ", TOTAL_TABLES);
        if (scanf("%d", &tableNo) != 1 || tableNo < 1 || tableNo > TOTAL_TABLES) {
            clearBuffer();
            printf("Invalid table number! Must be between 1 and %d.\n", TOTAL_TABLES);
            return;
        }
        clearBuffer();

        int currentOccupancy = getTableOccupancy(tableNo);
        if (guests[idx].tableNo != tableNo && currentOccupancy >= SEATS_PER_TABLE) {
            printf("Table %d is full (%d/%d seats occupied)!\n",
                   tableNo, currentOccupancy, SEATS_PER_TABLE);
            return;
        }

        guests[idx].tableNo = tableNo;
        saveGuests();
        printf("Guest G%03d (%s) assigned to Table %d.\n", id, guests[idx].name, tableNo);

    } else if (mode == 2) {
        int assignedCount = 0;

        for (int i = 0; i < guestCount; i++) {
            if (guests[i].tableNo != 0) continue;

            int chosenTable = 0;

            // Step A: Look for a table where same group is already seated with available space
            for (int j = 0; j < guestCount; j++) {
                if (guests[j].tableNo > 0 &&
                    strcmp(guests[j].group, guests[i].group) == 0 &&
                    getTableOccupancy(guests[j].tableNo) < SEATS_PER_TABLE) {
                    chosenTable = guests[j].tableNo;
                    break;
                }
            }

            // Step B: Otherwise pick the first table with available seats
            if (chosenTable == 0) {
                for (int t = 1; t <= TOTAL_TABLES; t++) {
                    if (getTableOccupancy(t) < SEATS_PER_TABLE) {
                        chosenTable = t;
                        break;
                    }
                }
            }

            if (chosenTable > 0) {
                guests[i].tableNo = chosenTable;
                assignedCount++;
            } else {
                printf("All %d tables (%d seats) are full! Could not seat remaining guests.\n",
                       TOTAL_TABLES, TOTAL_TABLES * SEATS_PER_TABLE);
                break;
            }
        }

        saveGuests();
        printf("Auto-assignment complete! %d guest(s) assigned to tables.\n", assignedCount);
    } else {
        printf("Invalid choice!\n");
    }
}

// 7. Table Availability
void viewTableAvailability() {
    printf("\n========== TABLE AVAILABILITY ==========\n");
    printf("%-10s %-12s %-12s %-12s\n", "TABLE", "CAPACITY", "OCCUPIED", "AVAILABLE");
    printf("------------------------------------------------\n");

    int totalOccupied = 0;
    for (int t = 1; t <= TOTAL_TABLES; t++) {
        int occ = getTableOccupancy(t);
        int avail = SEATS_PER_TABLE - occ;
        totalOccupied += occ;
        printf("Table %-4d %-12d %-12d %-12d\n", t, SEATS_PER_TABLE, occ, avail);
    }

    int totalCap = TOTAL_TABLES * SEATS_PER_TABLE;
    printf("------------------------------------------------\n");
    printf("Total Seating Capacity : %d\n", totalCap);
    printf("Total Occupied Seats   : %d\n", totalOccupied);
    printf("Total Available Seats  : %d\n", totalCap - totalOccupied);
}

// 8. Guest Check-in
void guestCheckIn() {
    if (guestCount == 0) {
        printf("\nNo guests registered yet.\n");
        return;
    }

    int id;
    printf("\n========== GUEST CHECK-IN ==========\n");
    printf("Enter Guest ID number to check in (e.g., 1 for G001): ");
    if (scanf("%d", &id) != 1) {
        clearBuffer();
        printf("Invalid ID!\n");
        return;
    }
    clearBuffer();

    int idx = findGuestIndexByID(id);
    if (idx == -1) {
        printf("Guest G%03d not found.\n", id);
        return;
    }

    if (guests[idx].checkedIn == 1) {
        printf("Guest G%03d (%s) is already checked in!\n", id, guests[idx].name);
        return;
    }

    guests[idx].checkedIn = 1;
    saveGuests();

    printf("Check-in Successful!\n");
    printf("Guest : G%03d - %s (%s)\n", guests[idx].id, guests[idx].name, guests[idx].group);
    if (guests[idx].tableNo > 0)
        printf("Table : Table %d\n", guests[idx].tableNo);
    else
        printf("Table : Not yet assigned (Use Option 6 to assign a table)\n");
}

// 9. Event Report
void eventReport() {
    int arrived = 0, pending = 0, seated = 0, unseated = 0;

    for (int i = 0; i < guestCount; i++) {
        if (guests[i].checkedIn)
            arrived++;
        else
            pending++;

        if (guests[i].tableNo > 0)
            seated++;
        else
            unseated++;
    }

    int totalSeats = TOTAL_TABLES * SEATS_PER_TABLE;

    printf("\n============== EVENT REPORT ==============\n");
    printf("Total Registered Guests : %d\n", guestCount);
    printf("Arrived (Checked-In)    : %d\n", arrived);
    printf("Pending Arrival         : %d\n", pending);
    printf("------------------------------------------\n");
    printf("Total Tables            : %d (8 seats/table)\n", TOTAL_TABLES);
    printf("Total Seating Capacity  : %d\n", totalSeats);
    printf("Occupied Seats          : %d\n", seated);
    printf("Available Seats         : %d\n", totalSeats - seated);
    printf("Unassigned Guests       : %d\n", unseated);
    printf("==========================================\n");
}

int main() {
    int choice;
    loadGuests();

    printf("\n========================================\n");
    printf("               GUESTGRID                \n");
    printf("        EVENT MANAGEMENT SYSTEM         \n");
    printf("========================================\n");

    do {
        printf("\n1. Add New Guest");
        printf("\n2. View All Guests");
        printf("\n3. Search Guest");
        printf("\n4. Update Guest");
        printf("\n5. Delete Guest");
        printf("\n6. Assign Table");
        printf("\n7. Table Availability");
        printf("\n8. Guest Check-in");
        printf("\n9. Event Report");
        printf("\n10. Exit");
        printf("\n\nEnter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch (choice) {
            case 1: addGuest(); break;
            case 2: viewGuests(); break;
            case 3: searchGuest(); break;
            case 4: updateGuest(); break;
            case 5: deleteGuest(); break;
            case 6: assignTable(); break;
            case 7: viewTableAvailability(); break;
            case 8: guestCheckIn(); break;
            case 9: eventReport(); break;
            case 10: printf("\nThank you for using GuestGrid!\n"); break;
            default: printf("\nInvalid choice! Try again.\n");
        }
    } while (choice != 10);

    return 0;
}