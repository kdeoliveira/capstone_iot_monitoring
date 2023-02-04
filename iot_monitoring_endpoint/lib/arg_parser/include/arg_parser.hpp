#pragma once

#include <string>
#include <map>
#include <vector>
#include <algorithm>


#define stringify(n) (#n)

#define GEN_CASE(n) case n:\
					return stringify(n);

namespace iot_monitoring {
	class ARGUMENTS {
	public:
		enum vals {
			INSTALL,
			UNINSTALL,
			HELP,
			PORT,
			INTERACTIVE
		};

		ARGUMENTS() = default;

		constexpr ARGUMENTS(vals val) :_value{ val } {}

		std::string toString() {
			switch (this->_value) {
			GEN_CASE(INSTALL)
			GEN_CASE(UNINSTALL)
			GEN_CASE(PORT)
			GEN_CASE(INTERACTIVE)
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
		std::vector<std::string> get_args();
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