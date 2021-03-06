/**
* File:		TheaterSeating.cpp
* Author:	Team B
* Course:	PRG/410
* Created:	June 4, 2018
* Modified:	June 4, 2018
* Version:	1.0
*
* Description:
* An application that enables the purchasing of theater tickets and the reserving of seats.
*
*/

#include "stdafx.h"
#include "TicketSystem.h"

using std::cout;
using std::cin;
using std::cerr;
using std::unique_ptr;
using std::make_unique;
using std::setprecision;
using std::fixed;
using std::setw;
using std::ios;
using std::vector;
using std::numeric_limits;
using std::streamsize;
// Using Bjarne Stroustrup's method/C++ Core Guidelines (Commit #357/#1018) for avoiding endl and flushing only when needed.
using std::flush;

// Prototypes
unsigned short displayMainScreen();
unsigned short displayPurchaseScreen(const unique_ptr<CTicketSystem>& ticketSystem);
unsigned short requestTicketQuantity(const SeatingStatistics& ticketInfo);
void requestSeatNumbers(const unique_ptr<CTicketSystem>& ticketSystem, const unsigned short& requestedAmount, vector<Seat>& seatSelections);
unsigned short displaySeatMapScreen(const unique_ptr<CTicketSystem> const& ticketSystem);
unsigned short displayStatsScreen(const unique_ptr<CTicketSystem> const& ticketSystem);
unsigned short returnToMainScreen();
void printLogo();
double formatCurrency(double amount);

int main()
{
	const auto ticketSystem = make_unique<CTicketSystem>(6.25F); // Ticket price will be $6.25.
	unsigned short choice{}; // Used for determining which screen to show.

	// The following handles the menu system and navigation.
	do
	{
		choice = displayMainScreen();
		switch (choice)
		{
		case 99:
			break; // Special code used to return to the main menu.
		case 1:
			choice = displayPurchaseScreen(ticketSystem);
			break;
		case 2:
			choice = displaySeatMapScreen(ticketSystem);
			break;
		case 3:
			choice = displayStatsScreen(ticketSystem);
			break;
		case 4: // Falls through to exit.
		default:
			// A controlled exit from the program.
			cout << "\nThe program will now exit. " << flush; // We flush buffer before system() screen I/O, which is required.
			system("pause");
			cout << "\n\n"; // Final blank line after system call for separation of operations.
			return 0;
		}
	}
	while (99 == choice); // Returns control back to main menu.
}

