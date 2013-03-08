/*
 * TestOrderBookApplication.hpp
 *
 *  Created on: 8 Mar 2013
 *      Author: chaste
 */

#ifndef TESTORDERBOOKAPPLICATION_HPP_
#define TESTORDERBOOKAPPLICATION_HPP_

#include <cmath>

#include <cxxtest/TestSuite.h>
#include "OrderBookApplication.cpp"

class TestOrderBookApplication : public CxxTest::TestSuite
{
public:

	void TestSetupDefaultOrder()
	{
		Order new_order;
		TS_ASSERT_DELTA(new_order.mPrice, 0.0, 1e-4);
		TS_ASSERT_EQUALS(new_order.mStartTime, 0u);
	}

	void TestSetupOrderUsingConstructor()
	{
		Order new_order(3.6, 1000);
		TS_ASSERT_DELTA(new_order.mPrice, 3.6, 1e-4);
		TS_ASSERT_EQUALS(new_order.mStartTime, 1000u);
	}

	void TestCompareOrders()
	{
		Order big_order(100.0, 1000);
		Order small_order(0.1, 200);

		std::pair<unsigned, Order> big_order_pair(0u, big_order);
		std::pair<unsigned, Order> small_order_pair(1u, small_order);

		TS_ASSERT(CompareOrders(small_order_pair, big_order_pair));
		TS_ASSERT(!CompareOrders(big_order_pair, small_order_pair));

		Order same_order(0.1, 200);
		std::pair<unsigned, Order> same_order_pair(1u, same_order);

		// Should always return false as we use strict inequality.
		TS_ASSERT(!CompareOrders(same_order_pair, small_order_pair));
	}

	void TestConstructOrderBook()
	{
		OrderBook book;

		// Make sure we return nan when the order book is empty
		TS_ASSERT(std::isnan(book.GetHighestPrice()));
		TS_ASSERT(book.mOrders.empty());

		TS_ASSERT_EQUALS(book.mTotalTimeHighPriceValid, 0);

		TS_ASSERT_EQUALS(book.mCurrentTimeStamp, 0);

		TS_ASSERT_DELTA(book.mRunningHighPriceTotal, 0.0, 1e-4);

		TS_ASSERT_DELTA(book.mHighestPrice, -DBL_MAX, 1e-4);

	}

	void TestInsertAndEraseOrders()
	{
		OrderBook book;

		// Make sure we return nan when the order book is empty
		TS_ASSERT(std::isnan(book.GetHighestPrice()));

		Order new_order(10.0, 10);
		book.InsertOrder(new_order, 10);

		TS_ASSERT_DELTA(book.GetHighestPrice(), 10.0, 1e-4);

		Order new_order2(13.0, 11);
		book.InsertOrder(new_order2, 11);

		TS_ASSERT_DELTA(book.GetHighestPrice(), 13.0, 1e-4);

		book.EraseOrder(10, 50);

		TS_ASSERT_EQUALS(book.mOrders.size(), 1u);
		TS_ASSERT_DELTA(book.GetHighestPrice(), 13.0, 1e-4);

		book.EraseOrder(11, 55);

		TS_ASSERT(book.mOrders.empty());
		TS_ASSERT(std::isnan(book.GetHighestPrice()));

		book.InsertOrder(new_order, 10);
		book.InsertOrder(new_order2, 11);

		book.EraseOrder(11, 55);

		TS_ASSERT_DELTA(book.GetHighestPrice(), 10.0, 1e-4);
	}

	void TestInputOperator()
	{
		std::ifstream data_file("test_data.txt");
		std::stringstream ss;
		ss << data_file.rdbuf();

		OrderBook order_book;

		ss >> order_book;

		TS_ASSERT_EQUALS(order_book.mOrders.size(), 1u);
		TS_ASSERT_DELTA(order_book.GetHighestPrice(), 10.0, 1e-4);

		TS_ASSERT(std::isnan(order_book.GetTimeWeightedAveragePrice()));

		ss >> order_book;

		TS_ASSERT_EQUALS(order_book.mOrders.size(), 2u);
		TS_ASSERT_DELTA(order_book.GetHighestPrice(), 13.0, 1e-4);

		TS_ASSERT_DELTA(order_book.GetTimeWeightedAveragePrice(), 10.0, 1e-4);

		ss >> order_book;

		TS_ASSERT_EQUALS(order_book.mOrders.size(), 3u);
		TS_ASSERT_DELTA(order_book.GetHighestPrice(), 13.0, 1e-4);

		TS_ASSERT_DELTA(order_book.GetTimeWeightedAveragePrice(), 10.5, 1e-4);

		ss >> order_book;

		TS_ASSERT_EQUALS(order_book.mOrders.size(), 2u);
		TS_ASSERT_DELTA(order_book.GetHighestPrice(), 13.0, 1e-4);

		TS_ASSERT_DELTA(order_book.GetTimeWeightedAveragePrice(), 10.857143, 1e-4);

		ss >> order_book;

		TS_ASSERT_EQUALS(order_book.mOrders.size(), 1u);
		TS_ASSERT_DELTA(order_book.GetHighestPrice(), 10.0, 1e-4);

		TS_ASSERT_DELTA(order_book.GetTimeWeightedAveragePrice(), 11.0, 1e-4);

		ss >> order_book;

		TS_ASSERT_EQUALS(order_book.mOrders.size(), 0u);
		TS_ASSERT(std::isnan(order_book.GetHighestPrice()));

		TS_ASSERT_DELTA(order_book.GetTimeWeightedAveragePrice(), 10.5, 1e-4);
	}
};

#endif /* TESTORDERBOOKAPPLICATION_HPP_ */
