#include "UserInterface.h"

namespace UI
{	
	/// @brief Constructs an Interval object
	/// @param range Range used to build interval
	/// @note Tested
	Interval::Interval (Range * range) : mRange(range), mCount(0)
	{
	}

	/// @brief Destructs an Interval object
	/// @note Tested
	Interval::~Interval (void)
	{
		mRange->mIntervals.erase(this);
	}

	/// @brief Updates an interval after deletion
	/// @param where Position of deletion
	/// @param count Amount of deleted values
	/// @note Tested
	void Interval::AdjustForDelete (Uint32 where, Uint32 count)
	{
		if (0 == mCount) return;

		// If this overlaps the deletion interval, reduce its count by the overlap amount.
		Uint32 begin = where, end = where + count;

		if (end > mWhere && begin < mWhere + mCount)
		{
			if (mWhere > begin)	begin = mWhere;

			if (mWhere + mCount < end) end = mWhere + mCount;

			mCount -= end - begin;
		}

		// If the interval follows the point of deletion, it must be moved back. Reduce its
		// index by the lesser of the delete amount and the distance from the point of
		// removal to the start of the interval.
		if (mWhere > where)
		{
			Uint32 adjust = mWhere - where;

			if (count < adjust) adjust = count;

			mWhere -= adjust;
		}
	}

	/// @brief Updates an interval after insertion
	/// @param where Position of insertion
	/// @param count Amount of inserted values
	/// @note Tested
	void Interval::AdjustForInsert (Uint32 where, Uint32 count)
	{
		if (0 == mCount) return;

		// If insertion occurred before the interval, move ahead by the insert amount.
		if (where < mWhere) mWhere += count;

		// Otherwise, if inserting into the interval, augment it by the insert amount.
		else if (where >= mWhere && where < mWhere + mCount) mCount += count;
	}
}