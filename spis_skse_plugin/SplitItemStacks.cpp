#include "SplitItemStacks.h"
#include "SpisBasicUtils.h"
#include "SpisSerializations.h"
#include "UIExtensions.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

typedef tList<ExtraContainerChanges::EntryData> EntryDataList;

namespace plugin_spis
{
	//global trackers, created at runtime
	DurabilityTracker * globalDurabilityTracker;
	CurrentContainer * globalCurrentContainer;
	CurrentDurability * globalCurrentDurability;
	EquippedStates * globalEquippedStates;
	CurrentGroundKey * globalCurrentGroundKey;
	
	bool isInitialized = false;

	//durabilitytracker function defines, some are deprecated and commented (because I'm a code hoarder)

	DurabilityTracker::DurabilityTracker()
	{
		GroundEntries = new std::unordered_map< GroundKey, GroundValue, GroundKeyHash >;
		ContainerEntries = new std::unordered_map < ContainerKey, std::unordered_map<TESForm*, ContainerValue>, ContainerKeyHash >;
	}

	DurabilityTracker::GroundKey DurabilityTracker::FindEntryGround(TESObjectREFR * item, UInt32 durability)
	{
		return GroundKey(item);
	}

	//untested function, if there's a problem look here first
	SInt32 DurabilityTracker::FindEntryContainer(UInt8 findType, TESObjectREFR * container, TESForm * item, UInt32 durability, UInt32 maxDurability, UInt32 nth = 0)
	{
		SInt32 lowest = -1;
		SInt32 nthlowest = -1;
		SInt32 highest = -1;
		SInt32 nthhighest = -1;
		UInt32 n = 0;

		std::vector<std::pair<UInt32, UInt32> > durs = (*ContainerEntries)[ContainerKey(container)][item].Durabilities;

		if (ContainerEntries->count(ContainerKey(container)))
		{
			switch (findType)
			{
				case 0:
					for (SInt32 retIndex = 0; retIndex < durs.size(); retIndex++)
					{
						if (durs[retIndex].second == durability)
						{
							if (n++ == nth)
							{
								return retIndex;
							}
						}
					}
					return -1;

				case 1:
					for (SInt32 retIndex = 0; retIndex < durs.size(); retIndex++)
					{
						UInt32 tempDurComp = durs[retIndex].second;
						if (tempDurComp <= durs[lowest].second)
						{
							if (tempDurComp == durs[lowest].second)
							{
								if (n++ == nth)
								{
									nthlowest = retIndex;
								}
								else
								{
									n = 0;
								}
							}

							lowest = retIndex;
						}
					}
					if (nthlowest > -1)
					{
						if (durs[lowest].second == durs[nthlowest].second)
						{
							return nthlowest;
						}
					}
					return lowest;

				case 2:
					for (SInt32 retIndex = 0; retIndex < durs.size(); retIndex++)
					{
						UInt32 tempDurComp = durs[retIndex].second;
						if (tempDurComp >= durs[highest].second)
						{
							if (tempDurComp == durs[highest].second)
							{
								if (n++ == nth)
								{
									nthhighest = retIndex;
								}
							}
							else
							{
								n = 0;
							}

							highest = retIndex;
						}
					}
					if (nthhighest > -1)
					{
						if (durs[highest].second == durs[nthhighest].second)
						{
							return nthhighest;
						}
					}
					return highest;
			}
		}
		else
		{
			return -1;
		}
	}

	DurabilityTracker::ContainerValue * DurabilityTracker::FindDurabilityContainer(TESObjectREFR * container, UInt32 formID)
	{
		if (ContainerEntries->count(ContainerKey(container)))
		{
			return &((*ContainerEntries)[ContainerKey(container)][LookupFormByID(formID)]);
		}
		else
		{
			return nullptr;
		}
	}

