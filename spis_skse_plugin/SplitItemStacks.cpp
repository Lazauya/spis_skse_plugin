#include "SplitItemStacks.h"
#include "SpisBasicUtils.h"
#include "SpisSerializations.h"
//#include "UIExtensions.h"

/*#include "skse/GameRTTI.h"
#include "skse/GameObjects.h"
#include "skse/GameData.h"
#include "skse/GameTypes.h"
#include "skse/GameAPI.h"*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

typedef tList<ExtraContainerChanges::EntryData> EntryDataList;

namespace plugin_spis
{
	/*bool special_BaseExtraAdd(UInt8 type, BSExtraData* toAdd, BaseExtraList*& newBEL, AlchemyItem )
	{
		BSExtraData* next = newBEL->m_data;
		newBEL->m_data = toAdd;
		toAdd->next = next;
		newBEL->MarkType(type, false);
		return true;
	};*/

	/*void RearrangeExtraContChanges(StaticFunctionTag *base, TESObjectREFR * ContainerREF, TESForm * FormToSplit)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim\\SKSE\\spis_plugin.log");
		_MESSAGE("Opened new log");

		ExtraContainerChanges * intermContREFCasted = static_cast<ExtraContainerChanges*>(ContainerREF->extraData.GetByType(kExtraData_ContainerChanges));
		_MESSAGE("1");

		ExtraContainerChanges::EntryData * newEntryData = ExtraContainerChanges::EntryData::Create(FormToSplit, 0); _MESSAGE("2");
		ExtraContainerChanges::EntryDataList * tempExtraDataList;// = ExtraContainerChanges::EntryDataList::Create();
		tempExtraDataList = intermContREFCasted->data->objList; _MESSAGE("3");
		_MESSAGE("Casted 'n declared");

		tempExtraDataList->Push(newEntryData);
		_MESSAGE("Pused newEntryData to tempExtraDataList");

		intermContREFCasted->data->objList = tempExtraDataList;
		_MESSAGE("Set intermContR to tempXDL");

		ContainerREF->extraData.Add(kExtraData_ContainerChanges, intermContREFCasted);
		_MESSAGE("Using add method from skaar");
		_MESSAGE("pro'ly didn't crash if you see this (yay)");

	};*/

	/*void spisDebug(StaticFunctionTag *base, TESObjectREFR * contRef, TESObjectREFR * ContainerREF2, TESObjectWEAP * Weapon, EnchantmentItem * Enchant)
	{
		//gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim\\SKSE\\spis_plugin.log");

		for (UInt32 n = 1; n < 5; n++)
		{
			//_MESSAGE("-------run: %d--------", n);
			ExtraContainerChanges * pToContRefExC = static_cast<ExtraContainerChanges*>(contRef->extraData.GetByType(kExtraData_ContainerChanges));
			if (!pToContRefExC){ return; };
			EntryDataList * pToContRefExCEDL = pToContRefExC->data->objList;
			if (!pToContRefExCEDL){ return; };

			ExtraContainerChanges::EntryData * tpEntryDataUnst = ExtraContainerChanges::EntryData::Create(Weapon, 1);

			/*ExtraEnchantment * tpxEnch = ExtraEnchantment::Create();

			tpxEnch->enchant = Enchant;
			tpxEnch->maxCharge = 0;

			BaseExtraList * npBaseExtraList;
			npBaseExtraList->Add(kExtraData_Charge, tpxEnch);

			tpEntryDataUnst->extendDataList->Push(npBaseExtraList);

			/*BaseExtraList * tpBEL = pToContRefExCEDL->GetNthItem(0)->extendDataList->GetNthItem(0); _MESSAGE(".");
			tpEntryDataUnst->extendDataList->Push(tpBEL); _MESSAGE(".");
			
			if (tpEntryDataUnst->extendDataList->GetNthItem(0)->HasType(kExtraData_Charge))
			{
				_MESSAGE("Has Charge");
				_MESSAGE("has charge extraData: %d", tpEntryDataUnst->extendDataList->GetNthItem(0)->HasType(kExtraData_Charge));
				ExtraCharge* xCharge = static_cast<ExtraCharge*>(tpEntryDataUnst->extendDataList->GetNthItem(0)->GetByType(kExtraData_Charge));
				xCharge->charge = n;
				_MESSAGE("charge: %d, n: %d", xCharge->charge, n);
			}
			else
			{
				_MESSAGE("Doesn't have charge");
				ExtraCharge * tpExtraCharge = ExtraCharge::Create(); _MESSAGE(".");

				tpExtraCharge->charge = n; _MESSAGE(".");
				_MESSAGE("n value: %d", n);
				_MESSAGE("charge value: %d", tpExtraCharge->charge);
				tpEntryDataUnst->extendDataList->GetNthItem(0)->Add(kExtraData_Charge, tpExtraCharge); _MESSAGE("Just invoked add with the charge extradata");
			}
			
			pToContRefExCEDL->Push(tpEntryDataUnst); //_MESSAGE(".");

			/*_MESSAGE("trying to change charge value in post");
			xCharge->count = 10.0;
			_MESSAGE("if worked, vale should be 10: %d", xCharge->count);
		}
		//boy that was some smooooooooth code
		/*_MESSAGE("1");
		ExtraCount * tpExtraCount = ExtraCount::Create();
		_MESSAGE("2");
		tpExtraCount->count = 1;
		_MESSAGE("3");
		pToContRefExCEDL->GetNthItem(0)->extendDataList->GetNthItem(0)->Add(kExtraData_Count, tpExtraCount);
		_MESSAGE("4... It didn't crash. What?");

	};*/

	/*DurabilityTracker::DurabilityTracker()
	{
		WrappedEntries->
	}*/
	
	//global tracker, created at runtime

	DurabilityTracker * globalDurabilityTracker;
	CurrentContainer * globalCurrentContainer;
	CurrentDurability * globalCurrentDurability;
	bool isInitialized = false;

	//durabilitytracker function defines, some are deprecated and commented (because I'm a code hoarder)
	
	/*void DurabilityTracker::WrapEntries(TESObjectREFR * contRef)
	{
		ExtraContainerChanges * pToContRefExC = static_cast<ExtraContainerChanges*>(contRef->extraData.GetByType(kExtraData_ContainerChanges));
		if (!pToContRefExC){ return; };
		EntryDataList * pToContRefExCEDL = pToContRefExC->data->objList;
		if (!pToContRefExCEDL){ return; };

		UInt32 n = 0;
		
		while (pToContRefExCEDL->GetNthItem(n))
		{
			DurabilityEntry * tEntry = new DurabilityEntry(contRef, pToContRefExCEDL->GetNthItem(n), 100);
			tEntry->indexOfItemInList = n;
			WrappedEntries->push_back(tEntry);
			n++;
		}
	}

	DurabilityTracker::DurabilityEntry * DurabilityTracker::GetEntryWithTypeAndLowestDurability(TESForm * type, TESObjectREFR * contRef)
	{
		ExtraContainerChanges * pToContRefExC = static_cast<ExtraContainerChanges*>(contRef->extraData.GetByType(kExtraData_ContainerChanges));
		if (!pToContRefExC){ return nullptr; };
		EntryDataList * pToContRefExCEDL = pToContRefExC->data->objList;
		if (!pToContRefExCEDL){ return nullptr; };

		DurabilityEntry * tLowest; //declare outside of scope

		//get first
		for (UInt32 n = 0; n < WrappedEntries->size(); n++)
		{
			if ((*WrappedEntries)[n]->pEntryData->type == type && (*WrappedEntries)[n]->owner == contRef)
			{
				tLowest = (*WrappedEntries)[n];
				break;
			}
		}
		
		//find lowest
		for (UInt32 n = 0; n < WrappedEntries->size(); n++)
			if ((*WrappedEntries)[n]->owner == contRef && (*WrappedEntries)[n]->pEntryData->type == type)
			{
				if ( (*WrappedEntries)[n]->GetDurability() < tLowest->GetDurability() && pToContRefExCEDL->GetNthItem(n)->countDelta != 0)
				{
					tLowest = (*WrappedEntries)[n];
				}
			}
		if (tLowest){ return tLowest; }
		else{ return nullptr; } //returns this if none exist in current container, hopefully this doesn't crash anything
	}
	
	//void DurabilityTracker::AddDurabilityEntry(DurabilityEntry * entry)
	//{

	//}

	void DurabilityTracker::RemoveEntryWithTypeAndLowestDurability(TESForm * type, TESObjectREFR * contRef)
	{
		ExtraContainerChanges * pToContRefExC = static_cast<ExtraContainerChanges*>(contRef->extraData.GetByType(kExtraData_ContainerChanges));
		if (!pToContRefExC){ return; };
		EntryDataList * pToContRefExCEDL = pToContRefExC->data->objList;
		if (!pToContRefExCEDL){ return; };

		UInt32 tUnchecked = GetEntryWithTypeAndLowestDurability(type, contRef)->indexOfItemInList;

		if (!tUnchecked){ return; }

		pToContRefExCEDL->GetNthItem(tUnchecked)->countDelta = 0;
	}*/

	DurabilityTracker::DurabilityTracker()
	{
		_MESSAGE("_DEBUG_PREF_INIT_init called");
		GroundEntries = new std::unordered_map< GroundKey, GroundValue, GroundKeyHash >;
		_MESSAGE("_DEBUG_PPEF_INIT_ground entries just declared");
		ContainerEntries = new std::unordered_map < ContainerKey, std::unordered_map<TESForm*, ContainerValue>, ContainerKeyHash >;
	}

	DurabilityTracker::GroundKey DurabilityTracker::FindEntryGround(TESObjectREFR * item, UInt32 durability)
	{
		return GroundKey(item);
	}

	SInt32 DurabilityTracker::FindEntryContainer(UInt8 findType, TESObjectREFR * container, TESForm * item, UInt32 durability)
	{
		SInt32 lowest = 0;
		SInt32 highest = 0;
		if (ContainerEntries->count(ContainerKey(container)))
		{
			switch (findType)
			{
				case 0:
					for (SInt32 retIndex = 0; retIndex < (*ContainerEntries)[ContainerKey(container)][item].Durabilities.size(); retIndex++)
					{
						if ((*ContainerEntries)[ContainerKey(container)][item].Durabilities[retIndex].second == durability)
						{
							return retIndex;
						}
					}
					return -1;
				case 1:
					for (SInt32 retIndex = 0; retIndex < (*ContainerEntries)[ContainerKey(container)][item].Durabilities.size(); retIndex++)
					{
						UInt32 tempDurComp = (*ContainerEntries)[ContainerKey(container)][item].Durabilities[retIndex].second;
						if (tempDurComp == durability && tempDurComp < (*ContainerEntries)[ContainerKey(container)][item].Durabilities[lowest].second)
						{
							lowest = retIndex;
						}
					}
					return lowest;
				case 2:
					for (SInt32 retIndex = 0; retIndex < (*ContainerEntries)[ContainerKey(container)][item].Durabilities.size(); retIndex++)
					{
						UInt32 tempDurComp = (*ContainerEntries)[ContainerKey(container)][item].Durabilities[retIndex].second;
						if (tempDurComp == durability && tempDurComp >(*ContainerEntries)[ContainerKey(container)][item].Durabilities[highest].second)
						{
							highest = retIndex;
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

	DurabilityTracker::ContainerValue DurabilityTracker::FindDurabilityContainer(TESObjectREFR * container, UInt32 formID)
	{
		if (ContainerEntries->count(ContainerKey(container)))
		{
			return (*ContainerEntries)[ContainerKey(container)][LookupFormByID(formID)];
		}
		else
		{
			return ContainerValue();
		}
	}

	bool DurabilityTracker::AddEntry(UInt8 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability, UInt32 maxDurability)
	{
		_MESSAGE("_DEBUG_PREF_ADEN_called");
		ContainerKey tempContKey(container);
		//GroundKey tempGroundKey(groundItem);
		switch (space)
		{
			case 0:
				/*ExtraContainerChanges * pToContRefExC = static_cast<ExtraContainerChanges*>(container->extraData.GetByType(kExtraData_ContainerChanges));
				if (!pToContRefExC){ return false; };
				EntryDataList * pToContRefExCEDL = pToContRefExC->data->objList;
				if (!pToContRefExCEDL){ return false; };*/
				_MESSAGE("_DEBUG_PREF_ADEN_1");
				//populates structure if not found

				if (ContainerEntries->count(tempContKey))
				{
					_MESSAGE("_DEBUG_PREF_ADEN_2");
					if ((*ContainerEntries)[tempContKey].count(item))
					{
						_MESSAGE("_DEBUG_PREF_ADEN_3");
						//THIS IS A PLACEHOLDER, NOT REPRESENTING FINALNESS
						(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
					}
					else
					{
						_MESSAGE("_DEBUG_PREF_ADEN_4");
						(*ContainerEntries)[tempContKey].insert(std::pair<TESForm*, ContainerValue>(item, ContainerValue(container, item, 1)));

						if ((*ContainerEntries)[tempContKey].count(item))
						{
							_MESSAGE("_DEBUG_PREF_ADEN_5");
							//THIS IS A PLACEHOLDER, NOT REPRESENTING FINALNESS
							(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
							(*ContainerEntries)[tempContKey][item].count += amount;
						}
						else
						{
							_MESSAGE("_DEBUG_PREF_ADEN_6");
							return false;
						}
					}
				}
				else
				{
					_MESSAGE("_DEBUG_PREF_ADEN_7");
					std::unordered_map<TESForm*, ContainerValue> tempContEntVal;
					_MESSAGE("_DEBUG_PREF_ADEN_7.1");
					// ### uncompacted code, probably temporary
					//tempContEntVal.insert(std::pair<TESForm*, ContainerValue>(item, ContainerValue(container, item))); //original statement
						ContainerValue t1(container, item, 1);
						_MESSAGE("_DEBUG_PREF_ADEN_7.1.1");
						std::pair<TESForm*, ContainerValue> t2(item, t1);
						_MESSAGE("_DEBUG_PREF_ADEN_7.1.2");
						tempContEntVal.insert(t2);
						_MESSAGE("_DEBUG_PREF_ADEN_7.1.3");
					// ### end uncompacted
					_MESSAGE("_DEBUG_PREF_ADEN_7.2");
					ContainerEntries->insert(std::pair<ContainerKey, std::unordered_map<TESForm*, ContainerValue> >(tempContKey, tempContEntVal));
					_MESSAGE("_DEBUG_PREF_ADEN_7.3");
					if ((*ContainerEntries)[tempContKey].count(item))
					{
						_MESSAGE("_DEBUG_PREF_ADEN_8");
						//THIS IS A PLACEHOLDER, NOT REPRESENTING FINALNESS
						(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
					}
					else
					{
						_MESSAGE("_DEBUG_PREF_ADEN_9");
						(*ContainerEntries)[tempContKey].insert(std::pair<TESForm*, ContainerValue>(item, ContainerValue(container, item, 1)));

						if ((*ContainerEntries)[tempContKey].count(item))
						{
							_MESSAGE("_DEBUG_PREF_ADEN_10");
							//THIS IS A PLACEHOLDER, NOT REPRESENTING FINALNESS
							(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
						}
						else
						{
							_MESSAGE("_DEBUG_PREF_ADEN_11");
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

	bool DurabilityTracker::RemoveEntry(UInt8 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt8 removeType, UInt32 durability)
	{
		ContainerKey tempContKey(container);
		GroundKey tempGroundKey(groundItem);

		switch (space)
		{
			case 0:
				if (ContainerEntries->count(tempContKey))
				{
					if ((*ContainerEntries)[tempContKey].count(item) && (FindEntryContainer(removeType, container, item, durability) > -1))
					{
						(*ContainerEntries)[tempContKey][item].Durabilities.erase((*ContainerEntries)[tempContKey][item].Durabilities.begin() + FindEntryContainer(removeType, container, item, durability));
						(*ContainerEntries)[tempContKey][item].count -= amount;
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
		}
	}

	bool DurabilityTracker::MoveEntry(UInt8 space, TESObjectREFR * containerFrom, TESObjectREFR * containerTo, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability)
	{
		bool c1 = false;
		bool c2 = false;
		UInt32 t_maxdurabil;
		UInt32 t_durability;
		switch (space)
		{
		case 0:
			//UInt32 t_durability = globalDurabilityTracker->FindEntryContainer(containerFrom) //don't need this because durability is passed by Scaleform to papyrus
			c1 = AddEntry(space, containerTo, item, groundItem, amount, durability, (*ContainerEntries)[ContainerKey(containerFrom)][item].Durabilities[FindEntryContainer(0, containerFrom, item, durability)].first);
			c2 = RemoveEntry(space, containerFrom, item, groundItem, amount, 0, durability);
			return c1 && c2;
		case 1:
			return false; //invalid, this doesn't make sense because obj ref is static (and coords are just pointers) so no movement "between" world space can occur
		case 2:
			if (GroundEntries->count(FindEntryGround(groundItem, 0)))
			{
				t_maxdurabil = (*GroundEntries)[FindEntryGround(groundItem, durability)].first;
				t_durability = (*GroundEntries)[FindEntryGround(groundItem, durability)].second; //need this because durability is passed by durability associated w/ specific object reference
				c2 = RemoveEntry(1, containerFrom, item, groundItem, amount, 0, t_durability);
				c1 = AddEntry(0, containerTo, groundItem->baseForm, groundItem, amount, t_durability, t_maxdurabil);
			}
			else
			{
				AddEntry(0, containerTo, groundItem->baseForm, groundItem, amount, LookupDurabilityInfo(item), LookupDurabilityInfo(item));
			}
			return c1 && c2;
		case 3:
			c1 = AddEntry(1, containerTo, item, groundItem, amount, durability, (*ContainerEntries)[ContainerKey(containerFrom)][item].Durabilities[FindEntryContainer(0, containerFrom, item, durability)].first);
			c2 = RemoveEntry(0, containerFrom, item, groundItem, amount, 0, durability);
			return c1 && c1;
		}
	}

	bool DurabilityTracker::WrapEntries(TESObjectREFR* container)
	{
		//TESContainer adds always
		TESContainer* pContainer = DYNAMIC_CAST(container->baseForm, TESForm, TESContainer);

		for (UInt32 i = 0; i < pContainer->numEntries; i++)
		{
			if (pContainer->entries[i]->form->IsWeapon() || pContainer->entries[i]->form->IsArmor())
				for (UInt32 j = 0; j < pContainer->entries[i]->count; j++)
				{
					AddEntry(0, container, pContainer->entries[i]->form, nullptr, 1, LookupDurabilityInfo(pContainer->entries[i]->form), LookupDurabilityInfo(pContainer->entries[i]->form));
				}
		}

		//container changes adds if positive count delta, takes away entries if negative count delta
		ExtraContainerChanges * pToContRefExC = static_cast<ExtraContainerChanges*>(container->extraData.GetByType(kExtraData_ContainerChanges));

		UInt32 i = 0;

		while (pToContRefExC->data->objList->GetNthItem(i))
		{
			if (pToContRefExC->data->objList->GetNthItem(i)->type->IsWeapon() || pToContRefExC->data->objList->GetNthItem(i)->type->IsArmor())
			{
				if (pToContRefExC->data->objList->GetNthItem(i)->countDelta > 0)
				{
					for (UInt32 j = 0; j < pToContRefExC->data->objList->GetNthItem(i)->countDelta; j++)
					{
						AddEntry(0, container, pToContRefExC->data->objList->GetNthItem(i)->type, nullptr, 1, LookupDurabilityInfo(pToContRefExC->data->objList->GetNthItem(i)->type), LookupDurabilityInfo(pToContRefExC->data->objList->GetNthItem(i)->type));
					}
				}
				if (pToContRefExC->data->objList->GetNthItem(i)->countDelta < 0)
				{
					for (UInt32 j = 0; j < pToContRefExC->data->objList->GetNthItem(i)->countDelta; j++)
					{
						RemoveEntry(0, container, pToContRefExC->data->objList->GetNthItem(i)->type, nullptr, 1, 1, 0);
					}
				}
			}
			i++;
		}

		return true;
	}

	//papyrus function defines

	bool InitializeDurabilityTracker(StaticFunctionTag *base)
	{
		if (!isInitialized)
		{
			globalDurabilityTracker = new DurabilityTracker();
			globalCurrentContainer = new CurrentContainer(nullptr);
			globalCurrentDurability = new CurrentDurability(0, 0);
			isInitialized = true;
		}
		return isInitialized;
	}

	//don't judge me for copying a function from stackoverflow... i mean i edited a little... stop looking at me like that!
	inline std::string int_to_hex(UInt32 i)
	{
		std::stringstream stream;
		stream << std::setfill('0') << std::setw(sizeof(UInt32) * 2) << std::hex << i;
		return stream.str();
	}

	UInt32 LookupDurabilityInfo(TESForm* item)
	{
		//_MESSAGE("_DEBUG_PREF_LODI_called function");
		//_MESSAGE("_DEBUG_PREF_LODI_%d", item->formID);
		std::ifstream DurabilitiesInfo("durabilities.txt");
		std::string foundLine;
		UInt32 durability = 0xFFFFFFFF;

		std::string tempFindString = int_to_hex(item->formID);

		//_MESSAGE("_DEBUG_PREF_LODI_%s", tempFindString.c_str());

		while (std::getline(DurabilitiesInfo, foundLine))
		{
			//_MESSAGE("_DEBUG_PREF_LODI_%s", foundLine.c_str());
			if (!foundLine.find(tempFindString))
			{
				std::stringstream ss;
				ss << std::hex << foundLine.substr(10, 8);
				ss >> durability;
				//_MESSAGE("_DEBUG_PREF_LODI_found durability: %d", durability);
			}
		}
		DurabilitiesInfo.close();
		return durability;
	}

	bool AddEntry(StaticFunctionTag *base, UInt32 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability, UInt32 maxDurability)
	{
		return globalDurabilityTracker->AddEntry(space, container, item, groundItem, amount, durability, maxDurability);
	}

	bool RemoveEntry(StaticFunctionTag *base, UInt32 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 removeType, UInt32 durability)
	{
		return globalDurabilityTracker->RemoveEntry(space, container, item, groundItem, amount, removeType, durability);
	}

	bool MoveEntry(StaticFunctionTag *base, UInt32 space, TESObjectREFR * containerFrom, TESObjectREFR * containerTo, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability)
	{
		return globalDurabilityTracker->MoveEntry(space, containerFrom, containerTo, item, groundItem, amount, durability);
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

	void PrintFound(StaticFunctionTag *base, UInt32 findType, TESObjectREFR * container, TESForm * item, UInt32 durability)
	{
		_MESSAGE("_DEBUG_PREF_PFND_called");
		SInt32 found = globalDurabilityTracker->FindEntryContainer(findType, container, item, durability);
		_MESSAGE("_DEBUG_PREF_PFND_found dur: %d", (*(globalDurabilityTracker->ContainerEntries))[container][item].Durabilities[found].second);
		UInt32 np = LookupDurabilityInfo(item);
		_MESSAGE("_DEBUG_PREF_PFND_np:%d", np);
	}

	UInt32 GetCurrentDurability(StaticFunctionTag *base)
	{
		return globalCurrentDurability->GetDurability();
	};

	UInt32 GetCurrentMaxDurability(StaticFunctionTag *base)
	{
		return globalCurrentDurability->GetMaxDurability();
	};

	//old, bad stuff, but keeping it around because of nostalgia 
	/*void MarkTESContainerZero(StaticFunctionTag *base, TESObjectREFR* contRef)
	{
		plugin_spis_utils::MarkTESContainerZero(contRef);
	}

	void outputToOpenDebugLog(StaticFunctionTag *base, BSFixedString msg)
	{
		_MESSAGE(msg.data);
		return;
	}

	void GetContainerReady(StaticFunctionTag *base, TESObjectREFR* contRef)
	{
		plugin_spis_utils::CopyTESContainerContentsToExtraDataUnst(contRef);
		plugin_spis_utils::MarkTESContainerZero(contRef);
	}

	void UpdateContainer(StaticFunctionTag *base, TESObjectREFR* contRef)
	{
		plugin_spis_utils::UpdateContainer(contRef);
	}*/

	//UI functions
	//keeping this here for now so it can interact with globalDurabilityTracker
	//theres probably a better way to do this, perhaps look back at this later. for now it works
	class SKSEScaleform_GetExtras : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args)
		{
			_MESSAGE("_DEBUG_PREF_SCDG_invoked");
			/*DurabilityTracker::ContainerValue entries;
			GFxValue* entriesArray = &(args->args[0]);
			GFxValue currentElement;
			GFxValue lastElement;
			GFxValue currentId;
			GFxValue lastId;
			UInt32 currentIndex = 0;
			bool equippedRset = false;
			bool equippedLset = false;
			_MESSAGE("_DEBUG_PREF_SCDG_1");
			//GFxValue* out_array = &(args->args[1]); //idk, before it was nagging me, so maybe this works? I'm the only one that has to deal with it so w/e
			//GFxValue* tempObj = &(args->args[2]);
			//GFxValue* tempNum = &(args->args[3]);
			//GFxValue* tempBool = &(args->args[4]);
			//send maxdur in array
			/*for (UInt32 i = 0; i < entries.size(); i++)
			{
				entries[i].first
				tempObj->SetMember("maxDurability", );
				out_array->PushBack(tempNum);
			}
			//first run, just unrolling the loop a little so no extra if's needed
			entriesArray->GetElement(0, &currentElement); _MESSAGE("_DEBUG_PREF_SCDG_2");
			currentElement.GetMember("formId", &currentId); _MESSAGE("_DEBUG_PREF_SCDG_3");
			if ((*globalDurabilityTracker->ContainerEntries)[globalCurrentContainer->GetContainer()].count(LookupFormByID( UInt32( currentId.GetNumber() ) ))); //make sure that item exists w/ count
			{
				entries = globalDurabilityTracker->FindDurabilityContainer(globalCurrentContainer->GetContainer(), UInt32(currentId.GetNumber())); _MESSAGE("_DEBUG_PREF_SCDG_4");
				RegisterNumber(&currentElement, "maxDurability", double(entries.Durabilities[currentIndex].first)); _MESSAGE("_DEBUG_PREF_SCDG_5");
				RegisterNumber(&currentElement, "durability", double(entries.Durabilities[currentIndex].second)); _MESSAGE("_DEBUG_PREF_SCDG_5.1");
			}

			for (UInt32 i = 1; i < entriesArray->GetArraySize(); i++)
			{
				lastElement = currentElement; _MESSAGE("_DEBUG_PREF_SCDG_6,%d", i);
				entriesArray->GetElement(i, &currentElement); _MESSAGE("_DEBUG_PREF_SCDG_7,%d", i);

				lastElement.GetMember("formId", &lastId); _MESSAGE("_DEBUG_PREF_SCDG_8,&d", i);
				currentElement.GetMember("formId", &currentId); _MESSAGE("_DEBUG_PREF_SCDG_9,%d", i);

				entries = globalDurabilityTracker->FindDurabilityContainer(globalCurrentContainer->GetContainer(), UInt32(currentId.GetNumber())); _MESSAGE("_DEBUG_PREF_SCDG_10,%d", i);

				if ((*globalDurabilityTracker->ContainerEntries)[globalCurrentContainer->GetContainer()].count(LookupFormByID(UInt32(currentId.GetNumber()))))
				{
					if (lastId.GetNumber() == currentId.GetNumber())
					{
						currentIndex++; _MESSAGE("_DEBUG_PREF_SCDG_11,%d", i);
					}
					else
					{
						currentIndex = 0; _MESSAGE("_DEBUG_PREF_SCDG_12,%d", i);
					}

					_MESSAGE("_DEBUG_PREF_SCDG_12.1,%d, arraysize:%d, index:%d", i, entries.Durabilities.size(), currentIndex);
					_MESSAGE("_DEBUG_PREF_SCDG_12.2,%d, lastid:%d, currentid:%d", i, lastId.GetNumber(), currentId.GetNumber());

					RegisterNumber(&currentElement, "maxDurability", double(entries.Durabilities[currentIndex].first)); _MESSAGE("_DEBUG_PREF_SCDG_13,%d", i);
					RegisterNumber(&currentElement, "durability", double(entries.Durabilities[currentIndex].second)); _MESSAGE("_DEBUG_PREF_SCDG_14,%d", i);

					/*if (std::get<0>(entries.EquippedState) == entries.Durabilities[currentIndex].second)
					{
						if (!equippedRset)
						{
							GFxValue * equipState; _MESSAGE("_DEBUG_PREF_SCDG_15,%d", i);
							currentElement.GetMember("equipState", equipState); _MESSAGE("_DEBUG_PREF_SCDG_16,%d", i);
							equipState->SetNumber(double(3)); _MESSAGE("_DEBUG_PREF_SCDG_17,%d", i);
							currentElement.SetMember("equipState", equipState); _MESSAGE("_DEBUG_PREF_SCDG_18,%d", i);
							equippedRset = true; _MESSAGE("_DEBUG_PREF_SCDG_19,%d", i);
						}
						/*else if (!equippedLset)
						{
							GFxValue * equipState;
							currentElement->GetMember("equipState", equipState);
							equipState->SetNumber(double(2));
							currentElement->SetMember("equipState", equipState);
							equippedLset = true;
						}
					}
					else if (std::get<2>(entries.EquippedState) == entries.Durabilities[currentIndex].second)
					{
						/*if (!equippedRset)
						{
							GFxValue * equipState;
							currentElement->GetMember("equipState", equipState);
							equipState->SetNumber(double(3));
							currentElement->SetMember("equipState", equipState);
							equippedRset = true;
						}
						if (!equippedLset)
						{
							GFxValue * equipState; _MESSAGE("_DEBUG_PREF_SCDG_20,&d", i);
							currentElement.GetMember("equipState", equipState); _MESSAGE("_DEBUG_PREF_SCDG_21,&d", i);
							equipState->SetNumber(double(2)); _MESSAGE("_DEBUG_PREF_SCDG_22,&d", i);
							currentElement.SetMember("equipState", equipState); _MESSAGE("_DEBUG_PREF_SCDG_23,&d", i);
							equippedLset = true; _MESSAGE("_DEBUG_PREF_SCDG_24,&d", i);
						}
					}
				}
				else
				{
					currentIndex = 0; _MESSAGE("_DEBUG_PREF_SCDG_25,&d", i);
				}
			}*/
			GFxValue * obj		= &args->args[0];
			GFxValue * index	= &args->args[1];
			GFxValue   formId;
			obj->GetMember("formId", &formId);
			_MESSAGE("_DEBUG_PREF_SCDG_formId:%d", UInt32(formId.GetNumber()));
			// ### UNCOMPRESSED CODE
			UInt32 tid = UInt32(formId.GetNumber());
			TESObjectREFR* cc = globalCurrentContainer->GetContainer();
			//_MESSAGE("_DEBUG_PREF_SCDG_ccfId:%d", cc->baseForm->formID);
			// ### UNCOMPRESSED CODE END
			if ((*globalDurabilityTracker->ContainerEntries)[cc].count(LookupFormByID(tid)))
			{
				DurabilityTracker::ContainerValue entries = globalDurabilityTracker->FindDurabilityContainer(globalCurrentContainer->GetContainer(), UInt32(formId.GetNumber()));

				UInt32 ind = index->GetNumber();

				_MESSAGE("_DEBUG_PREF_SCDG_formId:%d", UInt32(formId.GetNumber()));
				_MESSAGE("_DEBUG_PREF_SCDG_ind:%d", ind);
				_MESSAGE("_DEBUG_PREF_SCDG_md:%d", entries.Durabilities[ind].first);
				_MESSAGE("_DEBUG_PREF_SCDG_d:%d", entries.Durabilities[ind].second);

				RegisterNumber(obj, "maxDurability", entries.Durabilities[ind].first);
				RegisterNumber(obj, "durability", entries.Durabilities[ind].second);
			}
		}
	};

	class SKSEScaleform_SetCurrentMaxDurability : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args)
		{
			globalCurrentDurability->SetMaxDurability(args->args[0].GetNumber());
		}
	};

	class SKSEScaleform_SetCurrentDurability : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args)
		{
			globalCurrentDurability->SetDurability(args->args[0].GetNumber());
		}
	};

	//SERIALIZATION TES ########################
	void Serialization_Revert(SKSESerializationInterface * intfc)
	{
		_MESSAGE("revert");
	}

	const UInt32 kSerializationDataVersion = 1;

	void Serialization_Save(SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_invoked");
		SerializeGnrlContainerMap(globalDurabilityTracker->ContainerEntries, intfc);
	}
	/*{
		_MESSAGE("save");

		if (intfc->OpenRecord('DATA', kSerializationDataVersion))
		{
			char	kData[] = "hello world";

			intfc->WriteRecordData(kData, sizeof(kData));
		}
	}*/

	void Serialization_Load(SKSESerializationInterface * intfc)
	{
		_MESSAGE("load");

		UInt32	type;
		UInt32	version;
		UInt32	length;
		bool	error = false;

		while (!error && intfc->GetNextRecordInfo(&type, &version, &length))
		{
			switch (type)
			{
			case 'DATA':
			{
				if (version == kSerializationDataVersion)
				{
					if (length)
					{
						char	* buf = new char[length];

						intfc->ReadRecordData(buf, length);
						buf[length - 1] = 0;

						_MESSAGE("read data: %s", buf);
					}
					else
					{
						_MESSAGE("empty data?");
					}
				}
				else
				{
					error = true;
				}
			}
			break;

			default:
				_MESSAGE("unhandled type %08X", type);
				error = true;
				break;
			}
		}
	}

	//endtests

	//registry functions

	bool RegisterFuncsPapyrus(VMClassRegistry* registry)
	{
		/*
		//registry->RegisterFunction(new NativeFunction2 <StaticFunctionTag, void, TESObjectREFR*, TESForm*>("RearrangeExtraContChanges", "plugin_spis", plugin_spis::RearrangeExtraContChanges, registry));

		//registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, void, TESObjectREFR*, TESObjectREFR*, TESObjectWEAP*, EnchantmentItem*>("spisDebug", "plugin_spis", plugin_spis::spisDebug, registry));

		//registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESObjectREFR*>("UpdateContainer", "plugin_spis", plugin_spis::UpdateContainer, registry));

		//registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESObjectREFR*>("GetContainerReady", "plugin_spis", plugin_spis::GetContainerReady, registry));

		//registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, BSFixedString>("outputToOpenDebugLog", "plugin_spis", plugin_spis::outputToOpenDebugLog, registry));

		//registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, TESObjectREFR*>("MarkTESContainerZero", "plugin_spis", plugin_spis::MarkTESContainerZero, registry));
		*/
		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, bool>("InitializeDurabilityTracker", "plugin_spis", plugin_spis::InitializeDurabilityTracker, registry));

		registry->RegisterFunction(new NativeFunction7 <StaticFunctionTag, bool, UInt32, TESObjectREFR*, TESForm*, TESObjectREFR*, UInt32, UInt32, UInt32>("AddEntry", "plugin_spis", plugin_spis::AddEntry, registry));

		registry->RegisterFunction(new NativeFunction7 <StaticFunctionTag, bool, UInt32, TESObjectREFR*, TESForm*, TESObjectREFR*, UInt32, UInt32, UInt32>("RemoveEntry", "plugin_spis", plugin_spis::RemoveEntry, registry));

		registry->RegisterFunction(new NativeFunction7 <StaticFunctionTag, bool, UInt32, TESObjectREFR*, TESObjectREFR*, TESForm*, TESObjectREFR*, UInt32, UInt32>("MoveEntry", "plugin_spis", plugin_spis::MoveEntry, registry));

		registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, void, UInt32, TESObjectREFR*, TESForm*, UInt32>("PrintFound", "plugin_spis", plugin_spis::PrintFound, registry));

		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, bool, TESObjectREFR*>("WrapEntries", "plugin_spis", plugin_spis::WrapEntries, registry));

		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, bool, TESObjectREFR*>("SetCurrentContainer", "plugin_spis", plugin_spis::SetCurrentContainer, registry));

		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, TESObjectREFR*>("GetCurrentContainer", "plugin_spis", plugin_spis::GetCurrentContainer, registry));

		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("GetCurrentDurability", "plugin_spis", plugin_spis::GetCurrentDurability, registry));

		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("GetCurrentMaxDurability", "plugin_spis", plugin_spis::GetCurrentMaxDurability, registry));

		//registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, void, BSFixedString>("outputToOpenDebugLog", "plugin_spis", plugin_spis::outputToOpenDebugLog, registry));

		return true;
	};

	bool RegisterFuncsScaleform(GFxMovieView * view, GFxValue * root)
	{
		//RegisterFunction <SKSEScaleform_ExampleFunction>(root, view, "ExampleFunction");

		RegisterFunction <SKSEScaleform_GetExtras>(root, view, "GetExtras");

		RegisterFunction <SKSEScaleform_SetCurrentMaxDurability>(root, view, "SetCurrentMaxDurability");

		RegisterFunction <SKSEScaleform_SetCurrentDurability>(root, view, "SetCurrentDurability");

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