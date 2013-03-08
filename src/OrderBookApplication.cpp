#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <map>
#include <float.h>

struct Order
{
	int mStartTime;
	double mPrice;

	Order() : mStartTime(0u), mPrice(0.0) {}

	Order(double price, int time) : mStartTime(time), mPrice(price) {}
};

bool CompareOrders(const std::pair<unsigned, Order>& order_left, const std::pair<unsigned, Order>& order_right)
{
	return order_left.second.mPrice < order_right.second.mPrice;
}

class OrderBook	/* A class that reads and stores orders and their time data. */
{
private:

	std::map<unsigned, Order> mOrders;

	double mHighestPrice;

	int mCurrentTimeStamp;

	double mRunningHighPriceTotal;

	int mTotalTimeHighPriceValid;

	void InsertOrder(Order& order, unsigned id)
	{
		mHighestPrice = (order.mPrice > mHighestPrice) ? order.mPrice : mHighestPrice;

		mOrders[id] = order;
	}

	void EraseOrder(unsigned id, int newTimeStamp)
	{
		std::map<unsigned, Order>::iterator position = mOrders.find(id);
		double removed_price = position->second.mPrice;

		mOrders.erase(position);

		if (!(removed_price < mHighestPrice))	// We try to avoid using CalculateHighestCurrentPrice whenever possible for efficiency.
		{
			CalculateHighestCurrentPrice();
		}
	}

	void CalculateHighestCurrentPrice() // This method is quite expensive, O(mOrders.size^2)
	{
		mHighestPrice = mOrders.empty() ? 0.0 : std::max_element(mOrders.begin(), mOrders.end(), CompareOrders)->second.mPrice;
	}

	void UpdateBeforeProcessingInstruction(int timeStamp)
	{
		if (!mOrders.empty())	// If there were existing orders in the period, add to the total.
		{
			mRunningHighPriceTotal += (timeStamp - mCurrentTimeStamp)*GetHighestPrice();
			mTotalTimeHighPriceValid += timeStamp - mCurrentTimeStamp;
		}

		mCurrentTimeStamp = timeStamp;
	}

public:

	OrderBook() :  mHighestPrice(-DBL_MAX), mCurrentTimeStamp(0), mTotalTimeHighPriceValid(0), mRunningHighPriceTotal(0.0) {}

	double GetTimeWeightedAveragePrice()
	{
		return mRunningHighPriceTotal / (double)(mTotalTimeHighPriceValid);
	}

	double GetHighestPrice()
	{
		return mOrders.empty() 	?	std::numeric_limits<double>::quiet_NaN() : mHighestPrice;
	}

	std::stringstream& operator>>(std::stringstream& s)
	{
		int timeStamp; 	char type; 	unsigned id;	double price;
		s >> timeStamp;	s >> type;	s >> id;

		UpdateBeforeProcessingInstruction(timeStamp);

		if (type == 'I')
		{
			s >> price;
			Order new_order(price, timeStamp);
			InsertOrder(new_order, id);
		}
		else
		{
			EraseOrder(id, timeStamp);
		}
		return s;
	}
};

std::stringstream& operator>>(std::stringstream& stream, OrderBook& book)
{
	book.operator >> (stream);
	return stream;
}

int main (int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: application_name data_file \n";
		return 0;
	}

	std::ifstream data_file(argv[1]);
	std::stringstream ss;
	ss << data_file.rdbuf();

	OrderBook order_book;

	while (!ss.eof())
	{
		ss >> order_book;
	}
	std::cout << "Time weighted average price is " << order_book.GetTimeWeightedAveragePrice() << "\n";

	return 0;
}
