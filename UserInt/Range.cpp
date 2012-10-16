#include "UserInterface.h"

namespace UI
{
	/// @brief Deletes a series of items
	/// @param where Index of deletion
	/// @param count [in-out] Count of items to delete; on success, the count of items
	///		   deleted
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked during signal propagation
	/// @note Tested
	int Range::DeleteItems (Uint32 where, Uint32 & count)
	{
		if (!IsItemValid(where, false)) return 0;
		if (eSignalTesting == mState->mMode) return 0;
		if (eIssuingEvents == mState->mMode) return 0;

		// Trim the count to the range.
		count = Trim(where, count);

		// Adjust each used spot and interval for deletion. Erase the requested items.
		for (IterS_S sIter = mSpots.begin(); sIter != mSpots.end(); ++sIter)
		{
			(*sIter)->AdjustForDelete(where, count);
		}

		for (IterI_S iIter = mIntervals.begin(); iIter != mIntervals.end(); ++iIter)
		{
			(*iIter)->AdjustForDelete(where, count);
		}

		mItems.erase(where, count);

		return 1;
	}

	/// @brief Gets the currently entered item
	/// @param item [out] On success, the entered item
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int Range::GetEnteredItem (Uint32 & where)
	{
		if (!mEnteredSpot.Valid()) return 0;

		where = mEnteredSpot.mWhere;

		return 1;
	}

	/// @brief Gets the currently grabbed item
	/// @param item [out] On success, the grabbed item
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int Range::GetGrabbedItem (Uint32 & where)
	{
		if (!mGrabbedSpot.Valid()) return 0;

		where = mGrabbedSpot.mWhere;

		return 1;
	}

	/// @brief Gets an item's context
	/// @param where Index of item
	/// @param context [out] On success, the user-defined context
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int Range::GetItemContext (Uint32 where, Uint32 & context)
	{
		if (!IsItemValid(where, false)) return 0;

		context = mItems[where];

		return 1;
	}

	/// @brief Gets the count of items
	/// @param count [out] On success, the item count
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int Range::GetItemCount (Uint32 & count)
	{
		count = static_cast<Uint32>(mItems.size());

		return 1;
	}

	/// @brief Inserts new items
	/// @param where Index of insertion
	/// @param count Count of items to add
	/// @return 0 on failure, non-0 on success
	/// @note NOP if invoked during signal propagation
	/// @note Tested
	int Range::InsertItems (Uint32 where, Uint32 count)
	{
		if (!IsItemValid(where, true)) return 0;
		if (eSignalTesting == mState->mMode) return 0;
		if (eIssuingEvents == mState->mMode) return 0;

		// Adjust each used spot and interval for insertion.
		for (IterS_S sIter = mSpots.begin(); sIter != mSpots.end(); ++sIter)
		{
			(*sIter)->AdjustForInsert(where, count);
		}

		for (IterI_S iIter = mIntervals.begin(); iIter != mIntervals.end(); ++iIter)
		{
			(*iIter)->AdjustForInsert(where, count);
		}

		// Insert a group of items with default context.
		mItems.insert(where, count, 0);

		return 1;
	}

	/// @brief Sets an item's context
	/// @param where Index of item
	/// @param context User-defined context
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int Range::SetItemContext (Uint32 where, Uint32 context)
	{
		if (!IsItemValid(where, false)) return 0;

		mItems[where] = context;

		return 1;
	}

	/// @brief Creates a custom interval
	/// @return 0 on failure; spot on success
	/// @note Tested
	Interval * Range::CreateInterval (void)
	{
		Interval * interval = 0;

		try {
			interval = new UI::Interval(this);

			mIntervals.insert(interval);
		} catch (std::bad_alloc &) {}

		return interval;
	}

	/// @brief Adds a custom spot
	/// @param bAddSpot If true, the spot may occupy the end of the range
	/// @param bMigrate If true, the spot will migrate if deleted
	/// @return 0 on failure; spot on success
	/// @note Tested
	Spot * Range::CreateSpot (bool bAddSpot, bool bMigrate)
	{
		Spot * spot = 0;

		try {
			spot = new Spot(this, bAddSpot, bMigrate);

			mSpots.insert(spot);
		} catch (std::bad_alloc &) {}

		return spot;
	}

	/// @brief Used to signal an item
	/// @param where Index of item in the range
	/// @note NOP if not invoked from a signal test callback
	/// @note Tested
	void Range::SignalItem (Uint32 where)
	{
		if (mState->mMode != eSignalTesting) return;
		if (!IsItemValid(where, true)) return;

		Signal();

		mSignaledItem = where;

		mStatus.set(eItemSignaled);
	}

	/// @brief Indicates whether an item is signaled
	/// @param where [out] On success, the index of the signaled item
	/// @note Tested
	bool Range::IsItemSignaled (Uint32 & where)
	{
		if (mState->mMode != eSignalTesting) return false;
		if (mStatus.test(eItemSignaled)) return false;
		
		where = mSignaledItem;

		return true;
	}

	/// @brief Indicates whether an item is valid
	/// @param item Index of item
	/// @param bAddItem If true, the item after the range is considered valid
	/// @return If true, the item is valid
	/// @note Tested
	bool Range::IsItemValid (Uint32 where, bool bAddItem)
	{
		Uint32 extent = static_cast<Uint32>(mItems.size());

		if (bAddItem) ++extent;

		return where < extent;
	}
}