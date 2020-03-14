#pragma once
#include <vector>
#include <array>
#include <utility>
#include <optional>
#include <algorithm>

#undef min
#undef max
#pragma warning( push )
#pragma warning(disable:26444)

namespace tools {
	template<class it, class T = void>
	using enable_if_InputItr = typename std::enable_if_t<std::is_convertible<typename std::iterator_traits<it>::iterator_category, std::input_iterator_tag>::value, T>;



	template<class arrvec, class array_t, class vector_t>
	class _ArrVec_const_iterator {
	private:
		friend typename arrvec;
		using arr_it_type = typename array_t::const_iterator;
		using vec_it_type = typename vector_t::const_iterator;

		arr_it_type a_it, end;
		vec_it_type v_it, begin;

		//    array part    |     vector part
		//       a_it	 end begin   v_it
	public:
		using value_type = typename arrvec::value_type;
		using reference = typename arrvec::const_reference;
		using pointer = typename arrvec::const_pointer;
		using difference_type = std::common_type_t<typename arr_it_type::difference_type, typename vec_it_type::difference_type>;
		using iterator_category = std::random_access_iterator_tag;


		_ArrVec_const_iterator() noexcept {}
		_ArrVec_const_iterator(const _ArrVec_const_iterator& other) :a_it(other.a_it), end(other.end), v_it(other.v_it), begin(other.begin) {}
		_ArrVec_const_iterator(const typename arrvec::iterator& other) :a_it(other.a_it), end(other.end), v_it(other.v_it), begin(other.begin) {}
		_ArrVec_const_iterator(_ArrVec_const_iterator&& other) noexcept :a_it(std::move(other.a_it)), end(std::move(other.end)), v_it(std::move(other.v_it)), begin(std::move(other.begin)) {}
		_ArrVec_const_iterator(typename arrvec::iterator&& other) noexcept :a_it(std::move(other.a_it)), end(std::move(other.end)), v_it(std::move(other.v_it)), begin(std::move(other.begin)) {}
		_ArrVec_const_iterator& operator=(const _ArrVec_const_iterator& other) {
			a_it = other.a_it;
			end = other.end;
			begin = other.begin;
			v_it = other.v_it;
			return *this;
		}
		_ArrVec_const_iterator& operator=(_ArrVec_const_iterator&& other) noexcept {
			a_it = std::move(other.a_it);
			end = std::move(other.end);
			begin = std::move(other.begin);
			v_it = std::move(other.v_it);
			return *this;
		}
		~_ArrVec_const_iterator() noexcept {}

		operator void() {}
		reference operator*() const {
			if (a_it == end)
				return *v_it;
			else
				return *a_it;
		}
		pointer operator->() const {
			if (a_it == end)
				return &*v_it;
			else
				return &*a_it;
		}
		reference operator[](const difference_type n) const {
			auto tmp = *this;
			tmp += n;
			return *tmp;
		}
		_ArrVec_const_iterator& operator++() {
			if (a_it == end)
				++v_it;
			else
				++a_it;
			return *this;
		}
		_NODISCARD _ArrVec_const_iterator operator++(int) {
			auto tmp = *this;
			this->operator++();
			return tmp;
		}
		_ArrVec_const_iterator& operator--() {
			if (v_it == begin)
				--a_it;
			else
				--v_it;
			return *this;
		}
		_NODISCARD _ArrVec_const_iterator operator--(int) {
			auto tmp = *this;
			this->operator--();
			return tmp;
		}
		_ArrVec_const_iterator& operator+=(const difference_type n) {
			if (n < 0) return this->operator-=(-n);
			if (a_it == end)
				v_it += n;
			else if (auto dis = std::distance(a_it, end); dis >= n)
				a_it += n;
			else {
				v_it += n - dis;
				a_it = end;
			}
			return *this;
		}
		_ArrVec_const_iterator operator+(const difference_type n) const {
			auto tmp = *this;
			return tmp += n;
		}
		_ArrVec_const_iterator& operator-=(const difference_type n) {
			if (n < 0) return this->operator+=(-n);
			if (v_it == begin)
				a_it -= n;
			else if (auto dis = std::distance(begin, v_it); dis >= n)
				v_it -= n;
			else {
				a_it -= n - dis;
				v_it = begin;
			}
			return *this;
		}
		_ArrVec_const_iterator operator-(const difference_type n) const {
			auto tmp = *this;
			return tmp -= n;
		}
		difference_type operator-(const _ArrVec_const_iterator& op) const {
			return (a_it == end ? std::distance(begin, v_it) : -std::distance(a_it, end)) - (op.a_it == end ? std::distance(begin, op.v_it) : -std::distance(op.a_it, end));
		}

