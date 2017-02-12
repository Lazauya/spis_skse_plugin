//#include "SplitItemStacks.h"
#include "skse/GameRTTI.h"
#include "skse/GameObjects.h"
#include "skse/GameData.h"
#include "skse/GameTypes.h"
//#include "skse/GameAPI.h"

#include <vector>
#include <utility>

typedef tList<ExtraContainerChanges::EntryData> EntryDataList;
//typedef std::vector<std::pair<std::pair<ExtraContainerChanges::EntryData, UInt32>, UInt32> > DurabilityVector;


namespace plugin_spis_utils
{
	//simple inline function for making sure entrydata is armor/weapon. kinda unessecary, but whatever
	inline bool checkEDType(ExtraContainerChanges::EntryData * ed)
	{
		return ed->type->IsArmor() || ed->type->IsWeapon();
	}

	//makes the item count variable zero of the tescontainer obj so the game ignores hardassigned objects
	void MarkTESContainerZero(TESObjectREFR* contRef)
	{
		TESContainer* pContainer = NULL;
		TESForm* pBaseForm = contRef->baseForm;

		if (pBaseForm)
		{
			pContainer = DYNAMIC_CAST(pBaseForm, TESForm, TESContainer);
		}
		else
		{
			return;
		}

		pContainer->numEntries = 0;
	}

	//simple implement for the moment, will include extra cases later - should be easy
	void UpdateContainer(TESObjectREFR* contRef)
	{
		ExtraContainerChanges * pToContRefExC = static_cast<ExtraContainerChanges*>(contRef->extraData.GetByType(kExtraData_ContainerChanges));
		if (!pToContRefExC){ return; };
		EntryDataList * pToContRefExCEDL = pToContRefExC->data->objList;
		if (!pToContRefExCEDL){ return; };

		UInt32 n = 0;

		while (pToContRefExCEDL->GetNthItem(n))
		{
			ExtraContainerChanges::EntryData * tptcr = pToContRefExCEDL->GetNthItem(n);

			if (checkEDType(tptcr) && (tptcr->countDelta > 1))
			{
				UInt32 lim = tptcr->countDelta;
				ExtraContainerChanges::EntryData * tpEntryDataUnst = ExtraContainerChanges::EntryData::Create(tptcr->type, 1);
				
				pToContRefExCEDL->GetNthItem(n)->countDelta = 1;

				for (UInt32 i = 0; i < lim - 1; i++)
				{
					pToContRefExCEDL->Push(tpEntryDataUnst);
				};

			};

			n++;
		};
	}

	void CopyTESContainerContentsToExtraDataUnst(TESObjectREFR* contRef)
	{
		TESContainer* pContainer = NULL;
		TESForm* pBaseForm = contRef->baseForm;

		if (pBaseForm)
		{pContainer = DYNAMIC_CAST(pBaseForm, TESForm, TESContainer);}
		else
		{return;}

		ExtraContainerChanges * pToContRefExC = static_cast<ExtraContainerChanges*>(contRef->extraData.GetByType(kExtraData_ContainerChanges));
		if (!pToContRefExC){ return; };
		EntryDataList * pToContRefExCEDL = pToContRefExC->data->objList;
		if (!pToContRefExCEDL){ return; };

		for (int i = 0; i < pContainer->numEntries; i++)
		{
			for (int j = 0; j < pContainer->entries[i]->count; j++)
			{
				ExtraContainerChanges::EntryData * tpEntryDataUnst = ExtraContainerChanges::EntryData::Create(pContainer->entries[i]->form, 1);

				pToContRefExCEDL->Push(tpEntryDataUnst);
			}
		}
	}

	//ExtraPoison * GetReplaceExtraPoison
	

	/*class SPISObjectTracker
	{
		EntryDataList * olist;
		EntryDataList * nlist;

		DurabilityVector dvec;

		SPISObjectTracker(EntryDataList * _olist)
		{
			olist, nlist = _olist;
			
		}

		public:
		
			void IncrementDurability()
			{

			}

			void UpdateDVec()
			{
				dvec 
			}
	};*/
}