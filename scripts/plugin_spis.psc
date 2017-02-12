ScriptName plugin_spis Hidden

;Function RearrangeExtraContChanges(ObjectReference ContainerREF, Form FormToSplit) global native                                              	NONEXISTANT
;Function spisDebug(ObjectReference ContainerREF, ObjectReference ContainerREF2, Weapon WeaponREF, Enchantment Enchant) global native          	NONEXISTANT
;Function outputToOpenDebugLog(String msg) global native																						NONEXISTANT
;Function MarkTESContainerZero(ObjectReference ContainerREF) global native																		NONEXISTANT
;Function GetContainerReady(ObjectReference ContainerREF) global native																			NONEXISTANT
;Function UpdateContainer(ObjectReference ContainerREF) global native																			NONEXISTANT

;bool AddEntry(StaticFunctionTag *base, UInt32 space, TESObjectREFR * container, TESForm * item, TESObjectREFR * groundItem, UInt32 amount, UInt32 durability, UInt32 maxDurability)
;void PrintFound(StaticFunctionTag *base, UInt32 findType, TESObjectREFR * container, TESForm * item, UInt32 durability)

Bool Function InitializeDurabilityTracker() global native
Bool Function AddEntry(Int space, ObjectReference cont, Form item, ObjectReference groundItem, Int amount, Int durability, Int maxDurability) global native
Bool Function RemoveEntry(Int space, ObjectReference cont, Form item, ObjectReference groundItem, Int amount, Int removeType, Int durability) global native
Bool Function MoveEntry(Int space, ObjectReference contFrom, ObjectReference contTo, Form item, Int amount, Int durability) global native
Function PrintFound(Int findType, ObjectReference cont, Form item, Int durability) global native 
Bool Function WrapEntries(ObjectReference cont) global native 
ObjectReference Function GetCurrentContainer() global native
Bool Function SetCurrentContainer(ObjectReference cont) global native 
Int Function GetCurrentDurability() global native
Int Function GetCurrentMaxDurability() global native 