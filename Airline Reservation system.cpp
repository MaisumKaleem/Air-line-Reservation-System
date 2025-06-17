#include <iostream>    
#include <vector>       
#include <string>       
#include <fstream>     
#include <iomanip>      
#include <algorithm>    
#include <chrono>      
#include <random>       
#include <limits>      

// Using namespace std for brevity. In larger projects, it's often preferred to qualify names (e.g., std::cout).
using namespace std;

// --- Struct Definitions for Data Organization ---

/**
 * @brief Represents a single passenger's details.
 * This struct helps organize related information about a passenger.
 */
struct Passenger {
    string name;        // Passenger's name
    int age;            // Passenger's age
    int seatNumber;     // Assigned seat number
    string travelClass; // "Business Class" or "Economy Class"

    // Default constructor for Passenger struct
    Passenger() : name(""), age(0), seatNumber(0), travelClass("") {}

    // Parameterized constructor for easy initialization
    Passenger(string n, int a, int s, string tc) : name(n), age(a), seatNumber(s), travelClass(tc) {}

    // Overload the equality operator for comparing Passenger objects (useful for searching)
    bool operator==(const Passenger& other) const {
        return name == other.name && age == other.age && seatNumber == other.seatNumber && travelClass == other.travelClass;
    }
};

/**
 * @brief Represents a complete flight reservation.
 * This struct encapsulates all details for a booking, including multiple passengers.
 */
struct Reservation {
    string referenceNumber;     // Unique identifier for the reservation
    string destination;         // Flight destination
    string departureTime;       // Scheduled departure time
    double totalPrice;          // Total cost of the reservation
    double discountApplied;     // Total discount given
    vector<Passenger> passengers; // Dynamic array to store all passengers in this reservation
    int numAdults;              // Count of adult passengers
    int numKids;                // Count of kid passengers

    // Default constructor for Reservation struct
    Reservation() : referenceNumber(""), destination(""), departureTime(""), totalPrice(0.0), discountApplied(0.0), numAdults(0), numKids(0) {}

    // Overload the equality operator for comparing Reservation objects (useful for searching)
    bool operator==(const Reservation& other) const {
        return referenceNumber == other.referenceNumber; // Compare by reference number
    }
    
    // Overload the less-than operator for sorting Reservations by total price
    bool operator<(const Reservation& other) const {
        return totalPrice < other.totalPrice;
    }
     // Overload the less-than operator for sorting Reservations by reference number (for binary search)
    bool operator>(const Reservation& other) const {
        return referenceNumber > other.referenceNumber;
    }
};

// --- Global Variables (Reduced and Managed) ---
vector<Reservation> allReservations; // Stores all reservations made in the system
string currentDestination;           // Temporarily stores the chosen destination
string currentDepartureTime;         // Temporarily stores the chosen departure time

// --- Utility Functions ---

/**
 * @brief Clears the console screen.
 * Uses platform-specific commands.
 */
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * @brief Prompts the user to press any key to continue.
 * Used to pause execution and allow the user to read information.
 */
void pressAnyKey() {
    cout << "\n(Enter any key to continue...)\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer before getting input
    cin.get();
}

/**
 * @brief Generates a unique reference number for a reservation.
 * Uses a simple random string generation for demonstration.
 * @return A unique string reference number.
 */
string generateReferenceNumber() {
    static const char alphanumeric[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string refNum = "RB"; // Prefix
    // Generate a random string of 6 characters
    for (int i = 0; i < 6; ++i) {
        refNum += alphanumeric[rand() % (sizeof(alphanumeric) - 1)];
    }
    return refNum;
}

/**
 * @brief Displays the seat layout.
 */
void displaySeats() {
    cout << "\n____________________________________________________________________\n\n\n";
    cout << "                         01         02         03                    \n";
    cout << "                         04         05         06         BUSINESS   \n";
    cout << "                         07         08         09         CLASS      \n";
    cout << "                         10         11         12                    \n";
    cout << "                         13         14         15                    \n";
    cout << "                       ________     _____     ________               \n\n";
    cout << "                         16  17     18  19      21  22               \n";
    cout << "                         23  24     25  26      27  28               \n";
    cout << "                         29  30     31  32      33  34               \n";
    cout << "                         35  36     37  38      39  40               \n";
    cout << "                         41  42     43  44      45  46     ECONOMY   \n";
    cout << "                         47  48     49  50      51  52     CLASS     \n";
    cout << "                         53  54     55  56      57  58               \n";
    cout << "                         59  60     61  62      63  64               \n";
    cout << "                         65  66     67  68      69  70               \n";
    cout << "                         71  72     72  73      74  75               \n";
    cout << "                         76  77     78  79      80  81               \n\n";
    cout << "____________________________________________________________________\n\n";
    cout << "Choose seat (1-81)\n";
}

/**
 * @brief Gets passenger details and validates seat selection.
 * @param passengerNum The sequential number of the passenger (e.g., 1st, 2nd).
 * @param takenSeats A vector of already taken seat numbers for the current reservation.
 * @return A populated Passenger struct.
 */
Passenger getPassengerDetails(int passengerNum, const vector<int>& takenSeats) {
    Passenger p;
    cout << "\n\nEnter " << passengerNum << "st/nd/rd/th passenger name\n";
    // Clear buffer after previous numeric input before reading string
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, p.name);

    cout << "\n\nEnter " << passengerNum << "st/nd/rd/th passenger age\n";
    cin >> p.age;
    while (cin.fail() || p.age < 0) {
        cout << "\n\n***** E R R O R *****\nInvalid age. Please enter a valid non-negative number.\n*********************\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\n\nEnter " << passengerNum << "st/nd/rd/th passenger age\n";
        cin >> p.age;
    }

    displaySeats();
    int seat;
    bool seatTaken;
    do {
        seatTaken = false;
        cin >> seat;
        while (cin.fail() || seat > 81 || seat < 1) {
            cout << "\n\n***** E R R O R *****\nAvailable seats for this flight is 1-81 only\n*********************\nChoose available seat\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> seat;
        }

        // Check if the seat is already taken in the current reservation
        for (int s : takenSeats) {
            if (s == seat) {
                cout << "\n\n***** E R R O R *****\nSeat " << seat << " has been taken\n*********************\nChoose another seat\n";
                seatTaken = true;
                break;
            }
        }
    } while (seatTaken);

    p.seatNumber = seat;
    p.travelClass = (p.seatNumber >= 1 && p.seatNumber <= 15) ? "Business Class" : "Economy Class";

    clearScreen();
    return p;
}

