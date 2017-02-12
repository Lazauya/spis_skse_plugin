ScriptName DurabilityTrackers Extends Form

import ObjectReference
import plugin_spis
import UI

Actor Property PlayerREF auto

Event OnLoad()
	If(plugin_spis.InitializeDurabilityTracker())
		Debug.Trace("_DEBUG_PREF_DTRK_Loaded/Initialized")
		RegisterForMenu("TweenMenu")
		plugin_spis.WrapEntries((PlayerREF as ObjectReference))
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
			plugin_spis.MoveEntry(0, akSourceContainer, (PlayerREF as ObjectReference), akItemReference, aiItemCount, plugin_spis.GetCurrentDurability())
		Else
			plugin_spis.MoveEntry(2, akSourceContainer, (PlayerREF as ObjectReference), akItemReference, aiItemCount, 0)
		EndIf
	EndIf
EndEvent

Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
	If( (akBaseItem as Weapon) || (akBaseItem as Armor) )
		If(akDestContainer)
			If(IsMenuOpen("Container Menu"))
				plugin_spis.SetCurrentContainer(akDestContainer)
			EndIf
			plugin_spis.MoveEntry(0, (PlayerREF as ObjectReference), akDestContainer, akItemReference, aiItemCount, plugin_spis.GetCurrentDurability())
		Else
			plugin_spis.MoveEntry(3, (PlayerREF as ObjectReference), akDestContainer, akItemReference, aiItemCount, plugin_spis.GetCurrentDurability())
		EndIf	
	EndIf
EndEvent

