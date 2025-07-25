# Car Park System

A console-based parking management system for efficiently tracking and managing vehicles in a parking facility.

## Features

- **Parking Status Display**: Visual grid showing available and occupied parking spots
- **Vehicle Entry Management**: Record detailed information about vehicles entering the parking lot
- **Vehicle Exit Processing**: Calculate parking fees based on duration and generate receipts
- **Search Functionality**: Look up parking history by owner name or license plate
- **Data Persistence**: All parking data is stored in files for reliable record-keeping

## Technical Details

- **Language**: C
- **Platform**: Windows
- **Interface**: Console-based with visual elements
- **Storage**: Text file-based data storage

## System Requirements

- Windows operating system
- C compiler (if building from source)

## Usage

### Main Menu

The system provides the following options:

1. **Display Parking Status**: View a visual representation of all parking spots
2. **Add Car**: Register a new vehicle entry with owner details
3. **Remove Car**: Process a vehicle exit and calculate parking fees
4. **Search Records**: Look up parking history by owner name or license plate
5. **Exit System**: Close the application

### Adding a Vehicle

When adding a vehicle, you'll need to provide:
- Owner's name
- License plate number
- Phone number (10 digits)
- Address
- Parking spot number (1-100)

### Removing a Vehicle

To remove a vehicle, simply enter the license plate number. The system will:
- Calculate the parking duration
- Determine the parking fee (Rs. 0.03 per second)
- Generate a receipt with entry/exit times and total fee
- Update the parking history

### Searching Records

Search options include:
- **By Owner Name**: View all vehicles and parking instances for a specific owner
- **By License Plate**: View all owners and parking instances for a specific vehicle

## Data Storage

The system uses two text files for data storage:
- `parking_spots.txt`: Current status of all parking spots
- `parking_history.txt`: Complete history of all parking transactions

## Building from Source

1. Clone the repository
2. Compile using your preferred C compiler:
   ```
   gcc Car_Park_System.c -o Car_Park_System.exe
   ```
3. Run the executable:
   ```
   Car_Park_System.exe
   ```
---
BALM.
