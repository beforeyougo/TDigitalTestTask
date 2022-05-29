#include "OrderBook.h"

OrderBook::OrderBook():
	bidOrders_(OrderType::BID),
	askOrders_(OrderType::ASK)
{}

bool OrderBook::Empty() const
{
	return bidOrders_.Empty() && askOrders_.Empty();
}

void OrderBook::Clear()
{
	bidOrders_.Clear();
	askOrders_.Clear();
}

double OrderBook::GetBestBidPrice() const
{
	return bidOrders_.GetBestPrice();
}

double OrderBook::GetBestAskPrice() const
{
	return askOrders_.GetBestPrice();
}

void OrderBook::HandleOrderUpdate(const double price, const double quantity, const OrderType orderType)
{
	switch (orderType)
	{
	case OrderType::BID:
		bidOrders_.HandleOrderUpdate(price, quantity);
		askOrders_.ValidateOrdersToOtherSide(bidOrders_);
		break;
	case OrderType::ASK:
		askOrders_.HandleOrderUpdate(price, quantity);
		bidOrders_.ValidateOrdersToOtherSide(askOrders_);
		break;
	}
}
