#pragma once
#include "OrderBook.h"
#include <iostream>
#include <optional>

namespace ordbkfeatures
{
/**
 * @struct OrderBookFeatures
 * @brief Struct that stores features calculated using the order book
 */
struct OrderBookFeatures
{
	/**
	 * @brief Clears all stored values
	 */
	void Clear();

	// averageVolume = Sum of all orders quantities / number of all orders
	std::optional<double> averageVolume;
	// bidAverageVolume = Sum of bid orders quantities / number of bid orders
	std::optional<double> bidAverageVolume;
	// askAverageVolume = Sum of ask orders quantities / number of ask orders
	std::optional<double> askAverageVolume;

	// volumeWeightedAveragePrice = Sum of price * quantity for all orders / Sum of all orders quantities
	std::optional<double> volumeWeightedAveragePrice;
	// bidVolumeWeightedAveragePrice = Sum of price * quantity for bid orders / Sum of bid orders quantities
	std::optional<double> bidVolumeWeightedAveragePrice;
	// askVolumeWeightedAveragePrice = Sum of price * quantity for ask orders / Sum of ask orders quantities
	std::optional<double> askVolumeWeightedAveragePrice;

	// volumeWeightedAverageSquaredPrice = Sum of price^2 * quantity for all orders / Sum of all orders quantities
	std::optional<double> volumeWeightedAverageSquaredPrice;
	// bidVolumeWeightedAverageSquaredPrice = Sum of price^2 * quantity for bid orders / Sum of bid orders quantities
	std::optional<double> bidVolumeWeightedAverageSquaredPrice;
	// askVolumeWeightedAverageSquaredPrice = Sum of price^2 * quantity for ask orders / Sum of ask orders quantities
	std::optional<double> askVolumeWeightedAverageSquaredPrice;

	// volumeImbalance = (b - a) / (b + a)
	// where b = Sum of bid orders quantities, a = Sum of ask orders quantities
	std::optional<double> volumeImbalance;

	// Denote abs(price - mid price) as deviation
	// b = Sum of deviation * quantity for bid orders
	// a = Sum of deviation * quantity for ask orders
	// dollarImbalance = (b - a) / (b + a)
	std::optional<double> dollarImbalance;
};

std::ostream& operator<<(std::ostream& strm, const std::optional<double>& optValue);
std::ostream& operator<<(std::ostream& strm, const OrderBookFeatures& orderBookFeatures);

void CalculateOrderBookFeatures(const OrderBook& orderBook,
                                OrderBookFeatures& orderBookFeatures);
}
