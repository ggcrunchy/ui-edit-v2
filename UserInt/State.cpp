#include "UserInterface.h"

namespace UI
{
	/// @brief Aborts a signal propagation in action
	/// @note NOP if not invoked during a signal test callback
	/// @note Tested
	int State::AbortSignalTests (void)
	{
		if (mMode != eSignalTesting) return 0;

		Unsignal();

		mMode = eNormal;

		return 1;
	}

	/// @brief Clears the input state
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked during signal propagation
	/// @note Tested
	int State::ClearInput (void)
	{
		if (eSignalTesting == mMode) return 0;
		if (eIssuingEvents == mMode) return 0;

		// Clear the choice. Reset the press record.
		ClearChoice();

		mStatus.reset(eWasPressed);

		return 1;
	}

	/// @brief Gets the size of the frame
	/// @param size [out] On success, the count of framed widgets
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int State::GetFrameSize (Uint32 & size)
	{
		size = static_cast<Uint32>(mFrame.size());

		return 1;
	}

	/// @brief Gets the current press state
	/// @param bPress [out] On success, the press state
	/// @return 0 on failure, non-0 for success
	/// @note NOP if not invoked during signal propagation
	/// @note Tested
	int State::GetPress (bool & bPress)
	{
		if (eNormal == mMode) return 0;
		if (eUpdating == mMode) return 0;

		bPress = mStatus.test(eIsPressed);

		return 1;
	}

	/// @brief Propagates input through the state and issues events in response
	/// @param bPressed If true, there is a press
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked during signal propagation
	/// @note Tested
	int State::PropagateSignal (bool bPressed)
	{
		if (eSignalTesting == mMode) return 0;
		if (eIssuingEvents == mMode) return 0;

		// Cache the press state.
		mStatus.set(eIsPressed, bPressed);

		// Scan the frame front to back for a signal. If found, resolve it.
		mMode = eSignalTesting;

		try {
			for (Widget::Iter_L wIter = mFrame.begin(); wIter != mFrame.end(); ++wIter)
			{
				(*wIter)->SignalTest();
			}
		} catch (...) {};
			
		if (mMode != eNormal) ResolveSignal();

		return 1;
	}

	/// @brief Binds a user-defined context
	/// @param context User-defined context
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int State::SetContext (void * context)
	{
		mContext = context;

		return 1;
	}

	/// @brief Updates the state
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked during updating
	/// @note Tested
	int State::Update (void)
	{
		if (eUpdating == mMode) return 0;

		// Update the frame back to front.
		mMode = eUpdating;

		for (Widget::RIter_L wIter = mFrame.rbegin(); wIter != mFrame.rend(); ++wIter)
		{
			(*wIter)->Update();
		}

		mMode = eNormal;

		return 1;
	}

	/// @brief Constructs a widget of a given type
	/// @param type Type of the widget to create
	/// @return 0 on failure; widget on success
	/// @note Tested
	Widget * State::CreateWidget (WidgetType type)
	{
		Widget * widget = 0;

		// Construct and store a widget of the appropriate type.
		try {
			switch (type)
			{
			case eComposite:
				widget = new Composite(this);
				break;
			case eRange:
				widget = new Range(this);
				break;
			}

			mWidgets.insert(widget);
		} catch (std::bad_alloc &) {}

		return widget;
	}

	/// @brief Finds a widget
	/// @param tag The tag used to look up the widget
	/// @return 0 on failure; widget on success
	/// @note Tested
	Widget * State::FindWidget (std::wstring const & tag)
	{
		std::map<std::wstring, Widget*>::iterator tagIter = mTags.find(tag);
		
		if (tagIter == mTags.end()) return 0;

		return tagIter->second;
	}

	/// @brief Gets the chosen widget
	/// @return 0 on failure; choice on success
	/// @note Tested
	Widget * State::GetChoice (void)
	{
		return mChoice;
	}

	/// @brief Gets the frame head
	/// @return 0 on failure; head on success
	/// @note Tested
	Widget * State::GetFrameHead (void)
	{
		if (mFrame.empty()) return 0;

		return mFrame.front();
	}

	/// @brief Gets the user-defined context
	/// @return 0 on failure; user-defined context on success
	/// @note Tested
	void * State::GetContext (void)
	{
		return mContext;
	}

	/// @brief Clears the signaled widget
	/// @note Tested
	void State::Unsignal (void)
	{
		if (eIssuingEvents == mMode) return;

		if (mSignal != 0) mSignal->ClearSignals();

		mSignal = 0;
	}
}