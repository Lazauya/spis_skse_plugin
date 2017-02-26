#include "skse/ScaleformCallbacks.h"
#include "skse/ScaleformMovie.h"
#include "skse/PapyrusForm.h"

#include <shlobj.h>

//circular inlcude
#include "SplitItemStacks.h"

#ifndef uiext
#define uiext

/* function GetExtras(outObject: Object): Void
gets some info that globalDurabilityTrackerProvides
outObject MUST have:
	formId
	jth (this is the jth object with this formId, if you need a better xpl, ask)
outObject recieves:
	durability
	maxDurability
	nth
	equipState (its modified from original value)
*/
void GetExtras(GFxFunctionHandler::Args * args, plugin_spis::DurabilityTracker * dt, plugin_spis::CurrentContainer * cc, plugin_spis::EquippedStates * es)
{
	GFxValue * obj = &(args->args[0]);

	GFxValue jthTemp;
	GFxValue formIDTemp;

	obj->GetMember("jth", &jthTemp);
	obj->GetMember("formId", &formIDTemp);

	UInt32 formID = UInt32(formIDTemp.GetNumber());
	UInt32 jth = UInt32(jthTemp.GetNumber());

	std::vector<std::pair< UInt32, UInt32 > > durabilities = (*dt->ContainerEntries)[cc->GetContainer()][LookupFormByID(formID)].Durabilities;

	//get durability info
	UInt32 maxDurability = durabilities[jth].first;
	UInt32 durability = durabilities[jth].second;

	RegisterNumber(obj, "maxDurability", double(maxDurability));
	RegisterNumber(obj, "durability", double(durability));

	//get equipped info

	//calculate/get nth
	UInt32 nth = 0;

	for (SInt32 i = SInt32(jth); i >= 0; i--)
	{
		if ((durabilities[jth].first == durabilities[i].first) && (durabilities[jth].second == durabilities[i].second))
		{
			nth++;
		}
	}

	nth--;
		
	RegisterNumber(obj, "nth", double(nth));

	//set equipped state
	UInt8 equipState = es->FindInList(LookupFormByID(formID), durability, maxDurability, nth);

	GFxValue equipStateTemp;
	equipStateTemp.SetNumber(double(equipState));
	obj->SetMember("equipState", &equipStateTemp);
}

/* function SetEquippedState(itemToSet: Object, hand: Number): Void
equips/unequips the object in globalEquippedStates
itemToSet object MUST have:
	formId
	nth
	maxDurability
	durability

hand can be:
	0 - right
	1 - left
hand does not matter for 2 handed weapons and armor
*/
void SetEquippedState(GFxFunctionHandler::Args * args, plugin_spis::EquippedStates * es)
{
	GFxValue * obj = &(args->args[0]);
	GFxValue * handTemp = &(args->args[1]);

	GFxValue formIDTemp;
	GFxValue nthTemp;
	GFxValue maxDurabilityTemp;
	GFxValue durabilityTemp;

	obj->GetMember("formId", &formIDTemp);
	obj->GetMember("nth", &nthTemp);
	obj->GetMember("maxDurability", &maxDurabilityTemp);
	obj->GetMember("durability", &durabilityTemp);

	UInt32 nth = UInt32(nthTemp.GetNumber());
	UInt32 formID = UInt32(formIDTemp.GetNumber());
	UInt32 maxDurability = UInt32(maxDurabilityTemp.GetNumber());
	UInt32 durability = UInt32(durabilityTemp.GetNumber());
	UInt32 hand = UInt32(handTemp->GetNumber());

	es->SetState(LookupFormByID(formID), durability, maxDurability, hand, nth);
}

/* function SetCurrentDurabilities(itemToSet: Object): Void
sets the durability and maxDurability that papyrus uses to make descisions
itemToSet MUST have:
	durability
	maxDurability
*/
void SetCurrentDurabilities(GFxFunctionHandler::Args * args, plugin_spis::CurrentDurability * cc)
{
	GFxValue * obj = &(args->args[0]);

	GFxValue maxDurabilityTemp;
	GFxValue durabilityTemp;

	obj->GetMember("maxDurability", &maxDurabilityTemp);
	obj->GetMember("durability", &durabilityTemp);

	UInt32 maxDurability = UInt32(maxDurabilityTemp.GetNumber());
	UInt32 durability = UInt32(durabilityTemp.GetNumber());

	cc->SetDurability(durability);
	cc->SetMaxDurability(maxDurability);
}

/* function IncrementDurability(itemToIncrement: Object, amount: Number): Void
adds/removes durability from items
itemToIncrement object MUST have:
	formId
	nth
	maxDurability
	durability
*/
void IncrementDurability(GFxFunctionHandler::Args * args, plugin_spis::DurabilityTracker * dt, plugin_spis::CurrentContainer * cc)
{
	GFxValue * obj = &(args->args[0]);
	GFxValue * amt = &(args->args[1]);
	
	GFxValue formIDTemp;
	GFxValue nthTemp;
	GFxValue maxDurabilityTemp;
	GFxValue durabilityTemp;

	obj->GetMember("formId", &formIDTemp);
	obj->GetMember("nth", &nthTemp);
	obj->GetMember("maxDurability", &maxDurabilityTemp);
	obj->GetMember("durability", &durabilityTemp);

	UInt32 nth = UInt32(nthTemp.GetNumber());
	UInt32 formID = UInt32(formIDTemp.GetNumber());
	UInt32 maxDurability = UInt32(maxDurabilityTemp.GetNumber());
	UInt32 durability = UInt32(durabilityTemp.GetNumber());
	SInt32 amount = SInt32(amt->GetNumber());
	
	SInt32 pos = dt->FindEntryContainer(0, cc->GetContainer(), LookupFormByID(formID), durability, maxDurability, nth);
	if (pos > -1)
	{
		(*dt->ContainerEntries)[cc->GetContainer()][LookupFormByID(formID)].Durabilities[pos].second += amount;
	}
}

/* function GetRepairList(outArray: Array): Void
out array is the array that is modified
objects in outArray have:
	name
	nth (the nth item with this durability, if you need a better expl, ask)
	formId
	durability
*/
void GetRepairList(GFxFunctionHandler::Args * args, plugin_spis::DurabilityTracker * dt, plugin_spis::CurrentContainer * cc)
{
	GFxValue * entryArray = &(args->args[0]);

	GFxValue name;
	GFxValue durability;
	GFxValue formID;
	GFxValue nth;

	UInt32 n = 0;

	auto itt = (*dt->ContainerEntries)[cc->GetContainer()].begin();

	for (auto it = (*dt->ContainerEntries)[cc->GetContainer()].begin(); it != (*dt->ContainerEntries)[cc->GetContainer()].end(); it++)
	{
		GFxValue entryElement;
		args->movie->CreateObject(&entryElement);

		n = 0;

		for (UInt32 i = 0; i < it->second.Durabilities.size(); i++)
		{
			for (SInt32 j = SInt32(i); j >= 0; j--)
			{
				if (it->second.Durabilities[i] == it->second.Durabilities[j])
				{
					n++;
				}
			}

			n--;
				
			RegisterNumber(&entryElement, "nth", double(n));
			RegisterString(&entryElement, args->movie, "name", papyrusForm::GetName(it->first).data);
			RegisterNumber(&entryElement, "formId", double(it->first->formID));
			RegisterNumber(&entryElement, "durability", double(it->second.Durabilities[i].second));

			_MESSAGE("_DB_SCRL_1");
			entryArray->PushBack(&entryElement);
			_MESSAGE("_DB_SCRL_2");
		}
	}
}

#endif