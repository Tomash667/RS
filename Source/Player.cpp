#include "Pch.h"
#include "Player.h"
#include "Item.h"


Player::Player() : hp(60.f), hpmax(100.f), food(33.f), foodmax(100.f), action(Action::NONE), anim(Animation::NONE), new_anim(Animation::STAND),
item_before(nullptr), weapon_index(-1)
{
	ItemSlot empty(nullptr, 0, 0);
	items.resize(10, empty);
}

uint Player::GetAmmoLeft(Item* item) const
{
	assert(item && item->type == Item::Type::WEAPON);
	uint count = 0;
	// currently there is only one type of ammo
	for(const ItemSlot& slot : items)
	{
		if(slot.item != nullptr && slot.item->type == Item::Type::AMMO)
			count += slot.count;
	}
	return count;
}

uint Player::AddItem(Item* item, uint count, uint ammo_count)
{
	assert(item && count != 0);
	if(item->IsStackable())
	{
		// first try to stack
		const uint empty_index = (uint)items.size();
		uint free_index = empty_index, index = 0;
		for(ItemSlot& slot : items)
		{
			if(!slot.item)
			{
				if(free_index == empty_index)
					free_index = index;
			}
			else if(slot.item == item)
			{
				if(item->max_stack != slot.count)
				{
					uint left = min(item->max_stack - slot.count, count);
					slot.count += left;
					count -= left;
					if(count == 0)
						break;
				}
			}
			++index;
		}

		// then insert in empty place
		if(free_index == empty_index || count == 0)
			return count;
		index = free_index;
		for(; index < empty_index; ++index)
		{
			ItemSlot& slot = items[index];
			if(!slot.item)
			{
				slot.item = item;
				slot.ammo_count = 0;
				slot.count = min(count, item->max_stack);
				count -= slot.count;
				if(count == 0)
					break;
			}
		}
	}
	else
	{
		// insert in first empty space
		for(ItemSlot& slot : items)
		{
			if(!slot.item)
			{
				slot.item = item;
				slot.count = 1;
				slot.ammo_count = ammo_count;
				--count;
				if(count == 0)
					break;
			}
		}
	}
	return count;
}
