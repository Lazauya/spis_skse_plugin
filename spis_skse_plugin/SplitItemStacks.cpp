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
	
	
	//global tracker, created at runtime

	DurabilityTracker * globalDurabilityTracker;
	CurrentContainer * globalCurrentContainer;
	CurrentDurability * globalCurrentDurability;
	EquippedStates * globalEquippedStates;
	bool isInitialized = false;

	//durabilitytracker function defines, some are deprecated and commented (because I'm a code hoarder)

	DurabilityTracker::DurabilityTracker()
	{
		//_MESSAGE("_DEBUG_PREF_INIT_init called");
		GroundEntries = new std::unordered_map< GroundKey, GroundValue, GroundKeyHash >;
		//_MESSAGE("_DEBUG_PPEF_INIT_ground entries just declared");
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
		if (!amount)
		{
			return true;
		}

		//_MESSAGE("_DEBUG_PREF_ADEN_called");
		ContainerKey tempContKey(container);
		//GroundKey tempGroundKey(groundItem);
		switch (space)
		{
			case 0:
				/*ExtraContainerChanges * pToContRefExC = static_cast<ExtraContainerChanges*>(container->extraData.GetByType(kExtraData_ContainerChanges));
				if (!pToContRefExC){ return false; };
				EntryDataList * pToContRefExCEDL = pToContRefExC->data->objList;
				if (!pToContRefExCEDL){ return false; };*/
				//_MESSAGE("_DEBUG_PREF_ADEN_1");
				//populates structure if not found

				if (ContainerEntries->count(tempContKey))
				{
					//_MESSAGE("_DEBUG_PREF_ADEN_2");
					if ((*ContainerEntries)[tempContKey].count(item))
					{
						//_MESSAGE("_DEBUG_PREF_ADEN_3");
						//THIS IS A PLACEHOLDER, NOT REPRESENTING FINALNESS
						(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
					}
					else
					{
						//_MESSAGE("_DEBUG_PREF_ADEN_4");
						(*ContainerEntries)[tempContKey].insert(std::pair<TESForm*, ContainerValue>(item, ContainerValue(item, 1)));

						if ((*ContainerEntries)[tempContKey].count(item))
						{
							//_MESSAGE("_DEBUG_PREF_ADEN_5");
							//THIS IS A PLACEHOLDER, NOT REPRESENTING FINALNESS
							(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
							(*ContainerEntries)[tempContKey][item].count += amount;
						}
						else
						{
							//_MESSAGE("_DEBUG_PREF_ADEN_6");
							return false;
						}
					}
				}
				else
				{
					//_MESSAGE("_DEBUG_PREF_ADEN_7");
					std::unordered_map<TESForm*, ContainerValue> tempContEntVal;
					//_MESSAGE("_DEBUG_PREF_ADEN_7.1");
					// ### uncompacted code, probably temporary
					//tempContEntVal.insert(std::pair<TESForm*, ContainerValue>(item, ContainerValue(container, item))); //original statement
						ContainerValue t1(item, 1);
						//_MESSAGE("_DEBUG_PREF_ADEN_7.1.1");
						std::pair<TESForm*, ContainerValue> t2(item, t1);
						//_MESSAGE("_DEBUG_PREF_ADEN_7.1.2");
						tempContEntVal.insert(t2);
						//_MESSAGE("_DEBUG_PREF_ADEN_7.1.3");
					// ### end uncompacted
					//_MESSAGE("_DEBUG_PREF_ADEN_7.2");
					ContainerEntries->insert(std::pair<ContainerKey, std::unordered_map<TESForm*, ContainerValue> >(tempContKey, tempContEntVal));
					//_MESSAGE("_DEBUG_PREF_ADEN_7.3");
					if ((*ContainerEntries)[tempContKey].count(item))
					{
						//_MESSAGE("_DEBUG_PREF_ADEN_8");
						//THIS IS A PLACEHOLDER, NOT REPRESENTING FINALNESS
						(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
					}
					else
					{
						//_MESSAGE("_DEBUG_PREF_ADEN_9");
						(*ContainerEntries)[tempContKey].insert(std::pair<TESForm*, ContainerValue>(item, ContainerValue(item, 1)));

						if ((*ContainerEntries)[tempContKey].count(item))
						{
							//_MESSAGE("_DEBUG_PREF_ADEN_10");
							//THIS IS A PLACEHOLDER, NOT REPRESENTING FINALNESS
							(*ContainerEntries)[tempContKey][item].Durabilities.push_back(ContainerValue::DurabilityEntry(maxDurability, durability));
						}
						else
						{
							//_MESSAGE("_DEBUG_PREF_ADEN_11");
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
		if (!amount)
		{
			return true;
		}

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
		if (!amount)
		{
			return true;
		}

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
					_MESSAGE("_DEBUG_PREF_WETC_%d,%d", j, pContainer->entries[i]->form->formID);
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
						_MESSAGE("_DEBUG_PREF_WEAE_%d,%d", j, pContainer->entries[i]->form->formID);
						AddEntry(0, container, pToContRefExC->data->objList->GetNthItem(i)->type, nullptr, 1, LookupDurabilityInfo(pToContRefExC->data->objList->GetNthItem(i)->type), LookupDurabilityInfo(pToContRefExC->data->objList->GetNthItem(i)->type));
					}
				}
				if (pToContRefExC->data->objList->GetNthItem(i)->countDelta < 0)
				{
					for (UInt32 j = 0; j < pToContRefExC->data->objList->GetNthItem(i)->countDelta; j++)
					{
						_MESSAGE("_DEBUG_PREF_WERE_%d,%d", j, pContainer->entries[i]->form->formID);
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
			globalEquippedStates = new EquippedStates;
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
		////_MESSAGE("_DEBUG_PREF_LODI_called function");
		////_MESSAGE("_DEBUG_PREF_LODI_%d", item->formID);
		std::ifstream DurabilitiesInfo("durabilities.txt");
		std::string foundLine;
		UInt32 durability = 0xFFFFFFFF;

		std::string tempFindString = int_to_hex(item->formID);

		////_MESSAGE("_DEBUG_PREF_LODI_%s", tempFindString.c_str());

		while (std::getline(DurabilitiesInfo, foundLine))
		{
			////_MESSAGE("_DEBUG_PREF_LODI_%s", foundLine.c_str());
			if (!foundLine.find(tempFindString))
			{
				std::stringstream ss;
				ss << std::hex << foundLine.substr(10, 8);
				ss >> durability;
				////_MESSAGE("_DEBUG_PREF_LODI_found durability: %d", durability);
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
		//_MESSAGE("_DEBUG_PREF_PFND_called");
		SInt32 found = globalDurabilityTracker->FindEntryContainer(findType, container, item, durability);
		//_MESSAGE("_DEBUG_PREF_PFND_found dur: %d", (*(globalDurabilityTracker->ContainerEntries))[container][item].Durabilities[found].second);
		UInt32 np = LookupDurabilityInfo(item);
		//_MESSAGE("_DEBUG_PREF_PFND_np:%d", np);
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

	//UI functions
	//keeping this here for now so it can interact with globalDurabilityTracker
	//theres probably a better way to do this, perhaps look back at this later. for now it works
	class SKSEScaleform_GetExtras : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args)
		{
			//_MESSAGE("_DEBUG_PREF_SCDG_invoked");
			
			GFxValue * obj		= &args->args[0];
			GFxValue * index	= &args->args[1];
			GFxValue   formId;
			obj->GetMember("formId", &formId);
			//_MESSAGE("_DEBUG_PREF_SCDG_formId:%d", UInt32(formId.GetNumber()));
			// ### UNCOMPRESSED CODE
			UInt32 tid = UInt32(formId.GetNumber());
			TESObjectREFR* cc = globalCurrentContainer->GetContainer();
			////_MESSAGE("_DEBUG_PREF_SCDG_ccfId:%d", cc->baseForm->formID);
			// ### UNCOMPRESSED CODE END
			if ((*globalDurabilityTracker->ContainerEntries)[cc].count(LookupFormByID(tid)))
			{
				DurabilityTracker::ContainerValue entries = globalDurabilityTracker->FindDurabilityContainer(globalCurrentContainer->GetContainer(), UInt32(formId.GetNumber()));

				UInt32 ind = index->GetNumber();

				//_MESSAGE("_DEBUG_PREF_SCDG_formId:%d", UInt32(formId.GetNumber()));
				//_MESSAGE("_DEBUG_PREF_SCDG_ind:%d", ind);
				//_MESSAGE("_DEBUG_PREF_SCDG_md:%d", entries.Durabilities[ind].first);
				//_MESSAGE("_DEBUG_PREF_SCDG_d:%d", entries.Durabilities[ind].second);

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

	class SKSEScaleform_GetEquippedState : public GFxFunctionHandler
	{
		virtual void Invoke(Args * args)
		{
			GFxValue * obj = &(args->args[0]);
			GFxValue tmaxdur;
			GFxValue tdur;
			GFxValue tformid;
			GFxValue equipState;
			obj->GetMember("maxDurability", &tmaxdur);
			obj->GetMember("durability", &tdur);
			obj->GetMember("formId", &tformid);
			equipState.SetNumber(globalEquippedStates->FindInList(LookupFormByID(tformid.GetNumber()), tdur.GetNumber(), tmaxdur.GetNumber()));
			obj->SetMember("equipState", &equipState);
			
		}
	};

	//serialization functions

	void Serialization_Revert(SKSESerializationInterface * intfc)
	{
		_MESSAGE("revert");
	}

	const UInt32 kSerializationDataVersion = 0;

	void Serialization_Save(SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_invoked");
		SerializeGnrlContainerMap(globalDurabilityTracker->ContainerEntries, intfc); _MESSAGE("_DEBUG_PREF_SAVE_1");
		SerializeGroundMap(globalDurabilityTracker->GroundEntries, intfc); _MESSAGE("_DEBUG_PREF_SAVE_2");
		SerializeEquippedStates(intfc, globalEquippedStates); _MESSAGE("_DEBUG_PREF_SAVE_3");
		SerializeCurrentContainer(intfc, globalCurrentContainer); _MESSAGE("_DEBUG_PREF_SAVE_4");
		SerializeCurrentDurability(intfc, globalCurrentDurability); _MESSAGE("_DEBUG_PREF_SAVE_5");
		intfc->OpenRecord(kTypeInitialized, 0); _MESSAGE("_DEBUG_PREF_SAVE_6");
		intfc->WriteRecordData(&isInitialized, sizeof(bool));
	}

	void Serialization_Load(SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_LOAD_invoked");
		UInt32 type, ver, len;
		bool ii;
		while (intfc->GetNextRecordInfo(&type, &ver, &len))
		{
			//_MESSAGE("_DEBUG_PREF_LOAD_1");
			switch(type)
			{
			case kTypeContainerMap:
				//_MESSAGE("_DEBUG_PREF_LOAD_2");
				*(globalDurabilityTracker->ContainerEntries) = UnserializeGnrlContianerMap(intfc);
				break;
			case kTypeGroundMap:
				//_MESSAGE("_DEBUG_PREF_LOAD_2.5");
				
				*(globalDurabilityTracker->GroundEntries) = UnserializeGroundMap(intfc);
				break;
			case kTypeInitialized:
				//_MESSAGE("_DEBUG_PREF_LOAD_3");
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

		registry->RegisterFunction(new NativeFunction7 <StaticFunctionTag, bool, UInt32, TESObjectREFR*, TESForm*, TESObjectREFR*, UInt32, UInt32, UInt32>("RemoveEntry", "plugin_spis", plugin_spis::RemoveEntry, registry));

		registry->RegisterFunction(new NativeFunction7 <StaticFunctionTag, bool, UInt32, TESObjectREFR*, TESObjectREFR*, TESForm*, TESObjectREFR*, UInt32, UInt32>("MoveEntry", "plugin_spis", plugin_spis::MoveEntry, registry));

		registry->RegisterFunction(new NativeFunction4 <StaticFunctionTag, void, UInt32, TESObjectREFR*, TESForm*, UInt32>("PrintFound", "plugin_spis", plugin_spis::PrintFound, registry));

		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, bool, TESObjectREFR*>("WrapEntries", "plugin_spis", plugin_spis::WrapEntries, registry));

		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, bool, TESObjectREFR*>("SetCurrentContainer", "plugin_spis", plugin_spis::SetCurrentContainer, registry));

		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, TESObjectREFR*>("GetCurrentContainer", "plugin_spis", plugin_spis::GetCurrentContainer, registry));

		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("GetCurrentDurability", "plugin_spis", plugin_spis::GetCurrentDurability, registry));

		registry->RegisterFunction(new NativeFunction0 <StaticFunctionTag, UInt32>("GetCurrentMaxDurability", "plugin_spis", plugin_spis::GetCurrentMaxDurability, registry));

		registry->RegisterFunction(new NativeFunction1 <StaticFunctionTag, bool, TESObjectREFR*>("WrapEquipStates", "plugin_spis", plugin_spis::WrapEquipStates, registry));

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