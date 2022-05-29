#include "OrderBookFeaturesCalculator.h"
#include <algorithm>
#include <cmath>

void AggregateOrders(const Orders& orders, const double midPriceCents, double& weightedSum,
                     double& weightedMidPriceDeviationsSum, double& weightedSquaredSum, double& sum)
{
	weightedSum = 0.0;
	weightedMidPriceDeviationsSum = 0.0;
	sum = 0.0;
	for (const Order& order : orders) {
		const double weightedPrice = order.first * order.second;
		weightedSum += weightedPrice;
		weightedSquaredSum += weightedPrice * order.first;
		weightedMidPriceDeviationsSum += std::abs(order.first - midPriceCents) * order.second;
		sum += order.second;
	}
}

void ordbkfeatures::CalculateOrderBookFeatures(const OrderBook& orderBook,
                                               OrderBookFeatures& orderBookFeatures)
{
	orderBookFeatures.Clear();

	const size_t bidSize = orderBook.GetBidOrders().Size(), askSize = orderBook.GetAskOrders().Size();
	if (!bidSize && !askSize) {
		return;
	}

	double midPriceCents = 0;
	if (bidSize && askSize) {
		midPriceCents = orderBook.GetBidOrders().GetBestPriceCents() +
                        orderBook.GetAskOrders().GetBestPriceCents();
		midPriceCents *= 0.5;
	}
	else if (bidSize) {
		midPriceCents = orderBook.GetBidOrders().GetBestPriceCents();
	}
	else if (askSize) {
		midPriceCents = orderBook.GetAskOrders().GetBestPriceCents();
	}

	double weightedBidDeviationsSum = 0.0;
	if (bidSize) {
		orderBookFeatures.bidAverageVolume = 0.0;
		orderBookFeatures.bidVolumeWeightedAveragePrice = 0.0;
		orderBookFeatures.bidVolumeWeightedAverageSquaredPrice = 0.0;
        AggregateOrders(orderBook.GetBidOrders(), midPriceCents,
                        orderBookFeatures.bidVolumeWeightedAveragePrice.value(),
                        weightedBidDeviationsSum, orderBookFeatures.bidVolumeWeightedAverageSquaredPrice.value(),
                        orderBookFeatures.bidAverageVolume.value());
	}

	double weightedAskDeviationsSum = 0.0;
	if (askSize) {
		orderBookFeatures.askAverageVolume = 0.0;
		orderBookFeatures.askVolumeWeightedAveragePrice = 0.0;
		orderBookFeatures.askVolumeWeightedAverageSquaredPrice = 0.0;
		AggregateOrders(orderBook.GetAskOrders(), midPriceCents,
                        orderBookFeatures.askVolumeWeightedAveragePrice.value(),
                        weightedAskDeviationsSum, orderBookFeatures.askVolumeWeightedAverageSquaredPrice.value(),
                        orderBookFeatures.askAverageVolume.value());
	}

	if (bidSize) {
		weightedBidDeviationsSum /= Orders::GetPriceHashMultiplier();
		orderBookFeatures.bidVolumeWeightedAveragePrice.value() /= Orders::GetPriceHashMultiplier();
		orderBookFeatures.bidVolumeWeightedAverageSquaredPrice.value() /= Orders::GetPriceHashMultiplier();
		orderBookFeatures.bidVolumeWeightedAverageSquaredPrice.value() /= Orders::GetPriceHashMultiplier();
	}
	
	if (askSize) {
		weightedAskDeviationsSum /= Orders::GetPriceHashMultiplier();
		orderBookFeatures.askVolumeWeightedAveragePrice.value() /= Orders::GetPriceHashMultiplier();
		orderBookFeatures.askVolumeWeightedAverageSquaredPrice.value() /= Orders::GetPriceHashMultiplier();
		orderBookFeatures.askVolumeWeightedAverageSquaredPrice.value() /= Orders::GetPriceHashMultiplier();
	}

	orderBookFeatures.averageVolume = orderBookFeatures.bidAverageVolume.value_or(0.0) +
                                      orderBookFeatures.askAverageVolume.value_or(0.0);

	orderBookFeatures.volumeWeightedAveragePrice = orderBookFeatures.bidVolumeWeightedAveragePrice.value_or(0.0) +
                                                   orderBookFeatures.askVolumeWeightedAveragePrice.value_or(0.0);

	orderBookFeatures.volumeWeightedAverageSquaredPrice = orderBookFeatures.bidVolumeWeightedAverageSquaredPrice.value_or(0.0) +
                                                   orderBookFeatures.askVolumeWeightedAverageSquaredPrice.value_or(0.0);

	orderBookFeatures.volumeWeightedAveragePrice.value() /= orderBookFeatures.averageVolume.value();
	orderBookFeatures.volumeWeightedAverageSquaredPrice.value() /= orderBookFeatures.averageVolume.value();

	orderBookFeatures.volumeImbalance = orderBookFeatures.bidAverageVolume.value_or(0.0) -
                                        orderBookFeatures.askAverageVolume.value_or(0.0);

	orderBookFeatures.volumeImbalance.value() /= orderBookFeatures.averageVolume.value();

	orderBookFeatures.dollarImbalance = weightedBidDeviationsSum - weightedAskDeviationsSum;
	orderBookFeatures.dollarImbalance.value() /= weightedBidDeviationsSum + weightedAskDeviationsSum;

	orderBookFeatures.averageVolume.value() /= (bidSize + askSize);

	if (bidSize) {
		orderBookFeatures.bidVolumeWeightedAveragePrice.value() /= orderBookFeatures.bidAverageVolume.value();
		orderBookFeatures.bidVolumeWeightedAverageSquaredPrice.value() /= orderBookFeatures.bidAverageVolume.value();
		orderBookFeatures.bidAverageVolume.value() /= bidSize;
	}

	if (askSize) {
		orderBookFeatures.askVolumeWeightedAveragePrice.value() /= orderBookFeatures.askAverageVolume.value();
		orderBookFeatures.askVolumeWeightedAverageSquaredPrice.value() /= orderBookFeatures.askAverageVolume.value();
		orderBookFeatures.askAverageVolume.value() /= askSize;
	}
}

