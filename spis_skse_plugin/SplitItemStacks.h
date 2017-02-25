#include "skse/GameForms.h"
#include "skse/GameExtraData.h"
#include "skse/PapyrusNativeFunctions.h"
#include "skse/PapyrusVM.h"
#include "skse/PluginAPI.h"
#include "skse/PapyrusActor.h"
#include "skse/HashUtil.h"

#include "skse/GameRTTI.h"
#include "skse/GameObjects.h"
#include "skse/GameData.h"
#include "skse/GameTypes.h"

//#include "UIExtensions.h"

#include "skse/ScaleformExtendedData.h"

#include <vector>
#include <unordered_map>
#include <utility>
#include <functional>
#include <bitset>
//#include <hash> doesn't exist? Lol

//IDebugLog spisLog;

#ifndef durtracker_def
#define durtracker_def

namespace plugin_spis
{
	//utils for new frontend unstacking implementation

	/*
	types:
	0 - add
	1 - remove
	2 - move
	*/

	UInt32 LookupDurabilityInfo(TESForm* item);
	void ChangeEntries(StaticFunctionTag *base, UInt8 type, UInt8 space, TESObjectREFR * containerFrom, TESObjectREFR * containerTo, TESForm * item, UInt32 amount);

	//large object initializer
	bool InitializeDurabilityTracker(StaticFunctionTag *base);

	//serialization functions



	//register
	bool RegisterFuncsPapyrus(VMClassRegistry* registry);
	bool RegisterFuncsScaleform(GFxMovieView * view, GFxValue * root);

	bool RegisterSerializationCallbacks(SKSESerializationInterface * intfc, PluginHandle plgnhndl);

	SInt32 CalcItemId(TESForm * form, BaseExtraList * extraList);

	class DurabilityTracker
	{
	public:
		
		DurabilityTracker();
		/*{
			//_MESSAGE("_DEBUG_PREF_INIT_init called");
			GroundEntries = new std::unordered_map< GroundKey, GroundValue, GroundKeyHash >;
			//_MESSAGE("_DEBUG_PPEF_INIT_ground entries just declared");
			ContainerEntries = new std::unordered_map < ContainerKey, std::unordered_map<TESForm*, ContainerValue, TESFormHash, TESFormEquality>, ContainerKeyHash >;
		}*/

		//structures for ground maps

		struct GroundKey
		{
		private:
			TESObjectREFR* ObjectRef;
			//this is for creating "temporary" ground keys, because the additem doesn't give an objref for ground items
			UInt32 hashKey;
		public:
			GroundKey(TESObjectREFR* object) : ObjectRef(object), hashKey(ObjectRef->CreateRefHandle()) {};
			GroundKey() : ObjectRef(nullptr), hashKey(0) {};
			void setObjectRef(TESObjectREFR* object)
			{
				ObjectRef = object;
			}
			TESObjectREFR* getObjectRef() const { return ObjectRef; }

			bool operator==(const GroundKey &other) const
			{
				return this->hashKey == other.hashKey;
			}

			void sethashKey(UInt32 key)
			{
				hashKey = key;
			}
			UInt32 gethashKey() const
			{
				return hashKey;
			}
		};

		//check DurabilityEntry for specs

		typedef std::pair<UInt32, UInt32> GroundValue;

		//structures for container maps

		struct ContainerKey
		{
		private:
			TESObjectREFR* ContainerRef;
		public:
			ContainerKey(TESObjectREFR* object) : ContainerRef(object) {};
			ContainerKey() : ContainerRef(nullptr) {};
			void setContainerRef(TESObjectREFR* object)
			{
				ContainerRef = object;
			}

			TESObjectREFR* getContainerRef() const { return ContainerRef; }

			bool operator==(const ContainerKey &other) const
			{
				return this->ContainerRef == other.ContainerRef;
			}

			void operator=(const ContainerKey &other)
			{
				this->setContainerRef(other.getContainerRef());
			}
		};
		
		struct ContainerValue
		{
			ContainerValue(TESForm * type, UInt32 count) : count(count), pBaseForm(type) {};

			ContainerValue()
			{
				pBaseForm = nullptr;
				count = 0;
			}