/**
 * @brief Displays the boarding pass for a given reservation.
 * @param res The Reservation object to display.
 */
void displayBoardingPass(const Reservation& res) {
    clearScreen();
    cout << "\n\n\n___________________________________________________________________________________________\n\n";
    cout << "          RAUB AIRLINE             e-Boarding Pass         [Reference Number : " << res.referenceNumber << "]";
    cout << "\n__________________________________________________________________________________________\n\n";
    cout << "        PASSENGER & FLIGHT DETAILS\n";

    for (const auto& p : res.passengers) {
        cout << "\n        " << p.name;
        cout << "\n        Age " << p.age << "         Flight  RB370                   " << p.travelClass;
        cout << "\n        Seat " << p.seatNumber;
        cout << "\n        KUALA LUMPUR to " << res.destination << "     " << res.departureTime << endl;
    }
    cout << "\n        TOTAL AMOUNT : RM" << fixed << setprecision(2) << res.totalPrice;
    cout << "\n__________________________________________________________________________________________ \n";
    pressAnyKey();
}

// --- Reservation Logic Functions ---

/**
 * @brief Handles the manual reservation process.
 * Gathers passenger details, calculates price, applies coupons.
 * @return A new Reservation object.
 */
Reservation createManualReservation() {
    Reservation newReservation;
    newReservation.referenceNumber = generateReferenceNumber(); // Assign a unique reference number

    // Destination selection
    int mDest;
    double priceAdultBase = 1000.0, priceKidBase = 500.0, priceBusinessAdd = 0.0;
    
    cout << "\n========== M A N U A L   R E S E R V A T I O N ==========\n\n____________________________________________________\n";
    cout << "\nYou will depart at KUALA LUMPUR\n\nAvailable DESTINATION today :\n";
    cout << "  1. Jakarta\n  2. Bangkok\n  3. Makkah\n  4. Tokyo\n  5. Paris \n  6. London\n  7. Chicago\n____________________________________________________";
    cout << "\nChoose your destination\n";
    do {
        cin >> mDest;
        if (cin.fail() || mDest < 1 || mDest > 7) {
            cout << "\n\n***** E R R O R *****\nInvalid number chosen (Choose 1-7 only)\n*********************\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            switch (mDest) {
                case 1: newReservation.destination = "JAKARTA"; priceAdultBase = 1000; priceKidBase = 500; priceBusinessAdd = 500; break;
                case 2: newReservation.destination = "BANGKOK"; priceAdultBase = 1100; priceKidBase = 550; priceBusinessAdd = 600; break;
                case 3: newReservation.destination = "MAKKAH"; priceAdultBase = 1200; priceKidBase = 600; priceBusinessAdd = 700; break;
                case 4: newReservation.destination = "TOKYO"; priceAdultBase = 1300; priceKidBase = 650; priceBusinessAdd = 800; break;
                case 5: newReservation.destination = "PARIS"; priceAdultBase = 1400; priceKidBase = 700; priceBusinessAdd = 900; break;
                case 6: newReservation.destination = "LONDON"; priceAdultBase = 1500; priceKidBase = 750; priceBusinessAdd = 1000; break;
                case 7: newReservation.destination = "CHICAGO"; priceAdultBase = 1600; priceKidBase = 800; priceBusinessAdd = 1100; break;
            }
        }
    } while (mDest < 1 || mDest > 7 || cin.fail());
    clearScreen();

    // Number of tickets
    int numTickets;
    cout << "\n\nEnter number of tickets (maximum 4)\n";
    do {
        cin >> numTickets;
        if (cin.fail() || numTickets < 1 || numTickets > 4) {
            cout << "\n\n***** E R R O R *****\nInvalid number of tickets chosen (1-4 only)\n*********************\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (numTickets < 1 || numTickets > 4 || cin.fail());
    clearScreen();

    newReservation.passengers.reserve(numTickets); // Pre-allocate memory for passengers
    vector<int> takenSeats; // To keep track of seats taken in this reservation

    for (int i = 0; i < numTickets; ++i) {
        Passenger p = getPassengerDetails(i + 1, takenSeats);
        takenSeats.push_back(p.seatNumber); // Add the newly taken seat to the list
        newReservation.passengers.push_back(p);
        
        // Calculate price for this passenger
        double passengerPrice = (p.age >= 18) ? priceAdultBase : priceKidBase;
        if (p.travelClass == "Business Class") {
            passengerPrice += priceBusinessAdd;
        }
        newReservation.totalPrice += passengerPrice;

        if (p.age >= 18) newReservation.numAdults++;
        else newReservation.numKids++;
    }

    // Departure time
    char departureChoice;
    cout << "\n\nYour flight is Boeing-770 (RB 370)";
    cout << "\n\n A - 8.00AM\n B - 1.30PM\n C - 5.00PM\n D - 10.30PM\nChoose departure time\n";
    do {
        cin >> departureChoice;
        departureChoice = toupper(departureChoice); // Convert to uppercase for consistent comparison
        if (departureChoice == 'A') newReservation.departureTime = "8.00AM";
        else if (departureChoice == 'B') newReservation.departureTime = "1.30PM";
        else if (departureChoice == 'C') newReservation.departureTime = "5.00PM";
        else if (departureChoice == 'D') newReservation.departureTime = "10.30PM";
        else cout << "\nChoose (A / B / C / D) only\n";
    } while (departureChoice != 'A' && departureChoice != 'B' && departureChoice != 'C' && departureChoice != 'D');     
    clearScreen();

    // Coupon application
    int couponOption;
    string couponCode;
    double currentPriceBeforeCoupon = newReservation.totalPrice; // Store price before potential coupon discount

    do {
        cout << "\nTotal amount is RM" << fixed << setprecision(2) << newReservation.totalPrice;
        cout << "\nDo you want to apply any coupons? (Once)\n1. Yes\n2. No\n";
        cin >> couponOption;
        clearScreen();

        if (couponOption == 1) {
            bool couponApplied = false;
            do {
                cout << "\nEnter your coupon\n";
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer for string input
                getline(cin, couponCode);

                double discountPercent = 0.0;
                if (couponCode == "AEROAMEEN") {
                    discountPercent = 0.15;
                    cout << "\nSuccess, 15% off applied!";
                    couponApplied = true;
                } else if (couponCode == "CAPTAINAFIQ") {
                    discountPercent = 0.05;
                    cout << "\nSuccess, 5% off applied!";
                    couponApplied = true;
                } else if (couponCode == "COPILOTAMIR" || couponCode == "STEWARDFARIS") {
                    discountPercent = 0.10;
                    cout << "\nSuccess, 10% off applied!";
                    couponApplied = true;
                } else {
                    int couponMenuOption;
                    do {
                        cout << "\nInvalid coupon\n1. Apply coupon again\n2. Continue\n";
                        cin >> couponMenuOption;
                        if (cin.fail() || (couponMenuOption != 1 && couponMenuOption != 2)) {
                            cout << "\n\n***** E R R O R *****\nInvalid option chosen (1-Enter coupon again   2-Continue without coupon)\n*********************\n";
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        }
                    } while (couponMenuOption != 1 && couponMenuOption != 2 || cin.fail());
                    clearScreen();
                    if (couponMenuOption == 2) break; // Exit coupon loop if user chooses to continue
                }

                if (couponApplied) {
                    newReservation.discountApplied = newReservation.totalPrice * discountPercent;
                    newReservation.totalPrice -= newReservation.discountApplied;
                }
            } while (!couponApplied);
        } else if (couponOption != 2) { // If not 1 and not 2
            cout << "\n\n***** E R R O R *****\nInvalid option chosen (1-YES   2-NO)\n*********************\n";
        }
    } while (couponOption != 1 && couponOption != 2); // Loop until valid option (1 or 2) is chosen

    cout << "\n\nYou have completed your information and details\nTotal amount : RM" << fixed << setprecision(2) << newReservation.totalPrice << "\n";
    cout << "\n(Enter any key to CONFIRM PURCHASE)\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer before final get
    cin.get();
    
    cout << "\n\n===== P A Y M E N T   S U C C E S S F U L =====\n\n";
    cout << "(Enter any key to get your BOARDING PASS)\n";
    cin.get();

    return newReservation;
}

/**
 * @brief Handles the package reservation process (2 Adults, 2 Kids).
 * @param packageChoice The chosen package (A, B, C).
 * @return A new Reservation object.
 */
Reservation createPackageReservation(char packageChoice) {
    Reservation newReservation;
    newReservation.referenceNumber = generateReferenceNumber();
    newReservation.numAdults = 2;
    newReservation.numKids = 2;

    double basePriceAdult = 1000.0;
    double basePriceKid = 500.0;
    double packageDiscountPercentage = 0.0;

    // Set package specific details
    switch (toupper(packageChoice)) {
        case 'A': // KUALA LUMPUR to LONDON
            newReservation.destination = "LONDON";
            newReservation.totalPrice = (basePriceAdult + 500) * 2 + (basePriceKid + 250) * 2; // Original prices
            packageDiscountPercentage = 0.30; // 30% discount
            break;
        case 'B': // KUALA LUMPUR to TOKYO
            newReservation.destination = "TOKYO";
            newReservation.totalPrice = (basePriceAdult + 300) * 2 + (basePriceKid + 150) * 2; // Original prices
            packageDiscountPercentage = 0.20; // 20% discount
            break;
        case 'C': // KUALA LUMPUR to MAKKAH
            newReservation.destination = "MAKKAH";
            newReservation.totalPrice = (basePriceAdult + 200) * 2 + (basePriceKid + 100) * 2; // Original prices
            packageDiscountPercentage = 0.35; // 35% discount
            break;
        default:
            // Should not happen if input is validated in main menu
            return newReservation;
    }

    // Apply package discount
    newReservation.discountApplied = newReservation.totalPrice * packageDiscountPercentage;
    newReservation.totalPrice -= newReservation.discountApplied;

    clearScreen();

    // Collect passenger details (hardcoded for 2 adults, 2 kids with age validation)
    vector<int> takenSeats;
    int adultCount = 0;
    int kidCount = 0;

    // First passenger
    Passenger p1 = getPassengerDetails(1, takenSeats);
    takenSeats.push_back(p1.seatNumber);
    newReservation.passengers.push_back(p1);
    if (p1.age >= 18) adultCount++; else kidCount++;

    // Second passenger
    Passenger p2 = getPassengerDetails(2, takenSeats);
    takenSeats.push_back(p2.seatNumber);
    newReservation.passengers.push_back(p2);
    if (p2.age >= 18) adultCount++; else kidCount++;

    // Third passenger - enforce adult/kid balance for package
    Passenger p3;
    do {
        p3 = getPassengerDetails(3, takenSeats);
        if ((adultCount == 2 && p3.age >= 18) || (kidCount == 2 && p3.age < 18)) {
            cout << "\n\n\n\n_______________________________________________________________________________________________";
            cout << "\nThis package is for 2 adults and 2 kids only. Current adults: " << adultCount << ", kids: " << kidCount;
            cout << "\n3rd passenger age (" << p3.age << ") violates package rules.";
            cout << "\n_______________________________________________________________________________________________\n";
        }
    } while ((adultCount == 2 && p3.age >= 18) || (kidCount == 2 && p3.age < 18));
    takenSeats.push_back(p3.seatNumber);
    newReservation.passengers.push_back(p3);
    if (p3.age >= 18) adultCount++; else kidCount++;

    // Fourth passenger - enforce adult/kid balance for package
    Passenger p4;
    do {
        p4 = getPassengerDetails(4, takenSeats);
        if ((adultCount == 2 && p4.age >= 18) || (kidCount == 2 && p4.age < 18)) {
            cout << "\n\n\n\n_______________________________________________________________________________________________";
            cout << "\nThis package is for 2 adults and 2 kids only. Current adults: " << adultCount << ", kids: " << kidCount;
            cout << "\n4th passenger age (" << p4.age << ") violates package rules.";
            cout << "\n_______________________________________________________________________________________________\n";
        }
    } while ((adultCount == 2 && p4.age >= 18) || (kidCount == 2 && p4.age < 18));
    takenSeats.push_back(p4.seatNumber);
    newReservation.passengers.push_back(p4);
    if (p4.age >= 18) adultCount++; else kidCount++;

    // Departure time selection
    char departureChoice;
    cout << "\n\nYour flight is Boeing-770 (RB 370)";
    cout << "\n\n A - 8.00AM\n B - 1.30PM\n C - 5.00PM\n D - 10.30PM";
    cout << "\nChoose departure time\n";
    do {
        cin >> departureChoice;
        departureChoice = toupper(departureChoice);
        if (departureChoice == 'A') newReservation.departureTime = "8.00AM";
        else if (departureChoice == 'B') newReservation.departureTime = "1.30PM";
        else if (departureChoice == 'C') newReservation.departureTime = "5.00PM";
        else if (departureChoice == 'D') newReservation.departureTime = "10.30PM";
        else cout << "\n\n***** E R R O R *****\nChoose (A / B / C / D) only\n*********************\n"; 
    } while (departureChoice != 'A' && departureChoice != 'B' && departureChoice != 'C' && departureChoice != 'D');
    clearScreen();              

    cout << "\n\nYou have completed your information and details\nTotal amount : RM" << fixed << setprecision(2) << newReservation.totalPrice << "\n";
    cout << "\n(Enter any key to CONFIRM PURCHASE)\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer before final get
    cin.get();
    
    cout << "\n\n========== P A Y M E N T   S U C C E S S F U L ==========\n\n";
    cout << "(Enter any key to get your BOARDING PASS)\n";
    cin.get();

    return newReservation;
}

// --- File Handling Functions ---

/**
 * @brief Saves all reservations to a file.
 * Each reservation and its passengers are written in a structured text format.
 * @param reservations The vector of Reservation objects to save.
 * @param filename The name of the file to save to.
 */
void saveReservations(const vector<Reservation>& reservations, const string& filename = "reservations.txt") {
    ofstream outFile(filename); // Open file for writing

    if (!outFile.is_open()) {
        cerr << "Error: Could not open file " << filename << " for writing.\n";
        return;
    }

    for (const auto& res : reservations) {
        outFile << "REF:" << res.referenceNumber << "\n";
        outFile << "DEST:" << res.destination << "\n";
        outFile << "TIME:" << res.departureTime << "\n";
        outFile << "PRICE:" << fixed << setprecision(2) << res.totalPrice << "\n";
        outFile << "DISCOUNT:" << fixed << setprecision(2) << res.discountApplied << "\n";
        outFile << "NUM_ADULTS:" << res.numAdults << "\n";
        outFile << "NUM_KIDS:" << res.numKids << "\n";
        outFile << "NUM_PASSENGERS:" << res.passengers.size() << "\n";
        for (const auto& p : res.passengers) {
            outFile << "PASSENGER:" << p.name << "," << p.age << "," << p.seatNumber << "," << p.travelClass << "\n";
        }
        outFile << "END_RESERVATION\n"; // Marker for end of each reservation
    }

    outFile.close(); // Close the file
    // cout << "Reservations saved to " << filename << endl; // For debugging
}

/**
 * @brief Loads reservations from a file.
 * Reads data in the structured text format.
 * @param filename The name of the file to load from.
 * @return A vector of loaded Reservation objects.
 */
vector<Reservation> loadReservations(const string& filename = "reservations.txt") {
    vector<Reservation> loadedReservations;
    ifstream inFile(filename); // Open file for reading

    if (!inFile.is_open()) {
        // cerr << "Warning: Could not open file " << filename << " for reading. Starting with empty data.\n"; // For debugging
        return loadedReservations; // Return empty vector if file doesn't exist or can't be opened
    }

    string line;
    Reservation currentRes;
    while (getline(inFile, line)) {
        if (line.rfind("REF:", 0) == 0) { // Starts with "REF:"
            currentRes = Reservation(); // Reset for new reservation
            currentRes.referenceNumber = line.substr(4);
        } else if (line.rfind("DEST:", 0) == 0) {
            currentRes.destination = line.substr(5);
        } else if (line.rfind("TIME:", 0) == 0) {
            currentRes.departureTime = line.substr(5);
        } else if (line.rfind("PRICE:", 0) == 0) {
            currentRes.totalPrice = stod(line.substr(6)); // Convert string to double
        } else if (line.rfind("DISCOUNT:", 0) == 0) {
            currentRes.discountApplied = stod(line.substr(9));
        } else if (line.rfind("NUM_ADULTS:", 0) == 0) {
            currentRes.numAdults = stoi(line.substr(11));
        } else if (line.rfind("NUM_KIDS:", 0) == 0) {
            currentRes.numKids = stoi(line.substr(9));
        } else if (line.rfind("NUM_PASSENGERS:", 0) == 0) {
            // Not strictly needed for loading, as passenger data is read directly below
        } else if (line.rfind("PASSENGER:", 0) == 0) {
            string passengerData = line.substr(10);
            size_t pos1 = passengerData.find(',');
            string name = passengerData.substr(0, pos1);
            
            size_t pos2 = passengerData.find(',', pos1 + 1);
            int age = stoi(passengerData.substr(pos1 + 1, pos2 - (pos1 + 1)));
            
            size_t pos3 = passengerData.find(',', pos2 + 1);
            int seat = stoi(passengerData.substr(pos2 + 1, pos3 - (pos2 + 1)));
            
            string travelClass = passengerData.substr(pos3 + 1);
            
            currentRes.passengers.emplace_back(name, age, seat, travelClass);
        } else if (line == "END_RESERVATION") {
            loadedReservations.push_back(currentRes);
        }
    }

    inFile.close();
    // cout << "Reservations loaded from " << filename << endl; // For debugging
    return loadedReservations;
}

// --- Sorting Algorithms ---

/**
 * @brief Sorts a vector of reservations using Bubble Sort.
 * Compares adjacent elements and swaps them if they are in the wrong order.
 * Sorts by totalPrice in ascending order.
 * @param arr The vector of Reservation objects to sort.
 */
void bubbleSort(vector<Reservation>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            // Compare based on totalPrice
            if (arr[j].totalPrice > arr[j+1].totalPrice) {
                // Swap if the current element is greater than the next
                swap(arr[j], arr[j+1]);
            }
        }
    }
}

