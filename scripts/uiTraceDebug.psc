ScriptName uiTraceDebug extends ObjectReference
{for debugging the "hooked" menu}

Import plugin_spis
Import ModEvent

String TEST = "TEST"
Actor Property PlayerREF auto


Event OnInit()
	;ModEvent.Create("uiInventoryMenuInit")
	;;;;ModEvent.PushString("uiInventoryMenuInit", )
	;ModEvent.Create("uiInitExtensions")
	;ModEvent.Create("uihandleInput")
	;ModEvent.Create("uionExitMenuRectClick")
	;ModEvent.Create("uiStartMenuFade")
	;ModEvent.Create("uionFadeCompletion")
	;ModEvent.Create("uionShowItemsList")
	;ModEvent.Create("uiOnHighlightChange")
	;ModEvent.Create("uiUpdateBottomBarButtons")
	;ModEvent.Create("uionHidItemsList")
	;ModEvent.Create("uionItemSelect")
	;;;;ModEvent.PushString()
	;ModEvent.Create("uiAttemptEquip")
	;ModEvent.Create("uiDropItem")
	;ModEvent.Create("uiAttemptChargeItem")
	;ModEvent.Create("uionQuantityMenuSelect")
	;ModEvent.Create("uionMouseRotationFastClick")
	;ModEvent.Create("uionItemCardListPress")
	;ModEvent.Create("uionItemCardSubMenuAction")
	;ModEvent.Create("uiSetPlatform")
	;ModEvent.Create("uiItemRotating")
	
	;RegisterForModEvent("uiInventoryMenuInit", "uiInventoryMenuInit")
	;RegisterForModEvent("uiInitExtensions", "uiInitExtensions")
	;RegisterForModEvent("uihandleInput", "uihandleInput")
	;RegisterForModEvent("uionExitMenuRectClick", "uionExitMenuRectClick")
	;RegisterForModEvent("uiStartMenuFade", "uiStartMenuFade")
	;RegisterForModEvent("uionFadeCompletion", "uionFadeCompletion")
	;RegisterForModEvent("uionShowItemsList", "uionShowItemsList")
	;RegisterForModEvent("uiOnHighlightChange", "uiOnHighlightChange")
	;RegisterForModEvent("uiUpdateBottomBarButtons", "uiUpdateBottomBarButtons")
	;RegisterForModEvent("uionHidItemsList", "uionHidItemsList")
	;RegisterForModEvent("uionItemSelect", "uionItemSelect")
	;RegisterForModEvent("uiAttemptEquip", "uiAttemptEquip")
	;RegisterForModEvent("uiDropItem", "uiDropItem")
	;RegisterForModEvent("uiAttemptChargeItem", "uiAttemptChargeItem")
	;RegisterForModEvent("uionQuantityMenuSelect", "uionQuantityMenuSelect")
	;RegisterForModEvent("uionMouseRotationFastClick", "uionMouseRotationFastClick")
	;RegisterForModEvent("uionItemCardListPress", "uionItemCardListPress")
	;RegisterForModEvent("uionItemCardSubMenuAction", "uionItemCardSubMenuAction")
	;RegisterForModEvent("uiSetPlatform", "uiSetPlatform")
	;RegisterForModEvent("uiItemRotating", "uiItemRotating")
	
	RegisterForModEvent("uiAction", "onuiAction")
EndEvent

Event onuiAction(String eventName, String strArg, float numArg, Form sender)
	plugin_spis.outputToOpenDebugLog(strArg)
EndEvent