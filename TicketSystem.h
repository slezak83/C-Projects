/**
* File:		TicketSystem.h
* Author:	Team B
* Course:	PRG/410
* Created:	June 4, 2018
* Modified:	June 4, 2018
*
* Description:
* The header file for the CTicketSystem class declarations.
*
*/

#pragma once

// Custom data type to better hold and share seating statistics.
struct SeatingStatistics
{
	unsigned short availableTotal;
	unsigned short reservedTotal;
	unsigned short availableByRow[10];
	unsigned short reservedByRow[10];
};

struct Seat
{
	unsigned short row;
	unsigned short column;

	// Overload operator== to work with a struct vector and std::find which  
	// validates if seat is not already selected in hand while not reserved.
	bool operator==(const Seat& seat) const
	{
		return (row == seat.row && column == seat.column);
	}
};

class CTicketSystem
{
private:
	char m_seats[10][9]; // Seating array that stores the state of our seating map.
	const float m_TICKET_PRICE; // Price of tickets that can be centrally changed.
	double m_totalSales;
	const int m_ROW_LENGTH;
	const int m_COLUMN_LENGTH;

public:
	explicit CTicketSystem(const float& ticketPrice);
	void setSeatState(const int& row, const int& column, const char& seatState) { m_seats[row][column] = seatState; }
	const char& getSeatState(const int& row, const int& column) const { return m_seats[row][column]; }
	const float& getTicketPrice() const { return m_TICKET_PRICE; }
	void setTotalSales(const double& salesAmount) { m_totalSales = salesAmount; }
	const double& getTotalSales() const { return m_totalSales; }
	const int& getRowCount() const { return m_ROW_LENGTH; }
	const int& getColumnCount() const { return m_COLUMN_LENGTH; }
	void printSeatMap() const;
	const SeatingStatistics& getStats() const;
	const double& getTotalTicketPrice(const unsigned int& numOfTckets) const;
	bool reserveSeat(const int& row, const int& column);
	const bool& isSeatAvailable(const int& row, const int& column) const;
};

