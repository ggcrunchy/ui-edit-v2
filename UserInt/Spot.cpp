#include "UserInterface.h"

namespace UI
{
	/// @brief Gets the position of a spot
	/// @param where [out] On success, spot position
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int Spot::Get (Uint32 & where)
	{
		if (!Valid()) return 0;

		where = mWhere;

		return 1;
	}

	/// @brief Removes the spot from the range
	/// @note Tested
	void Spot::Remove (void)
	{
		delete this;
	}

	/// @brief Assigns the position
	/// @param where Position to assign
	/// return 0 on failure, non-0 for success
	/// @note Tested
	int Spot::Set (Uint32 where)
	{
		if (!mRange->IsItemValid(where, mStatus.test(eAddSpot))) return 0;

		mWhere = where;

		mStatus.set(eValid);

		return 1;
	}
}