std::ostream& ordbkfeatures::operator<<(std::ostream& strm, const std::optional<double>& optValue)
{
	if (optValue) {
		strm << optValue.value();
	}

	return strm;
}

std::ostream& ordbkfeatures::operator<<(std::ostream& strm, const OrderBookFeatures& orderBookFeatures)
{
	strm << orderBookFeatures.averageVolume << ",";
	strm << orderBookFeatures.bidAverageVolume << ",";
	strm << orderBookFeatures.askAverageVolume << ",";
	strm << orderBookFeatures.volumeWeightedAveragePrice << ",";
	strm << orderBookFeatures.bidVolumeWeightedAveragePrice << ",";
	strm << orderBookFeatures.askVolumeWeightedAveragePrice << ",";
	strm << orderBookFeatures.volumeWeightedAverageSquaredPrice << ",";
	strm << orderBookFeatures.bidVolumeWeightedAverageSquaredPrice << ",";
	strm << orderBookFeatures.askVolumeWeightedAverageSquaredPrice << ",";
	strm << orderBookFeatures.volumeImbalance << ",";
	strm << orderBookFeatures.dollarImbalance;
	return strm;
}

void ordbkfeatures::OrderBookFeatures::Clear()
{
	averageVolume.reset();
	bidAverageVolume.reset();
	askAverageVolume.reset();

	volumeWeightedAveragePrice.reset();
	bidVolumeWeightedAveragePrice.reset();
	askVolumeWeightedAveragePrice.reset();

	volumeWeightedAverageSquaredPrice.reset();
	bidVolumeWeightedAverageSquaredPrice.reset();
	askVolumeWeightedAverageSquaredPrice.reset();

	volumeImbalance.reset();
	dollarImbalance.reset();
}