		bool operator==(const _ArrVec_const_iterator& op) const {
			return a_it == op.a_it && v_it == op.v_it;
		}
		bool operator!=(const _ArrVec_const_iterator& op) const {
			return !this->operator==(op);
		}
		bool operator<(const _ArrVec_const_iterator& op) const {
			if (op.a_it != end)
				return a_it < op.a_it;
			else if (a_it == end)
				return v_it < op.v_it;
			else
				return true;
		}
		bool operator>=(const _ArrVec_const_iterator& op) const { return !this->operator<(op); }
		bool operator>(const _ArrVec_const_iterator& op) const {
			if (a_it != end)
				return op.a_it < a_it;
			else if (op.a_it == end)
				return op.v_it < v_it;
			else
				return true;
		}
		bool operator<=(const _ArrVec_const_iterator& op) const { return !this->operator>(op); }

	private:
		explicit _ArrVec_const_iterator(arr_it_type end, vec_it_type begin, arr_it_type a_it, vec_it_type v_it)noexcept :end(end), begin(begin), a_it(a_it), v_it(v_it) {}
	};
	template<class arrvec, class array_t, class vector_t>
	_ArrVec_const_iterator<arrvec, array_t, vector_t> operator+(
		typename _ArrVec_const_iterator<arrvec, array_t, vector_t>::difference_type n, _ArrVec_const_iterator<arrvec, array_t, vector_t> op) {
		return op += n;
	}


	template<class arrvec, class array_t, class vector_t>
	class _ArrVec_iterator {
	private:
		friend typename arrvec;
		friend typename arrvec::const_iterator;
		using arr_it_type = typename array_t::iterator;
		using vec_it_type = typename vector_t::iterator;

		arr_it_type a_it, end;
		vec_it_type v_it, begin;

		//    array part    |     vector part
		//       a_it	 end begin   v_it
	public:
		using value_type = typename arrvec::value_type;
		using reference = typename arrvec::reference;
		using pointer = typename arrvec::pointer;
		using difference_type = std::common_type_t<typename arr_it_type::difference_type, typename vec_it_type::difference_type>;
		using iterator_category = std::random_access_iterator_tag;


		_ArrVec_iterator() noexcept {}
		_ArrVec_iterator(const _ArrVec_iterator& other) :a_it(other.a_it), end(other.end), v_it(other.v_it), begin(other.begin) {}
		_ArrVec_iterator(_ArrVec_iterator&& other) noexcept :a_it(std::move(other.a_it)), end(std::move(other.end)), v_it(std::move(other.v_it)), begin(std::move(other.begin)) {}
		_ArrVec_iterator& operator=(const _ArrVec_iterator& other) {
			a_it = other.a_it;
			end = other.end;
			begin = other.begin;
			v_it = other.v_it;
			return *this;
		}
		_ArrVec_iterator& operator=(_ArrVec_iterator&& other) noexcept {
			a_it = std::move(other.a_it);
			end = std::move(other.end);
			begin = std::move(other.begin);
			v_it = std::move(other.v_it);
			return *this;
		}
		~_ArrVec_iterator() noexcept {}

