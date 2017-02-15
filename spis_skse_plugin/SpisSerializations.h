#include "SplitItemStacks.h"

namespace plugin_spis
{
	void Serialization_Revert(SKSESerializationInterface * intfc);
	void Serialization_Save(SKSESerializationInterface * intfc);
	/*{
		//_MESSAGE("_DEBUG_PREF_SAVE_invoked");
		SerializeGnrlContainerMap(globalDurabilityTracker->ContainerEntries, intfc);
	}*/
	void Serialization_Load(SKSESerializationInterface * intfc);

	//for container maps load/save
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
		kTypeContainerMap,
		kTypeInitialized,
		kTypeGroundMap,
		kTypeEquippedStates,
		kTypeCurrentContainer,
		kTypeCurrentDurability
	};

	void SerializeContainerKey(DurabilityTracker::ContainerKey key, SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_SAVE_scnk1");
		/*//intfc->OpenRecord('CMAP', 0);
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
		//}*/
		//TESObjectREFR* ref = key.getContainerRef();
		//ExtraReferenceHandle * actualrefhan = static_cast<ExtraReferenceHandle*>(ref->extraData.GetByType(kExtraData_ReferenceHandle));
		////_MESSAGE("_DEBUG_PREF_SAVE_scnk1.1,%d", actualrefhan);
		UInt32 refhandle = key.getContainerRef()->CreateRefHandle();
		//_MESSAGE("_DEBUG_PREF_SAVE_scnk2,%d", refhandle);
		intfc->WriteRecordData(&refhandle, sizeof(UInt32));
	}

	void SerializeTESForm(TESForm* form, SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_SAVE_stsf1");
		//intfc->OpenRecord(kTypeForm, 0);
		intfc->WriteRecordData(&(form->formID), sizeof(UInt32));
		//_MESSAGE("_DEBUG_PREF_SAVE_stsf2,%d", form->formID);
	}
	
	void SerializeTESObjectREFR(TESObjectREFR * ref);

	//only for basic types, sorry boys!
	template<typename T1, typename T2>
	void SerializePair(std::pair<T1, T2> pair, SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_SAVE_szpr1,%d,%d",pair.first,pair.second);
		//intfc->OpenRecord(kTypePair, 0);
		intfc->WriteRecordData(&(pair.first), sizeof(T1));
		intfc->WriteRecordData(&(pair.second), sizeof(T2));
	}

	template<typename T1, typename T2>
	void SerializePairVector(std::vector<std::pair<T1, T2> > vecpair, SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_SAVE_szpv1");
		//intfc->OpenRecord(kTypeVectorSize, 0);
		////_MESSAGE("_DEBUG_PREF_SAVE_pairvec1");
		UInt32 vs = vecpair.size();
		//_MESSAGE("_DEBUG_PREF_SAVE_szpv2,%d",vs);
		////_MESSAGE("_DEBUG_PREF_SAVE_pairvec2");
		intfc->WriteRecordData(&(vs), sizeof(vs));
		////_MESSAGE("_DEBUG_PREF_SAVE_pairvec3");

		//intfc->OpenRecord(kTypeVector, 0);
		for (UInt16 i = 0; i < vecpair.size(); i++)
		{
			//_MESSAGE("_DEBUG_PREF_SAVE_szpv3,%d",i);
			SerializePair<T1, T2>(vecpair[i], intfc);
		}
	}

	void SerializeSpecContainerMap(std::unordered_map<TESForm*, DurabilityTracker::ContainerValue> * specmap, SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_SAVE_sscm1");
		//intfc->OpenRecord(kTypeSpecMapCount, 0);
		UInt32 ms = specmap->size(); //_MESSAGE("_DEBUG_PREF_SAVE_sscm2,%d", ms);
		intfc->WriteRecordData(&ms, sizeof(UInt32));

		int i = 0;
		for (auto it = specmap->begin(); it != specmap->end(); it++, i++)
		{
			//_MESSAGE("_DEBUG_PREF_SAVE_sscm3,%d",i);
			SerializeTESForm(it->first, intfc);
			SerializePairVector<UInt32, UInt32>(it->second.Durabilities, intfc);
		}
	}

	void SerializeGnrlContainerMap(std::unordered_map<DurabilityTracker::ContainerKey, std::unordered_map < TESForm*, DurabilityTracker::ContainerValue>, DurabilityTracker::ContainerKeyHash >* gnrlmap, SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_SAVE_sgcm1");
		intfc->OpenRecord(kTypeContainerMap, 0);
		UInt32 ms = gnrlmap->size(); //_MESSAGE("_DEBUG_PREF_SAVE_sgcm2,%d", ms);
		intfc->WriteRecordData(&ms, sizeof(UInt32));

		int i = 0;
		for (auto itt = gnrlmap->begin(); itt != gnrlmap->end(); itt++, i++)
		{
			//_MESSAGE("_DEBUG_PREF_SAVE_sgcm3,%d",i);
			SerializeContainerKey(itt->first, intfc);
			SerializeSpecContainerMap(&(itt->second), intfc);
		}
	}

	template<typename T1, typename T2>
	std::pair<T1, T2> UnserializePair(SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_UCMP_uspr1");
		T1 out1;
		T2 out2;
		//_MESSAGE("_DEBUG_PREF_UCMP_uspr2");
		intfc->ReadRecordData(&out1, sizeof(T1));
		intfc->ReadRecordData(&out2, sizeof(T2));
		//_MESSAGE("_DEBUG_PREF_UCMP_uspr3,%d,%d",out1,out2);
		return std::pair<T1, T2>(out1, out2);
	}

	template<typename T1, typename T2>
	std::vector<std::pair<T1, T2> > UnserializePairVector(SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_UCMP_uspv1");
		UInt32 vecsize;
		std::vector<std::pair<T1, T2> > outvec;

		intfc->ReadRecordData(&vecsize, sizeof(vecsize));
		_MESSAGE("_DEBUG_PREF_UCMP_uspv2,%d",vecsize);
		for (UInt32 i = 0; i < vecsize; i++)
		{
			//_MESSAGE("_DEBUG_PREF_UCMP_uspv3,%d",i);
			outvec.push_back(UnserializePair<T1, T2>(intfc));
		}
		//_MESSAGE("_DEBUG_PREF_UCMP_uspv4");
		return outvec;
	}

	std::pair<TESForm*, DurabilityTracker::ContainerValue> UnserializeSpecContainerValueTESFormPair(SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_UCMP_uscv1");
		UInt32 formid;
		intfc->ReadRecordData(&formid, sizeof(UInt32));
		//_MESSAGE("_DEBUG_PREF_UCMP_uscv2,%d", formid);
		std::vector<std::pair<UInt32, UInt32> > mapvec = UnserializePairVector<UInt32, UInt32>(intfc); //_MESSAGE("_DEBUG_PREF_UCMP_uscv3");
		DurabilityTracker::ContainerValue outcontval(LookupFormByID(formid), mapvec.size()); //_MESSAGE("_DEBUG_PREF_UCMP_uscv4");
		outcontval.Durabilities = mapvec; //_MESSAGE("_DEBUG_PREF_UCMP_uscv5");

		return std::pair<TESForm*, DurabilityTracker::ContainerValue>(LookupFormByID(formid), outcontval);
	}

	std::unordered_map < TESForm*, DurabilityTracker::ContainerValue > UnserializeSpecContainerMap(SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_UCMP_uscm1");
		UInt32 paircount;
		intfc->ReadRecordData(&paircount, sizeof(UInt32));
		//_MESSAGE("_DEBUG_PREF_UCMP_uscm2,%d", paircount);
		std::unordered_map < TESForm*, DurabilityTracker::ContainerValue > outmap;
		//_MESSAGE("_DEBUG_PREF_UCMP_uscm3");
		for (UInt32 i = 0; i < paircount; i++)
		{
			//_MESSAGE("_DEBUG_PREF_UCMP_uscm4,%d", i);
			outmap.insert(UnserializeSpecContainerValueTESFormPair(intfc));
		}
		//_MESSAGE("_DEBUG_PREF_UCMP_uscm5");

		return outmap;
	}

	DurabilityTracker::ContainerKey UnserializeContainerKey(SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_UCMP_usck1");
		UInt32 refhandle;
		intfc->ReadRecordData(&refhandle, sizeof(UInt32));
		//_MESSAGE("_DEBUG_PREF_UCMP_usck2,%d", refhandle);

		TESObjectREFR * outref = nullptr;
		//ExtraReferenceHandle * tempextraref = ExtraReferenceHandle::Create();
		//tempextraref->handle = refhandle;
		LookupREFRByHandle(&refhandle, &outref);
		//LookupREFRByHandle(&refhandle, &outref);
		//_MESSAGE("_DEBUG_PREF_UCMP_usck3,%d,%d", outref->baseForm->formID ,outref->handleRefObject.GetRefCount());
		return DurabilityTracker::ContainerKey(outref);
	}

	std::unordered_map<DurabilityTracker::ContainerKey, std::unordered_map < TESForm*, DurabilityTracker::ContainerValue>, DurabilityTracker::ContainerKeyHash > UnserializeGnrlContianerMap(SKSESerializationInterface * intfc)
	{
		//_MESSAGE("_DEBUG_PREF_UCMP_ugcm1");
		UInt32 mapsize;
		//_MESSAGE("_DEBUG_PREF_UCMP_ugcm2");
		intfc->ReadRecordData(&mapsize, sizeof(mapsize));
		//_MESSAGE("_DEBUG_PREF_UCMP_ugcm3");
		std::unordered_map<DurabilityTracker::ContainerKey, std::unordered_map < TESForm*, DurabilityTracker::ContainerValue>, DurabilityTracker::ContainerKeyHash > outmap;
		//_MESSAGE("_DEBUG_PREF_UCMP_ugcm4,%d", mapsize);
		for (UInt32 i = 0; i < mapsize; i++)
		{
			//_MESSAGE("_DEBUG_PREF_UCMP_ugcm5,%d", i);
			DurabilityTracker::ContainerKey tk = UnserializeContainerKey(intfc);
			//_MESSAGE("_DEBUG_PREF_UCMP_ugcm5.1,%d", i);
			std::unordered_map < TESForm*, DurabilityTracker::ContainerValue> tm = UnserializeSpecContainerMap(intfc);
			//_MESSAGE("_DEBUG_PREF_UCMP_ugcm5.2,%d", i);
			outmap.insert(std::pair<DurabilityTracker::ContainerKey, std::unordered_map < TESForm*, DurabilityTracker::ContainerValue> >(tk, tm));
		}
		//_MESSAGE("_DEBUG_PREF_UCMP_ugcm6", mapsize);
		return outmap;
	}

	//groundmaps load/save

	void SerializeGroundKey(DurabilityTracker::GroundKey key, SKSESerializationInterface * intfc)
	{
		UInt32 refhandle = key.getObjectRef()->CreateRefHandle();
		intfc->WriteRecordData(&refhandle, sizeof(UInt32));
	}

	void SerializeGroundValue(DurabilityTracker::GroundValue val, SKSESerializationInterface * intfc)
	{
		SerializePair<UInt32, UInt32>(val, intfc);
	}
	
	void SerializeGroundMap(std::unordered_map<DurabilityTracker::GroundKey, DurabilityTracker::GroundValue, DurabilityTracker::GroundKeyHash> * map, SKSESerializationInterface * intfc)
	{
		intfc->OpenRecord(kTypeGroundMap, 0);
		UInt32 mapsize = map->size();
		intfc->WriteRecordData(&mapsize, sizeof(UInt32));
		
		for (auto it = map->begin(); it != map->end(); it++)
		{
			SerializeGroundKey(it->first, intfc);
			SerializeGroundValue(it->second, intfc);
		}
	}

	DurabilityTracker::GroundKey UnserializeGroundKey(SKSESerializationInterface * intfc)
	{
		UInt32 refhandle;
		intfc->ReadRecordData(&refhandle, sizeof(UInt32));

		TESObjectREFR * outref = nullptr;
		LookupREFRByHandle(&refhandle, &outref);

		return DurabilityTracker::GroundKey(outref);
	}

	DurabilityTracker::GroundValue UnserializeGroundValue(SKSESerializationInterface * intfc)
	{
		return UnserializePair<UInt32, UInt32>(intfc);
	}

	std::unordered_map<DurabilityTracker::GroundKey, DurabilityTracker::GroundValue, DurabilityTracker::GroundKeyHash> UnserializeGroundMap(SKSESerializationInterface * intfc)
	{
		UInt32 mapsize;
		intfc->ReadRecordData(&mapsize, sizeof(mapsize));

		std::unordered_map<DurabilityTracker::GroundKey, DurabilityTracker::GroundValue, DurabilityTracker::GroundKeyHash> outmap;

		for (UInt32 i = 0; i < mapsize; i++)
		{
			DurabilityTracker::GroundKey tgk = UnserializeGroundKey(intfc);
			DurabilityTracker::GroundValue tgv = UnserializeGroundValue(intfc);
			outmap.insert(std::pair<DurabilityTracker::GroundKey, DurabilityTracker::GroundValue>(tgk, tgv));
		}

		return outmap;
	}

	void SerializeFormUInt32UInt32Tuple(SKSESerializationInterface * intfc, std::tuple<TESForm*, UInt32, UInt32> tup)
	{
		//SerializeTESForm(std::get<0>(tup), intfc);
		UInt32 id = std::get<0>(tup) == nullptr ? 0 : std::get<0>(tup)->formID, max = std::get<1>(tup) ? std::get<1>(tup) : 0, dur = std::get<2>(tup) ? std::get<2>(tup) : 0;
		intfc->WriteRecordData(&id, sizeof(UInt32));
		_MESSAGE("_DEBUG_PREF_SFUU_1,%d", std::get<0>(tup) == nullptr ? 0 : std::get<0>(tup)->formID);
		intfc->WriteRecordData(&max, sizeof(UInt32));
		_MESSAGE("_DEBUG_PREF_SFUU_2,%d", std::get<1>(tup));
		intfc->WriteRecordData(&dur, sizeof(UInt32));
		_MESSAGE("_DEBUG_PREF_SFUU_3,%d", std::get<2>(tup));
	}
	
	void SerializeEquippedStates(SKSESerializationInterface * intfc, EquippedStates * equips)
	{
		intfc->OpenRecord(kTypeEquippedStates, 0);
		for (UInt32 i = 0; i < 18; i++)
		{
			SerializeFormUInt32UInt32Tuple(intfc, equips->EquippedEntries[i]);
		}
		intfc->WriteRecordData(&(equips->twoHanded), sizeof(bool));
	}

	std::tuple<TESForm*, UInt32, UInt32> UnserializeFormUInt32UInt32Tuple(SKSESerializationInterface * intfc)
	{
		UInt32 outid, dur, maxdur;
		intfc->ReadRecordData(&outid, sizeof(UInt32));
		TESForm * outform = LookupFormByID(outid);
		_MESSAGE("_DEBUG_PREF_UFUU_1,%d", outform == nullptr ? 0 : outform->formID);
		intfc->ReadRecordData(&maxdur, sizeof(UInt32));
		_MESSAGE("_DEBUG_PREF_UFUU_2,%d", maxdur);
		intfc->ReadRecordData(&dur, sizeof(UInt32));
		_MESSAGE("_DEBUG_PREF_UFUU_3,%d", dur);
		return std::tuple<TESForm*, UInt32, UInt32>(outform, maxdur, dur);
	}

	EquippedStates UnserializeEquippedStates(SKSESerializationInterface * intfc)
	{
		EquippedStates eqs;
		for (UInt32 i = 0; i < 18; i++)
		{
			_MESSAGE("_DEBUG_PREF_UEQS_1,%d", i);
			eqs.EquippedEntries[i] = UnserializeFormUInt32UInt32Tuple(intfc);
		}
		bool outbool;
		intfc->ReadRecordData(&outbool, sizeof(bool));
		_MESSAGE("_DEBUG_PREF_UEQS_2,%d", outbool);
		eqs.twoHanded = outbool;
		_MESSAGE("_DEBUG_PREF_UEQS_3");
		return eqs;
	}

	void SerializeCurrentContainer(SKSESerializationInterface * intfc, CurrentContainer * cc)
	{
		intfc->OpenRecord(kTypeCurrentContainer, 0);
		UInt32 refhan = cc->GetContainer() == nullptr ? 0 : cc->GetContainer()->CreateRefHandle();
		intfc->WriteRecordData(&refhan, sizeof(UInt32));
	}

	CurrentContainer UnserializeCurrentContainer(SKSESerializationInterface * intfc)
	{
		UInt32 refhan;
		intfc->ReadRecordData(&refhan, sizeof(UInt32));
		TESObjectREFR * outref = nullptr;
		LookupREFRByHandle(&refhan, &outref);
		return CurrentContainer(outref);
	}

	void SerializeCurrentDurability(SKSESerializationInterface * intfc, CurrentDurability * cd)
	{
		intfc->OpenRecord(kTypeCurrentDurability, 0);
		UInt32 d = cd->GetDurability();
		UInt32 md = cd->GetMaxDurability();
		intfc->WriteRecordData(&md, sizeof(UInt32));
		intfc->WriteRecordData(&d, sizeof(UInt32));
	}

	CurrentDurability UnserializeCurrentDurability(SKSESerializationInterface * intfc)
	{
		UInt32 outd, outmd;
		intfc->ReadRecordData(&outmd, sizeof(UInt32));
		intfc->ReadRecordData(&outd, sizeof(UInt32));
		return CurrentDurability(outmd, outd);
	}
}