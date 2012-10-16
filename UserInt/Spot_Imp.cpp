#include "UserInterface.h"

namespace UI
{
	/// @brief Constructs a Spot object
	/// @param range Range used to build spot
	/// @param bAddSpot Indicates whether spot can occupy end of range
	/// @param bMigrate Indicates whether spot can migrate if removed
	/// @note Tested
	Spot::Spot (Range * range, bool bAddSpot, bool bMigrate) : mRange(range), mWhere(0)
	{
		mStatus.set(eAddSpot, bAddSpot);
		mStatus.set(eMigrate, bMigrate);

		if (bMigrate) mStatus.set(eValid, Valid());
	}

	/// @brief Destructs a Spot object
	/// @note Tested
	Spot::~Spot (void)
	{
		mRange->mSpots.erase(this);
	}

	/// @brief Updates a spot after deletion
	/// @param where Position of deletion
	/// @param count Amount of deleted values
	/// @note Tested
	void Spot::AdjustForDelete (Uint32 where, Uint32 count)
	{
		if (!Valid()) return;

		// If a migratory spot follows the point of removal, it must be moved back. Reduce
		// its index by the lesser of the delete amount and the distance from the point of
		// deletion to the index.
		if (mStatus.test(eMigrate))
		{
			if (mWhere > where)
			{
				Uint32 adjust = mWhere - where;

				if (count < adjust) adjust = count;

				mWhere -= adjust;
			}

			// If a block was deleted at the end of the items, and it included the spot,
			// the spot will now be after the items. Move it back if this is not allowed.
			if (mWhere + count == mRange->mItems.size())
			{
				if (mStatus.test(eAddSpot) || count == mRange->mItems.size()) return;
				
				--mWhere;
			}
		}

		// If a non-migratory spot follows the interval being deleted, move it back by the
		// delete amount; clear the spot if it is within the interval.
		else
		{
			if (mWhere >= where + count) mWhere -= count;

			else if (mWhere >= where) Clear();
		}
	}

	/// @brief Updates a spot after insertion
	/// @param where Position of insertion
	/// @param count Amount of inserted values
	/// @note Tested
	void Spot::AdjustForInsert (Uint32 where, Uint32 count)
	{
		if (!Valid()) return;
		
		// Move the spot up if it is not before the insert position.
		if (mWhere >= where) mWhere += count;

		// If the range is empty, the spot will be after the range. Move it back if this is
		// not allowed.
		if (mRange->mItems.empty() && !mStatus.test(eAddSpot)) --mWhere;
	}

	/// @brief Invalidates a spot
	/// @note Tested
	void Spot::Clear (void)
	{
		mStatus.reset(eValid);
	}

	/// @brief Indicates spot validity
	/// @return If true, the spot is valid
	/// @note Tested
	bool Spot::Valid (void)
	{
		if (!mStatus.test(eMigrate) && !mStatus.test(eValid)) return false;
		if (!mRange->IsItemValid(mWhere, mStatus.test(eAddSpot))) return false;

		return true;
	}
}