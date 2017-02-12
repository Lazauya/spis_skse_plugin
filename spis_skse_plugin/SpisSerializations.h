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
		kTypeGnrlMapCount,
		kTypeInitialized
	};

	void SerializeContainerKey(DurabilityTracker::ContainerKey key, SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_contkey");
		//intfc->OpenRecord('CMAP', 0);
		//intfc->WriteRecord('CMAP', )
		//write coord.x,y,z then rot.x,y,z (32*6 bits, 24bytes)
		//if (intfc->OpenRecord(kTypeContainerKey, 0))
		//{
		NiPoint3 tcp = key.getcoordinates();
		NiPoint3 trp = key.getrotation();
		intfc->WriteRecordData(&(tcp.x), sizeof(float));
		intfc->WriteRecordData(&(tcp.y), sizeof(float));
		intfc->WriteRecordData(&(tcp.z), sizeof(float));
		intfc->WriteRecordData(&(trp.x), sizeof(float));
		intfc->WriteRecordData(&(trp.y), sizeof(float));
		intfc->WriteRecordData(&(trp.z), sizeof(float));
		//}
	}

	//void SerializeContainerValue(DurabilityTracker::ContainerValue val, SKSESerializationInterface * intfc);

	void SerializeGroundKey(DurabilityTracker::GroundKey key, SKSESerializationInterface * intfc);

	void SerializeGroundValue(DurabilityTracker::GroundValue val, SKSESerializationInterface * intfc);

	void SerializeTESForm(TESForm* form, SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_tesform");
		//intfc->OpenRecord(kTypeForm, 0);
		intfc->WriteRecordData(&(form->formID), sizeof(UInt32));
		_MESSAGE("_DEBUG_PREF_SAVE_tesformid:%d", form->formID);
	}
	
	void SerializeTESObjectREFR(TESObjectREFR * ref);

	//only for basic types, sorry boys!
	template<typename T1, typename T2>
	void SerializePair(std::pair<T1, T2> pair, SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_pair");
		//intfc->OpenRecord(kTypePair, 0);
		intfc->WriteRecordData(&(pair.first), sizeof(T1));
		intfc->WriteRecordData(&(pair.second), sizeof(T2));
	}

	template<typename T1, typename T2>
	void SerializePairVector(std::vector<std::pair<T1, T2> > vecpair, SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_pairvec");
		//intfc->OpenRecord(kTypeVectorSize, 0);
		//_MESSAGE("_DEBUG_PREF_SAVE_pairvec1");
		UInt32 vs = vecpair.size();
		//_MESSAGE("_DEBUG_PREF_SAVE_pairvec2");
		intfc->WriteRecordData(&(vs), sizeof(vs));
		//_MESSAGE("_DEBUG_PREF_SAVE_pairvec3");

		//intfc->OpenRecord(kTypeVector, 0);
		for (UInt16 i = 0; i < vecpair.size(); i++)
		{
			SerializePair<T1, T2>(vecpair[i], intfc);
		}
	}

	void SerializeSpecContainerMap(std::unordered_map<TESForm*, DurabilityTracker::ContainerValue> * specmap, SKSESerializationInterface * intfc)
	{
		_MESSAGE("_DEBUG_PREF_SAVE_speccontmap");
		//intfc->OpenRecord(kTypeSpecMapCount, 0);
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

	template<typename T1, typename T2>
	std::pair<T1, T2> UnserializePair(SKSESerializationInterface * intfc)
	{
		/*UInt32 ver = 0;
		UInt32 type = kTypePair;
		UInt32 len = sizeof(T1);
		UInt32 out = 0;

		//UInt32 ver, type, len;

		//intfc->OpenRecord(kTypePair, 0);

		//while (intfc->GetNextRecordInfo(&type, &ver, &len))
		//{
			//intfc->ReadRecordData(&out, sizeof(T1))
		//}
		*/

		T1 out1;
		T2 out2;

		intfc->ReadRecordData(&out1, sizeof(T1));
		intfc->ReadRecordData(&out2, sizeof(T2));

		return std::pair<T1, T2>(out1, out2);
	}

	template<typename T1, typename T2>
	std::vector<std::pair<T1, T2> > UnserializePairVector(SKSESerializationInterface * intfc)
	{
		UInt32 vecsize;
		std::vector<std::pair<T1, T2> > outvec;

		intfc->ReadRecordData(&vecsize, sizeof(vecsize));

		for (UInt32 i = 0; i < vecsize; i++)
		{
			outvec.push_back(UnserializePair<T1, T2>(intfc));
		}

		return outvec;
	}

	//TESForm * UnserializeTESForm(SKSESerializationInterface * intfc)
	//{
		
	//}

	std::pair<TESForm*, DurabilityTracker::ContainerValue> UnserializeSpecContainerValueTESFormPair(SKSESerializationInterface * intfc)
	{
		UInt32 formid;
		intfc->ReadRecordData(&formid, sizeof(UInt32));

		std::vector<std::pair<UInt32, UInt32> > mapvec = UnserializePairVector<UInt32, UInt32>(intfc);
		DurabilityTracker::ContainerValue outcontval(nullptr, LookupFormByID(formid), mapvec.size());
		outcontval.Durabilities = mapvec;

		return std::pair<TESForm*, DurabilityTracker::ContainerValue>(LookupFormByID(formid), outcontval);
	}

	std::unordered_map < TESForm*, DurabilityTracker::ContainerValue > UnserializeSpecContainerMap(SKSESerializationInterface * intfc)
	{
		UInt32 paircount;
		intfc->ReadRecordData(&paircount, sizeof(UInt32));

		std::unordered_map < TESForm*, DurabilityTracker::ContainerValue > outmap;

		for (UInt32 i = 0; i < paircount; i++)
		{
			outmap.insert(UnserializeSpecContainerValueTESFormPair(intfc));
		}

		return outmap;
	}

	DurabilityTracker::ContainerKey UnserializeContainerKey(SKSESerializationInterface * intfc)
	{
		UInt32 x1,x2,y1,y2,z1,z2;
		
		intfc->ReadRecordData(&x1, sizeof(UInt32));
		intfc->ReadRecordData(&y1, sizeof(UInt32));
		intfc->ReadRecordData(&z1, sizeof(UInt32));
		intfc->ReadRecordData(&x2, sizeof(UInt32));
		intfc->ReadRecordData(&y2, sizeof(UInt32));
		intfc->ReadRecordData(&z2, sizeof(UInt32));

		NiPoint3 * coords = new NiPoint3(x1, y1, z1);
		NiPoint3 * rot = new NiPoint3(x2, y2, z2);
		
		return DurabilityTracker::ContainerKey(coords, rot);
	}

	std::unordered_map<DurabilityTracker::ContainerKey, std::unordered_map < TESForm*, DurabilityTracker::ContainerValue>, DurabilityTracker::ContainerKeyHash > UnserializeGnrlContianerMap(SKSESerializationInterface * intfc)
	{
		UInt32 mapsize;
		intfc->ReadRecordData(&mapsize, sizeof(mapsize));

		std::unordered_map<DurabilityTracker::ContainerKey, std::unordered_map < TESForm*, DurabilityTracker::ContainerValue>, DurabilityTracker::ContainerKeyHash > outmap;

		for (UInt32 i = 0; i < mapsize; i++)
		{
			outmap.insert(std::pair<DurabilityTracker::ContainerKey, std::unordered_map < TESForm*, DurabilityTracker::ContainerValue> >(UnserializeContainerKey(intfc), UnserializeSpecContainerMap(intfc)));
		}

		return outmap;
	}
}