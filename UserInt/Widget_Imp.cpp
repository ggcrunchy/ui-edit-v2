#include "UserInterface.h"

namespace UI
{
	/// @brief Constructs a Widget object
	/// @param state State used to build widget
	/// @note Tested
	Widget::Widget (State * state) : Signalee(state), mParent(0)
	{
	}

	/// @brief Destructs a Widget object
	/// @note Tested
	Widget::~Widget (void)
	{
		Untag();

		while (!mDock.empty()) mDock.front()->Unload();

		mState->mWidgets.erase(this);
	}

	/// @brief Performs choose logic
	/// @param bPressed If true, there is a press
	/// @note Tested
	void Widget::Choose (bool bPressed)
	{
		IssueEvent(eWidgetEvent_PreChoose);

		Enter();

		if (bPressed) Grab();

		IssueEvent(eWidgetEvent_PostChoose);
	}

	/// @brief Issues an event to the widget
	/// @param event Type of event to issue
	/// @note Tested
	void Widget::IssueEvent (Event event)
	{
		mState->mEventFunc(this, event);
	}

	/// @brief Runs a signal test on the widget and through its dock
	/// @note Tested
	void Widget::SignalTest (void)
	{
		// Iterate through the dock, recursing on each widget.
		if (!mStatus.test(eCannotDockSignalTest))
		{
			for (Iter_L wIter = mDock.begin(); wIter != mDock.end(); ++wIter)
			{
				(*wIter)->SignalTest();
			}
		}

		// Perform the signal test. Testing is complete if a signal is obtained or if the
		// user aborted testing.
		if (!mStatus.test(eCannotSignalTest))
		{
			mState->mSignalFunc(this);

			if (mState->mSignal != 0) throw true;
			if (eNormal == mState->mMode) throw false;
		}
	}

	/// @brief Performs an update on the widget and through its dock
	/// @note Tested
	void Widget::Update (void)
	{
		if (!mStatus[eCannotUpdate]) mState->mUpdateFunc(this);

		// Iterate backward through the dock, recursing on each widget.
		if (mStatus[eCannotDockUpdate]) return;

		for (RIter_L wIter = mDock.rbegin(); wIter != mDock.rend(); ++wIter)
		{
			(*wIter)->Update();
		}
	}

	/// @brief Performs upkeep logic
	/// @param bPressed If true, there is a press
	/// @note Tested
	void Widget::Upkeep (bool bPressed)
	{
		IssueEvent(eWidgetEvent_PreUpkeep);

		// Perform leave logic.
		Leave();

		// If the widget is signaled, perform enter logic; on a press, perform grab logic.
		if (IsSignaled())
		{
			Enter();

			if (bPressed) Grab();
		}

		// If there is no press, perform drop logic.
		if (!bPressed) Drop();

		// If the widget remains chosen, post-process it; otherwise, abandon it.
		if (!IsSignaled() && !IsChosen())
		{
			mState->ClearChoice();

			IssueEvent(eWidgetEvent_Abandon);
		}

		else IssueEvent(eWidgetEvent_PostUpkeep);
	}

	/// @brief Clears all important input state
	/// @note Tested
	void Widget::Clear (void)
	{
		mStatus.reset(eGrabbed);
		mStatus.reset(eEntered);
	}

	/// @brief Performs drop logic
	/// @note Tested
	void Widget::Drop (void)
	{
		if (!mStatus.test(eGrabbed)) return;

		mStatus.reset(eGrabbed);

		IssueEvent(eWidgetEvent_Drop);
	}

	/// @brief Performs enter logic
	/// @note Tested
	void Widget::Enter (void)
	{
		if (mStatus.test(eEntered)) return;

		mStatus.set(eEntered);

		IssueEvent(eWidgetEvent_Enter);
	}

	/// @brief Performs grab logic
	/// @note Tested
	void Widget::Grab (void)
	{
		if (mStatus.test(eGrabbed)) return;

		mStatus.set(eGrabbed);

		IssueEvent(eWidgetEvent_Grab);
	}

	/// @brief Performs leave logic
	/// @note Tested
	void Widget::Leave (void)
	{
		if (IsSignaled() || !mStatus.test(eEntered)) return;

		mStatus.reset(eEntered);

		IssueEvent(eWidgetEvent_Leave);
	}
}