/**
 * @brief Merges two sorted sub-arrays into one.
 * Helper function for Merge Sort.
 * @param arr The main array containing the sub-arrays.
 * @param l The starting index of the first sub-array.
 * @param m The ending index of the first sub-array.
 * @param r The ending index of the second sub-array.
 */
void merge(vector<Reservation>& arr, int l, int m, int r) {
    int n1 = m - l + 1; // Size of left sub-array
    int n2 = r - m;     // Size of right sub-array

    // Create temporary arrays
    vector<Reservation> L(n1);
    vector<Reservation> R(n2);

    // Copy data to temporary arrays
    for (int i = 0; i < n1; ++i)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; ++j)
        R[j] = arr[m + 1 + j];

    // Merge the temporary arrays back into arr[l..r]
    int i = 0; // Initial index of first sub-array
    int j = 0; // Initial index of second sub-array
    int k = l; // Initial index of merged sub-array

    while (i < n1 && j < n2) {
        // Compare based on totalPrice
        if (L[i].totalPrice <= R[j].totalPrice) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy remaining elements of L[], if any
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copy remaining elements of R[], if any
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

/**
 * @brief Sorts a vector of reservations using Merge Sort.
 * A recursive, divide-and-conquer sorting algorithm.
 * Sorts by totalPrice in ascending order.
 * @param arr The vector of Reservation objects to sort.
 * @param l The starting index of the sub-array to sort.
 * @param r The ending index of the sub-array to sort.
 */
void mergeSort(vector<Reservation>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2; // Find the middle point
        mergeSort(arr, l, m);    // Sort first half
        mergeSort(arr, m + 1, r); // Sort second half
        merge(arr, l, m, r);     // Merge the sorted halves
    }
}