		operator void() {}
		reference operator*() const {
			if (a_it == end)
				return *v_it;
			else
				return *a_it;
		}
		pointer operator->()const {
			if (a_it == end)
				return &*v_it;
			else
				return &*a_it;
		}
		reference operator[](const difference_type n) const {
			auto tmp = *this;
			tmp += n;
			return *tmp;
		}
		_ArrVec_iterator& operator++() {
			if (a_it == end)
				++v_it;
			else
				++a_it;
			return *this;
		}
		_NODISCARD _ArrVec_iterator operator++(int) {
			auto tmp = *this;
			this->operator++();
			return tmp;
		}
		_ArrVec_iterator& operator--() {
			if (v_it == begin)
				--a_it;
			else
				--v_it;
			return *this;
		}
		_NODISCARD _ArrVec_iterator operator--(int) {
			auto tmp = *this;
			this->operator--();
			return tmp;
		}
		_ArrVec_iterator& operator+=(const difference_type n) {
			if (n < 0) return this->operator-=(-n);
			if (a_it == end)
				v_it += n;
			else if (auto dis = std::distance(a_it, end); dis >= n)
				a_it += n;
			else {
				v_it += n - dis;
				a_it = end;
			}
			return *this;
		}
		_ArrVec_iterator operator+(const difference_type n) const {
			auto tmp = *this;
			return tmp += n;
		}
		_ArrVec_iterator& operator-=(const difference_type n) {
			if (n < 0) return this->operator+=(-n);
			if (v_it == begin)
				a_it -= n;
			else if (auto dis = std::distance(begin, v_it); dis >= n)
				v_it -= n;
			else {
				a_it -= n - dis;
				v_it = begin;
			}
			return *this;
		}
		_ArrVec_iterator operator-(const difference_type n) const {
			auto tmp = *this;
			return tmp -= n;
		}
		difference_type operator-(const _ArrVec_iterator& op) const {
			return (a_it == end ? std::distance(begin, v_it) : -std::distance(a_it, end)) - (op.a_it == end ? std::distance(begin, op.v_it) : -std::distance(op.a_it, end));
		}

		bool operator==(const _ArrVec_iterator& op) const {
			return a_it == op.a_it && v_it == op.v_it;
		}
		bool operator!=(const _ArrVec_iterator& op) const {
			return !this->operator==(op);
		}
		bool operator<(const _ArrVec_iterator& op) const {
			if (op.a_it != end)
				return a_it < op.a_it;
			else if (a_it == end)
				return v_it < op.v_it;
			else
				return true;
		}
		bool operator>=(const _ArrVec_iterator& op) const { return !this->operator<(op); }
		bool operator>(const _ArrVec_iterator& op) const {
			if (a_it != end)
				return op.a_it < a_it;
			else if (op.a_it == end)
				return op.v_it < v_it;
			else
				return true;
		}
		bool operator<=(const _ArrVec_iterator& op) const { return !this->operator>(op); }

	private:
		explicit _ArrVec_iterator(arr_it_type end, vec_it_type begin, arr_it_type a_it, vec_it_type v_it)noexcept :end(end), begin(begin), a_it(a_it), v_it(v_it) {}
	};
	template<class arrvec, class array_t, class vector_t>
	_ArrVec_iterator<arrvec, array_t, vector_t> operator+(
		typename _ArrVec_iterator<arrvec, array_t, vector_t>::difference_type n, _ArrVec_iterator<arrvec, array_t, vector_t> op) {
		return op += n;
	}


