#include "skse/GameForms.h"
#include "skse/GameExtraData.h"
#include "skse/PapyrusNativeFunctions.h"
#include "skse/PapyrusVM.h"
#include "skse/PluginAPI.h"

#include "skse/GameRTTI.h"
#include "skse/GameObjects.h"
#include "skse/GameData.h"
#include "skse/GameTypes.h"

#include "UIExtensions.h"

#include "skse/ScaleformExtendedData.h"

#include <vector>
#include <unordered_map>
#include <utility>
#include <functional>
//#include <hash> doesn't exist? Lol

//IDebugLog spisLog;

#ifndef durtracker_def
#define durtracker_def

namespace plugin_spis
{
	
	class DurabilityTracker
	{
	public:
		
		DurabilityTracker();
		/*{
			_MESSAGE("_DEBUG_PREF_INIT_init called");
			GroundEntries = new std::unordered_map< GroundKey, GroundValue, GroundKeyHash >;
			_MESSAGE("_DEBUG_PPEF_INIT_ground entries just declared");
			ContainerEntries = new std::unordered_map < ContainerKey, std::unordered_map<TESForm*, ContainerValue, TESFormHash, TESFormEquality>, ContainerKeyHash >;
		}*/

		//structures for ground maps

		struct GroundKey
		{
		private:
			TESObjectREFR* ObjectRef;
			NiPoint3 * coordinates;
			NiPoint3 * rotation;
			TESObjectCELL * cell;
		public:
			GroundKey(TESObjectREFR* object) : ObjectRef(object), coordinates(&(ObjectRef->pos)), rotation(&(ObjectRef->rot)), cell(ObjectRef->parentCell) { _MESSAGE("_DEBUG_PREF_GRIN_init called"); };
			GroundKey() : ObjectRef(nullptr), coordinates(nullptr), rotation(nullptr), cell(nullptr) {};
			void setObjectRef(TESObjectREFR* object)
			{
				ObjectRef = object;
				coordinates = &(ObjectRef->pos);
				rotation = &(ObjectRef->rot);
				cell = ObjectRef->parentCell;
			}
			TESObjectREFR* getObjectRef() const { return ObjectRef; }
			NiPoint3 getcoordinates() { return *coordinates; }
			NiPoint3 getrotation() { return *rotation; }
			TESForm* getform() { return ObjectRef->baseForm; }
			TESObjectCELL* getcell() { return cell; }

			bool operator==(const GroundKey &other) const
			{
				return (this->ObjectRef) == (other.ObjectRef);
			}

			void operator=(const GroundKey &other)
			{
				this->setObjectRef(other.getObjectRef());
			}
		};

		//check DurabilityEntry for specs
		typedef std::pair<UInt32, UInt32> GroundValue;

		//structures for container maps

		struct ContainerKey
		{
		private:
			TESObjectREFR* ContainerRef;
			NiPoint3 * coordinates;
			NiPoint3 * rotation;
			TESObjectCELL * cell;
		public:
			ContainerKey(TESObjectREFR* object) : ContainerRef(object), coordinates(object != nullptr ? &(ContainerRef->pos) : nullptr), rotation(object != nullptr ? &(ContainerRef->rot) : nullptr), cell((object != nullptr ? ContainerRef->parentCell : nullptr)) { _MESSAGE("_DEBUG_PREF_CTIN_init called"); };
			ContainerKey() : ContainerRef(nullptr), coordinates(nullptr), rotation(nullptr), cell(nullptr) {};
			ContainerKey(NiPoint3 * c, NiPoint3 * r) : coordinates(c), rotation(r) {};
			void setContainerRef(TESObjectREFR* object)
			{
				ContainerRef = object;
				coordinates = &(ContainerRef->pos);
				rotation = &(ContainerRef->rot);
				cell = ContainerRef->parentCell;
			}

			//probably irrelevant, found something that makes this obselete
			/*
			//need this for serialization, will probably add an extra method for resyncing the actual object ref/cell
			//whenever player interacts with it. but maybe not.
			void setContainerKey(NiPoint3* coord, NiPoint3* rot)
			{
				ContainerRef = nullptr;
				coordinates = coord;
				rotation = rot;
				cell = nullptr;
			}*/

			TESObjectREFR* getContainerRef() const { return ContainerRef; }
			NiPoint3 getcoordinates() const { return *coordinates; }
			NiPoint3 getrotation() const { return *rotation; }
			TESObjectCELL* getcell() { return cell; }

