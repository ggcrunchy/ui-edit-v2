#include "UserInterface.h"

namespace UI
{
	/// @brief Constructs a Signalee object
	/// @param state
	/// @note Tested
	Signalee::Signalee (State * state) : mState(state), mContext(0)
	{
	}

	/// @brief Destructs a Signalee object
	/// @note Tested
	Signalee::~Signalee (void)
	{
	}
}