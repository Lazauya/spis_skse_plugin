#include "SplitItemStacks.h"

namespace plugin_spis
{
	void Serialization_Revert(SKSESerializationInterface * intfc);
	void Serialization_Save(SKSESerializationInterface * intfc);
	/*{
		_MESSAGE("_DEBUG_PREF_SAVE_invoked");
		SerializeGnrlContainerMap(globalDurabilityTracker->ContainerEntries, intfc);
	}*/
	void Serialization_Load(SKSESerializationInterface * intfc);

	enum
	{
		kTypeContainerKey,
		kTypeContainerValue,
		kTypeGroundKey,
		kTypeGroundValue,
		kTypeForm,
		kTypePair,
		kTypeVector,
		kTypeSpecMapCount,
		kTypeVectorSize,
		kTypeGnrlMapCount
	};

	void SerializeContainerKey(DurabilityTracker::ContainerKey key, SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_contkey");
		//intfc->OpenRecord('CMAP', 0);
		//intfc->WriteRecord('CMAP', )
		//write coord.x,y,z then rot.x,y,z (32*6 bits, 24bytes)
		if (intfc->OpenRecord(kTypeContainerKey, 0))
		{
			NiPoint3 tcp = key.getcoordinates();
			NiPoint3 trp = key.getrotation();
			intfc->WriteRecordData(&(tcp.x), sizeof(float));
			intfc->WriteRecordData(&(tcp.y), sizeof(float));
			intfc->WriteRecordData(&(tcp.z), sizeof(float));
			intfc->WriteRecordData(&(trp.x), sizeof(float));
			intfc->WriteRecordData(&(trp.y), sizeof(float));
			intfc->WriteRecordData(&(trp.z), sizeof(float));
		}
	}

	//void SerializeContainerValue(DurabilityTracker::ContainerValue val, SKSESerializationInterface * intfc);

	void SerializeGroundKey(DurabilityTracker::GroundKey key, SKSESerializationInterface * intfc);

	void SerializeGroundValue(DurabilityTracker::GroundValue val, SKSESerializationInterface * intfc);

	void SerializeTESForm(TESForm* form, SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_tesform");
		intfc->OpenRecord(kTypeForm, 0);
		intfc->WriteRecordData(&(form->formID), sizeof(UInt32));
		_MESSAGE("_DEBUG_PREF_SAVE_tesformid:%d", form->formID);
	}
	
	void SerializeTESObjectREFR(TESObjectREFR * ref);

	//template<class T>
	//void SerializeVector(std::vector<T> vec)
	//{
		
	//}

	//only for basic types, sorry boys!
	template<typename T1, typename T2>
	void SerializePair(std::pair<T1, T2> pair, SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_pair");
		intfc->OpenRecord(kTypePair, 0);
		intfc->WriteRecordData(&(pair.first), sizeof(T1));
		intfc->WriteRecordData(&(pair.second), sizeof(T2));
	}

	template<typename T1, typename T2>
	void SerializePairVector(std::vector<std::pair<T1, T2> > vecpair, SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_pairvec");
		intfc->OpenRecord(kTypeVectorSize, 0);
		//_MESSAGE("_DEBUG_PREF_SAVE_pairvec1");
		UInt32 vs = vecpair.size();
		//_MESSAGE("_DEBUG_PREF_SAVE_pairvec2");
		intfc->WriteRecordData(&(vs), sizeof(vs));
		//_MESSAGE("_DEBUG_PREF_SAVE_pairvec3");

		intfc->OpenRecord(kTypeVector, 0);
		for (UInt16 i = 0; i < vecpair.size(); i++)
		{
			SerializePair<T1, T2>(vecpair[i], intfc);
		}
	}

	void SerializeSpecContainerMap(std::unordered_map<TESForm*, DurabilityTracker::ContainerValue> * specmap, SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_speccontmap");
		intfc->OpenRecord(kTypeSpecMapCount, 0);
		UInt32 ms = specmap->size();
		intfc->WriteRecordData(&ms, sizeof(UInt32));

		for (auto it = specmap->begin(); it != specmap->end(); it++)
		{
			SerializeTESForm(it->first, intfc);
			SerializePairVector<UInt32, UInt32>(it->second.Durabilities, intfc);
		}
	}

	void SerializeGnrlContainerMap(std::unordered_map<DurabilityTracker::ContainerKey, std::unordered_map < TESForm*, DurabilityTracker::ContainerValue>, DurabilityTracker::ContainerKeyHash >* gnrlmap, SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_gnrlcontmap");
		intfc->OpenRecord(kTypeGnrlMapCount, 0);
		UInt32 ms = gnrlmap->size();
		intfc->WriteRecordData(&ms, sizeof(UInt32));

		for (auto itt = gnrlmap->begin(); itt != gnrlmap->end(); itt++)
		{
			SerializeContainerKey(itt->first, intfc);
			SerializeSpecContainerMap(&(itt->second), intfc);
		}
	}
}