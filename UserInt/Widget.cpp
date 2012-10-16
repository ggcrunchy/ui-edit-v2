#include "UserInterface.h"

namespace UI
{
	/// @brief Adds the widget to its frame
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked from an update or signal test callback
	/// @note Tested
	int Widget::AddToFrame (void)
	{
		if (eSignalTesting == mState->mMode) return 0;
		if (eUpdating == mState->mMode) return 0;

		// Unload the pane if it is already loaded.
		Unload();

		// Put the pane in the frame. Indicate that the pane is framed.
		mLookup = mState->mFrame.insert(mState->mFrame.end(), this);

		mStatus.set(eFramed);

		return 1;
	}

	/// @brief Allows or disallows signal testing docked widgets
	/// @param bAllow If true, allow signal testing
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked from a signal test callback
	/// @note Tested
	int Widget::AllowDockSignalTest (bool bAllow)
	{
		if (eSignalTesting == mState->mMode) return 0;

		mStatus.set(eCannotDockSignalTest, !bAllow);

		return 1;
	}

	/// @brief Allows or disallows updating loaded widgets
	/// @param bAllow If true, allow updating
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked from an update callback
	/// @note Tested
	int Widget::AllowDockUpdate (bool bAllow)
	{
		if (eUpdating == mState->mMode) return 0;

		mStatus.set(eCannotDockUpdate, !bAllow);

		return 1;
	}

	/// @brief Allows or disallows signal testing this widget
	/// @param bAllow If true, allow signal testing
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked from a signal test callback
	/// @note Tested
	int Widget::AllowSignalTest (bool bAllow)
	{
		if (eSignalTesting == mState->mMode) return 0;

		mStatus.set(eCannotSignalTest, !bAllow);

		return 1;
	}

	/// @brief Allows or disallows updating this widget
	/// @param bAllow If true, allow updating
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked from an update callback
	/// @note Tested
	int Widget::AllowUpdate (bool bAllow)
	{
		if (eUpdating == mState->mMode) return 0;

		mStatus.set(eCannotUpdate, !bAllow);

		return 1;
	}

	/// @brief Terminates the instance
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked during updating or signal propagation
	/// @note Tested
	int Widget::Destroy (void)
	{
		if (mState->mMode != eNormal) return 0;

		// Ensure that the widget is unloaded.
		Unload();

		// Destroy the widget.
		delete this;

		return 1;
	}

	/// @brief Docks a widget
	/// @param widget Handle to the widget
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked from an update or signal test callback
	/// @note Tested
	int Widget::Dock (Widget * widget)
	{
		if (0 == widget) return 0;
		if (this == widget) return 0;
		if (mState != widget->mState) return 0;
		if (eSignalTesting == mState->mMode) return 0;
		if (eUpdating == mState->mMode) return 0;

		// Unload the widget, put it in the dock, and bind it and the parent to one another.
		widget->Unload();

		widget->mLookup = mDock.insert(mDock.end(), widget);
		widget->mParent = this;

		return 1;
	}

	/// @brief Gets the tag
	/// @param tag [out] On success, the tag
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int Widget::GetTag (std::wstring & tag)
	{
		if (!IsTagged()) return 0;

		tag = mTagLookup->first;

		return 1;
	}

	/// @brief Puts the widget at the head of its parent's dock
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked from an update or signal test callback
	/// @note Tested
	int Widget::PromoteToDockHead (void)
	{
		if (!IsDocked()) return 0;
		if (eSignalTesting == mState->mMode) return 0;
		if (eUpdating == mState->mMode) return 0;

		mParent->mDock.splice(mParent->mDock.begin(), mParent->mDock, mLookup);

		return 1;
	}

	/// @brief Puts the widget at the head of its frame
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked from an update or signal test callback
	/// @note Tested
	int Widget::PromoteToFrameHead (void)
	{
		if (!IsFramed()) return 0;
		if (eSignalTesting == mState->mMode) return 0;
		if (eUpdating == mState->mMode) return 0;

		mState->mFrame.splice(mState->mFrame.begin(), mState->mFrame, mLookup);

		return 1;
	}

	/// @brief Sets the tag
	/// @param tag Tag to associate with widget
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int Widget::SetTag (std::wstring const & tag)
	{
		// Remove any tag currently associated with the widget. If the tag itself is in use,
		// indicate that the widget associated with it no longer acknowledges it.
		Untag();

		std::map<std::wstring, Widget*>::iterator item = mState->mTags.find(tag);
		
		if (item != mState->mTags.end())
		{
			item->second->mStatus.reset(eTagged);
		}

		// Associate the name with the tag. In order to facilitate easy disassociation at a
		// later time, cache an iterator into the tag map. Indicate that the widget is tagged.
		mState->mTags[tag] = this;

		mTagLookup = mState->mTags.find(tag);

		mStatus.set(eTagged);

		return 1;
	}

