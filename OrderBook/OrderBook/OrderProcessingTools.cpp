#include "OrderProcessingTools.h"
#include <sstream>
#include <string>

struct LineInfo
{
	size_t time = 0;
	double price = 0.0;
	double quantity = 0.0;
	OrderType side = OrderType::BID;
};

LineInfo& ParseLine(const std::string& line, LineInfo& lineInfo)
{
	std::stringstream ss(line);
	std::string item;

	std::getline(ss, item, ',');
	std::stringstream(item) >> lineInfo.time;

	std::getline(ss, item, ',');
	lineInfo.side = static_cast<OrderType>(std::stoi(item));

	std::getline(ss, item, ',');
	lineInfo.price = std::stod(item);

	std::getline(ss, item, ',');
	lineInfo.quantity = std::stod(item);

	return lineInfo;
}

void LogCurrentBBO(std::ofstream& results, const OrderBook& orderBook, const size_t timeStamp,
                   const bool logFeatures = false)
{
	results << timeStamp << ",";
	double price = orderBook.GetBestBidPrice();
	if (price > 0) {
		results << price;
	}
	results << ",";
	price = orderBook.GetBestAskPrice();
	if (price > 0) {
		results << price;
	}

	if (logFeatures) {
		results << ",";
		static ordbkfeatures::OrderBookFeatures orderBookFeatures;
		ordbkfeatures::CalculateOrderBookFeatures(orderBook, orderBookFeatures);
		results << orderBookFeatures;
	}

	results << std::endl;
}

void ProcessSyncShotsUntillCurrentUpdate(std::ifstream& syncShots, std::ifstream& updates,
                                         std::ofstream& results, OrderBook& orderBook,
                                         LineInfo& syncShotLineInfo,
                                         const LineInfo& updateLineInfo,
                                         const bool logFeatures = false)
{
	// Clear orderBook before initializing it from the current sync shot
	orderBook.Clear();

	// Handle the current sync shot that was already parse before
	// We don't need to check whether the sync shots file is not over here
	// because if it had been over, the cycle would've been broken
	orderBook.HandleOrderUpdate(syncShotLineInfo.price, syncShotLineInfo.quantity, syncShotLineInfo.side);
	size_t prevSyncShotTime = syncShotLineInfo.time;
	static std::string line;

	// We iterate over syncshots until we get synchot that happened after the current update
	// If the current syncshot and the cuurrent update happened at the same timestamp,
	// we parse the sync shot first
	// If the updates file is over, we iterate till the end of the sync shots file
	while (std::getline(syncShots, line) &&
          (ParseLine(line, syncShotLineInfo).time <= updateLineInfo.time || !updates))
	{
		if (prevSyncShotTime < syncShotLineInfo.time) {
			// It means that the syncshot is over
			// But the cycle is not broken because next syncshot happened before the current update
			// Hence, we need to log bbo here
			LogCurrentBBO(results, orderBook, prevSyncShotTime, logFeatures);
			orderBook.Clear();
		}
		orderBook.HandleOrderUpdate(syncShotLineInfo.price, syncShotLineInfo.quantity, syncShotLineInfo.side);
		prevSyncShotTime = syncShotLineInfo.time;
	}

	// We can't log bbo until we make sure that the current update didn't happen
	// at the same time as previous sync shot
	if (updateLineInfo.time > prevSyncShotTime || !updates) {
		LogCurrentBBO(results, orderBook, prevSyncShotTime, logFeatures);
	}
}

void ProcessUpdatesUntillCurrentSyncShot(std::ifstream& syncShots, std::ifstream& updates,
                                         std::ofstream& results, OrderBook& orderBook,
	                                     const LineInfo& syncShotLineInfo,
	                                     LineInfo& updateLineInfo,
                                         const bool logFeatures = false)
{
	// Handle the current update that was already parse before
	orderBook.HandleOrderUpdate(updateLineInfo.price, updateLineInfo.quantity, updateLineInfo.side);

	size_t prevUpdateTime = updateLineInfo.time;
	static std::string line;

	// We iterate over updates until we get the update that happened
	// after or at the sime time as the current syncshot
	// If the sync shots file is over, we iterate till the end of the updates file
	while (std::getline(updates, line) &&
           (ParseLine(line, updateLineInfo).time < syncShotLineInfo.time || !syncShots))
	{
		if (prevUpdateTime < updateLineInfo.time) {
			LogCurrentBBO(results, orderBook, prevUpdateTime, logFeatures);
		}
		orderBook.HandleOrderUpdate(updateLineInfo.price, updateLineInfo.quantity, updateLineInfo.side);
		prevUpdateTime = updateLineInfo.time;
	}

	// When the cycle is over we need to log one more update
	LogCurrentBBO(results, orderBook, prevUpdateTime, logFeatures);
}

void ordtools::ProcessSyncShotsAndUpdates(std::ifstream& syncShots, std::ifstream& updates,
                                          std::ofstream& results, const bool logFeatures)
{
	// Skip columns
	static std::string line;
	std::getline(syncShots, line);
	std::getline(updates, line);

	OrderBook orderBook;
	LineInfo syncShotLineInfo, updateLineInfo;

	// Get first timestamp from sync shots
	std::getline(syncShots, line);
	ParseLine(line, syncShotLineInfo);

	// Skip all updates that happened before first sync shot
	while (std::getline(updates, line) &&
           ParseLine(line, updateLineInfo).time < syncShotLineInfo.time)
	{}

	results << "TimeStamp,BestBid,BestAsk";
	if (logFeatures) {
		results << ",AverageVolume,BidAverageVolume,AskAverageVolume,"
                   "VolumeWeightedAveragePrice,BidVolumeWeightedAveragePrice,AskVolumeWeightedAveragePrice,"
                   "VolumeWeightedAverageSquaredPrice,BidVolumeWeightedAverageSquaredPrice,AskVolumeWeightedAverageSquaredPrice,"
                   "VolumeImbalance,DollarImbalance";
	}
	results << std::endl;

	while (syncShots || updates)
	{
		ProcessSyncShotsUntillCurrentUpdate(syncShots, updates, results, orderBook,
                                            syncShotLineInfo, updateLineInfo, logFeatures);

		// If the updates file is over, there is no current update
		if (!updates) {
			continue;
		}

		ProcessUpdatesUntillCurrentSyncShot(syncShots, updates, results, orderBook,
                                            syncShotLineInfo, updateLineInfo, logFeatures);
	}
}