#pragma once
// Project includes
#include "EtherealIncludes.h"
#include "Platform/EEWindows.h"

// Standard library includes
#include <memory>


// Singleton for managing the context of the application
namespace Ethereal
{
	class ETHEREAL_API EEContext
	{
	public:
		// Get the singleton instance of EEContext
		static EEContext& Get()
		{
			static EEContext instance;
			return instance;
		}
		// Delete copy constructor and assignment operator to enforce singleton pattern
		EEContext(const EEContext&) = delete;
		EEContext& operator=(const EEContext&) = delete;
	private:
		// Private constructor to prevent instantiation from outside
		EEContext();
		// Destructor
		~EEContext();	

	public:
		void Initialize(); // Method to initialize the context

		// Assessors
		EEWindows& GetWindow() const
		{
			return *m_Window;
		} // Get the window object
	private:
		std::unique_ptr<EEWindows> m_Window; // Pointer to the window object
	};
}