	template<class T,
		std::size_t N,
		class Allocator = std::allocator<T>
	>class arrvec {
	private:
		using arr_type = std::array<T, N>;
		using vec_type = std::vector<T, Allocator>;

		size_t Size;
		arr_type arr;
		vec_type vec;

	public:
		using value_type = T;
		using allocator_type = Allocator;
		using size_type = std::size_t;
		using difference_type = std::common_type_t<typename arr_type::difference_type, typename vec_type::difference_type>;
		using pointer = std::common_type_t<typename arr_type::pointer, typename vec_type::pointer>;
		using const_pointer = std::common_type_t<typename arr_type::const_pointer, typename vec_type::const_pointer>;
		using reference = value_type&;
		using const_reference = const value_type&;

		using iterator = _ArrVec_iterator<arrvec, arr_type, vec_type>;
		using const_iterator = _ArrVec_const_iterator<arrvec, arr_type, vec_type>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	public:
		arrvec() noexcept(noexcept(Allocator())) :Size(0), arr(), vec() {}
		explicit arrvec(const Allocator& alloc) noexcept :Size(0), arr(), vec(alloc) {}
		explicit arrvec(size_type count, const T& value, const Allocator& alloc = Allocator()) :Size(count), vec(count > N ? count - N : 0, value, alloc), arr() {
			if (count <= N)
				for (size_type i = 0; i < count; ++i)
					arr[i] = value;
			else
				arr.fill(value);
		}
		explicit arrvec(size_type count, const Allocator& alloc = Allocator()) :arrvec(count, T(), alloc) {}
		template <std::size_t M, class Alloc>
		arrvec(const arrvec<T, M, Alloc>& other) : arrvec(other.cbegin(), other.cend(), other.get_allocator()) {}
		template <class InputIt, class = enable_if_InputItr<InputIt>>
		arrvec(InputIt first, InputIt last, const Allocator& alloc = Allocator()) : Size(0), vec(alloc) {
			for (auto it = arr.begin(), end = arr.end(); it != end && first != last; ++it, ++first, ++Size)
				*it = *first;
			vec.insert(vec.end(), first, last);
			Size += vec.size();
		}
		arrvec(const arrvec& other) :Size(other.Size), arr(other.arr), vec(other.vec) {}
		arrvec(const arrvec& other, const Allocator& alloc) :Size(other.Size), arr(other.arr), vec(other.vec, alloc) {}
		arrvec(arrvec&& other) noexcept :Size(other.Size), arr(std::move(other.arr)), vec(std::move(other.vec)) {}
		arrvec(arrvec&& other, const Allocator& alloc) :Size(other.Size), arr(std::move(other.arr)), vec(std::move(other.vec), alloc) {}
		arrvec(std::initializer_list<T> init, const Allocator& alloc = Allocator()) :arrvec(init.begin(), init.end(), alloc) {}


		arrvec& operator=(const arrvec& other) {
			Size = other.Size;
			arr = other.arr;
			vec = other.vec;
			return *this;
		}
		arrvec& operator=(arrvec&& other) noexcept {
			Size = other.Size;
			arr = std::move(other.arr);
			vec = std::move(other.vec);
			return *this;
		}
		arrvec& operator=(std::initializer_list<T> ilist) {
			return *this = std::move(arrvec(ilist));
		}

		void assign(size_type count, const T& value) {
			*this = std::move(arrvec(count, value));
		}
		template <class InputIt, class = enable_if_InputItr<InputIt>>
		void assign(InputIt first, InputIt last) {
			*this = std::move(arrvec(first, last));
		}
		void assign(std::initializer_list<T> ilist) {
			*this = std::move(ilist);
		}

		allocator_type get_allocator() const {
			return vec.get_allocator();
		}

		reference at(size_type pos) {
			if (pos < N)
				return arr[pos];
			else
				return vec[pos - N];
		}
		const_reference at(size_type pos) const {
			if (pos < N)
				return arr[pos];
			else
				return vec[pos - N];
		}
		reference operator[](size_type pos) {
			return this->at(pos);
		}
		const_reference operator[](size_type pos) const {
			return this->at(pos);
		}

		reference front() {
			return arr.front();
		}
		const_reference front() const {
			return arr.front();
		}

		reference back() {
			if (Size <= N)
				return arr[Size - 1];
			else
				return vec.back();
		}
		const_reference back() const {
			if (Size <= N)
				return arr[Size - 1];
			else
				return vec.back();
		}

		iterator begin() noexcept { return iterator(arr.end(), vec.begin(), arr.begin(), vec.begin()); }
		const_iterator begin() const noexcept { return begin(); }
		const_iterator cbegin() const noexcept { return const_iterator(arr.cend(), vec.cbegin(), arr.cbegin(), vec.cbegin()); }

		iterator end() noexcept {
			if (Size <= N)
				return iterator(arr.end(), vec.begin(), arr.begin() + Size, vec.end());
			else
				return iterator(arr.end(), vec.begin(), arr.end(), vec.end());
		}
		const_iterator end() const noexcept { return end(); }
		const_iterator cend() const noexcept {
			if (Size <= N)
				return const_iterator(arr.cend(), vec.cbegin(), arr.cbegin() + Size, vec.cend());
			else
				return const_iterator(arr.cend(), vec.cbegin(), arr.cend(), vec.cend());
		}

		reverse_iterator rbegin() noexcept { return std::make_reverse_iterator(end()); }
		const_reverse_iterator rbegin() const noexcept { return rbegin(); }
		const_reverse_iterator crbegin() const noexcept { return std::make_reverse_iterator(cend()); }

		reverse_iterator rend() noexcept { return std::make_reverse_iterator(begin()); }
		const_reverse_iterator rend() const noexcept { return rend(); }
		const_reverse_iterator crend() const noexcept { return std::make_reverse_iterator(cbegin()); }

		bool empty() const noexcept {
			return Size == 0;
		}
		size_type size() const noexcept {
			return Size;
		}
		size_type max_size() const noexcept {
			return vec.max_size() + N;
		}
		void reserve(size_type new_cap) {
			if (new_cap > N)
				vec.reserve(new_cap - N);
		}
		size_type capacity() const noexcept {
			return N + vec.capacity();
		}
		void shrink_to_fit() {
			vec.shrink_to_fit();
		}

		void clear() noexcept {
			vec.clear();
			Size = 0;
		}


		iterator insert(const const_iterator& pos, const T& value) {
			if (pos.a_it == pos.end) {
				++Size;
				auto it = vec.insert(pos.v_it, value);
				return iterator(arr.end(), vec.begin(), arr.end(), it);
			}
			else {
				++Size;
				auto it = arr.begin() + std::distance(arr.cbegin(), pos.a_it);
				auto val = std::move(*it);
				for (auto to = std::next(it), end = std::next(arr.begin(), std::min(Size, N)); to != end; ++to)
					std::swap(val, *to);
				if (Size > N)
					vec.insert(vec.begin(), std::move(val));
				*it = value;
				return iterator(arr.end(), vec.begin(), it, vec.begin());
			}
		}
		iterator insert(const const_iterator& pos, T&& value) {
			if (pos.a_it == pos.end) {
				++Size;
				auto it = vec.insert(pos.v_it, std::move(value));
				return iterator(arr.end(), vec.begin(), arr.end(), it);
			}
			else {
				++Size;
				auto it = arr.begin() + std::distance(arr.cbegin(), pos.a_it);
				auto val = std::move(*it);
				for (auto to = std::next(it), end = std::next(arr.begin(), std::min(Size, N)); to != end; ++to)
					std::swap(val, *to);
				if (Size > N)
					vec.insert(vec.begin(), std::move(val));
				*it = std::move(value);
				return iterator(arr.end(), vec.begin(), it, vec.begin());
			}
		}
		iterator insert(const_iterator pos, size_type count, const T& value) {
			if (count == 0)
				return iterator(arr.end(), vec.begin(), arr.begin() + std::distance(arr.cbegin(), pos.a_it), vec.begin());

			if (pos.a_it == pos.end) {
				Size += count;
				auto it = vec.insert(pos.v_it, count, value);
				return iterator(arr.end(), vec.begin(), arr.end(), it);
			}
			else {
				auto copy_begin = arr.begin() + std::distance(arr.cbegin(), pos.a_it);
				auto copy_end = Size > N ? arr.end() : arr.begin() + Size;
				auto sep = count > N ? copy_begin : std::clamp(arr.end() - count, copy_begin, copy_end);

				vec.insert(vec.begin(), sep, copy_end);

				if (sep != copy_begin) {
					auto itr = sep;
					do {
						--itr;
						*(itr + count) = std::move(*itr);
					} while (itr != copy_begin);
				}
				Size += count;
				for (auto itr = copy_begin, end = arr.end(); itr != end && count; --count, ++itr)
					*itr = value;
				vec.insert(vec.begin(), count, value);
				return iterator(arr.end(), vec.begin(), copy_begin, vec.begin());
			}
		}
		template <class InputIt, class = enable_if_InputItr<InputIt>>
		iterator insert(const_iterator pos, InputIt first, InputIt last) {
			auto count = std::distance(first, last);
			if (count == 0)
				return iterator(arr.end(), vec.begin(), arr.begin() + std::distance(arr.cbegin(), pos.a_it), vec.begin());

			if (pos.a_it == pos.end) {
				Size += count;
				auto it = vec.insert(pos.v_it, first, last);
				return iterator(arr.end(), vec.begin(), arr.end(), it);
			}
			else {
				auto copy_begin = arr.begin() + std::distance(arr.cbegin(), pos.a_it);
				auto copy_end = Size > N ? arr.end() : arr.begin() + Size;
				auto sep = count > N ? copy_begin : std::clamp(arr.end() - count, copy_begin, copy_end);

				vec.insert(vec.begin(), sep, copy_end);

				if (sep != copy_begin) {
					auto itr = sep;
					do {
						--itr;
						*(itr + count) = std::move(*itr);
					} while (itr != copy_begin);
				}
				Size += count;
				for (auto itr = copy_begin, end = arr.end(); itr != end && count; --count, ++itr)
					*itr = *first++;
				vec.insert(vec.begin(), first, last);
				return iterator(arr.end(), vec.begin(), copy_begin, vec.begin());
			}

		}
		iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
			return insert(pos, ilist.begin(), ilist.end());
		}

