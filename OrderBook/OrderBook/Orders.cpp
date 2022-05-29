#include "Orders.h"
#include <algorithm>
#include <cmath>

Orders::Orders(const OrderType orderType) : 
	orderType_(orderType)
{}

bool Orders::Empty() const
{
	return orders_.empty();
}

void Orders::Clear()
{
	orders_.clear();
}

size_t Orders::Size() const
{
	return orders_.size();
}

OrderType Orders::GetOrderType() const
{
	return orderType_;
}

void Orders::HandleOrderUpdate(const double price, const double quantity)
{
	const size_t priceHash = GetPriceCents(price);

	if (std::abs(quantity) <= 1e-6) {
		orders_.erase(priceHash);
	}
	else {
		orders_[priceHash] = quantity;
	}
}

double Orders::GetBestPrice() const
{
	if (orders_.empty()) {
		return -1.0;
	}

	return GetBestPriceCents() / priceHashMultiplier;
}

size_t Orders::GetBestPriceCents() const
{
	if (orders_.empty()) {
		return -1;
	}

	switch (orderType_)
	{
	case OrderType::BID:
		return orders_.rbegin()->first;
		break;
	case OrderType::ASK:
		return orders_.begin()->first;
		break;
	default:
		return -1;
	}
}

void Orders::ValidateOrdersToOtherSide(const Orders& otherSide)
{
	if (Empty() || otherSide.Empty()) {
		return;
	}

	switch (orderType_)
	{
	case OrderType::BID:
	{
		if (otherSide.GetOrderType() == OrderType::ASK &&
			orders_.rbegin()->first >= otherSide.GetBestPriceCents())
		{
			const auto lmbd = [&otherSide](const Order& order)
			{ return order.first >= otherSide.GetBestPriceCents(); };
			const auto it = std::find_if(orders_.begin(), orders_.end(), lmbd);
			orders_.erase(it, orders_.end());
		}
		break;
	}
	case OrderType::ASK:
	{
		if (otherSide.GetOrderType() == OrderType::BID &&
			otherSide.GetBestPriceCents() >= orders_.begin()->first)
		{
			const auto lmbd = [&otherSide](const Order& order)
			{ return order.first > otherSide.GetBestPriceCents(); };
			const auto it = std::find_if(orders_.begin(), orders_.end(), lmbd);
			orders_.erase(orders_.begin(), it);
		}
		break;
	}
	}
}

double Orders::GetPriceHashMultiplier()
{
	return priceHashMultiplier;
}

size_t Orders::GetPriceCents(const double price)
{
	return std::llround(priceHashMultiplier * price);
}
