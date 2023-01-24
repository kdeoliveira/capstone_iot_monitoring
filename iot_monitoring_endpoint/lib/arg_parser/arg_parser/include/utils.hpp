#include <algorithm>
#include <string>

namespace iot_monitoring {
	namespace strings {
		static void to_lower(std::string& str) {
			std::transform(str.begin(), str.end(), str.begin(), std::tolower);
		}
		static void to_upper(std::string& str) {
			std::transform(str.begin(), str.end(), str.begin(), std::toupper);
		}
	}
}