		template< class... Args >
		iterator emplace(const_iterator pos, Args&&... args) {
			return insert(pos, std::move(T(std::forward<Args>(args)...)));
		}

		iterator erase(const_iterator pos) { return erase(pos, std::next(pos)); }
		iterator erase(const_iterator first, const_iterator last) {
			auto count = std::distance(first, last);
			if (count == 0)
				return iterator(arr.end(), vec.begin(), arr.begin() + std::distance(arr.cbegin(), first.a_it), vec.begin());

			if (first.a_it == first.end) {
				Size -= count;
				auto it = vec.erase(first.v_it, last.v_it);
				return iterator(arr.end(), vec.begin(), arr.end(), it);
			}
			else {
				auto itr = iterator(arr.end(), vec.begin(), arr.begin() + std::distance(arr.cbegin(), first.a_it), vec.begin());
				auto it = std::copy(last, cend(), itr);
				if (it.a_it == it.end) {
					auto tmp = vec.erase(it.v_it, vec.end());
					it = iterator(arr.end(), vec.begin(), arr.end(), tmp);
				}
				else
					vec.clear();
				Size -= count;
				return itr;
			}
		}

		void push_back(const T& value) {
			if (Size < N)
				arr[Size] = value;
			else
				vec.push_back(value);
			++Size;
		}
		void push_back(T&& value) {
			if (Size < N)
				arr[Size] = value;
			else
				vec.push_back(value);
			++Size;
		}