			void operator=(const ContainerValue& p)
			{
				//this->pEntryData = p.pEntryData;
				this->count = p.count;
				this->Durabilities = p.Durabilities;
				this->pBaseForm = p.pBaseForm;
			}

			bool operator==(const ContainerValue& p)
			{
				return this->pBaseForm == p.pBaseForm;
			}

			//ExtraContainerChanges::EntryData * pEntryData; i don't think i need this
			TESForm * pBaseForm;
			SInt32 count;

			//1: maxDurability
			//2: durability
			typedef std::pair<UInt32, UInt32> DurabilityEntry;

			std::vector<DurabilityEntry> Durabilities;
		};

		//hash functions for maps
		struct ContainerKeyHash
		{
			std::size_t operator()(const ContainerKey& k) const
			{
				////_MESSAGE("_DEBUG_PREF_CTKH_invoked");
				const TESObjectREFR* ttorp = k.getContainerRef();
				////_MESSAGE("_DEBUG_PREF_CTKH_gotconstval, refcount:%d", ttorp->CreateRefHandle());
				const UInt32 outhash = const_cast<TESObjectREFR*>(ttorp)->CreateRefHandle();
				return outhash;
			}
		};

		struct GroundKeyHash
		{
			std::size_t operator()(GroundKey const& k) const
			{
				
				const UInt32 outhash1 = k.gethashKey();
				if (outhash1)
				{
					_MESSAGE("_DEBUG_PREF_GKYH_1kh:%d", outhash1);
					return outhash1;
				}
				
				const TESObjectREFR* ttorp = k.getObjectRef();
				////_MESSAGE("_DEBUG_PREF_CTKH_gotconstval, refcount:%d", ttorp->CreateRefHandle());
				const UInt32 outhash = const_cast<TESObjectREFR*>(ttorp)->CreateRefHandle();
				_MESSAGE("_DEBUG_PREF_GKYH_2kh:%d", outhash);
				return outhash;
			}
		};

		std::unordered_map<GroundKey, GroundValue, GroundKeyHash>* GroundEntries;
		std::unordered_map<ContainerKey, std::unordered_map < TESForm*, ContainerValue>, ContainerKeyHash >* ContainerEntries;

		typedef std::unordered_map<ContainerKey, std::unordered_map < TESForm*, ContainerValue>, ContainerKeyHash >* GroundMap; //used for equip object
		//not relevant? ground-->container/container-->ground need to be defined in papyrus
		/*
		spaces:
			0 - container (to container)
			1 - ground
			2 - ground to container (move only)
			3 - container to ground (move only)
			4 - container, use CurrentGroundKey
		*/

		/*
		remove/findTypes:
			0 - exact value; take remove entry(s) with durability "durability"
			1 - lowest; remove entry(s) with lowest durability
			2 - highest; remove entry(s) with highest durability (idk why anyone would want it but here it is)

			MoveEntry assumes exact value (0)
			entries only differentiated by durability because it makes sense
		*/

		//using two because i'm a newb 'n I don't want to deal with templates
		GroundKey FindEntryGround(TESObjectREFR * item, UInt32 durability); //return key of first entry; only one function because of lack of ambiguity
		SInt32 FindEntryContainer(UInt8 findType, TESObjectREFR * container, TESForm * item, UInt32 durability, UInt32 maxDurability, UInt32 nth); //return index of first entry in array
		ContainerValue * FindDurabilityContainer(TESObjectREFR * container, UInt32 formID); //find durability(s) of items with this formID

		bool WrapEntries(TESObjectREFR * container);

		bool AddEntry(UInt8 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability, UInt32 maxDurability);
		bool RemoveEntry(UInt8 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt8 removeType, UInt32 durability, UInt32 maxDurability, UInt32 nth = 0);
		//only including this beacuse speed of papyrus is scheisse
		bool MoveEntry(UInt8 space, TESObjectREFR * containerFrom, TESObjectREFR * containerTo, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability, UInt32 maxDurability, UInt32 nth = 0);
	};

	class CurrentContainer
	{
		private:
			TESObjectREFR* currentContainer;

