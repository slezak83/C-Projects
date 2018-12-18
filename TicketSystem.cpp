/**
* File:		TicketSystem.cpp
* Author:	Team B
* Course:	PRG/410
* Created:	June 4, 2018
* Modified:	June 4, 2018
*
* Description:
* The implementation file for the CTicketSystem class.
*
*/

#include "stdafx.h"
#include "TicketSystem.h"

using std::cout;
using std::setw;
using std::ios;

// Constructor that sets the ticket price and preps the seating information.
CTicketSystem::CTicketSystem(const float& ticketPrice)
	: m_seats{}, m_TICKET_PRICE{ ticketPrice }, m_totalSales{0}, m_ROW_LENGTH { sizeof m_seats / sizeof m_seats[0] },
	m_COLUMN_LENGTH { sizeof m_seats[0] / sizeof m_seats[0][0] }
{
	// A simple way to fill the seats array with asterisks to make all seats available.
	std::fill(&m_seats[0][0], &m_seats[0][0] + (sizeof m_seats / sizeof m_seats[0][0]) + 1, '#');
}

// Generates and displays a visual of the seating map in its current state.
void CTicketSystem::printSeatMap() const
{
	// We use different sizes because the seats array is not a perfect square.
	const int ROW_LENGTH = getRowCount();
	const int COLUMN_LENGTH = getColumnCount();

	// Creates column header for seat map
	cout << "\n" << setw(6) << "|";
	for (int i = 0; i < COLUMN_LENGTH; i++)
	{
		cout << setw(3) << i + 1; // Labels the columns.
	}
	cout << "\n";

	// Creates the underline below the header for separation.
	for (int i = 0; i < COLUMN_LENGTH; i++)
	{
		cout << "____";
	}
	cout << "\n";

	// Outputs a table view of the seating arrangement.
	for (int i = 0; i < ROW_LENGTH; i++) // Outer loop to iterate down the rows.
	{
		// Labels the row headers on the left.
		cout << "R " << setiosflags(ios::left) << setw(3) << i + 1 << "|" << resetiosflags(ios::left);

		for (int j = 0; j < COLUMN_LENGTH; j++) // Inner loop to iterate across the columns.
		{
			cout << setw(3) << getSeatState(i, j); // Outputs seat value/state.
		}

		cout << "\n"; // Creates a new row.
	}
}

// Returns a structure containing all the seating statistics needed. 
const SeatingStatistics& CTicketSystem::getStats() const
{
	SeatingStatistics stats{};
	// We use different sizes because the seats array is not a perfect square.
	const int ROW_LENGTH = getRowCount();
	const int COLUMN_LENGTH = getColumnCount();

	for (int i = 0; i < ROW_LENGTH; i++) // Outer loop to iterate down the rows.
	{
		for (int j = 0; j < COLUMN_LENGTH; j++) // Inner loop to iterate across the columns.
		{
			// Tallies up the total and by row statistics of available and reserved seats.
			if (isSeatAvailable(i, j))
			{
				stats.availableTotal++;
				stats.availableByRow[i]++;
			}
			else
			{
				stats.reservedTotal++;
				stats.reservedByRow[i]++;
			}
		}
	}

	return stats;
}

// Gets the total price for a number of tickets, i. e. total overall sales or total for an individual sale.
const double& CTicketSystem::getTotalTicketPrice(const unsigned int& numOfTckets) const
{
	return getTicketPrice() * numOfTckets;
}

// Reserves a seat and returns whether the reservation was successful or false if it was occupied.
bool CTicketSystem::reserveSeat(const int& row, const int& column)
{
	if (getSeatState(row, column) == '#')
	{
		setSeatState(row, column, '*');
		return true;
	}

	return false;
}

// Checks to see if a seat is available or not.
const bool& CTicketSystem::isSeatAvailable(const int& row, const int& column) const
{
	return (getSeatState(row, column) == '#');
}