		template< class... Args >
		decltype(auto) emplace_back(Args&&... args) {
			if (Size < N)
				return arr[Size++] = T(std::forward<Args>(args)...);
			else {
				++Size;
				return vec.emplace_back(std::forward<Args>(args)...);
			}
		}

		void pop_back() {
			if (Size == 0) return;
			if (Size > N) vec.pop_back();
			--Size;
		}
		std::optional<T> pop_take_back() {
			if (Size == 0) return std::optional<T>();
			--Size;
			if (Size < N)
				return arr[Size];
			else {
				auto tmp = vec.back();
				vec.pop_back();
				return std::move(tmp);
			}
		}

		void resize(size_type count, const value_type& value = T()) {
			if (count <= N) {
				if (Size < count)
					std::fill(arr.begin() + Size, arr.begin() + count, value);
				vec.clear();
			}
			else {
				if (Size < N)
					std::fill(arr.begin() + Size, arr.end(), value);
				vec.resize(count - N, value);
			}
			Size = count;
		}

		void swap(arrvec& other) noexcept {
			std::swap(Size, other.Size);
			std::swap(arr, other.arr);
			std::swap(vec, other.vec);
		}

		void fill(const T& value) {
			if (Size <= N)
				std::fill_n(arr.begin(), Size, value);
			else {
				arr.fill(value);
				std::fill(vec.begin(), vec.end(), value);
			}
		}
	};
}
#pragma warning( pop )