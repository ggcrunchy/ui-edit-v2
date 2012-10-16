#include "UserInterface.h"

namespace UI
{
	/// @brief Constructs a Composite object
	/// @param state State used to build composite
	/// @note Tested
	Composite::Composite (State * state) : Widget(state), mGrabbedPart(0), mEnteredPart(0), mSignaledPart(0)
	{
	}

	/// @brief Destructs a Composite object
	/// @note Tested
	Composite::~Composite (void)
	{
		while (!mParts.empty()) (*mParts.begin())->Remove();
	}

	/// @brief Clears all important input state
	/// @note Tested
	void Composite::Clear (void)
	{
		Widget::Clear();

		mGrabbedPart = 0;
		mEnteredPart = 0;
	}

	/// @brief Clears all signals
	/// @note Tested
	void Composite::ClearSignals (void)
	{
		mSignaledPart = 0;
	}

	/// @brief Performs drop logic
	/// @note Tested
	void Composite::Drop (void)
	{
		Widget::Drop();

		if (mGrabbedPart != 0) mGrabbedPart->Drop();
	}

	/// @brief Performs enter logic
	/// @note Tested
	void Composite::Enter (void)
	{
		Widget::Enter();

		if (mSignaledPart != 0) mSignaledPart->Enter();
	}

	/// @brief Performs grab logic
	/// @note Tested
	void Composite::Grab (void)
	{
		if (mSignaledPart != 0) mSignaledPart->Grab();

		else Widget::Grab();
	}

	/// @brief Performs leave logic
	/// @note Tested
	void Composite::Leave (void)
	{
		if (0 == mSignaledPart && mEnteredPart != 0) mEnteredPart->Leave();

		Widget::Leave();
	}

	/// @brief Indicates whether the pane is chosen
	/// @return If true, the pane is chosen
	/// @note Tested
	bool Composite::IsChosen (void)
	{
		return mStatus.test(eGrabbed) || mGrabbedPart != 0;
	}

	/// @brief Returns the composite type
	/// @return The composite type
	/// @note Tested
	WidgetType Composite::Type (void)
	{
		return eComposite;
	}
}