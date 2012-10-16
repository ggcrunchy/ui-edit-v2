#include "UserInterface.h"

namespace UI
{
	/// @brief Adds a custom part
	/// @return 0 on failure; part on success
	/// @note Tested
	Part * Composite::CreatePart (void)
	{
		Part * part = 0;

		try {
			part = new Part(this);

			mParts.insert(part);
		} catch (std::bad_alloc &) {}

		return part;
	}
}