			bool operator==(const ContainerKey &other) const
			{
				/*return ((*(this->coordinates)).x) == ((*(other.coordinates)).x) &&
					   ((*(this->coordinates)).y) == ((*(other.coordinates)).y) &&
					   ((*(this->coordinates)).z) == ((*(other.coordinates)).z) &&
					   ((*(this->rotation)).x)    == ((*(other.rotation)).x)    &&
					   ((*(this->rotation)).y)    == ((*(other.rotation)).y)    && 
					   ((*(this->rotation)).z)    == ((*(other.rotation)).z);*/
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
				_MESSAGE("_DEBUG_PREF_CTKH_invoked");
				const TESObjectREFR* ttorp = k.getContainerRef();
				//_MESSAGE("_DEBUG_PREF_CTKH_gotconstval, refcount:%d", ttorp->CreateRefHandle());
				const UInt32 outhash = const_cast<TESObjectREFR*>(ttorp)->CreateRefHandle();
				return outhash;
			}
		};

		struct GroundKeyHash
		{
			std::size_t operator()(GroundKey const& k) const
			{

				const TESObjectREFR* ttorp = k.getObjectRef();
				//_MESSAGE("_DEBUG_PREF_CTKH_gotconstval, refcount:%d", ttorp->CreateRefHandle());
				const UInt32 outhash = const_cast<TESObjectREFR*>(ttorp)->CreateRefHandle();
				return outhash;
			}
		};

		/*struct TESFormHash
		{
			std::size_t operator()(const TESForm*& k) const
			{
				_MESSAGE("TESFormHash");
				//return (const_cast<TESForm*&>(k))->formID;
				return k->formID;
			};
		};

		struct TESFormEquality
		{
			bool operator()(TESForm*& const p1, const TESForm*& p2) const
			{
				_MESSAGE("TESFormEquality");
				//return (const_cast<TESForm*&>(p1))->formID == (const_cast<TESForm*&>(p2))->formID;
				return (p1->formID == p2->formID);
			};
		};

		/*bool TESFormEqualityFunction(const TESForm*& p1, const TESForm*& p2)
		{
			return p1->formID == p2->formID;
		};*/

		std::unordered_map<GroundKey, GroundValue, GroundKeyHash>* GroundEntries;
		std::unordered_map<ContainerKey, std::unordered_map < TESForm*, ContainerValue>, ContainerKeyHash >* ContainerEntries; // ContainerEntries; old thing, maybe I'll look at this again
		//std::unordered_map<ContainerKey, std::unordered_map<UInt32, ContainerValue>, ContainerKeyHash> * ContainerEntries;
		//std::unordered_map<TESForm, std::vector<ContainerValue> >PlayerInventory; //idk this seems unnecssecary and too complicated

		//not relevant? ground-->container/container-->ground need to be defined in papyrus
		/*
		spaces:
			0 - container (to container)
			1 - ground
			2 - ground to container (move only)
			3 - container to ground (move only)
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
		SInt32 FindEntryContainer(UInt8 findType, TESObjectREFR * container, TESForm * item, UInt32 durability); //return index of first entry in array
		ContainerValue FindDurabilityContainer(TESObjectREFR * container, UInt32 formID); //find durability(s) of items with this formID

		//std::pair<UInt32, UInt32> LookupDurabilityInfo(TESForm* item);

		bool WrapEntries(TESObjectREFR * container);

		bool AddEntry(UInt8 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability, UInt32 maxDurability);
		bool RemoveEntry(UInt8 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt8 removeType, UInt32 durability);
		//only including this beacuse speed of papyrus is sheisse
		bool MoveEntry(UInt8 space, TESObjectREFR * containerFrom, TESObjectREFR * containerTo, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability);
	};

	class CurrentContainer
	{
		private:
			TESObjectREFR* currentContainer;

		public:
			CurrentContainer(TESObjectREFR * container) : currentContainer(container) {};

			bool ChangeContainer(TESObjectREFR * container)
			{
				_MESSAGE("_DEBUG_PREF_CCCC_invoked");
				currentContainer = container;
				_MESSAGE("_DEBUG_PREF_CCCC_container set?");
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

	//utils functions (obselete?) (probably, it's not like I really cared about the backend representation or anything, baka!)
	//void MarkTESContainerZero(StaticFunctionTag *base, TESObjectREFR* contRef);
	//void outputToOpenDebugLog(StaticFunctionTag *base, BSFixedString msg);
	//void GetContainerReady(StaticFunctionTag *base, TESObjectREFR* contRef);
	//void UpdateContainer(StaticFunctionTag *base, TESObjectREFR* contRef);

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
};

#endif