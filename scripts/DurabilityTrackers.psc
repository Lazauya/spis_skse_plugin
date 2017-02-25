ScriptName DurabilityTrackers Extends ReferenceAlias

import ObjectReference
import plugin_spis
import UI

Actor Property PlayerREF auto
ObjectReference Property CurrentRef auto

Event OnLoad()
	If(plugin_spis.InitializeDurabilityTracker())
		Debug.Trace("_DEBUG_PREF_DTRK_Loaded/Initialized")
		RegisterForMenu("TweenMenu")
		RegisterForCrosshairRef()
		plugin_spis.WrapEntries((PlayerREF as ObjectReference))
		plugin_spis.WrapEquipStates((PlayerREF as ObjectReference))
	EndIf
EndEvent

Event OnMenuOpen(string asMenuName)
	If((asMenuName == "TweenMenu"))
		plugin_spis.SetCurrentContainer((PlayerREF as ObjectReference))
		Debug.Trace("_DEBUG_PREF_DTRK_InventoryMenu opened")
	EndIf
EndEvent

Event OnItemAdded(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akSourceContainer)
	Debug.Trace("_DEBUG_PREF_DTRK_ItemAdded")
	If( (akBaseItem as Weapon) || (akBaseItem as Armor) )
		If(akSourceContainer)
			If(IsMenuOpen("Container Menu"))
				plugin_spis.SetCurrentContainer(akSourceContainer)
			EndIf
			Debug.Trace("_DEBUG_PREF_DTRK_ia1")
			plugin_spis.MoveEntry(0, akSourceContainer, (PlayerREF as ObjectReference), akBaseItem, akItemReference, aiItemCount, plugin_spis.GetCurrentDurability(), plugin_spis.GetCurrentMaxDurability())
		ElseIf(!(plugin_spis.IsCurrentGroundKeyNull()))
			plugin_spis.MoveEntry(2, akSourceContainer, (PlayerREF as ObjectReference), akBaseItem, akItemReference, aiItemCount, 0, 0)
			plugin_spis.SetCurrentGroundKeyNull()
		Else
			Debug.Trace("_DEBUG_PREF_DTRK_ia2")
			plugin_spis.MoveEntry(4, akSourceContainer, (PlayerREF as ObjectReference), akBaseItem, akItemReference, aiItemCount, 0, 0)
		EndIf
	EndIf
EndEvent

Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
	Debug.Trace("_DEBUG_PREF_DTRK_ItemRemoved")
	If( (akBaseItem as Weapon) || (akBaseItem as Armor) )
		If(akDestContainer)
			If(IsMenuOpen("Container Menu"))
				plugin_spis.SetCurrentContainer(akDestContainer)
			EndIf
			Debug.Trace("_DEBUG_PREF_DTRK_ir1")
			plugin_spis.MoveEntry(0, (PlayerREF as ObjectReference), akDestContainer, akBaseItem, akItemReference, aiItemCount, plugin_spis.GetCurrentDurability(), plugin_spis.GetCurrentMaxDurability())
		Else
			Debug.Trace("_DEBUG_PREF_DTRK_ir2")
			plugin_spis.MoveEntry(3, (PlayerREF as ObjectReference), akDestContainer, akBaseItem, akItemReference, aiItemCount, plugin_spis.GetCurrentDurability(), plugin_spis.GetCurrentMaxDurability())
		EndIf	
	EndIf
EndEvent

Event OnCrosshairRefChange(ObjectReference ref)
	Debug.Trace("_DEBUG_PREF_DTRK_crosshairrefchange")
	If(ref)
		plugin_spis.SetCurrentGroundKey(ref)
	EndIf
EndEvent 