	bool DurabilityTracker::AddEntry(UInt8 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability, UInt32 maxDurability)
	{
		if (!amount)
		{
			return true;
		}

		ContainerKey tempContKey(container);

		switch (space)
		{
			case 0:
				if (ContainerEntries->count(tempContKey))
				{
					if ((*ContainerEntries)[tempContKey].count(item))
					{
						(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
					}
					else
					{
						(*ContainerEntries)[tempContKey].insert(std::pair<TESForm*, ContainerValue>(item, ContainerValue(item, 1)));

						if ((*ContainerEntries)[tempContKey].count(item))
						{
							(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
							(*ContainerEntries)[tempContKey][item].count += amount;
						}
						else
						{
							return false;
						}
					}
				}
				else
				{
					std::unordered_map<TESForm*, ContainerValue> tempContEntVal;

					// ### uncompacted code, probably temporary
					//tempContEntVal.insert(std::pair<TESForm*, ContainerValue>(item, ContainerValue(container, item))); //original statement
						ContainerValue t1(item, 1);
						
						std::pair<TESForm*, ContainerValue> t2(item, t1);
						
						tempContEntVal.insert(t2);
						
					// ### end uncompacted

					ContainerEntries->insert(std::pair<ContainerKey, std::unordered_map<TESForm*, ContainerValue> >(tempContKey, tempContEntVal));

					if ((*ContainerEntries)[tempContKey].count(item))
					{
						(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
					}
					else
					{
						(*ContainerEntries)[tempContKey].insert(std::pair<TESForm*, ContainerValue>(item, ContainerValue(item, 1)));

						if ((*ContainerEntries)[tempContKey].count(item))
						{
							(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
						}
						else
						{
							return false;
						}
					}
				}
				break;

			case 1:
				GroundKey tempGroundKey(groundItem);
				if (GroundEntries->count(tempGroundKey))
				{
					return false;
				}
				else
				{
					GroundValue tempGroundValue(maxDurability, durability);
					GroundEntries->insert(std::pair<GroundKey, GroundValue>(tempGroundKey, tempGroundValue));
				}

				break;
		}

	}

	// 8^( this will 4evr b spaghetti
	bool DurabilityTracker::RemoveEntry(UInt8 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt8 removeType, UInt32 durability, UInt32 maxDurability, UInt32 nth)
	{
		if (!amount)
		{
			return true;
		}

		//std::vector<std::pair<UInt32, UInt32> > durs;

		ContainerKey tempContKey(container);
		GroundKey tempGroundKey;
		if (groundItem)
		{
			tempGroundKey.setObjectRef(groundItem);
		}
		switch (space)
		{
			case 0:
				if (ContainerEntries->count(tempContKey))
				{
					if ((*ContainerEntries)[tempContKey].count(item) && (FindEntryContainer(removeType, container, item, durability, maxDurability, nth) > -1))
					{
						if (globalEquippedStates->FindInList(item, (*ContainerEntries)[ContainerKey(container)][item].Durabilities[FindEntryContainer(removeType, container, item, durability, maxDurability, nth)].second, (*ContainerEntries)[ContainerKey(container)][item].Durabilities[FindEntryContainer(removeType, container, item, durability, maxDurability, nth)].first, nth))
						{
							for (UInt32 i = 0; i < 18; i++)
							{
								for (UInt32 j = 0; j < (*ContainerEntries)[ContainerKey(container)][item].Durabilities.size(); j++)
								{
									if (std::get<0>(globalEquippedStates->EquippedEntries[i]) == item || (*ContainerEntries)[tempContKey][item].Durabilities[j].first == std::get<1>(globalEquippedStates->EquippedEntries[i]) || (*ContainerEntries)[tempContKey][item].Durabilities[j].second == std::get<2>(globalEquippedStates->EquippedEntries[i]))
									{
										globalEquippedStates->EquippedEntries[i] = std::tuple<TESForm*, UInt32, UInt32, UInt32>(nullptr, 0, 0, 0);
									}
								}
							}
						}

						(*ContainerEntries)[tempContKey][item].Durabilities.erase((*ContainerEntries)[tempContKey][item].Durabilities.begin() + FindEntryContainer(removeType, container, item, durability, maxDurability, nth));
						(*ContainerEntries)[tempContKey][item].count -= amount;
						if ((*ContainerEntries)[ContainerKey(container)][item].Durabilities.size() == 0)
						{
							(*ContainerEntries)[tempContKey].erase(item);
							return true;
						}
						
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}

			case 1:
				if (GroundEntries->count(tempGroundKey))
				{
					GroundEntries->erase(tempGroundKey);
					return true;
				}
				else
				{
					return false;
				}

			case 4:
				if (GroundEntries->count(globalCurrentGroundKey->GetGroundKey()))
				{
					GroundEntries->erase(globalCurrentGroundKey->GetGroundKey());
					return true;
				}
				else
				{
					return false;
				}
		}
	}

	bool DurabilityTracker::MoveEntry(UInt8 space, TESObjectREFR * containerFrom, TESObjectREFR * containerTo, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability, UInt32 maxDurability, UInt32 nth)
	{
		_MESSAGE("_DB_MVEN_-1, %s, %d", papyrusForm::GetName(item), amount);
		if (!amount)
		{
			return true;
		}
		bool c1 = false;
		bool c2 = false;
		UInt32 t_maxdurabil;
		UInt32 t_durability;
		TESObjectREFR * gkr = nullptr;
		UInt32 hk = globalCurrentGroundKey->GetGroundKey().gethashKey();
		UInt32 thk = hk;
		switch (space)
		{
		case 0:
			if (!containerFrom || !containerTo || !item || !amount || !durability || !maxDurability)
			{
				return false;
			}
			c1 = AddEntry(space, containerTo, item, groundItem, amount, durability, (*ContainerEntries)[ContainerKey(containerFrom)][item].Durabilities[FindEntryContainer(0, containerFrom, item, durability, maxDurability, nth)].first);
			c2 = RemoveEntry(space, containerFrom, item, groundItem, amount, 0, durability, maxDurability, nth);
			return c1 && c2;

		case 1:
			return false; //invalid, this doesn't make sense because obj ref is static (and coords are just pointers) so no movement "between" world space can occur

		case 2:
			if (hk)
			{
				LookupREFRByHandle(&(hk), &gkr);
			}

			if ((gkr != 0) || !thk)
			{
				AddEntry(0, containerTo, item, groundItem, amount, LookupDurabilityInfo(item), LookupDurabilityInfo(item));
				return true;
			}
			else
			{
				if (((*GroundEntries)[globalCurrentGroundKey->GetGroundKey()].first == 0) && ((*GroundEntries)[globalCurrentGroundKey->GetGroundKey()].second))
				{
					AddEntry(0, containerTo, item, groundItem, amount, LookupDurabilityInfo(item), LookupDurabilityInfo(item));
				}
				else
				{
					t_maxdurabil = (*GroundEntries)[globalCurrentGroundKey->GetGroundKey()].first;
					t_durability = (*GroundEntries)[globalCurrentGroundKey->GetGroundKey()].second;

					//uses special remove that just checks currentgroundkey for item in question
					c1 = AddEntry(0, containerTo, item, groundItem, amount, t_durability, t_maxdurabil);
					c2 = RemoveEntry(4, containerFrom, item, groundItem, amount, 0, 0, 0);

					return c1 && c2;
				}
			}

		case 3:
			if (!containerFrom || !item || !groundItem || !amount || !durability || !maxDurability)
			{
				return false;
			}
			c1 = AddEntry(1, containerTo, item, groundItem, amount, (*ContainerEntries)[ContainerKey(containerFrom)][item].Durabilities[FindEntryContainer(0, containerFrom, item, durability, maxDurability, nth)].second, (*ContainerEntries)[ContainerKey(containerFrom)][item].Durabilities[FindEntryContainer(0, containerFrom, item, durability, maxDurability, nth)].first);
			globalCurrentGroundKey->SetGroundKey(groundItem);
			c2 = RemoveEntry(0, containerFrom, item, groundItem, amount, 0, durability, maxDurability, nth);
			
			return c1 && c1;
		}
	}


	//may need to revist this. this function is misbehaving, something in the game fires an add entry during initialization
	//and that cause 2 sets of armor to appear in the container map. this function is only a 1 time use function, and will
	//never fire again after a new save. so i may not need to "fix" it, or i may. i'll see.
	bool DurabilityTracker::WrapEntries(TESObjectREFR* container)
	{
		//TESContainer adds always
		TESContainer* pContainer = DYNAMIC_CAST(container->baseForm, TESForm, TESContainer);

		for (UInt32 i = 0; i < pContainer->numEntries; i++)
		{
			if (pContainer->entries[i]->form->IsWeapon() || pContainer->entries[i]->form->IsArmor())
				for (UInt32 j = 0; j < pContainer->entries[i]->count; j++)
				{
					//_MESSAGE("_BG_WREN1, %s, %d, %d, %d", papyrusForm::GetName(pContainer->entries[i]->form), LookupDurabilityInfo(pContainer->entries[i]->form), LookupDurabilityInfo(pContainer->entries[i]->form), pContainer->entries[i]->count);
					AddEntry(0, container, pContainer->entries[i]->form, nullptr, 1, LookupDurabilityInfo(pContainer->entries[i]->form), LookupDurabilityInfo(pContainer->entries[i]->form));
				}
		}

		//container changes adds if positive count delta, takes away entries if negative count delta
		ExtraContainerChanges * pToContRefExC = static_cast<ExtraContainerChanges*>(container->extraData.GetByType(kExtraData_ContainerChanges));

		UInt32 i = 0;

		while (pToContRefExC->data->objList->GetNthItem(i))
		{
			if (pToContRefExC->data->objList->GetNthItem(i)->type->IsWeapon()/* || pToContRefExC->data->objList->GetNthItem(i)->type->IsArmor()*/)
			{
				if (pToContRefExC->data->objList->GetNthItem(i)->countDelta > 0)
				{
					for (UInt32 j = 0; j < pToContRefExC->data->objList->GetNthItem(i)->countDelta; j++)
					{
						//_MESSAGE("_BG_WREN2, %s, %d, %d, %d", papyrusForm::GetName(pToContRefExC->data->objList->GetNthItem(i)->type), LookupDurabilityInfo(pToContRefExC->data->objList->GetNthItem(i)->type), LookupDurabilityInfo(pToContRefExC->data->objList->GetNthItem(i)->type), pToContRefExC->data->objList->GetNthItem(i)->countDelta);
						AddEntry(0, container, pToContRefExC->data->objList->GetNthItem(i)->type, nullptr, 1, LookupDurabilityInfo(pToContRefExC->data->objList->GetNthItem(i)->type), LookupDurabilityInfo(pToContRefExC->data->objList->GetNthItem(i)->type));
					}
				}
				if (pToContRefExC->data->objList->GetNthItem(i)->countDelta < 0)
				{
					for (UInt32 j = 0; j < pToContRefExC->data->objList->GetNthItem(i)->countDelta; j++)
					{
						//_MESSAGE("_BG_WREN3, %s, %d", papyrusForm::GetName(pToContRefExC->data->objList->GetNthItem(i)->type), pToContRefExC->data->objList->GetNthItem(i)->countDelta);
						RemoveEntry(0, container, pToContRefExC->data->objList->GetNthItem(i)->type, nullptr, 1, 1, 0, 0);
					}
				}
			}
			i++;
		}

		return true;
	}

	//other function defines
	//don't judge me for copying a function from stackoverflow... i mean i edited a little... stop looking at me like that!
	inline std::string int_to_hex(UInt32 i)
	{
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(sizeof(UInt32) * 2) << std::hex << i;
		return stream.str();
	}

	//copied function from papyrusactor.cpp, wasn't defined in header so.....
	SInt32 CalcItemId(TESForm * form, BaseExtraList * extraList)
	{
		if (!form || !extraList)
			return 0;

		const char * name = extraList->GetDisplayName(form);

		// No name in extra data? Use base form name
		if (!name)
		{
			TESFullName* pFullName = DYNAMIC_CAST(form, TESForm, TESFullName);
			if (pFullName)
				name = pFullName->name.data;
		}

		if (!name)
			return 0;

		return (SInt32)HashUtil::CRC32(name, form->formID & 0x00FFFFFF);
	}

	UInt32 LookupDurabilityInfo(TESForm* item)
	{
		std::ifstream DurabilitiesInfo("durabilities.txt");
		std::string foundLine;
		UInt32 durability = 0xFFFFFFFF;

		std::string tempFindString = int_to_hex(item->formID);

		while (std::getline(DurabilitiesInfo, foundLine))
		{
			if (!foundLine.find(tempFindString))
			{
				std::stringstream ss;
				ss << std::hex << foundLine.substr(10, 8);
				ss >> durability;
			}
		}
		DurabilitiesInfo.close();
		return durability;
	}

	//papyrus function defines
	bool InitializeDurabilityTracker(StaticFunctionTag *base)
	{
		if (!isInitialized)
		{
			globalDurabilityTracker = new DurabilityTracker();
			globalCurrentContainer = new CurrentContainer(nullptr);
			globalCurrentDurability = new CurrentDurability(0, 0);
			globalEquippedStates = new EquippedStates;
			globalCurrentGroundKey = new CurrentGroundKey();
			isInitialized = true;
		}
		return isInitialized;
	}

	bool AddEntry(StaticFunctionTag *base, UInt32 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability, UInt32 maxDurability)
	{
		return globalDurabilityTracker->AddEntry(space, container, item, groundItem, amount, durability, maxDurability);
	}

	bool RemoveEntry(StaticFunctionTag *base, UInt32 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 removeType, UInt32 durability, UInt32 maxDurability, UInt32 nth)
	{
		return globalDurabilityTracker->RemoveEntry(space, container, item, groundItem, amount, removeType, durability, maxDurability, nth);
	}

	bool MoveEntry(StaticFunctionTag *base, UInt32 space, TESObjectREFR * containerFrom, TESObjectREFR * containerTo, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability, UInt32 maxDurability, UInt32 nth)
	{
		return globalDurabilityTracker->MoveEntry(space, containerFrom, containerTo, item, groundItem, amount, durability, maxDurability, nth);
	}

	bool WrapEntries(StaticFunctionTag *base, TESObjectREFR* container)
	{
		return globalDurabilityTracker->WrapEntries(container);
	}

	bool SetCurrentContainer(StaticFunctionTag *base, TESObjectREFR* container)
	{
		return globalCurrentContainer->ChangeContainer(container);
	}

	TESObjectREFR * GetCurrentContainer(StaticFunctionTag *base)
	{
		return globalCurrentContainer->GetContainer();
	}

	void PrintFound(StaticFunctionTag *base, UInt32 findType, TESObjectREFR * container, TESForm * item, UInt32 durability, UInt32 maxDurability, UInt32 nth)
	{
		for (auto it = (*(globalDurabilityTracker->ContainerEntries)).begin(); it != (*(globalDurabilityTracker->ContainerEntries)).end(); it++)
		{
			//UInt32 han = ;
			_MESSAGE("_pf_container:%s", it->first.getContainerRef()->GetFullName());
			for (auto itt = it->second.begin(); itt != it->second.end(); itt++)
			{
				_MESSAGE("_pf_. |-->item:%s", itt->first->GetFullName());
				for (UInt32 i = 0; i < itt->second.Durabilities.size(); i++)
				{
					_MESSAGE("_pf_. | . . |-->d/md:%d/%d", itt->second.Durabilities[i].second, itt->second.Durabilities[i].first);
				}
			}
		}
		_MESSAGE("_PF_1");
		if (globalDurabilityTracker->GroundEntries->size() == 0)
		{
			return;
		}
		_MESSAGE("_PF_1");
		for (auto it = (*(globalDurabilityTracker->GroundEntries)).begin(); it != (*(globalDurabilityTracker->GroundEntries)).end(); it++)
		{
			//UInt32 han = ;
			_MESSAGE("_pf_refhan:%d, hashkey:%d, ", it->first.getObjectRef()->CreateRefHandle(), it->first.gethashKey());
			_MESSAGE("_pf_. |-->d/md:%d/%d", it->second.second, it->second.first);

		}
	}

	UInt32 GetCurrentDurability(StaticFunctionTag *base)
	{
		return globalCurrentDurability->GetDurability();
	};

	UInt32 GetCurrentMaxDurability(StaticFunctionTag *base)
	{
		return globalCurrentDurability->GetMaxDurability();
	};

	bool WrapEquipStates(StaticFunctionTag * base, TESObjectREFR * actor)
	{
		return globalEquippedStates->WrapEquipStates(actor, globalDurabilityTracker);
	}

	bool IncrementEquippedDurability(StaticFunctionTag* base, UInt32 slot, UInt32 amount, TESObjectREFR* actor)
	{
		SInt32 pos = globalDurabilityTracker->FindEntryContainer(0, actor, std::get<0>(globalEquippedStates->EquippedEntries[slot]), std::get<2>(globalEquippedStates->EquippedEntries[slot]), std::get<1>(globalEquippedStates->EquippedEntries[slot]), std::get<3>(globalEquippedStates->EquippedEntries[slot]));
		if (pos > -1)
		{
			globalDurabilityTracker->FindDurabilityContainer(actor, std::get<0>(globalEquippedStates->EquippedEntries[slot])->formID)->Durabilities[pos].second += amount;
			std::get<2>(globalEquippedStates->EquippedEntries[slot]) = globalDurabilityTracker->FindDurabilityContainer(actor, std::get<0>(globalEquippedStates->EquippedEntries[slot])->formID)->Durabilities[pos].second;
			//finds last entry with new inc/decremented durability and updates n value in equipped states
			UInt32 n = 0;
			while (globalDurabilityTracker->FindEntryContainer(0, actor, std::get<0>(globalEquippedStates->EquippedEntries[slot]), globalDurabilityTracker->FindDurabilityContainer(actor, std::get<0>(globalEquippedStates->EquippedEntries[slot])->formID)->Durabilities[pos].second, globalDurabilityTracker->FindDurabilityContainer(actor, std::get<0>(globalEquippedStates->EquippedEntries[slot])->formID)->Durabilities[pos].first, n) > -1)
			{

				n++;
				std::get<3>(globalEquippedStates->EquippedEntries[slot]) = n;
			}
			return true;
		}
	}

	bool DecrementEquippedDurability(StaticFunctionTag* base, UInt32 slot, UInt32 amount, TESObjectREFR* actor)
	{
		SInt32 pos = globalDurabilityTracker->FindEntryContainer(0, actor, std::get<0>(globalEquippedStates->EquippedEntries[slot]), std::get<2>(globalEquippedStates->EquippedEntries[slot]), std::get<1>(globalEquippedStates->EquippedEntries[slot]), std::get<3>(globalEquippedStates->EquippedEntries[slot]));
		if (pos > -1)
		{
			globalDurabilityTracker->FindDurabilityContainer(actor, std::get<0>(globalEquippedStates->EquippedEntries[slot])->formID)->Durabilities[pos].second -= amount;
			std::get<2>(globalEquippedStates->EquippedEntries[slot]) = globalDurabilityTracker->FindDurabilityContainer(actor, std::get<0>(globalEquippedStates->EquippedEntries[slot])->formID)->Durabilities[pos].second;
			
			UInt32 n = 0;
			while (globalDurabilityTracker->FindEntryContainer(0, actor, std::get<0>(globalEquippedStates->EquippedEntries[slot]), globalDurabilityTracker->FindDurabilityContainer(actor, std::get<0>(globalEquippedStates->EquippedEntries[slot])->formID)->Durabilities[pos].second, globalDurabilityTracker->FindDurabilityContainer(actor, std::get<0>(globalEquippedStates->EquippedEntries[slot])->formID)->Durabilities[pos].first, n) > -1)
			{
				std::get<3>(globalEquippedStates->EquippedEntries[slot]) = n;
				n++;
			}
			return true;
		}
		return false;
	}

	DurabilityTracker::GroundKey GetCurrentGroundKey(StaticFunctionTag * base)
	{
		return globalCurrentGroundKey->GetGroundKey();
	};

	bool IsCurrentGroundKeyNull(StaticFunctionTag * base)
	{
		return globalCurrentGroundKey->GetGroundKey().gethashKey();
	}

	bool SetCurrentGroundKeyNull(StaticFunctionTag * base)
	{
		globalCurrentGroundKey->GetGroundKey().sethashKey(0);
		return true;
	}

	bool SetCurrentGroundKey(StaticFunctionTag * base, TESObjectREFR * obj)
	{
		globalCurrentGroundKey->SetGroundKey(obj);
		return true;
	}

	//UI functions
	class SKSEScaleform_GetExtras : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args)
		{
			GetExtras(args, globalDurabilityTracker, globalCurrentContainer, globalEquippedStates);
		}
	};

	class SKSEScaleform_SetCurrentDurabilities : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args)
		{
			SetCurrentDurabilities(args, globalCurrentDurability);
		}
	};

	class SKSEScaleform_SetEquippedState : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args)
		{
			SetEquippedState(args, globalEquippedStates);
		}
	};

	class SKSEScaleform_IncrementDurability : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args)
		{
			IncrementDurability(args, globalDurabilityTracker, globalCurrentContainer);
		}
	};

	class SKSEScaleform_GetRepairList : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args)
		{
			GetRepairList(args, globalDurabilityTracker, globalCurrentContainer);
		}
	};

	//serialization functions

	void Serialization_Revert(SKSESerializationInterface * intfc)
	{
		//_MESSAGE("revert");
	}

	const UInt32 kSerializationDataVersion = 0;

	void Serialization_Save(SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_SAVE_invoked");
		SerializeGnrlContainerMap(globalDurabilityTracker->ContainerEntries, intfc);
		SerializeGroundMap(globalDurabilityTracker->GroundEntries, intfc);
		SerializeEquippedStates(intfc, globalEquippedStates);
		SerializeCurrentContainer(intfc, globalCurrentContainer);
		SerializeCurrentDurability(intfc, globalCurrentDurability);
		intfc->OpenRecord(kTypeInitialized, 0);
		intfc->WriteRecordData(&isInitialized, sizeof(bool));
	}

	void Serialization_Load(SKSESerializationInterface * intfc)
	{
		
		UInt32 type, ver, len;
		bool ii;
		while (intfc->GetNextRecordInfo(&type, &ver, &len))
		{
			
			switch(type)
			{
			case kTypeContainerMap:
				*(globalDurabilityTracker->ContainerEntries) = UnserializeGnrlContianerMap(intfc);
				break;

			case kTypeGroundMap:
				*(globalDurabilityTracker->GroundEntries) = UnserializeGroundMap(intfc);
				break;

			case kTypeInitialized:
				intfc->ReadRecordData(&ii, sizeof(bool));
				isInitialized = ii;
				break;

			case kTypeEquippedStates:
				*(globalEquippedStates) = UnserializeEquippedStates(intfc);
				break;

			case kTypeCurrentContainer:
				*(globalCurrentContainer) = UnserializeCurrentContainer(intfc);
				break;

			case kTypeCurrentDurability:
				*(globalCurrentDurability) = UnserializeCurrentDurability(intfc);
				break;
			}
		}
	}

	//registry functions

	bool RegisterFuncsPapyrus(VMClassRegistry* registry)
	{
		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, bool>("InitializeDurabilityTracker", "plugin_spis", plugin_spis::InitializeDurabilityTracker, registry));

		registry->RegisterFunction(new NativeFunction7 <StaticFunctionTag, bool, UInt32, TESObjectREFR*, TESForm*, TESObjectREFR*, UInt32, UInt32, UInt32>("AddEntry", "plugin_spis", plugin_spis::AddEntry, registry));

		registry->RegisterFunction(new NativeFunction9 <StaticFunctionTag, bool, UInt32, TESObjectREFR*, TESForm*, TESObjectREFR*, UInt32, UInt32, UInt32, UInt32, UInt32>("RemoveEntry", "plugin_spis", plugin_spis::RemoveEntry, registry));

		registry->RegisterFunction(new NativeFunction9 <StaticFunctionTag, bool, UInt32, TESObjectREFR*, TESObjectREFR*, TESForm*, TESObjectREFR*, UInt32, UInt32, UInt32, UInt32>("MoveEntry", "plugin_spis", plugin_spis::MoveEntry, registry));

		registry->RegisterFunction(new NativeFunction6 <StaticFunctionTag, void, UInt32, TESObjectREFR*, TESForm*, UInt32, UInt32, UInt32>("PrintFound", "plugin_spis", plugin_spis::PrintFound, registry));

		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, bool, TESObjectREFR*>("WrapEntries", "plugin_spis", plugin_spis::WrapEntries, registry));

		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, bool, TESObjectREFR*>("SetCurrentContainer", "plugin_spis", plugin_spis::SetCurrentContainer, registry));

		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, TESObjectREFR*>("GetCurrentContainer", "plugin_spis", plugin_spis::GetCurrentContainer, registry));

		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("GetCurrentDurability", "plugin_spis", plugin_spis::GetCurrentDurability, registry));

		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("GetCurrentMaxDurability", "plugin_spis", plugin_spis::GetCurrentMaxDurability, registry));

		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, bool, TESObjectREFR*>("WrapEquipStates", "plugin_spis", plugin_spis::WrapEquipStates, registry));

		registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, UInt32, UInt32, TESObjectREFR*>("IncrementEquippedDurability", "plugin_spis", plugin_spis::IncrementEquippedDurability, registry));

		registry->RegisterFunction(new NativeFunction3 <StaticFunctionTag, bool, UInt32, UInt32, TESObjectREFR*>("DecrementEquippedDurability", "plugin_spis", plugin_spis::DecrementEquippedDurability, registry));

		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, bool, TESObjectREFR*>("SetCurrentGroundKey", "plugin_spis", plugin_spis::SetCurrentGroundKey, registry));

		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, bool>("IsCurrentGroundKeyNull", "plugin_spis", plugin_spis::IsCurrentGroundKeyNull, registry));

		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, bool>("SetCurrentGroundKeyNull", "plugin_spis", plugin_spis::SetCurrentGroundKeyNull, registry));

		//registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, BSFixedString>("outputToOpenDebugLog", "plugin_spis", plugin_spis::outputToOpenDebugLog, registry));

		return true;
	};

	bool RegisterFuncsScaleform(GFxMovieView * view, GFxValue * root)
	{

		RegisterFunction <SKSEScaleform_GetExtras>(root, view, "GetExtras");

		RegisterFunction <SKSEScaleform_SetCurrentDurabilities>(root, view, "SetCurrentDurabilities");

		RegisterFunction <SKSEScaleform_SetEquippedState>(root, view, "SetEquippedState");

		RegisterFunction <SKSEScaleform_IncrementDurability>(root, view, "IncrementDurability");

		RegisterFunction <SKSEScaleform_GetRepairList>(root, view, "GetRepairList");

		return true;
	}

	bool RegisterSerializationCallbacks(SKSESerializationInterface * intfc, PluginHandle handle)
	{
		intfc->SetUniqueID(handle, 'SPIS');
		intfc->SetRevertCallback(handle, Serialization_Revert);
		intfc->SetSaveCallback(handle, Serialization_Save);
		intfc->SetLoadCallback(handle, Serialization_Load);
		return true;
	}
};