// Wrapper for mergeSort to be called easily
void mergeSort(vector<Reservation>& arr) {
    if (!arr.empty()) {
        mergeSort(arr, 0, arr.size() - 1);
    }
}

// --- Searching Algorithms ---

/**
 * @brief Searches for a reservation by reference number using Linear Search.
 * Checks each element sequentially until a match is found.
 * @param arr The vector of Reservation objects to search.
 * @param refNum The reference number to search for.
 * @return The index of the found reservation, or -1 if not found.
 */
int linearSearch(const vector<Reservation>& arr, const string& refNum) {
    for (int i = 0; i < arr.size(); ++i) {
        if (arr[i].referenceNumber == refNum) {
            return i; // Found at index i
        }
    }
    return -1; // Not found
}

/**
 * @brief Searches for a reservation by reference number using Binary Search.
 * Requires the array to be sorted by reference number.
 * Repeatedly divides the search interval in half.
 * @param arr The vector of Reservation objects (must be sorted by referenceNumber).
 * @param refNum The reference number to search for.
 * @return The index of the found reservation, or -1 if not found.
 */
int binarySearch(const vector<Reservation>& arr, const string& refNum) {
    int low = 0;
    int high = arr.size() - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (arr[mid].referenceNumber == refNum) {
            return mid; // Found
        }
        if (arr[mid].referenceNumber < refNum) {
            low = mid + 1; // Search in the right half
        } else {
            high = mid - 1; // Search in the left half
        }
    }
    return -1; // Not found
}

