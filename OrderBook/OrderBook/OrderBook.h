#pragma once
#include "Orders.h"


/**
 * @class OrderBook
 * @brief Implements logic of storage of bid and ask orders.
 * Provides access to orders of both type via GetBidOrders and GetAskOrders methods.
 * Modification of orders is available via Clear and HandleOrderUpdate method.
 */
class OrderBook
{
public:
	/**
	 * @brief Constructor.
	 * Creates full functional instance of order book
	 */
	OrderBook();

	/**
	 * @return             True if no bid or ask order is stored
	 */
	bool Empty() const;

	/**
	 * @brief Removes all bid and ask orders
	 */
	void Clear();

public:
	/**
	 * @return             Bid Orders
	 */
	const Orders& GetBidOrders() const { return bidOrders_; }

	/**
	 * @return             Ask Orders
	 */
	const Orders& GetAskOrders() const { return askOrders_; }

	/**
	 * @return             Max bid price if at least one bid order exists, else -1
	 */
	double GetBestBidPrice() const;

	/**
	 * @return             Min ask price if at least one bid order exists, else -1
	 */
	double GetBestAskPrice() const;

public:
	/**
	 * @brief Processes update of quantity for given price and order type
	 * If order with given price exists, updates its quantity, else adds new order.
	 * If quantity == 0, removes existing order
	 *
	 * @param price        order price
	 * @param quantity     order quantity
	 */
	void HandleOrderUpdate(const double price, const double quantity, const OrderType orderType);

private:
	Orders bidOrders_;
	Orders askOrders_;
};

