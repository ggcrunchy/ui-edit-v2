#include "UserInterface.h"

namespace UI
{
	/// @brief Constructs a Part object
	/// @param owner Composite used to build part
	/// @note Tested
	Part::Part (Composite * owner) : mOwner(owner), mContext(0)
	{
	}

	/// @brief Destructs a Part object
	/// @note Tested
	Part::~Part (void)
	{
		mOwner->mParts.erase(this);
	}

	/// @brief Performs drop logic
	/// @note Tested
	void Part::Drop (void)
	{
		mOwner->mGrabbedPart = 0;

		mOwner->IssueEvent(eCompositeEvent_DropPart);
	}

	/// @brief Performs enter logic
	/// @note Tested
	void Part::Enter (void)
	{
		if (mOwner->mEnteredPart == this) return;

		mOwner->mEnteredPart = this;

		mOwner->IssueEvent(eCompositeEvent_EnterPart);
	}

	/// @brief Performs grab logic
	/// @note Tested
	void Part::Grab (void)
	{
		if (mOwner->mGrabbedPart == this) return;

		mOwner->mGrabbedPart = this;

		mOwner->IssueEvent(eCompositeEvent_GrabPart);
	}

	/// @brief Performs leave logic
	/// @note Tested
	void Part::Leave (void)
	{
		mOwner->mEnteredPart = 0;

		mOwner->IssueEvent(eCompositeEvent_LeavePart);
	}
}