#pragma once

#include <string>
#include <map>
#include <vector>
#include <algorithm>


#define stringify(n) (#n)

namespace iot_monitoring {
	class ARGUMENTS {
	public:
		enum vals {
			INSTALL,
			UNINSTALL,
			HELP,
			PORT,
		};

		ARGUMENTS() = default;

		constexpr ARGUMENTS(vals val) :_value{ val } {}

		std::string toString() {
			switch (this->_value) {
			case INSTALL:
				return stringify(INSTALL);
			case UNINSTALL:
				return stringify(UNINSTALL);
			case PORT:
				return stringify(PORT);
			default:
				return stringify(HELP);
			}
		}

	private:
		vals _value;
	};


	class arg_parser {
	private:
		std::string _cmd;
		std::vector<std::string> _args;

	public:
		arg_parser() = delete;
		arg_parser(std::vector<std::string>::iterator, std::vector<std::string>::iterator);
		~arg_parser();

		
	};


	class arg_handler {
	private:
	std::vector<std::string> _arguments;
	public:
		arg_handler(int,char**);
		~arg_handler();

		arg_parser* handle(ARGUMENTS);

	};



}