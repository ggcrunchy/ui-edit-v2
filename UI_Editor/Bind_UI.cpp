#include "App.h"
#include "../UserInt/UserInterface.h"
#include <cstdio>
#include <cstdlib>

///
/// Globals
///
UI::State * g_Main;

///
/// Lua UI helper functions
///
static lua_State * Method (UI::Widget * widget, char const * method)
{
	lua_State * L = static_cast<lua_State*>(g_Main->GetContext());

	lua_getglobal(L, "W_PSet");	// "W_PSet" -> wp
	lua_pushlightuserdata(L, widget);	// W_PSet widget
	lua_pcall(L, 1, 1, 0);	// wp
	lua_pushstring(L, method);	// wp m
	lua_gettable(L, -2);// wp M
	lua_insert(L, -2);	// M wp

	return L;
}

static void AddCoordinates (lua_State * L, UI::Widget * widget)
{
	Method(widget, "GetRect");	// GetRect wp

	lua_pushstring(L, "xywh");	// GetRect wp "xywh"
	lua_pcall(L, 2, 4, 0);	// x y w h
}

///
/// Handlers
///
static void EventFunc (UI::Widget * widget, UI::Event event)
{
	lua_State * L = Method(widget, "ProcessEvent");	// ProcessEvent wp

	lua_pushnumber(L, event);	// ProcessEvent wp e
	lua_pcall(L, 2, 0, 0);
}

static void SignalFunc (UI::Widget * widget)
{
	lua_State * L = Method(widget, "SignalTest");	// SignalTest wp

	AddCoordinates(L, widget);	// SignalTest wp x y w h

	lua_pcall(L, 5, 0, 0);
}

static void UpdateFunc (UI::Widget * widget)
{
	lua_State * L = Method(widget, "Update");	// Update wp

	AddCoordinates(L, widget);	// Update wp x y w h
	
	lua_pcall(L, 5, 0, 0);
}

#define UW_(L, index) UDT<UI::Widget*>(L, index)
#define UP_(L, index) UDT<UI::Pane*>(L, index)
#define UR_(L, index) UDT<UI::Range*>(L, index)

/// @brief Int return; typed, unsigned, unsigned argument
template<typename T> static int I_TUU (lua_State * L, int (T::* func)(Uint32, Uint32))
{
	(UDT<T*>(L, 1)->*func)(U(L, 2), U(L, 3));

	return 0;
}

/// @brief Int return; typed, unsigned, [out] unsigned reference argument; in-out policy
template<typename T, bool bIO> static int I_TUrU (lua_State * L, int (T::* func)(Uint32, Uint32 &))
{
	Uint32 uint = bIO ? U(L, 3) : 0;
	
	if ((UDT<T*>(L, 1)->*func)(U(L, 2), uint) != 0)
	{
		lua_pushnumber(L, uint);

		return 1;
	}

	return 0;
}

/// @brief Int return; typed argument
template<typename T> static int I_T (lua_State * L, int (T::* func)(void))
{
	(UDT<T*>(L, 1)->*func)();

	return 0;
}

/// @brief Int return; typed argument, push policy
template<typename T> static int I_Tp (lua_State * L, int (T::* func)(void))
{
	lua_pushlightuserdata(L, g_Main);

	return I_T<T>(L, func);
}

/// @brief Int return; typed, boolean argument
template<typename T> static int I_TB (lua_State * L, int (T::* func)(bool))
{
	(UDT<T*>(L, 1)->*func)(B(L, 2));

	return 0;
}

/// @brief Typed return; typed argument
template<typename T, typename O> static int T_T (lua_State * L, O * (T::* func)(void))
{
	O * object = (UDT<T*>(L, 1)->*func)();
	
	if (object != 0)
	{
		lua_pushlightuserdata(L, object);

		return 1;
	}

	return 0;
}

/// @brief Typed return; typed argument, push policy
template<typename T, typename O> static int T_Tp (lua_State * L, O * (T::* func)(void))
{
	lua_pushlightuserdata(L, g_Main);

	return T_T<T, O>(L, func);
}

/// @brief Int return; typed, unsigned argument
template<typename T> static int I_TU (lua_State * L, int (T::* func)(Uint32))
{
	(UDT<T*>(L, 1)->*func)(U(L, 2));

	return 0;
}

/// @brief Int return; typed, [out] unsigned reference argument
template<typename T> static int I_TrU (lua_State * L, int (T::* func)(Uint32 &))
{
	Uint32 uint;	
	
	if ((UDT<T*>(L, 1)->*func)(uint) != 0)
	{
		lua_pushnumber(L, uint);

		return 1;
	}

	return 0;
}