	/// @brief Removes the widget from its parent or the frame
	/// @return 0 on failure, non-0 for success
	/// @note NOP if invoked from an update or signal test callback
	/// @note Tested
	int Widget::Unload (void)
	{
		if (eSignalTesting == mState->mMode) return 0;
		if (eUpdating == mState->mMode) return 0;

		if (IsLoaded())
		{
			// Clear the choice if it is this widget.
			if (mState->GetChoice() == this) mState->ClearChoice();

			// If the widget is docked, unbind it from the parent.
			if (IsDocked())
			{
				mParent->mDock.erase(mLookup);

				mParent = 0;
			}

			// Otherwise, remove the widget from the frame.
			else
			{
				mState->mFrame.erase(mLookup);

				mStatus.reset(eFramed);
			}
		}

		return 1;
	}

	/// @brief Removes the tag, if any
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int Widget::Untag (void)
	{
		// Remove the tag, if any, mapped to the widget, and indicate that it is untagged.
		if (mStatus.test(eTagged))
		{
			mState->mTags.erase(mTagLookup);

			mStatus.reset(eTagged);
		}

		return 1;
	}

	/// @brief Gets the type
	/// @param type [out] On success, the type
	/// @return 0 on failure, non-0 for success
	/// @note Tested
	int Widget::GetType (WidgetType & type)
	{
		type = Type();

		return 1;
	}

	/// @brief Gets the head of the dock
	/// @return 0 on failure; head on success
	/// @note Tested
	Widget * Widget::GetDockHead (void)
	{
		if (mDock.empty()) return 0;

		return mDock.front();
	}

	/// @brief Gets the next widget in its parent's dock
	/// @return 0 on failure; next widget on success
	/// @note Tested
	Widget * Widget::GetNextDockLink (void)
	{
		if (!IsDocked()) return 0;

		Iter_L iter = mLookup;

		if (++iter == mParent->mDock.end()) return 0;

		return *iter;
	}

	/// @brief Gets the next widget in the frame
	/// @return 0 on failure; next widget on success
	/// @note Tested
	Widget * Widget::GetNextFrameLink (void)
	{
		if (!IsFramed()) return 0;

		Iter_L iter = mLookup;

		if (++iter == mState->mFrame.end()) return 0;

		return *iter;
	}

	/// @brief Gets the parent in which the widget is docked
	/// @return 0 on failure; parent on success
	/// @note Tested
	Widget * Widget::GetParent (void)
	{
		return mParent;
	}

	/// @brief Used to signal the widget
	/// @note NOP if not invoked from a signal test callback
	/// @note Tested
	void Widget::Signal (void)
	{
		if (mState->mMode != eSignalTesting) return;

		// Clear any signaled widget and install this one.
		mState->Unsignal();

		mState->mSignal = this;
	}

	/// @brief Indicates whether the widget is docked
	/// @return If true, the widget is docked
	/// @note Tested
	bool Widget::IsDocked (void)
	{
		return mParent != 0;
	}

	/// @brief Indicates whether the widget is entered
	/// @return If true, the widget is entered
	/// @note Tested
	bool Widget::IsEntered (void)
	{
		return mStatus.test(eEntered);
	}

	/// @brief Indicates whether the pane is set in a frame
	/// @return If true, the pane is framed
	/// @note Tested
	bool Widget::IsFramed (void)
	{
		return mStatus.test(eFramed);
	}

	/// @brief Indicates whether the widget is grabbed
	/// @return If true, the widget is grabbed
	/// @note Tested
	bool Widget::IsGrabbed (void)
	{
		return mStatus.test(eGrabbed);
	}

	/// @brief Indicates whether the widget is docked or framed
	/// @return If true, the widget is loaded
	/// @note Tested
	bool Widget::IsLoaded (void)
	{
		return IsDocked() || IsFramed();
	}

	/// @brief Indicates whether the widget is signaled
	/// @return If true, the widget is signaled
	/// @note Tested
	bool Widget::IsSignaled (void)
	{
		return mState->mSignal == this;
	}

	/// @brief Indicates whether the widget can be signal tested
	/// @return If true, the widget can be signal tested
	/// @note Tested
	bool Widget::IsSignalTestAllowed (void)
	{
		// A widget can be signal tested if it is loaded and all its parents allow it.
		if (!IsLoaded()) return false;
		if (mStatus.test(eCannotSignalTest)) return false;

		Widget * widget = this;

		while (widget->IsDocked())
		{
			widget = widget->mParent;

			if (widget->mStatus.test(eCannotDockSignalTest)) return false;
		}

		return true;
	}

	/// @brief Indicates whether the widget is tagged
	/// @return If true, the widget is tagged
	/// @note Tested
	bool Widget::IsTagged (void)
	{
		return mStatus.test(eTagged);
	}

	/// @brief Indicates whether the widget can be updated
	/// @return If true, the widget can be updated
	/// @note Tested
	bool Widget::IsUpdateAllowed (void)
	{
		// A widget can be updated if it is loaded and all its parents allow it.
		if (!IsLoaded()) return false;
		if (mStatus.test(eCannotUpdate)) return false;

		Widget * widget = this;

		while (widget->IsDocked())
		{
			widget = widget->mParent;

			if (widget->mStatus.test(eCannotDockUpdate)) return false;
		}

		return true;
	}
}