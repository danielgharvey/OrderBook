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

	Order() {}

	Order(int time, double price) : mStartTime(time), mPrice(price) {}
};

bool CompareOrders(const std::pair<unsigned, Order>& order_left, const std::pair<unsigned, Order>& order_right)
{
	return order_left.second.mPrice < order_right.second.mPrice;
}

class OrderBook
{
private:

	std::map<unsigned, Order> mOrders;	// A map from unique ID to Order.

	int mTotalTimeExposed;

	int mCurrentTimeStamp;

	double mRunningPriceTotal;

	double mHighestCurrentPrice;

	void InsertOrder(unsigned id, Order& order)
	{
		mHighestCurrentPrice = (order.mPrice > mHighestCurrentPrice) ? order.mPrice : mHighestCurrentPrice;

		mOrders[id] = order;
	}

	void EraseOrder(unsigned id, int newTimeStamp)
	{
		std::map<unsigned, Order>::iterator position = mOrders.find(id);
		double removed_price = position->second.mPrice;

		mOrders.erase(position);

		if (!(removed_price < mHighestCurrentPrice))
		{
			CalculateHighestCurrentPrice();
		}
	}

	void CalculateHighestCurrentPrice() // This method is quite expensive, O(mOrders.size^2)
	{
		mHighestCurrentPrice = mOrders.empty() ? 0.0 : std::max_element(mOrders.begin(), mOrders.end(), CompareOrders)->second.mPrice;
	}

	void UpdateRunningPriceTotalAndTime(int newTimeStamp)
	{
		if (!mOrders.empty())	// If there were existing orders in the period, add to the total.
		{
			mRunningPriceTotal += (newTimeStamp - mCurrentTimeStamp)*GetHighestCurrentPrice();
			mTotalTimeExposed += newTimeStamp - mCurrentTimeStamp;
		}

		mCurrentTimeStamp = newTimeStamp;
	}

public:

	OrderBook() : mTotalTimeExposed(0), mRunningPriceTotal(0.0), mHighestCurrentPrice(-DBL_MAX) {}

	double GetTimeWeightedAveragePrice()
	{
		return mRunningPriceTotal / (double)(mTotalTimeExposed);
	}

	double GetHighestCurrentPrice()
	{
		return mOrders.empty() 	?	std::numeric_limits<double>::quiet_NaN() : mHighestCurrentPrice;
	}

	std::stringstream& operator>>(std::stringstream& s)
	{
		int newTimeStamp; 	char type; 	unsigned id;	double price;
		s >> newTimeStamp;	s >> type;	s >> id;

		UpdateRunningPriceTotalAndTime(newTimeStamp);

		if (type == 'I')
		{
			s >> price;
			Order new_order(newTimeStamp, price);
			InsertOrder(id, new_order);
		}
		else
		{
			EraseOrder(id, newTimeStamp);
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