/// @brief Void return; typed argument
template<typename T> static int V_T (lua_State * L, void (T::* func)(void))
{
	(UDT<T*>(L, 1)->*func)();

	return 0;
}

/// @brief Boolean return; typed argument
template<typename T> static int B_T (lua_State * L, bool (T::* func)(void))
{
	lua_pushboolean(L, (UDT<T*>(L, 1)->*func)());

	return 1;
}

/// @brief Boolean return; typed, unsigned argument
template<typename T> static int B_TU (lua_State * L, bool (T::* func)(Uint32))
{
	lua_pushboolean(L, (UDT<T*>(L, 1)->*func)(U(L, 2)));
	
	return 1;
}

/// @brief Boolean return; typed, unsigned, boolean argument
template<typename T> static int B_TUB (lua_State * L, bool (T::* func)(Uint32, bool))
{
	lua_pushboolean(L, (UDT<T*>(L, 1)->*func)(U(L, 2), B(L, 3)));

	return 1;
}

///
/// Composite functions
///
static int CompositeCreatePart (lua_State * L)
{
	return T_T<UI::Composite, UI::Part>(L, &UI::Composite::CreatePart);
}

///
/// Interval functions
///
static int IntervalClear (lua_State * L)
{
	return V_T<UI::Interval>(L, &UI::Interval::Clear);
}

static int IntervalGet (lua_State * L)
{
	Uint32 where, count;

	if (UDT<UI::Interval*>(L, 1)->Get(where, count) != 0)
	{
		lua_pushnumber(L, where);
		lua_pushnumber(L, count);

		return 2;
	}

	return 0;
}

static int IntervalRemove (lua_State * L)
{
	return V_T<UI::Interval>(L, &UI::Interval::Remove);
}

static int IntervalSet (lua_State * L)
{
	return I_TUrU<UI::Interval, true>(L, &UI::Interval::Set);
}

///
/// Part functions
///
static int PartRemove (lua_State * L)
{
	return V_T<UI::Part>(L, &UI::Part::Remove);
}

static int PartSetContext (lua_State * L)
{
	UDT<UI::Part*>(L, 1)->SetContext(UD(L, 2));

	return 0;
}

static int PartSignal (lua_State * L)
{
	return V_T<UI::Part>(L, &UI::Part::Signal);
}

static int PartIsEntered (lua_State * L)
{
	return B_T<UI::Part>(L, &UI::Part::IsEntered);
}

static int PartIsGrabbed (lua_State * L)
{
	return B_T<UI::Part>(L, &UI::Part::IsGrabbed);
}

static int PartGetContext (lua_State * L)
{
	return T_T<UI::Part, void>(L, &UI::Part::GetContext);
}

///
/// Range functions
///
static int RangeDeleteItems (lua_State * L)
{
	Uint32 uint = U(L, 3);
	
	if (UR_(L, 1)->DeleteItems(U(L, 2), uint) != 0)
	{
		lua_pushnumber(L, uint);

		return 1;
	}

	return 0;
}

static int RangeGetEnteredItem (lua_State * L)
{
	return I_TrU<UI::Range>(L, &UI::Range::GetEnteredItem);
}

static int RangeGetGrabbedItem (lua_State * L)
{
	return I_TrU<UI::Range>(L, &UI::Range::GetGrabbedItem);
}

static int RangeGetItemContext (lua_State * L)
{
	return I_TUrU<UI::Range, false>(L, &UI::Range::GetItemContext);
}

static int RangeSetItemContext (lua_State * L)
{
	return I_TUU<UI::Range>(L, &UI::Range::SetItemContext);
}

static int RangeGetItemCount (lua_State * L)
{
	return I_TrU<UI::Range>(L, &UI::Range::GetItemCount);
}

static int RangeInsertItems (lua_State * L)
{
	return I_TUU<UI::Range>(L, &UI::Range::InsertItems);
}

static int RangeCreateInterval (lua_State * L)
{
	return T_T<UI::Range, UI::Interval>(L, &UI::Range::CreateInterval);
}

static int RangeCreateSpot (lua_State * L)
{
	UI::Spot * spot = UR_(L, 1)->CreateSpot(B(L, 2), B(L, 3));

	if (spot != 0)
	{
		lua_pushlightuserdata(L, spot);

		return 1;
	}

	return 0;
}

static int RangeSignalItem (lua_State * L)
{
	UR_(L, 1)->SignalItem(U(L, 2));

	return 0;
}

