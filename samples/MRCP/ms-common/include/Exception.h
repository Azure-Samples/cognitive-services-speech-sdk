#pragma once

#include <exception>
#include <string>

namespace Microsoft
{
	namespace speechlib
	{

		class Exception : public std::exception
		{
		public:
			Exception(std::string what) : _message(std::move(what)) {}
			~Exception() noexcept override = default;

			const char* what() const noexcept override 
			{
				return _message.c_str();
			}

		private:
			std::string _message;
		};
	}
}