		public:
			CurrentContainer(TESObjectREFR * container) : currentContainer(container) {};

			bool ChangeContainer(TESObjectREFR * container)
			{
				////_MESSAGE("_DEBUG_PREF_CCCC_invoked");
				currentContainer = container;
				////_MESSAGE("_DEBUG_PREF_CCCC_container set?");
				return currentContainer == nullptr ? false : true;
			};

			TESObjectREFR* GetContainer() const
			{
				return currentContainer;
			}
	};

	class CurrentDurability //this is used alongside CurrentContainer to send info between skse, papyrus, and scaleform cuz reasons
	{
		private:
			UInt32 maxDurability;
			UInt32 durability;

		public:
			CurrentDurability(UInt32 max, UInt32 dur) : maxDurability(max), durability(dur) {};
			CurrentDurability() : maxDurability(0), durability(0) {};

			UInt32 GetDurability() { return durability; };
			UInt32 GetMaxDurability() { return maxDurability; };

			bool SetDurability(UInt32 dur)
			{
				durability = dur;
				return true;
			}

			bool SetMaxDurability(UInt32 max)
			{
				maxDurability = max;
				return true;
			}
	};
	
	class EquippedStates 
	{
	public:
		/*
		0 - righthand
		1 - lefthand
		2 - head
		3 - hair
		*/
		std::vector<std::tuple<TESForm*, UInt32, UInt32, UInt32> > EquippedEntries;
		//if true, weapon stuff only looks at first entry for equipped weapon and sets it to "equipped" in scaleform
		bool twoHanded;
	
		EquippedStates()
		{
			for (UInt8 i = 0; i < 18; i++)
			{
				EquippedEntries.push_back(std::tuple<TESForm*, UInt32, UInt32, UInt32>(nullptr, 0, 0, 0));
			}
			twoHanded = false;
		}

		bool setEquippedEntries(std::vector<std::tuple<TESForm*, UInt32, UInt32, UInt32> > entries)
		{
			for (UInt32 i = 0; i < 18; i++)
			{
				EquippedEntries[i] = entries[i];
			}
			return true;
		}

		std::vector<std::tuple<TESForm*, UInt32, UInt32, UInt32> > getEquippedEntries()
		{
			return EquippedEntries;
		}

		enum
		{
			kRightHand = 0,
			kLeftHand = 1
		};

		enum //copied enum from TESObjectWEAP because i'm lazy
		{
			kType_HandToHandMelee = 0,
			kType_OneHandSword = 1,
			kType_OneHandDagger = 2,
			kType_OneHandAxe = 3,
			kType_OneHandMace,
			kType_TwoHandSword,
			kType_TwoHandAxe,
			kType_Bow,
			kType_Staff,
			kType_CrossBow,
			kType_H2H,
			kType_1HS,
			kType_1HD,
			kType_1HA,
			kType_1HM,
			kType_2HS,
			kType_2HA,
			kType_Bow2,
			kType_Staff2,
			kType_CBow
		};

		enum //copied enum from BGSBipedObjectForm because i'm efficient
		{
			kPart_Head = 1 << 0,
			kPart_Hair = 1 << 1,
			kPart_Body = 1 << 2,
			kPart_Hands = 1 << 3,
			kPart_Forearms = 1 << 4,
			kPart_Amulet = 1 << 5,
			kPart_Ring = 1 << 6,
			kPart_Feet = 1 << 7,
			kPart_Calves = 1 << 8,
			kPart_Shield = 1 << 9,
			kPart_Unnamed10 = 1 << 10,
			kPart_LongHair = 1 << 11,
			kPart_Circlet = 1 << 12,
			kPart_Ears = 1 << 13,
			kPart_Unnamed14 = 1 << 14,
			kPart_Unnamed15 = 1 << 15,
			kPart_Unnamed16 = 1 << 16,
			kPart_Unnamed17 = 1 << 17,
			kPart_Unnamed18 = 1 << 18,
			kPart_Unnamed19 = 1 << 19,
			kPart_Unnamed20 = 1 << 20,
			kPart_Unnamed21 = 1 << 21,
			kPart_Unnamed22 = 1 << 22,
			kPart_Unnamed23 = 1 << 23,
			kPart_Unnamed24 = 1 << 24,
			kPart_Unnamed25 = 1 << 25,
			kPart_Unnamed26 = 1 << 26,
			kPart_Unnamed27 = 1 << 27,
			kPart_Unnamed28 = 1 << 28,
			kPart_Unnamed29 = 1 << 29,
			kPart_Unnamed30 = 1 << 30,
			kPart_FX01 = 1 << 31,
		};