static int RangeIsItemSignaled (lua_State * L)
{
	Uint32 item;

	if (UR_(L, 1)->IsItemSignaled(item))
	{
		lua_pushboolean(L, true);
		lua_pushnumber(L, item);

		return 2;
	}

	lua_pushboolean(L, false);
	
	return 1;
}

static int RangeIsItemValid (lua_State * L)
{
	return B_TUB<UI::Range>(L, &UI::Range::IsItemValid);
}

///
/// Spot functions
///
static int SpotGet (lua_State * L)
{
	return I_TrU<UI::Spot>(L, &UI::Spot::Get);
}

static int SpotSet (lua_State * L)
{
	return I_TU<UI::Spot>(L, &UI::Spot::Set);
}

static int SpotRemove (lua_State * L)
{
	return V_T<UI::Spot>(L, &UI::Spot::Remove);
}

///
/// State functions
///
static int StateSetup (lua_State * L)
{
	g_Main = new UI::State(EventFunc, SignalFunc, UpdateFunc);

	g_Main->SetContext(L);

	return 0;
}

static int StateClose (lua_State * L)
{
	delete g_Main;

	g_Main = 0;

	return 0;
}

static int StateAbortSignalTests (lua_State * L)
{
	return I_Tp<UI::State>(L, &UI::State::AbortSignalTests);
}

static int StateClearInput (lua_State * L)
{
	return I_Tp<UI::State>(L, &UI::State::ClearInput);
}

static int StateGetFrameSize (lua_State * L)
{
	lua_pushlightuserdata(L, g_Main);

	return I_TrU<UI::State>(L, &UI::State::GetFrameSize);
}

static int StateGetPress (lua_State * L)
{
	bool bPress;	

	if (g_Main->GetPress(bPress) != 0)
	{
		lua_pushboolean(L, bPress);

		return 1;
	}

	return 0;
}

static int StatePropagateSignal (lua_State * L)
{
	g_Main->PropagateSignal(B(L, 1));

	return 0;
}

static int StateSetContext (lua_State * L)
{
	g_Main->SetContext(UD(L, 1));

	return 0;
}

static int StateUpdate (lua_State * L)
{
	return I_Tp<UI::State>(L, &UI::State::Update);
}

static int StateCreateWidget (lua_State * L)
{
	UI::Widget * widget = g_Main->CreateWidget(static_cast<UI::WidgetType>(U(L, 1)));

	if (widget != 0)
	{
		lua_pushlightuserdata(L, widget);

		return 1;
	}

	return 0;
}

static int StateFindWidget (lua_State * L)
{
	wchar_t Tag[256];	mbstowcs(Tag, S(L, 1), 512);

	UI::Widget * widget = g_Main->FindWidget(Tag);
	
	if (widget != 0)
	{
		lua_pushlightuserdata(L, widget);

		return 1;
	}

	return 0;
}

static int StateGetChoice (lua_State * L)
{
	return T_Tp<UI::State, UI::Widget>(L, &UI::State::GetChoice);
}

static int StateGetFrameHead (lua_State * L)
{
	return T_Tp<UI::State, UI::Widget>(L, &UI::State::GetFrameHead);
}

static int StateGetContext (lua_State * L)
{
	return T_Tp<UI::State, void>(L, &UI::State::GetContext);
}

static int StateUnsignal (lua_State * L)
{
	g_Main->Unsignal();

	return 0;
}

///
/// Widget functions
///

static int WidgetAddToFrame (lua_State * L)
{
	return I_T<UI::Widget>(L, &UI::Widget::AddToFrame);
}

static int WidgetAllowDockSignalTest (lua_State * L)
{
	return I_TB<UI::Widget>(L, &UI::Widget::AllowDockSignalTest);
}

static int WidgetAllowDockUpdate (lua_State * L)
{
	return I_TB<UI::Widget>(L, &UI::Widget::AllowDockUpdate);
}

static int WidgetAllowSignalTest (lua_State * L)
{
	return I_TB<UI::Widget>(L, &UI::Widget::AllowSignalTest);
}

static int WidgetAllowUpdate (lua_State * L)
{
	return I_TB<UI::Widget>(L, &UI::Widget::AllowUpdate);
}

static int WidgetDestroy (lua_State * L)
{
	return I_T<UI::Widget>(L, &UI::Widget::Destroy);
}

static int WidgetDock (lua_State * L)
{
	UW_(L, 1)->Dock(UW_(L, 2));

	return 0;
}

static int WidgetGetTag (lua_State * L)
{
	std::wstring TagW;

	if (UW_(L, 1)->GetTag(TagW) != 0)
	{
		char TagA[256];	wcstombs(TagA, TagW.c_str(), 256);

		lua_pushstring(L, TagA);

		return 1;
	}

	return 0;
}