// --- Report Generation and DSA Integration ---

/**
 * @brief Generates and displays a report of all reservations.
 * Includes options for sorting and searching demonstration.
 */
void generateReport() {
    clearScreen();
    int totalTickets = 0;
    int totalAdults = 0;
    int totalKids = 0;
    double totalRevenue = 0.0;
    double totalDiscountGiven = 0.0;

    // Destination-wise ticket counts
    // Using a map to easily count tickets per destination without fixed variables
    map<string, int> destinationTicketCounts;

    for (const auto& res : allReservations) {
        totalTickets += res.passengers.size();
        totalAdults += res.numAdults;
        totalKids += res.numKids;
        totalRevenue += res.totalPrice;
        totalDiscountGiven += res.discountApplied;
        destinationTicketCounts[res.destination]++; // Increment count for each destination
    }

    cout << "\n\n========== R A U B   A I R L I N E   R E P O R T ==========";
    cout << "\n\nTotal Tickets Sold : " << totalTickets;
    cout << "\nTotal Adults         : " << totalAdults;
    cout << "\nTotal Kids           : " << totalKids;

    cout << "\n\nTotal tickets sold (by destination):";
    if (destinationTicketCounts.empty()) {
        cout << "\n- No tickets sold yet to any destination.";
    } else {
        for (const auto& pair : destinationTicketCounts) {
            cout << "\n- " << pair.first << " : " << pair.second << " reservations";
        }
    }

    cout << "\n\nTotal Discount Allowed : RM" << fixed << setprecision(2) << totalDiscountGiven;
    cout << "\nTotal Income           : RM" << fixed << setprecision(2) << totalRevenue;
    cout << "\nNET PROFIT             : RM" << fixed << setprecision(2) << (totalRevenue + totalDiscountGiven); // Profit is income + discount (since income is after discount)
    cout << "\n\n--- Data Structures and Algorithms Analysis ---";
    cout << "\n1. Sort Reservations by Total Price (Bubble Sort)";
    cout << "\n2. Sort Reservations by Total Price (Merge Sort)";
    cout << "\n3. Search Reservation by Reference Number (Linear Search)";
    cout << "\n4. Search Reservation by Reference Number (Binary Search)";
    cout << "\n5. View All Reservations";
    cout << "\n6. Back to Main Menu";
    cout << "\n\nChoose an option:\n";

    int reportChoice;
    cin >> reportChoice;
    clearScreen();

    // Make a copy of reservations for sorting/searching to avoid modifying the original list order if not desired
    // Or if the original order is needed for other report functions
    vector<Reservation> tempReservations = allReservations;
    string searchRefNum;
    int foundIndex;

    switch (reportChoice) {
        case 1: { // Bubble Sort
            if (tempReservations.empty()) {
                cout << "\nNo reservations to sort.\n";
                break;
            }
            cout << "\nPerforming Bubble Sort on reservations by total price...\n";
            auto start = chrono::high_resolution_clock::now();
            bubbleSort(tempReservations);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;
            cout << "Bubble Sort completed in: " << fixed << setprecision(6) << duration.count() << " seconds.\n";
            cout << "\nSorted Reservations (by Price):\n";
            for (const auto& res : tempReservations) {
                cout << "  Ref: " << res.referenceNumber << ", Dest: " << res.destination << ", Price: RM" << res.totalPrice << "\n";
            }
            break;
        }
        case 2: { // Merge Sort
            if (tempReservations.empty()) {
                cout << "\nNo reservations to sort.\n";
                break;
            }
            cout << "\nPerforming Merge Sort on reservations by total price...\n";
            auto start = chrono::high_resolution_clock::now();
            mergeSort(tempReservations); // Calls the wrapper
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;
            cout << "Merge Sort completed in: " << fixed << setprecision(6) << duration.count() << " seconds.\n";
            cout << "\nSorted Reservations (by Price):\n";
            for (const auto& res : tempReservations) {
                cout << "  Ref: " << res.referenceNumber << ", Dest: " << res.destination << ", Price: RM" << res.totalPrice << "\n";
            }
            break;
        }
        case 3: { // Linear Search
            if (allReservations.empty()) {
                cout << "\nNo reservations to search.\n";
                break;
            }
            cout << "\nEnter Reference Number to search (Linear Search):\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, searchRefNum);

            cout << "\nPerforming Linear Search...\n";
            auto start = chrono::high_resolution_clock::now();
            foundIndex = linearSearch(allReservations, searchRefNum);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;
            cout << "Linear Search completed in: " << fixed << setprecision(6) << duration.count() << " seconds.\n";

            if (foundIndex != -1) {
                cout << "Reservation found! Details:\n";
                displayBoardingPass(allReservations[foundIndex]); // Reuse display for found item
            } else {
                cout << "Reservation with Reference Number '" << searchRefNum << "' not found.\n";
            }
            break;
        }
        case 4: { // Binary Search
            if (allReservations.empty()) {
                cout << "\nNo reservations to search.\n";
                break;
            }
            cout << "\nEnter Reference Number to search (Binary Search):\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, searchRefNum);

            // Binary search requires sorted data. Sort a copy by reference number.
            vector<Reservation> sortedByRefNum = allReservations;
            cout << "\nSorting data for Binary Search...\n";
            sort(sortedByRefNum.begin(), sortedByRefNum.end(), [](const Reservation& a, const Reservation& b) {
                return a.referenceNumber < b.referenceNumber;
            });
            
            cout << "Performing Binary Search...\n";
            auto start = chrono::high_resolution_clock::now();
            foundIndex = binarySearch(sortedByRefNum, searchRefNum);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;
            cout << "Binary Search completed in: " << fixed << setprecision(6) << duration.count() << " seconds.\n";

            if (foundIndex != -1) {
                cout << "Reservation found! Details:\n";
                displayBoardingPass(sortedByRefNum[foundIndex]);
            } else {
                cout << "Reservation with Reference Number '" << searchRefNum << "' not found.\n";
            }
            break;
        }
        case 5: { // View All Reservations
            if (allReservations.empty()) {
                cout << "\nNo reservations to display.\n";
            } else {
                cout << "\n--- All Current Reservations ---\n";
                for (size_t i = 0; i < allReservations.size(); ++i) {
                    cout << "\nReservation " << i + 1 << ":\n";
                    displayBoardingPass(allReservations[i]); // Reuses display for individual items
                }
            }
            break;
        }
        case 6: // Back to Main Menu
            return;
        default:
            cout << "\nInvalid option. Please try again.\n";
            break;
    }
    pressAnyKey();
    clearScreen();
}

