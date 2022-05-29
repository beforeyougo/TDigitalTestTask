#pragma once
#include "OrderBookFeaturesCalculator.h"
#include <fstream>

namespace ordtools
{
/**
 * @brief Processes all order initial states from sync shots file and all order updates from updates file
 * and logs best bid and ask prices for every unique timestamp in both file.
 * All updates happened before first sync shot are ignored.
 * If some updates have the same timestamp, only the last update is logged.
 * If updates following after the sync shot have the same timestamp as this sync shot,
 * order book for the last update is logged.
 *
 * If logFeatures is false, the resulting filestream has following structure:
 * TimeStamp,BestBidPrice,BestAskPrice
 * If logFeatures is true, structure is following
 * TimeStamp,BestBidPrice,BestAskPrice,Feature_1,...,Feature_n
 *
 * @param syncShots    Sync shots input file stream
 * @param updates      Updates input file stream
 * @param resutls      Results output file stream to where order book statistics is logged
 * @logFeatures        Optional, if true, features calculated by OrderBookFeatureCalculator are logged
 */
void ProcessSyncShotsAndUpdates(std::ifstream& syncShots, std::ifstream& updates,
	                            std::ofstream& results, const bool logFeatures = false);
}