static int WidgetPromoteToDockHead (lua_State * L)
{
	return I_T<UI::Widget>(L, &UI::Widget::PromoteToDockHead);
}

static int WidgetPromoteToFrameHead (lua_State * L)
{
	return I_T<UI::Widget>(L, &UI::Widget::PromoteToFrameHead);
}

static int WidgetSetTag (lua_State * L)
{
	wchar_t Tag[256];	mbstowcs(Tag, S(L, 2), 512);

	UW_(L, 1)->SetTag(Tag);

	return 0;
}

static int WidgetUnload (lua_State * L)
{
	return I_T<UI::Widget>(L, &UI::Widget::Unload);
}

static int WidgetUntag (lua_State * L)
{
	return I_T<UI::Widget>(L, &UI::Widget::Untag);
}

static int WidgetGetDockHead (lua_State * L)
{
	return T_T<UI::Widget, UI::Widget>(L, &UI::Widget::GetDockHead);
}

static int WidgetGetNextDockLink (lua_State * L)
{
	return T_T<UI::Widget, UI::Widget>(L, &UI::Widget::GetNextDockLink);
}

static int WidgetGetNextFrameLink (lua_State * L)
{
	return T_T<UI::Widget, UI::Widget>(L, &UI::Widget::GetNextFrameLink);
}

static int WidgetGetParent (lua_State * L)
{
	return T_T<UI::Widget, UI::Widget>(L, &UI::Widget::GetParent);
}

static int WidgetSignal (lua_State * L)
{
	return V_T<UI::Widget>(L, &UI::Widget::Signal);
}

static int WidgetIsDocked (lua_State * L)
{
	return B_T<UI::Widget>(L, &UI::Widget::IsDocked);
}

static int WidgetIsEntered (lua_State * L)
{
	return B_T<UI::Widget>(L, &UI::Widget::IsEntered);
}

static int WidgetIsFramed (lua_State * L)
{
	return B_T<UI::Widget>(L, &UI::Widget::IsFramed);
}

static int WidgetIsGrabbed (lua_State * L)
{
	return B_T<UI::Widget>(L, &UI::Widget::IsGrabbed);
}

static int WidgetIsLoaded (lua_State * L)
{
	return B_T<UI::Widget>(L, &UI::Widget::IsLoaded);
}

static int WidgetIsSignaled (lua_State * L)
{
	return B_T<UI::Widget>(L, &UI::Widget::IsSignaled);
}

static int WidgetIsSignalTestAllowed (lua_State * L)
{
	return B_T<UI::Widget>(L, &UI::Widget::IsSignalTestAllowed);
}

static int WidgetIsTagged (lua_State * L)
{
	return B_T<UI::Widget>(L, &UI::Widget::IsTagged);
}

static int WidgetIsUpdateAllowed (lua_State * L)
{
	return B_T<UI::Widget>(L, &UI::Widget::IsUpdateAllowed);
}

static int WidgetGetType (lua_State * L)
{
	UI::WidgetType type;	UW_(L, 1)->GetType(type);

	lua_pushnumber(L, type);

	return 1;
}

#undef UW_
#undef UP_
#undef UR_
#define M_(t, w) { #w, t##w }

///
/// Function tables
///
static const luaL_reg CompositeFuncs[] = {
	M_(Composite, CreatePart),
	{ 0, 0 }
};

static const luaL_reg IntervalFuncs[] = {
	M_(Interval, Get),
	M_(Interval, Set),
	M_(Interval, Clear),
	M_(Interval, Remove),
	{ 0, 0 }
};

static const luaL_reg PartFuncs[] = {
	M_(Part, Remove),
	M_(Part, SetContext),
	M_(Part, Signal),
	M_(Part, IsEntered),
	M_(Part, IsGrabbed),
	M_(Part, GetContext),
	{ 0, 0 }
};

static const luaL_reg RangeFuncs[] = {
	M_(Range, DeleteItems),
	M_(Range, GetEnteredItem),
	M_(Range, GetGrabbedItem),
	M_(Range, GetItemContext),
	M_(Range, GetItemCount),
	M_(Range, InsertItems),
	M_(Range, SetItemContext),
	M_(Range, CreateInterval),
	M_(Range, CreateSpot),
	M_(Range, SignalItem),
	M_(Range, IsItemSignaled),
	M_(Range, IsItemValid),
	{ 0, 0 }
};

