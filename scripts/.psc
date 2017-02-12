ScriptName DurabilityTrackers Extends ObjectReference
{Tracks items that have durability}

import Armor
import Weapon
import Actor

import JContainers
import plugin_spis

Actor Property PlayerREF Auto


int PlayerInventory = JArray.object()

Function GetInvetory(ObjectReference ref, int Array)
	int tnumitems = ref.GetNumItems()
	int n = 0
	
	While n < tnumitems
		JArray.addForm(Array, ref.GetNthForm(n))
	EndWhile
	
EndFunction

Event OnInit()
	GetInventory(PlayerREF, PlayerInventory)
EndEvent 

Event OnActivate(ObjectReference akActionRef)
	outputToOpenSKSELog(JArray.getForm(PlayerInventory, 0).GetDisplayName())
EndEvent 