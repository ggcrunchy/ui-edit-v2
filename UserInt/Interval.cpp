#include "UserInterface.h"

namespace UI
{
	/// @brief Clears the selection
	/// @note Tested
	void Interval::Clear (void)
	{
		mCount = 0;
	}

	/// @brief Gets the starting position and select count
	/// @param where [out] On success, the starting position
	/// @param count [out] On success, the count of selected items
	/// @note Tested
	int Interval::Get (Uint32 & where, Uint32 & count)
	{
		if (0 == mCount) return 0;

		where = mWhere;
		count = mCount;

		return 1;
	}

	/// @brief Removes the interval from the range
	/// @note Tested
	void Interval::Remove (void)
	{
		delete this;
	}

	/// @brief Makes a selection
	/// @param where Index from which to begin selection
	/// @param count [in-out] Count of items to select; on success, the number selected
	/// @note Tested
	int Interval::Set (Uint32 where, Uint32 & count)
	{
		if (!mRange->IsItemValid(where, true)) return 0;

		mWhere = where;
		mCount = mRange->Trim(where, count);

		return 1;
	}
}