static const luaL_reg SignaleeFuncs[] = {
	{ 0, 0 }
};

static const luaL_reg SpotFuncs[] = {
	M_(Spot, Get),
	M_(Spot, Set),
	M_(Spot, Remove),
	{ 0, 0 }
};

static const luaL_reg StateFuncs[] = {
	M_(State, Setup),
	M_(State, Close),
	M_(State, AbortSignalTests),
	M_(State, ClearInput),
	M_(State, GetFrameSize),
	M_(State, GetPress),
	M_(State, PropagateSignal),
	M_(State, Update),
	M_(State, SetContext),
	M_(State, CreateWidget),
	M_(State, FindWidget),
	M_(State, GetChoice),
	M_(State, GetFrameHead),
	M_(State, GetContext),
	M_(State, Unsignal),
	{ 0, 0 }
};

static const luaL_reg WidgetFuncs[] = {
	M_(Widget, AddToFrame),
	M_(Widget, AllowDockSignalTest),
	M_(Widget, AllowDockUpdate),
	M_(Widget, AllowSignalTest),
	M_(Widget, AllowUpdate),
	M_(Widget, Destroy),
	M_(Widget, Dock),
	M_(Widget, GetTag),
	M_(Widget, PromoteToDockHead),
	M_(Widget, PromoteToFrameHead),
	M_(Widget, SetTag),
	M_(Widget, Unload),
	M_(Widget, Untag),
	M_(Widget, GetDockHead),
	M_(Widget, GetNextDockLink),
	M_(Widget, GetNextFrameLink),
	M_(Widget, GetParent),
	M_(Widget, Signal),
	M_(Widget, IsDocked),
	M_(Widget, IsEntered),
	M_(Widget, IsFramed),
	M_(Widget, IsGrabbed),
	M_(Widget, IsLoaded),
	M_(Widget, IsSignaled),
	M_(Widget, IsSignalTestAllowed),
	M_(Widget, IsTagged),
	M_(Widget, IsUpdateAllowed),
	M_(Widget, GetType),
	{ 0, 0 }
};

#undef M_
#define C_(t) { #t, UI::e##t }
#define E_(t, w) { #t#w, UI::e##t##Event_##w }

///
/// Constant tables
///
static const struct {
	char const * mName;	///< Name attached to constant
	UI::WidgetType mType;	///< Type value constant
} WidgetTypes[] = {
	C_(Composite),
	C_(Range),
	C_(NumWidgetTypes)
};

static const struct {
	char const * mName;	///< Name attached to constant
	UI::Event mEvent;	///< Event value constant
} WidgetEvents[] = {
	E_(Widget, PreChoose),
	E_(Widget, PostChoose),
	E_(Widget, Grab),
	E_(Widget, Drop),
	E_(Widget, Enter),
	E_(Widget, Leave),
	E_(Widget, PreUpkeep),
	E_(Widget, PostUpkeep),
	E_(Widget, Abandon),
	E_(Composite, GrabPart),
	E_(Composite, DropPart),
	E_(Composite, EnterPart),
	E_(Composite, LeavePart),
	E_(Range, GrabItem),
	E_(Range, DropItem),
	E_(Range, EnterItem),
	E_(Range, LeaveItem)
};

#undef C_
#undef E_

/// @brief Binds the UI system to the Lua scripting system
void luaopen_ui (lua_State * L)
{
	// Install tables for each user interface component.
	luaL_openlib(L, "Composite", CompositeFuncs, 0);
	luaL_openlib(L, "Interval", IntervalFuncs, 0);
	luaL_openlib(L, "Part", PartFuncs, 0);
	luaL_openlib(L, "Range", RangeFuncs, 0);
	luaL_openlib(L, "Signalee", SignaleeFuncs, 0);
	luaL_openlib(L, "Spot", SpotFuncs, 0);
	luaL_openlib(L, "UI", StateFuncs, 0);
	luaL_openlib(L, "Widget", WidgetFuncs, 0);

	// Install the widget type and event constants into the widget table.
	for (Uint32 index = 0; index < sizeof(WidgetTypes) / sizeof(WidgetTypes[0]); ++index)
	{
		lua_pushstring(L, WidgetTypes[index].mName);
		lua_pushnumber(L, WidgetTypes[index].mType);

		lua_settable(L, -3);
	}

	for (Uint32 index = 0; index < sizeof(WidgetEvents) / sizeof(WidgetEvents[0]); ++index)
	{
		lua_pushstring(L, WidgetEvents[index].mName);
		lua_pushnumber(L, WidgetEvents[index].mEvent);

		lua_settable(L, -3);
	}
}