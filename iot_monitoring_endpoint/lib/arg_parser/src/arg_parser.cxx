#include "arg_parser.hpp"
#include "utils.hpp"

namespace iot_monitoring {
	arg_parser::arg_parser(std::vector<std::string>::iterator iter, std::vector<std::string>::iterator end) {
		this->_cmd = *iter;

		std::for_each(iter, end, [&](std::string& val) {
			if(!val.empty() && val.at(0) != '-' && val.at(0) != '/') {
				this->_args.push_back(std::move(val));
		}
			});
	}

	arg_parser::~arg_parser() {
		this->_args.clear();
	}

	arg_handler::arg_handler(int argc, char** argv) {
		if (argc < 1) {
			return;
		}
		this->_arguments = std::vector<std::string>(argv+1, argv + argc);
			
		
	}

	arg_parser* arg_handler::handle(ARGUMENTS op) {

		auto iter_args = std::find_if(this->_arguments.begin(), this->_arguments.end(), [&](std::string& val) {
			if (!val.empty() && (val.at(0) == '-' || val.at(0) == '/')) {
				strings::to_upper(val);
				
				return val.substr(1, val.size())._Equal(op.toString());
			}
			else {
				return false;
			}
			});

		if (iter_args != this->_arguments.end())
			return new arg_parser(iter_args, this->_arguments.end());
		else
			return nullptr;
				
	}

	arg_handler::~arg_handler() {

	}
}