// --- Main Program Loop ---

int main() {
    srand(time(0)); // Seed the random number generator for reference IDs
    allReservations = loadReservations(); // Load existing reservations when program starts

    int choice1; // Main menu choice
    do {
        clearScreen();
        cout << "#############################################################################################\n";
        cout << "            * *\n";
        cout << "          * * * * * * * *\n";
        cout << "         * * * WELCOME TO AIRLINE        * * *\n";
        cout << "          * * * * RESERVATION SYSTEM     * * * *\n";
        cout << "           * * * *\n";
        cout << "#############################################################################################\n";
        cout << "\n\n===== M A I N   M E N U =====\n\n";
        cout << "  1. PACKAGES \n";
        cout << "  2. MANUAL RESERVATION\n";
        cout << "  3. Coupons\n";
        cout << "  4. Report & DSA Analysis\n"; // Renamed for clarity
        cout << "  5. Credits\n";
        cout << "  6. Exit\n";
        cout << "  ";

        cin >> choice1;
        while (cin.fail() || choice1 < 1 || choice1 > 6) {
            cout << "\n\n***** E R R O R *****\nInvalid option chosen (1-6 only)\n*********************\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  ";
            cin >> choice1;
        }
        clearScreen();

        if (choice1 == 1) { // PACKAGES
            char package;
            cout << "\n========== P A C K A G E S ==========\n\n____________________________________________________\n";
            cout << "\n A : KUALA LUMPUR to LONDON";
            cout << "\n     2 Adults 2 Kids             < DISCOUNT 30%";
            cout << "\n     RM3150 (After Discount) - Original price ~RM4500 (2x(1000+500) + 2x(500+250) for London)";
            cout << "\n\n B : KUALA LUMPUR to TOKYO";
            cout << "\n     2 Adults 2 Kids             < DISCOUNT 20%";
            cout << "\n     RM3120 (After Discount) - Original price ~RM3900 (2x(1000+300) + 2x(500+150) for Tokyo)";
            cout << "\n\n C : KUALA LUMPUR to MAKKAH";
            cout << "\n     2 Adults 2 Kids             < DISCOUNT 35%";
            cout << "\n     RM2340 (After Discount) - Original price ~RM3600 (2x(1000+200) + 2x(500+100) for Makkah)";
            cout << "\n____________________________________________________";
            cout << "\nChoose package (A / B / C). If NOT interested (M = Main Menu)\n";
            
            do {
                cin >> package;
                package = toupper(package);
                if (package == 'A' || package == 'B' || package == 'C') {
                    allReservations.push_back(createPackageReservation(package));
                    displayBoardingPass(allReservations.back()); // Display the last added reservation's boarding pass
                } else if (package != 'M') {
                    cout << "\n\n***** E R R O R *****\nChoose (A / B / C) for the packages OR (M = Main Menu) only\n*********************\n";
                }
            } while (package != 'A' && package != 'B' && package != 'C' && package != 'M');
        } else if (choice1 == 2) { // MANUAL RESERVATION
            allReservations.push_back(createManualReservation());
            displayBoardingPass(allReservations.back()); // Display the last added reservation's boarding pass
        } else if (choice1 == 3) { // COUPONS
            cout << "\n========== C O U P O N S ==========\n\nApply one of these coupons in Manual Reservation only\n\n";
            cout << "  - CAPTAINAFIQ   (5% OFF)\n";
            cout << "  - COPILOTAMIR   (10% OFF)\n";
            cout << "  - AEROAMEEN     (15% OFF)\n";
            cout << "  - STEWARDFARIS  (10% OFF)\n";
            pressAnyKey();
        } else if (choice1 == 4) { // REPORT & DSA ANALYSIS
            generateReport();
        } else if (choice1 == 5) { // CREDITS
            cout << "\n========== C R E D I T S ==========\n\nThis program is prepared by :\n\n";
            cout << "    1. Afiq Izzuddin Bin Mustapha\n";
            cout << "    2. Ahmad Faris Bin Ismail\n";
            cout << "    3. Muhammad Amir Iqbal Bin Mohd Tarmidzi\n";
            cout << "    4. Nur Ameerul Ameen Bin Nor Hassan\n";
            pressAnyKey();
        }
    } while (choice1 != 6); // EXIT

    saveReservations(allReservations); // Save all reservations before exiting
    cout << "\nThank you for using RAUB AIRLINE Reservation System. Goodbye!\n";
    return 0;
}