// Displays the main menu screen that provides access to different features.
unsigned short displayMainScreen()
{
	unsigned short selection{};

	do
	{
		printLogo();
		cout << "MAIN MENU:\n\n"
			<< "(1) - Purchase Ticket(s)\n"
			<< "(2) - View Seating Map\n"
			<< "(3) - View Statistics\n"
			<< "(4) - Quit\n"
			<< "\n"
			<< "Please enter a menu selection [1-4]: ";
		cin >> selection;

		// Ensures that we have a valid number without any characters.
		if (cin.fail())
		{
			cin.clear(); // Clears the fail state.
			selection = 0; // Resets our selection variable to remove junk data.
		}
		// Clears cin buffer and ensures that any extra values entered will not be automatically used when looping, i.e. "7 7 5".
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	while (selection < 1 || selection > 4);

	return selection;
}

// A screen that will take the user through the selection and purchasing process for the ticket(s).
unsigned short displayPurchaseScreen(const unique_ptr<CTicketSystem>& ticketSystem)
{
	unsigned short requestedAmount{};
	const SeatingStatistics ticketInfo = ticketSystem->getStats();
	vector<Seat> seatSelections;
	char confirmOrder{};
	double totalTicketPrice{};

	do
	{
		// STEP 1 of 4: We get the number of tickets that the customer requires while ensuring that the quantity requested is available.
		requestedAmount = requestTicketQuantity(ticketInfo);

		// STEP 2 of 4: We get the seat number/location for each of the requested tickets while ensuring that each seat is available.
		requestSeatNumbers(ticketSystem, requestedAmount, seatSelections);

		// STEP 3 of 4: We output the summary information of all the selections made plus the total cost.
		printLogo();
		cout << "TICKETING PURCHASE:\n\n"
			<< "Below is a summary of your order. Please review the details and ensure everything is correct.\n\n";

		cout << (seatSelections.size() > 1 ? "Seats" : "Seat") << " Selected:";
		for (int i = 0; i < seatSelections.size(); i++) {
			cout << "\nT" << i + 1 << ":[R" << seatSelections[i].row << ", C" << seatSelections[i].column << "]";
		}
		
		totalTicketPrice = ticketSystem->getTotalTicketPrice(requestedAmount);
		cout << "\n\nTickets Requested: " << requestedAmount << "\n"
			<< "Total Cost: " << fixed << setprecision(2) << "$" << formatCurrency(totalTicketPrice) << "\n\n";

		// STEP 4 of 4: Finally, We get the user's confirmation on whether the summary information is correct. If not, we start over.
		cout << "Would you like to place your order? [y/n]: ";
		cin >> confirmOrder;

		if ('Y' == toupper(confirmOrder))
		{
			for (const auto& seat : seatSelections)
			{
				ticketSystem->reserveSeat(seat.row - 1, seat.column - 1); // We now make the actual reservation.
			}
			
			// Update total overall sales in the system.
			ticketSystem->setTotalSales(ticketSystem->getTotalSales() + totalTicketPrice);

			printLogo();
			cout << "TICKETING PURCHASE:\n\n"
				<< "ORDER PROCESSED SUCCESSFULLY\n"
				<< "Your seats have now been reserved, thank you for your business.\n";
			ticketSystem->printSeatMap(); // Shows updated seating map.
		}
		else
		{
			seatSelections.clear();
			// We fix any errors before starting the process over again.
			if (cin.fail())
			{
				cin.clear(); // Clears the fail state.
			}
		}
		// Ensures that if any extra input was entered, it won't be automatically used when looping, i.e. "y y y" or "n n n".
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	while ('Y' != toupper(confirmOrder)); // Restart process only if user did not confirm order as correct.

	// Takes care of pausing and returning to main menu.
	return returnToMainScreen();
}

// This sub function/task takes care of step 1 of the purchase process wizard so that we don't have a have one large function.
unsigned short requestTicketQuantity(const SeatingStatistics& ticketInfo)
{
	unsigned short requestedAmount{};

	printLogo();
	cout << "TICKETING PURCHASE:\n\n"
		<< "Welcome to the ticket purchasing wizard. Just follow the prompts below to proceed.\n\n";

	cout << "How many tickets do you need [" << ticketInfo.availableTotal << " available]: ";
	cin >> requestedAmount;

	// Ensures that we have a valid amount and without any characters.
	while (cin.fail() || requestedAmount < 1 || requestedAmount > 90 || requestedAmount > ticketInfo.availableTotal)
	{
		cin.clear(); // Clears the fail state.
		cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Extracts and discards everything up to the new line in the buffer.
		cerr << "Error: Quantity requested is not available or you entered an invalid amount.\n\n";
		cout << "How many tickets do you need [" << ticketInfo.availableTotal << " available]: ";
		cin >> requestedAmount;
	}
	// Ensures that if more than one value is entered, none will be automatically used later, i.e. "1 2 3".
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	return requestedAmount;
}

// This sub function/task takes care of step 2 of the purchase process wizard so that we don't have a have one large function.
void requestSeatNumbers(const unique_ptr<CTicketSystem>& ticketSystem, const unsigned short& requestedAmount, vector<Seat>& seatSelections)
{
	Seat selectedSeat{};

	printLogo();
	cout << "TICKETING PURCHASE:\n\n"
		<< "Look at the seat selection map to see what seats are available for your choosing.\n\n";
		
	cout<< "Seat Selection Map - [#] available, [*] reserved\n";
	ticketSystem->printSeatMap();
	cout << "\n";

	for (int i = 0; i < requestedAmount; i++) // Loops for every seat requested.
	{
		cout << "Choose a seat for ticket " << i + 1 << " of " << requestedAmount << " (i.e. 2 5): ";
		cin >> selectedSeat.row >> selectedSeat.column;

		// Ensures that we have a valid seat number without any characters and that it is available and not reserved or selected.
		while (cin.fail() || selectedSeat.row < 1 || selectedSeat.row > 10 ||
			selectedSeat.column < 1 || selectedSeat.column > 9 ||
			!ticketSystem->isSeatAvailable(selectedSeat.row - 1, selectedSeat.column - 1) ||
			std::find(seatSelections.begin(), seatSelections.end(), selectedSeat) != seatSelections.end())
		{
			cin.clear(); // Clears the fail state.
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Extracts and discards everything up to the new line in the buffer.
			cerr << "Error: Seat requested is not available or you entered an invalid location.\n\n";
			cout << "Choose a seat for ticket " << i + 1 << " of " << requestedAmount << " (i.e. 2 5): ";
			cin >> selectedSeat.row >> selectedSeat.column;
		}
		// Ensures that if more than two values were entered, none will be automatically used when looping, i.e. "1 2 3".
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		// Stores seat selection only without reserving it yet.
		seatSelections.emplace_back(selectedSeat); 
	}
}

// A screen that will shows the seating map in its current state.
unsigned short displaySeatMapScreen(const unique_ptr<CTicketSystem> const& ticketSystem)
{
	printLogo();
	cout << "SEATING MAP:\n\n"
		<<"The current state of the seats available - [#] available, [*] reserved.\n";
	ticketSystem->printSeatMap();

	// Takes care of pausing and returning to main menu.
	return returnToMainScreen();
}

// Displays the statistics screen which contains all the seat statistics and total sales made so far.
unsigned short displayStatsScreen(const unique_ptr<CTicketSystem> const& ticketSystem)
{
	const SeatingStatistics seatStats = ticketSystem->getStats();
	const double totalSales = ticketSystem->getTotalTicketPrice(seatStats.reservedTotal);
	// Dynamically gets number of rows.
	const int ROW_LENGTH = ticketSystem->getRowCount();

	printLogo();
	cout << "STATISTICS:\n\n"
		<< "Below are the statistics for the seating availability and totals sales made.\n\n";

	cout << "Total Sales: " << fixed << setprecision(2) 
		<< "$" << formatCurrency(ticketSystem->getTotalSales()) << "\n";
	cout << "Seat Totals: " << seatStats.availableTotal << " available, " << seatStats.reservedTotal << " reserved\n\n";

	// Displays the by row statistics in a table like format.
	cout << setw(30) << "| Available | Reserved\n"
		<< "------------------------------\n";
	for (int i = 0; i < ROW_LENGTH; i++)
	{
		cout << "Row " << setiosflags(ios::left) << setw(2) << i + 1 << " | " << seatStats.availableByRow[i];
		cout << resetiosflags(ios::left) << setw(11) << "| " << seatStats.reservedByRow[i] << "\n";
	}

	// Takes care of pausing and returning to main menu.
	return returnToMainScreen();
}

// Handles returning back to the main menu.
unsigned short returnToMainScreen()
{
	cout << "\nReturning to the main menu. " << flush; // We flush buffer before system() screen I/O, which is required.
	system("pause");
	return 99; // Special code to return to main menu.
}

// Generates and displays the application's title logo.
void printLogo()
{
	system("cls"); // Clears the console window.
	
	// Outputs a text logo using a raw string. The lack of indentation is required.
	cout <<
R"(
 _______ _                _               _____            _   _             
|__   __| |              | |             / ____|          | | (_)            
   | |  | |__   ___  __ _| |_ ___ _ __  | (___   ___  __ _| |_ _ _ __   __ _ 
   | |  | '_ \ / _ \/ _` | __/ _ \ '__|  \___ \ / _ \/ _` | __| | '_ \ / _` |
   | |  | | | |  __/ (_| | ||  __/ |     ____) |  __/ (_| | |_| | | | | (_| |
   |_|  |_| |_|\___|\__,_|\__\___|_|    |_____/ \___|\__,_|\__|_|_| |_|\__, | v1.0
   By Team B - June 4, 2018                                             __/ |
                                                                       |___/

)";
}

// Helper function that formats a number into a currency format while ensuring correct rounding.
double formatCurrency(double amount)
{
	return round(amount * 100) / 100;
}