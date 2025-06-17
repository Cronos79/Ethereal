#pragma once
#include "Core/EtherealIncludes.h"
#include "Assets/IAsset.h"
#include <string>

namespace Ethereal
{
	class ETHEREAL_API GameConfig : public IAsset
	{
	public:
		GameConfig();
		virtual ~GameConfig();

		bool Load(const std::string& path);

		// Assessors
		std::string GetWindowsTitle()
		{
			return m_Title;
		}

		int32_t GetWidth()
		{
			return m_Width;
		}

		int32_t GetHeight()
		{
			return m_Height;
		}

		bool GetFullScreen()
		{
			return m_FullScreen;
		}

	private:
		std::string m_Title;
		int32_t m_Width;
		int32_t m_Height;
		bool m_FullScreen;
	};
}
