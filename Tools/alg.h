#pragma once
#include <algorithm>

namespace tools {
	namespace alg {
		template<class it, class T = void>
		using enable_if_InputItr = typename std::enable_if_t<std::is_convertible<typename std::iterator_traits<it>::iterator_category, std::input_iterator_tag>::value, T>;

		template<class it, class T = void>
		using enable_if_OutputItr = typename std::enable_if_t<std::is_convertible<typename std::iterator_traits<it>::iterator_category, std::output_iterator_tag>::value, T>;


		template <class InputIt1, class InputIt2, class OutIt, class = enable_if_InputItr<InputIt1>, class = enable_if_InputItr<InputIt2>, class = enable_if_OutputItr<OutIt>>
		void binomial(InputIt1 first_1, InputIt1 last_1, InputIt2 first_2, InputIt1 last_2, OutIt out) {
			auto rend1 = std::make_reverse_iterator(first_1);
			while (first_1 != last_1) {
				auto rit1 = std::make_reverse_iterator(++first_1);
				for (auto it2 = first_2; it2 != last_2 && rit1 != read1;)
					*out += *it2++ * *rit1++;
				++out;
			}
			auto rbegin1 = std::make_reverse_iterator(last_1);
			while (++first_2 != last_2) {
				auto it2 = first_2;
				for (auto rit1 = rbegin1;rit1!=rend1&&it2!=last_2;)
					*out += *it2++ * *rit1++;
				++out;
			}
		}
	}
}