		//n is the nth item with this durability, because some items may have the same durability but only one of them should
		//be equipped. other params are self explanatory
		bool SetState(TESForm * item, UInt32 durability, UInt32 maxDurability, UInt8 hand, UInt32 n)
		{
			TESObjectWEAP* weapon;
			TESObjectARMO* armor;
			UInt32 handType;

			std::tuple<TESForm*, UInt32, UInt32, UInt32> entryTuple = std::tuple<TESForm*, UInt32, UInt32, UInt32>(item, maxDurability, durability, n);
			std::tuple<TESForm*, UInt32, UInt32, UInt32> emptyTuple = std::tuple<TESForm*, UInt32, UInt32, UInt32>(nullptr, 0, 0, 0);

			//_MESSAGE("_DEBUG_PREF_SEST_1,%d", item->formID);
			TESObjectARMO * helmet = DYNAMIC_CAST(LookupFormByID(80227), TESForm, TESObjectARMO);
			//_MESSAGE("_DEBUG_PREF_SEST_1.1,%d", helmet->bipedObject.GetSlotMask());

			switch (item->formType)
			{
			case kFormType_Weapon:
				weapon = DYNAMIC_CAST(item, TESForm, TESObjectWEAP);
				if (!weapon) { return false; };
				////_MESSAGE("_DEBUG_PREF_SEST_2");
				////_MESSAGE("_DEBUG_PREF_SEST_3,%d", weapon->type());
				handType = ((weapon->type() == kType_OneHandSword) || (weapon->type() == kType_OneHandDagger) || (weapon->type() == kType_OneHandAxe) || (weapon->type() == kType_OneHandMace) || (weapon->type() == kType_1HS) || (weapon->type() == kType_1HD) || (weapon->type() == kType_1HA) || (weapon->type() == kType_OneHandMace)) ? 1 : 0;
				handType = (weapon->type() == kType_TwoHandSword || weapon->type() == kType_TwoHandAxe || weapon->type() == kType_Bow || weapon->type() == kType_Staff || weapon->type() == kType_CrossBow || weapon->type() == kType_2HS || weapon->type() == kType_2HA || weapon->type() == kType_Bow2 || weapon->type() == kType_Staff2 || weapon->type() == kType_CBow || weapon->type() == kType_HandToHandMelee || weapon->type() == kType_H2H) ? 2 : handType;
				////_MESSAGE("_DEBUG_PREF_SEST_4,%d", handType);
				switch (handType)
				{
				case 1: //onehanded
					switch (hand)
					{
					case kRightHand:
						if (EquippedEntries[0] == entryTuple) { EquippedEntries[0] = emptyTuple; }
						else if (EquippedEntries[1] == entryTuple) { EquippedEntries[1] = emptyTuple; EquippedEntries[0] = entryTuple; }
						else { EquippedEntries[0] = entryTuple; }
						twoHanded = false;
						return true;
					case kLeftHand:
						if (EquippedEntries[1] == entryTuple) { EquippedEntries[1] = emptyTuple; }
						else if (EquippedEntries[0] == entryTuple) { EquippedEntries[0] = emptyTuple; EquippedEntries[1] = entryTuple; }
						else { EquippedEntries[1] = entryTuple; }
						twoHanded = false;
						return true;
					default:
						return false;
					}
				case 2: //twohanded
					if (EquippedEntries[0] == entryTuple) { EquippedEntries[0] = emptyTuple; }
					else { EquippedEntries[0] = entryTuple; twoHanded = true; }
					return true;
				default:
					return false;
				}
			case kFormType_Armor:
				armor = DYNAMIC_CAST(item, TESForm, TESObjectARMO);
				if (!armor) { return false; };
				//_MESSAGE("_DEBUG_PREF_SEST_5,%d", armor->bipedObject.GetSlotMask());
				//i really have no idea why this is nessecary. i made it check the 2nd bit only, because something operates on entire
				//slot mask value and flips other bits. nothing in docs says anything about it (shrug)
				std::bitset<32> tb = armor->bipedObject.GetSlotMask();
				if (tb[1])
				{
					if (EquippedEntries[3] == entryTuple) { EquippedEntries[3] = emptyTuple; }
					else { EquippedEntries[3] = entryTuple; }
					return true;
				}
				switch (armor->bipedObject.GetSlotMask())
				{
				case kPart_Head:
					if (EquippedEntries[2] == entryTuple) { EquippedEntries[2] = emptyTuple; }
					else { EquippedEntries[2] = entryTuple; }
					return true;
				case kPart_Hair:
					if (EquippedEntries[3] == entryTuple) { EquippedEntries[3] = emptyTuple; }
					else { EquippedEntries[3] = entryTuple; }
					return true;
				case kPart_Body:
					if (EquippedEntries[4] == entryTuple) { EquippedEntries[4] = emptyTuple; }
					else { EquippedEntries[4] = entryTuple; }
					return true;
				case kPart_Hands:
					if (EquippedEntries[5] == entryTuple) { EquippedEntries[5] = emptyTuple; }
					else { EquippedEntries[5] = entryTuple; }
					return true;
				case kPart_Forearms:
					if (EquippedEntries[6] == entryTuple) { EquippedEntries[6] = emptyTuple; }
					else { EquippedEntries[6] = entryTuple; }
					return true;
				case kPart_Amulet:
					if (EquippedEntries[7] == entryTuple) { EquippedEntries[7] = emptyTuple; }
					else { EquippedEntries[7] = entryTuple; }
					return true;
				case kPart_Ring:
					if (EquippedEntries[8] == entryTuple) { EquippedEntries[8] = emptyTuple; }
					else { EquippedEntries[8] = entryTuple; }
					return true;
				case kPart_Feet:
					if (EquippedEntries[9] == entryTuple) { EquippedEntries[9] = emptyTuple; }
					else { EquippedEntries[9] = entryTuple; }
					return true;
				case kPart_Calves:
					if (EquippedEntries[10] == entryTuple) { EquippedEntries[10] = emptyTuple; }
					else { EquippedEntries[10] = entryTuple; }
					return true;
				case kPart_Shield:
					if (EquippedEntries[11] == entryTuple) { EquippedEntries[11] = emptyTuple; }
					else { EquippedEntries[11] = entryTuple; }
					return true;
				case kPart_Unnamed10:
					if (EquippedEntries[12] == entryTuple) { EquippedEntries[12] = emptyTuple; }
					else { EquippedEntries[12] = entryTuple; }
					return true;
				case kPart_LongHair:
					if (EquippedEntries[13] == entryTuple) { EquippedEntries[13] = emptyTuple; }
					else { EquippedEntries[13] = entryTuple; }
					return true;
				case kPart_Circlet:
					if (EquippedEntries[14] == entryTuple) { EquippedEntries[14] = emptyTuple; }
					else { EquippedEntries[14] = entryTuple; }
					return true;
				case kPart_Ears:
					if (EquippedEntries[15] == entryTuple) { EquippedEntries[15] = emptyTuple; }
					else { EquippedEntries[15] = entryTuple; }
					return true;
				default:
					return false;
				}
			}
		}

