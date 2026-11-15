# GuestGrid – Event Guest and Table Seating Manager

A pure C console-based event management application built to streamline guest registration, group-aware table seating, guest check-in tracking, and real-time event reporting with persistent file storage.

## Features
- **Guest Management:** Register new guests with auto-generated IDs (`G001`, `G002`, ...), prevent duplicate phone numbers, view all records, search by ID or name, update guest details, and delete records.
- **Smart Table Allocation:** Manages 10 tables (8 seats per table, 80 total seats) with manual seating or automatic group-based assignment that keeps family and group members seated together.
- **Table Availability:** Real-time breakdown of total capacity, occupied seats, and available seats per table.
- **Guest Check-In:** Track guest arrivals (`Arrived` vs. `Pending`) at the venue entrance.
- **Event Reports:** Instant summary of total registered guests, arrival statistics, seat occupancy, and unassigned guests.
- **Persistent File Storage:** Automatically saves and loads all guest records using `guests.txt`.

## Project Structure
```text
GuestGrid/
├── guestgrid.c      # Main C source code
├── guestgrid.exe    # Compiled Windows executable
├── guests.txt       # Persistent pipe-delimited guest database
└── README.md        # Project documentation