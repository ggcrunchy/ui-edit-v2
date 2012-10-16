#include "UserInterface.h"

namespace UI
{
	/// @brief Constructs an Item object
	/// @param range
	/// @note
	Item::Item (Range * range) : Signalee(range->mState), mRange(range)
	{
	}

	/// @brief Destructs an Item object
	/// @note Tested
	Item::~Item (void)
	{
	}
}