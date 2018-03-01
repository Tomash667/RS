#include "Pch.h"
#include "Player.h"
#include "Item.h"
#include <InputManager.h>
#include "Level.h"
#include <SceneNode.h>
#include <MeshInstance.h>
#include "GroundItem.h"


Player::Player(InputManager* input, Level* level) : input(input), level(level), hp(60.f), hpmax(100.f), food(33.f), foodmax(100.f), action(Action::NONE),
anim(Animation::NONE), new_anim(Animation::STAND), item_before(nullptr), weapon_index(-1)
{
	ItemSlot empty(nullptr, 0, 0);
	items.resize(10, empty);
}

bool Player::HaveAmmoLeft(Item* item) const
{
	assert(item && item->type == Item::Type::WEAPON);
	// currently there is only one type of ammo
	for(const ItemSlot& slot : items)
	{
		if(slot.item != nullptr && slot.item->type == Item::Type::AMMO)
			return true;
	}
	return false;
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

void Player::DropItem(uint index)
{
	assert(index < items.size() && items[index].item);
	assert(action == Action::NONE);
	action = Action::DROP;
	action_state = 0;
	action_var = index;
	new_anim = Animation::ACTION;
	node->GetMeshInstance()->Play("wyrzuc", 0, 0);
}

void Player::UseItem(uint index)
{
	assert(index < items.size() && items[index].item);
	assert(action == Action::NONE);
	ItemSlot& slot = items[index];
	switch(slot.item->type)
	{
	case Item::Type::WEAPON:
		if(index == weapon_index)
		{
			// unequip
			weapon_index = -1;
		}
		else
		{
			// equip
			weapon_index = index;
		}
		break;
	case Item::Type::FOOD:
		action = Action::CONSUME;
		action_state = 0;
		action_var = index;
		new_anim = Animation::ACTION;
		node->GetMeshInstance()->Play("je", PLAY_ONCE, 1);
		break;
	case Item::Type::AMMO:
		if(CanReload())
			StartReload(index);
		break;
	}
}

bool Player::CanReload() const
{
	if(action == Action::NONE && HaveWeapon())
	{
		const ItemSlot& slot = items[weapon_index];
		return slot.ammo_count != slot.item->count;
	}
	else
		return false;
}

void Player::Update(float dt)
{
	MeshInstance* inst = node->GetMeshInstance();

	if(action == Action::NONE)
	{
		// check items before player
		const float pick_range = 2.f;
		float best_range = pick_range;
		GroundItem* best_item = nullptr;
		for(GroundItem* item : level->items)
		{
			float dist = Vec3::Distance2d(node->pos, item->node->pos);
			if(dist < best_range)
			{
				float angle = AngleDiff(Clip(node->rot + PI / 2), Clip(-Vec3::Angle2d(node->pos, item->node->pos)));
				if(angle < PI / 4)
				{
					best_item = item;
					best_range = dist;
				}
			}
		}
		if(item_before && item_before != best_item)
		{
			item_before->node->tint = Vec3::One;
			item_before = nullptr;
		}
		if(best_item && best_item != item_before)
		{
			item_before = best_item;
			best_item->node->tint = Vec3(2, 2, 2);
		}

		if(item_before && input->Pressed(Key::E))
		{
			action = Action::PICKUP;
			action_state = 0;
			new_anim = Animation::ACTION;
			inst->Play("podnosi", 0, 0);
		}
	}
	else
	{
		switch(action)
		{
		case Action::PICKUP:
			{
				if(action_state == 0)
				{
					float speed = 3.f * dt;
					float expected_rot = Clip(-Vec3::Angle2d(node->pos, item_before->node->pos) - PI / 2);
					float dif = AngleDiff(node->rot, expected_rot);
					if(dif < speed)
					{
						node->rot = expected_rot;
						action_state = 1;
					}
					else
					{
						float arc = ShortestArc(node->rot, expected_rot);
						node->rot = Clip(node->rot + Sign(arc) * speed);
					}
				}

				if(action_state != 2 && inst->GetProgress(0) >= 19.f / 34)
				{
					uint left = AddItem(item_before->item, item_before->count, item_before->ammo_count);
					if(left == 0)
					{
						level->RemoveItem(item_before);
						item_before = nullptr;
					}
					else
					{
						item_before->count = left;
						// msg - full inventory
					}
					action_state = 2;
				}

				if(inst->GetEndResultClear(0))
				{
					new_anim = Animation::STAND;
					action = Action::NONE;
				}
			}
			break;
		case Action::DROP:
			{
				if(action_state == 0 && inst->GetProgress(0) >= 0.5f) // todo
				{
					if(action_var < (int)items.size() && items[action_var].item)
					{
						ItemSlot& slot = items[action_var];
						GroundItem* ground_item = new GroundItem;
						ground_item->item = slot.item;
						ground_item->count = slot.count;
						ground_item->ammo_count = slot.ammo_count;
						ground_item->node = new SceneNode;
						ground_item->node->SetMesh(ground_item->item->mesh);
						ground_item->node->pos = node->pos;
						ground_item->node->rot = Random(0.f, PI * 2);
						level->AddItem(ground_item);
						slot.item = nullptr;
					}
					action_state = 1;
				}

				if(inst->GetEndResultClear(0))
				{
					new_anim = Animation::STAND;
					action = Action::NONE;
				}
			}
			break;
		case Action::RELOAD:
			{
				if(action_state == 0 && inst->GetProgress(1) >= 0.5f) // todo
				{
					if(HaveWeapon())
					{
						ItemSlot& weapon = GetWeapon();
						if(action_var == -1)
						{
							// reload using any slot
							for(ItemSlot& slot : items)
							{
								if(slot.item && slot.item->type == Item::Type::AMMO)
								{
									uint used = min(slot.count, weapon.item->count - weapon.ammo_count);
									weapon.ammo_count += used;
									slot.count -= used;
									if(slot.count == 0)
										slot.item = nullptr;
									if(weapon.ammo_count == weapon.item->count)
										break;
								}
							}
						}
						else
						{
							// reload from single slot
							if(action_var < (int)items.size() && items[action_var].item && items[action_var].item->type == Item::Type::AMMO)
							{
								ItemSlot& slot = items[action_var];
								uint used = min(slot.count, weapon.item->count - weapon.ammo_count);
								weapon.ammo_count += used;
								slot.count -= used;
								if(slot.count == 0)
									slot.item = nullptr;
							}
						}
					}
					action_state = 1;
				}

				if(inst->GetEndResultClear(1))
				{
					new_anim = Animation::STAND;
					action = Action::NONE;
				}
			}
			break;
		default:
			assert(0);
			break;
		}

		if(action != Action::PICKUP)
		{
			if(item_before)
			{
				item_before->node->tint = Vec3::One;
				item_before = nullptr;
			}
		}
	}

	bool can_move = (action == Action::NONE || action == Action::RELOAD || action == Action::CONSUME);
	if(can_move)
	{
		if(CanReload() && input->Pressed(Key::R))
		{
			if(HaveAmmoLeft(GetWeapon().item))
				StartReload();
			else
			{
				// msg - no ammo
			}
		}

		// move player
		int dir = 0;
		if(input->Down(Key::W))
			dir += 10;
		if(input->Down(Key::S))
			dir -= 10;
		if(input->Down(Key::D))
			dir += 1;
		if(input->Down(Key::A))
			dir -= 1;
		if(dir != 0)
		{
			float speed, d;
			switch(dir)
			{
			case +1: // right
				d = 0;
				speed = 5.f;
				new_anim = Animation::RUN_RIGHT;
				break;
			case -9: // right back
				d = PI / 4;
				speed = 2.5f;
				new_anim = Animation::WALK_BACK;
				break;
			case -10: // back
				d = PI / 2;
				speed = 2.5f;
				new_anim = Animation::WALK_BACK;
				break;
			case -11: // left back
				d = PI * 3 / 4;
				speed = 2.5f;
				new_anim = Animation::WALK_BACK;
				break;
			case -1: // left
				d = PI;
				speed = 5.f;
				new_anim = Animation::RUN_LEFT;
				break;
			case +9: // left forward
				d = PI * 5 / 4;
				speed = 7.f;
				new_anim = Animation::RUN;
				break;
			default:
			case +10: // forward
				d = PI * 3 / 2;
				speed = 7.f;
				new_anim = Animation::RUN;
				break;
			case +11: // right forward
				d = PI * 7 / 4;
				speed = 7.f;
				new_anim = Animation::RUN;
				break;
			}

			if(anim == new_anim)
				speed *= node->GetMeshInstance()->GetGroup(0).GetBlendT();
			else
				speed = 0.f;
			speed *= dt;

			d += node->rot - PI / 2;
			node->pos += Vec3(sin(d)*speed, 0, cos(d)*speed);
		}
		else
			new_anim = Animation::STAND;

		// rotate player
		const Int2& mouse_dif = input->GetMouseMove();
		node->rot = Clip(node->rot + float(mouse_dif.x) / 800);
		if(new_anim == Animation::STAND && mouse_dif.x != 0)
		{
			if(mouse_dif.x > 0)
				new_anim = Animation::ROTATE_RIGHT;
			else
				new_anim = Animation::ROTATE_LEFT;
		}
	}

	// update animation
	if(anim != new_anim)
	{
		anim = new_anim;
		switch(anim)
		{
		case Animation::STAND:
			inst->Play("stoi", 0, 0);
			break;
		case Animation::RUN:
		case Animation::RUN_LEFT:
		case Animation::RUN_RIGHT:
			inst->Play("biegnie", 0, 0);
			break;
		case Animation::WALK_BACK:
			inst->Play("idzie", PLAY_BACK, 0);
			break;
		case Animation::ROTATE_LEFT:
			inst->Play("w_lewo", 0, 0);
			break;
		case Animation::ROTATE_RIGHT:
			inst->Play("w_prawo", 0, 0);
			break;
		}
	}
	inst->Update(dt);
}

void Player::StartReload(int slot)
{
	action = Action::RELOAD;
	action_state = 0;
	action_var = slot;
	node->GetMeshInstance()->Play("reload", PLAY_ONCE, 1);
	new_anim = Animation::ACTION;
}

void Player::Save(FileWriter& f)
{
	f << node->pos;
	f << node->rot;
	f << hp;
	f << hpmax;
	f << food;
	f << foodmax;
	f << action;
	f << action_state;
	f << action_var;
	f << weapon_index;
	f << anim;
	// todo: save mesh instance
	f << items.size();
	for(ItemSlot& slot : items)
	{
		if(!slot.item)
			f.Write0();
		else
		{
			f << slot.item->id;
			f << slot.count;
			f << slot.ammo_count;
		}
	}
}

void Player::Load(FileReader& f)
{
	f >> node->pos;
	f >> node->rot;
	f >> hp;
	f >> hpmax;
	f >> food;
	f >> foodmax;
	f >> action;
	f >> action_state;
	f >> action_var;
	f >> weapon_index;
	f >> anim;
	// todo: item before when picking up
	// todo: load mesh instance
	uint count;
	f >> count;
	items.resize(count);
	for(ItemSlot& slot : items)
	{
		const string& id = f.ReadString1();
		if(id.empty())
			slot.item = nullptr;
		else
		{
			slot.item = Item::Get(id);
			f >> slot.count;
			f >> slot.ammo_count;
		}
	}
}
