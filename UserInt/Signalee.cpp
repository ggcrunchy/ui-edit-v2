#include "UserInterface.h"

namespace UI
{
	/// @brief Binds a user-defined context
	/// @param context User-defined context
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	void Signalee::SetContext (void * context)
	{
		mContext = context;
	}

	/// @brief
	/// @note
	void Signalee::Signal (void)
	{
	}

	/// @brief
	/// @return
	/// @note
	bool Signalee::IsEntered (void)
	{
		return false;
	}

	/// @brief
	/// @return
	/// @note
	bool Signalee::IsGrabbed (void)
	{
		return false;
	}

	/// @brief
	/// @return
	/// @note
	bool Signalee::IsSignaled (void)
	{
		return false;
	}

	/// @brief Gets the signalee's owner state
	/// @return State to which signalee belongs
	/// @note Tested
	State * Signalee::GetState (void)
	{
		return mState;
	}

	/// @brief Gets the user-defined context
	/// @return 0 on failure; user-defined context on success
	/// @note Tested
	void * Signalee::GetContext (void)
	{
		return mContext;
	}
}