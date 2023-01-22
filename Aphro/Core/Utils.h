#ifndef APH_UTILS_H
#define APH_UTILS_H

#include <functional>

namespace aph {
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};
}

#endif // !APH_UTILS_H
