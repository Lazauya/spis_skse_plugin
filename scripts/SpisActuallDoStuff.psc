ScriptName SpisActuallDoStuff extends ObjectReference
{lets try it!}

Import plugin_spis
Import ModEvent

ObjectReference Property ContainerRefrp Auto
ObjectReference Property ContainerRefrc Auto
ObjectReference Property GroundItem Auto
Form Property formProp Auto
Actor Property PlayerREF auto
Weapon Property FormToSplitW Auto
Enchantment Property ench Auto

String hello = "I'm in!"

Event OnInit()
	;Int handle = ModEvent.Create("traceToSKSELog")
	RegisterForModEvent("traceToSKSELog", "ontraceToSKSELog")
endEvent

Event OnActivate(ObjectReference akActionRef)
    Debug.MessageBox("Hello, World!")
	Bool init = plugin_spis.InitializeDurabilityTracker()
	if(init)
		Debug.MessageBox("init true")
		plugin_spis.WrapEntries((PlayerREF as ObjectReference))
		plugin_spis.SetCurrentContainer((PlayerREF as ObjectReference))
		;plugin_spis.AddEntry(0, (PlayerREF as ObjectReference), (FormToSplitW as Form), GroundItem, 1, 50, 100)
		plugin_spis.PrintFound(0, (PlayerREF as ObjectReference), (FormToSplitW as Form), 100)
	endIf
endEvent

Event ontraceToSKSELog(string a_eventName, string a_strArg, float a_numArg, Form a_sender)
	Debug.MessageBox(hello)
	;plugin_spis.outPutToOpenDebugLog(hello)
	;UnregisterForModEvent("traceToSKSELog")
endEvent