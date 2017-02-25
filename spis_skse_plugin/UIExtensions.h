#include "skse/ScaleformCallbacks.h"
#include "skse/ScaleformMovie.h"

#include <shlobj.h>

//circular inlcude
#include "SplitItemStacks.h"

#ifndef uiext
#define uiext

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

	for (UInt32 i = jth; i > 0; i--)
	{
		if ((durabilities[jth].first == durabilities[i].first) && (durabilities[jth].second == durabilities[i].second))
		{
			nth++;
		}
	}
		
	RegisterNumber(obj, "nth", double(nth));

	//set equipped state
	UInt8 equipState = es->FindInList(LookupFormByID(formID), durability, maxDurability, nth);

	GFxValue equipStateTemp;
	equipStateTemp.SetNumber(double(equipState));
	obj->SetMember("equipState", &equipStateTemp);
}

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


#endif