		/*
		0 - none
		1 - equipped
		2 - left
		3 - right
		4 - left and right
		*/
		UInt8 FindInList(TESForm * item, UInt32 durability, UInt32 maxDurability, UInt32 n)
		{
			for (UInt8 i = 0; i < 18; i++)
			{
				if ((std::get<0>(EquippedEntries[i]) == item) && (std::get<1>(EquippedEntries[i]) == maxDurability) && (std::get<2>(EquippedEntries[i]) == durability) && (std::get<3>(EquippedEntries[i]) == n))
				{
					if (i == 0 && twoHanded)
					{
						return 1;
					}
					else if (i == 0 && !twoHanded)
					{
						return 3;
					}
					else if (i == 1 && !twoHanded)
					{
						return 2;
					}
					else
					{
						return 1;
					}
				}
			}

			return 0;
		}

		struct FormIDMatcher : public FormMatcher
		{
			UInt32 id;
			FormIDMatcher(TESForm* item) : id(item->formID) {};
			bool Matches(TESForm * item) const
			{
				return item->formID == id;
			}
		};

		bool WrapEquipStates(TESObjectREFR * actor, DurabilityTracker * dtracker)
		{
			//_MESSAGE("_DEBUG_PREF_WEQS_1");
			ExtraContainerChanges * pToContRefExC = static_cast<ExtraContainerChanges*>(actor->extraData.GetByType(kExtraData_ContainerChanges));
			if (!pToContRefExC){ return false; };
			//_MESSAGE("_DEBUG_PREF_WEQS_2");
			UInt32 i = 0;
			ExtraContainerChanges::EquipItemData outData;
			//_MESSAGE("_DEBUG_PREF_WEQS_2.1,%d", (*(dtracker->ContainerEntries))[DurabilityTracker::ContainerKey(actor)].size());
			for (auto it = (*(dtracker->ContainerEntries))[DurabilityTracker::ContainerKey(actor)].begin(); it != (*(dtracker->ContainerEntries))[DurabilityTracker::ContainerKey(actor)].end(); it++)
			{
				//_MESSAGE("_DEBUG_PREF_WEQS_3,%d", i++);
				FormIDMatcher match(it->first);
				EquipData equipData = pToContRefExC->FindEquipped(match);
				ExtraContainerChanges::EquipItemData foundData;

				if (equipData.pForm)
				{
					//_MESSAGE("_DEBUG_PREF_WEQS_3.1,%d,%d", i, equipData.pForm->formID);
					//_MESSAGE("_DEBUG_PREF_WEQS_4");
					SInt32 itemId = CalcItemId(equipData.pForm, equipData.pExtraData);
					pToContRefExC->data->GetEquipItemData(foundData, equipData.pForm, itemId);

					if ((foundData.itemCount >= 0) && foundData.isItemWorn)
					{
						//_MESSAGE("_DEBUG_PREF_WEQS_5");
						SetState(equipData.pForm, LookupDurabilityInfo(equipData.pForm), LookupDurabilityInfo(equipData.pForm), foundData.isItemWornLeft ? kLeftHand : kRightHand, 0);
					}
				}
			}

			for (UInt8 j = 0; j < 18; j++)
			{
				//_MESSAGE("_DEBUG_PREF_WEQS_6,j:%d,id:%d,m:%d,d:%d", j, std::get<0>(EquippedEntries[j]) == nullptr ? 0 : std::get<0>(EquippedEntries[j])->formID, std::get<1>(EquippedEntries[j]), std::get<2>(EquippedEntries[j]));
			}

			return true;
		}
	};

	class CurrentGroundKey
	{
	public:
		DurabilityTracker::GroundKey key;
		void SetGroundKey(TESObjectREFR * obj)
		{
			UInt32 hash = obj->CreateRefHandle();
			key.sethashKey(hash);
		}
		
		DurabilityTracker::GroundKey GetGroundKey() const
		{
			return key;
		}
	};
	//utils functions (obselete?) (probably, it's not like I really cared about the backend representation or anything, baka!)
	//void MarkTESContainerZero(StaticFunctionTag *base, TESObjectREFR* contRef);
	//void outputToOpenDebugLog(StaticFunctionTag *base, BSFixedString msg);
	//void GetContainerReady(StaticFunctionTag *base, TESObjectREFR* contRef);
	//void UpdateContainer(StaticFunctionTag *base, TESObjectREFR* contRef);

	bool IncrementEquippedDurability(StaticFunctionTag* base, UInt32 slot, SInt32 amount, TESObjectREFR* actor);
	bool DecrementEquippedDurability(StaticFunctionTag* base, UInt32 slot, SInt32 amount, TESObjectREFR* actor);
};

#endif