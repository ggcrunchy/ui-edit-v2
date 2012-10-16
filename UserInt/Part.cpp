#include "UserInterface.h"

namespace UI
{
	/// @brief Indicates whether the part is entered
	/// @return If true, the part is entered
	/// @note Tested
	bool Part::IsEntered (void)
	{
		return mOwner->mEnteredPart == this;
	}

	/// @brief Indicates whether the part is grabbed
	/// @return If true, the part is grabbed
	/// @note Tested
	bool Part::IsGrabbed (void)
	{
		return mOwner->mGrabbedPart == this;
	}

	/// @brief Removes the part from the composite
	/// @note NOP if invoked during signal propagation
	/// @note Tested
	void Part::Remove (void)
	{
		if (eSignalTesting == mOwner->mState->mMode) return;
		if (eIssuingEvents == mOwner->mState->mMode) return;

		// Clear properties in the composite that match this part.
		if (mOwner->mGrabbedPart == this) mOwner->mGrabbedPart = 0;
		if (mOwner->mEnteredPart == this) mOwner->mEnteredPart = 0;

		// Destroy the part.
		delete this;		
	}

	/// @brief
	/// @param context
	/// @note Tested
	void Part::SetContext (void * context)
	{
		mContext = context;
	}

	/// @brief
	/// @note NOP if not invoked from a signal test callback
	/// @note Tested
	void Part::Signal (void)
	{
		if (mOwner->mState->mMode != eSignalTesting) return;

		mOwner->Signal();

		mOwner->mSignaledPart = this;
	}

	/// @brief
	/// @return
	/// @note Tested
	void * Part::GetContext (void)
	{
		return mContext;
	}
}