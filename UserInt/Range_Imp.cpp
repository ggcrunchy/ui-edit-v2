#include "UserInterface.h"

namespace UI
{
	/// @brief Constructs a Range object
	/// @param state State used to build range
	/// @note Tested
	Range::Range (State * state) : Widget(state), mGrabbedSpot(0, true, false), mEnteredSpot(0, true, false)
	{
		mGrabbedSpot.mRange = this;
		mEnteredSpot.mRange = this;

		mSpots.insert(&mGrabbedSpot);
		mSpots.insert(&mEnteredSpot);
	}

	/// @brief Destructs a Range object
	/// @note Tested
	Range::~Range (void)
	{
		mSpots.erase(&mGrabbedSpot);
		mSpots.erase(&mEnteredSpot);

		while (!mSpots.empty()) (*mSpots.begin())->Remove();
		while (!mIntervals.empty()) (*mIntervals.begin())->Remove();
	}

	/// @brief Clears all important input state
	/// @note Tested
	void Range::Clear (void)
	{
		Widget::Clear();

		mGrabbedSpot.Clear();
		mEnteredSpot.Clear();
	}

	/// @brief Clears all signals
	/// @note Tested
	void Range::ClearSignals (void)
	{
		mStatus.reset(eItemSignaled);
	}

	/// @brief Performs drop logic
	/// @note Tested
	void Range::Drop (void)
	{
		Widget::Drop();

		if (mGrabbedSpot.Valid())
		{
			IssueEvent(eRangeEvent_DropItem);

			mGrabbedSpot.Clear();
		}
	}

	/// @brief Performs enter logic
	/// @note Tested
	void Range::Enter (void)
	{
		Widget::Enter();

		if (IsSwitch(true))
		{
			mEnteredSpot.Set(mSignaledItem);

			IssueEvent(eRangeEvent_EnterItem);
		}
	}

	/// @brief Performs grab logic
	/// @note Tested
	void Range::Grab (void)
	{
		if (mStatus.test(eItemSignaled))
		{
			if (mGrabbedSpot.Valid()) return;

			mGrabbedSpot.Set(mSignaledItem);

			IssueEvent(eRangeEvent_GrabItem);
		}

		else Widget::Grab();
	}

	/// @brief Performs leave logic
	/// @note Tested
	void Range::Leave (void)
	{
		if (IsSwitch(false))
		{
			IssueEvent(eRangeEvent_LeaveItem);

			mEnteredSpot.Clear();
		}

		Widget::Leave();
	}

	/// @brief Indicates whether the range is chosen
	/// @return If true, the range is chosen
	/// @note Tested
	bool Range::IsChosen (void)
	{
		return mStatus.test(eGrabbed) || mGrabbedSpot.Valid();
	}

	/// @brief Indicates whether the signaled item enters/leaves an item
	/// @param bEnter If true, test for an enter
	/// @return If true, there is a switch
	/// @note Tested
	bool Range::IsSwitch (bool bEnter)
	{
		// Enter tests fail trivially if there is no item to enter; then, they trivially
		// pass if there is no entered spot.
		if (bEnter)
		{
			if (!mStatus.test(eItemSignaled)) return false;
			if (!mEnteredSpot.Valid()) return true;
		}

		// Leave tests fail trivially if there is no entered spot; then, they trivially pass
		// if the signal is not on an item.
		else
		{
			if (!mEnteredSpot.Valid()) return false;
			if (!mStatus.test(eItemSignaled)) return true;
		}

		// There is a switch if the items vary.
		return mEnteredSpot.mWhere != mSignaledItem;
	}

	/// @brief Trims the interval [start, start + count) to [start, size)
	/// @param start The index of the start of the interval
	/// @param count The count of items in the interval
	/// @return The modified item count
	/// @note Tested
	Uint32 Range::Trim (Uint32 start, Uint32 count)
	{
		Uint32 size = static_cast<Uint32>(mItems.size());

		return start + count > size ? size - start : count;
	}

	/// @brief Returns the range type
	/// @return The range type
	/// @note Tested
	WidgetType Range::Type (void)
	{
		return eRange;
	}
}