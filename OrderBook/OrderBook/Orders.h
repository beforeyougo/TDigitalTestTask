#pragma once
#include <map>

enum class OrderType
{
	BID = 1,
	ASK = -1
};

// first - price in cents, second - quantity
using Order = std::pair<size_t, double>;

/**
 * @class Orders
 * @brief Implements logic of storage of orders of certain type.
 * Orders are stored inside std::map container with key = order price in cents and value = quantity.
 * Class provides const iterators for iterating over orders.
 * Modification of orders is available via Clear and HandleOrderUpdate method.
 */
class Orders
{
public:
	/**
	 * @brief Constructor.
	 * Creates full functional instance for given type of orders
	 *
	 * @param orderType    Type of stored orders: bid or ask
	 */
	Orders(const OrderType orderType = OrderType::BID);

	/**
	 * @return             True if no order is stored
	 */
	bool Empty() const;

	/**
	 * @brief Removes all orders
	 */
	void Clear();

	/**
	 * @return             Current number of stored orders
	 */
	size_t Size() const;

public:
	// key - price in cents, value - quantity
	using OrdersMap = std::map<std::size_t, double>;
	using const_iterator = OrdersMap::const_iterator;
	using const_reverse_iterator = OrdersMap::const_reverse_iterator;

	const_iterator begin() const { return orders_.begin(); }
	const_iterator end() const { return orders_.end(); }

	const_reverse_iterator rbegin() const { return orders_.rbegin(); }
	const_reverse_iterator rend() const { return orders_.rend(); }

public:
	/**
	 * @return             Type of stored orders
	 */
	OrderType GetOrderType() const;

	/**
	 * @brief Processes update of quantity for given price
	 * If order with given price exists, updates its quantity, else adds new order.
	 * If quantity == 0, removes existing order
	 *
	 * @param price        order price
	 * @param quantity     order quantity
	 */
	void HandleOrderUpdate(const double price, const double quantity);

	/**
	 * @brief Returns max price for bid orders and min price for ask orders
	 *
	 * @return             Best price if at least one order exists, else -1
	 */
	double GetBestPrice() const;

	/**
	 * @brief Returns max price in cents for bid orders and min price in cents for ask orders
	 *
	 * @return             Best price in cents if at least one order exists, else -1
	 */
	size_t GetBestPriceCents() const;

	/**
	 * @brief Validates orders for given other side orders.
	 * For bid orders removes all orders that >= ask best price
	 * For ask orders removes all orders that <= bid best price
	 *
	 * @param otherSide Orders of opposite type
	 */
	void ValidateOrdersToOtherSide(const Orders & otherSide);

public:
	/**
	 * @return             Multiplier used to convert price to cents (100.0)
	 */
	static double GetPriceHashMultiplier();

	/**
	 * @brief Converts price to cents
	 *
	 * @param price        Order price
	 * @return             Price in cents
	 */
	static size_t GetPriceCents(const double price);

private:
	const OrderType orderType_;
	OrdersMap orders_;
	static constexpr double priceHashMultiplier = 100.0;
};

