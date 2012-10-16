#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <SDL/SDL_types.h>
#include <bitset>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace UI
{
	/// @brief Descriptor for a widget type
	enum WidgetType {
		eComposite,	///< Widget is a composite
		eRange,	///< Widget is a range
		eNumWidgetTypes	///< Count of widget types
	};

	// @brief Descriptors for signalee-directed events
	enum Event {
	/* General */
		eWidgetEvent_Abandon,	///< The widget was abandoned as the choice
		eWidgetEvent_Drop,	///< A release following a widget grab
		eWidgetEvent_Enter,	///< The widget region was entered
		eWidgetEvent_Grab,	///< A press while the widget is entered
		eWidgetEvent_Leave,	///< The widget region was left
		eWidgetEvent_PreChoose,	///< Event issued to widget before it is chosen
		eWidgetEvent_PreUpkeep,	///< Event issued before upkeep to chosen widget
		eWidgetEvent_PostChoose,///< Event issued to widget after it is chosen
		eWidgetEvent_PostUpkeep,///< Event issued after upkeep to chosen widget
	/* Composite */
		eCompositeEvent_DropPart,	///< A release following a part grab
		eCompositeEvent_EnterPart,	///< The part region was entered
		eCompositeEvent_GrabPart,	///< A press while the part is entered
		eCompositeEvent_LeavePart,	///< The part region was left
	/* Sequence */
		eRangeEvent_DropItem,	///< A release following an item grab
		eRangeEvent_EnterItem,	///< The item region was entered
		eRangeEvent_GrabItem,	///< A press while the item is entered
		eRangeEvent_LeaveItem	///< The item region was left
	};

	/// @brief Base type for signallable elements
	class Signalee {
	public:
	// Members
		class State * mState;	///< State to which signalee belongs
		void * mContext;///< User-defined context
	// Methods
		Signalee (State * state);
		virtual ~Signalee (void);

		virtual void Drop (void) = 0;
		virtual void Grab (void) = 0;
		virtual void Enter (void) = 0;
		virtual void Leave (void) = 0;
	// Friendship

	public:
	// Interface
		void SetContext (void * context);
		void Signal (void);

		bool IsEntered (void);
		bool IsGrabbed (void);
		bool IsSignaled (void);

		State * GetState (void);

		void * GetContext (void);
	};

	/// @brief Representation of user interface elements
	class Widget : public Signalee {
	public:
	// Types
		typedef std::list<Widget*>::iterator Iter_L;///< List iterator
		typedef std::list<Widget*>::reverse_iterator RIter_L;	///< Reverse list iterator
	// Members
		std::bitset<32> mStatus;///< Current status
		std::list<Widget*> mDock;	///< Docked widgets
		std::map<std::wstring, Widget*>::iterator mTagLookup;	///< Tag lookup position
		Iter_L mLookup;	///< Lookup position in parent dock or frame
		Widget * mParent;	///< Widget in which this widget is docked
	// Flags
		enum {
			eCannotDockSignalTest,	///< Docked widgets cannot be signal tested
			eCannotDockUpdate,	///< Docked widgets cannot be updated
			eCannotSignalTest,	///< The widget cannot be signal tested
			eCannotUpdate,	///< The widget cannot be updated
			eEntered,	///< The widget is entered
			eFramed,///< The widget is set in a frame
			eGrabbed,	///< The widget is grabbed
			eTagged,///< The widget is tagged
			eWidgetNext	///< Used to continue flags in derived types
		};
	// Methods
		Widget (State * state);
		~Widget (void);

		void Choose (bool bPressed);
		void IssueEvent (Event event);
		void SignalTest (void);
		void Update (void);
		void Upkeep (bool bPressed);

		virtual void Clear (void);
		virtual void Drop (void);
		virtual void Enter (void);
		virtual void Grab (void);
		virtual void Leave (void);

		virtual void ClearSignals (void) = 0;

		virtual bool IsChosen (void) = 0;

		virtual WidgetType Type (void) = 0;
	// Friendship

	public:
	// Interface
		int AddToFrame (void);
		int AllowDockSignalTest (bool bAllow);
		int AllowDockUpdate (bool bAllow);
		int AllowSignalTest (bool bAllow);
		int AllowUpdate (bool bAllow);
		int Destroy (void);
		int Dock (Widget * widget);
		int GetTag (std::wstring & tag);
		int PromoteToDockHead (void);
		int PromoteToFrameHead (void);
		int SetTag (std::wstring const & tag);
		int Unload (void);
		int Untag (void);

	int GetType (WidgetType & type);

		Widget * GetDockHead (void);
		Widget * GetNextDockLink (void);
		Widget * GetNextFrameLink (void);
		Widget * GetParent (void);

		void Signal (void);

		bool IsDocked (void);
		bool IsEntered (void);
		bool IsFramed (void);
		bool IsGrabbed (void);
		bool IsLoaded (void);
		bool IsSignaled (void);
		bool IsSignalTestAllowed (void);
		bool IsTagged (void);
		bool IsUpdateAllowed (void);
	};

	/// @brief Representation of a custom part
	class Part {
	public:
	// Members
		class Composite * mOwner;	///< Composite used to build part
		void * mContext;///< User-defined context
	// Methods
		Part (Composite * owner);
		~Part (void);

		void Drop (void);
		void Grab (void);
		void Enter (void);
		void Leave (void);
	// Friendship

	public:
	// Interface
		void Remove (void);
		void SetContext (void * context);
		void Signal (void);

		bool IsEntered (void);
		bool IsGrabbed (void);

		void * GetContext (void);
	};

	/// @brief Representation of a composite widget
	class Composite : public Widget {
	public:
	// Members
		std::set<Part*> mParts;	///< Custom part storage
		Part * mEnteredPart;///< Currently entered part
		Part * mGrabbedPart;///< Currently grabbed part
		Part * mSignaledPart;	///< Currently signaled part
	// Methods
		Composite (State * state);
		~Composite (void);

		void Clear (void);
		void ClearSignals (void);
		void Drop (void);
		void Enter (void);
		void Grab (void);
		void Leave (void);

		bool IsChosen (void);

		WidgetType Type (void);
	public:
	// Interface
		Part * CreatePart (void);
	};

	/// @brief Representation of a custom spot
	class Spot {
	private:
	// Members
		class Range * mRange;	///< Range to which spot belongs
		std::bitset<32> mStatus;///< Current status of spot
		Uint32 mWhere;	///< Index of spot
	// Enumerations
		enum {
			eAddSpot,	///< Spot can occupy end of range
			eMigrate,	///< Spot can migrate if removed
			eValid	///< Spot is valid
		};
	// Methods
		Spot (Range * range, bool bAddSpot, bool bMigrate);
		~Spot (void);

		void AdjustForDelete (Uint32 where, Uint32 count);
		void AdjustForInsert (Uint32 where, Uint32 count);
		void Clear (void);

		bool Valid (void);
	// Friendship
		friend class Range;
	public:
	// Interface
		int Get (Uint32 & where);
		int Set (Uint32 where);

		void Remove (void);
	};

	/// @brief Representation of a custom interval
	class Interval {
	private:
	// Members
		class Range * mRange;	///< Range to which interval belongs
		Uint32 mCount;	///< Count of items in interval
		Uint32 mWhere;	///< Index of start of interval
	// Methods
		Interval (Range * range);
		~Interval (void);

		void AdjustForDelete (Uint32 where, Uint32 count);
		void AdjustForInsert (Uint32 where, Uint32 count);
	// Friendship
		friend class Range;
	public:
	// Interface
		int Get (Uint32 & where, Uint32 & count);
		int Set (Uint32 where, Uint32 & count);

		void Clear (void);
		void Remove (void);
	};

	/// @brief Representation of a sequence item
	class Item : public Signalee {
	public:
	// Members
		class Range * mRange;	///<
	// Methods
		Item (Range * range);
		~Item (void);
	// Friendship

	public:
	// Interface
		Uint32 GetIndex (void);
	};

	/// @brief Representation of a sequence widget
	class Range : public Widget {
	public:
	// Types
		typedef std::set<Interval*>::iterator IterI_S;	///< Interval set iterator
		typedef std::set<Spot*>::iterator IterS_S;	///< Spot set iterator
	// Members
		std::basic_string<Uint32> mItems;	///< Items loaded into the range
		std::set<Interval*> mIntervals;	///< Custom interval storage
		std::set<Spot*> mSpots;	///< Custom spot storage

			std::vector<Item*> mItems_;	///<

		Spot mEnteredSpot;	///< Currently entered spot
		Spot mGrabbedSpot;	///< Currently grabbed spot
		Uint32 mSignaledItem;	///< Index of signaled item
	// Flags
		enum {
			eItemSignaled = eWidgetNext	///< An item is signaled
		};
	// Methods
		Range (State * state);
		~Range (void);

		void Clear (void);
		void ClearSignals (void);
		void Drop (void);
		void Enter (void);
		void Grab (void);
		void Leave (void);

		bool IsChosen (void);
		bool IsSwitch (bool bEnter);

		Uint32 Trim (Uint32 where, Uint32 count);

		WidgetType Type (void);
	// Friendship

	public:
	// Interface
		int DeleteItems (Uint32 where, Uint32 & count);
		int GetEnteredItem (Uint32 & where);
		int GetGrabbedItem (Uint32 & where);
		int GetItemContext (Uint32 where, Uint32 & context);
		int GetItemCount (Uint32 & count);
		int InsertItems (Uint32 where, Uint32 count);
		int SetItemContext (Uint32 where, Uint32 context);

		Interval * CreateInterval (void);

		Spot * CreateSpot (bool bAddSpot, bool bMigrate);

		void SignalItem (Uint32 where);

		bool IsItemSignaled (Uint32 & where);
		bool IsItemValid (Uint32 where, bool bAddItem);
	};

	/// @brief Descriptor of user interface manager mode
	enum Mode {
		eIssuingEvents,	///< The system is issuing events
		eNormal,///< The system is operating normally
		eSignalTesting,	///< The system is signal testing
		eUpdating	///< The system is updating
	};

	/// @brief Representation of a user interface state
	class State {
	public:
	// Types
		typedef void (*EventFunc)(Widget *, Event);
		typedef void (*SignalFunc)(Widget *);
		typedef void (*UpdateFunc)(Widget *);
	public:
	// Members
		std::bitset<32> mStatus;///< Current status
		std::list<Widget*> mFrame;	///< Active widget frame
		std::map<std::wstring, Widget*> mTags;	///< Associative widget labels
		std::set<Widget*> mWidgets;	///< Widget storage
		Widget * mChoice;	///< Current chosen widget
		Widget * mSignal;	///< Current signaled widget
		Mode mMode;	///< Current state mode
		EventFunc mEventFunc;	///< Event handler
		SignalFunc mSignalFunc;	///< Signal test routine
		UpdateFunc mUpdateFunc;	///< Update routine
		void * mContext;///< User-defined context
	// Flags
		enum {
			eWasPressed,///< There was a press on the last signal propagation
			eIsPressed	///< There is a press on the current signal propagation
		};
	// Methods
		void ClearChoice (void);
		void ResolveSignal (void);
	// Friendship

	public:
	// Interface
		State (EventFunc eventFunc, SignalFunc signalFunc, UpdateFunc updateFunc);
		~State (void);

		int AbortSignalTests (void);
		int ClearInput (void);
		int GetFrameSize (Uint32 & size);
		int GetPress (bool & bPress);
		int PropagateSignal (bool bPressed);
		int SetContext (void * context);
		int Update (void);

		Widget * CreateWidget (WidgetType type);
		Widget * FindWidget (std::wstring const & tag);
		Widget * GetChoice (void);
		Widget * GetFrameHead (void);

		void * GetContext (void);

		void Unsignal (void);
	};
}

#endif // USER_INTERFACE_H