#ifndef CYCLIC_BUFFER_HPP
#define CYCLIC_BUFFER_HPP

#include <assert.h>

// The destructor of T will not be called until the entire
// cyclic buffer is destructed, so I'd suggest to only use
// this class with POD types.
// I didn't add iterator types to this because that would
// be overkill for the simple usecase here. It might be
// tempting to use some existing cyclic buffer library
// from somewhere but we can't use the standard library
// and also I don't want to use a C library so here we go.
template <typename T, unsigned int buffer_capacity>
class CyclicBuffer {
	static_assert(buffer_capacity > 0,
	              "Can't create cyclic buffer of capacity 0.");

public:
	using value_type = T;
	using reference = T &;
	using size_type = unsigned int;

	T &push(const T &val);
	T &push();
	void clear() noexcept;

	bool empty() const noexcept;
	bool full() const noexcept;

	size_type size() const noexcept;
	constexpr size_type capacity() const noexcept;

	T &operator[](size_type pos) noexcept;
	const T &operator[](size_type pos) const noexcept;

	T &back() noexcept;
	const T &back() const noexcept;

private:
	T data[buffer_capacity];
	size_type start_idx{0};
	size_type current_size{0};

	void increment_start_idx() noexcept;
	size_type calculate_internal_idx(size_type pos) const noexcept;
};

template <typename T, unsigned int buffer_capacity>
T &CyclicBuffer<T, buffer_capacity>::push(const T &val) {
	data[calculate_internal_idx(current_size)] = val;

	push();

	return back();
}

template <typename T, unsigned int buffer_capacity>
T &CyclicBuffer<T, buffer_capacity>::push() {
	if (full()) {
		increment_start_idx();
	} else {
		++current_size;
	}

	return back();
}

template <typename T, unsigned int buffer_capacity>
void CyclicBuffer<T, buffer_capacity>::clear() noexcept {
	current_size = 0;
}

template <typename T, unsigned int buffer_capacity>
bool CyclicBuffer<T, buffer_capacity>::empty() const noexcept {
	return size() == 0;
}

template <typename T, unsigned int buffer_capacity>
bool CyclicBuffer<T, buffer_capacity>::full() const noexcept {
	return size() == capacity();
}

template <typename T, unsigned int buffer_capacity>
unsigned int CyclicBuffer<T, buffer_capacity>::size() const noexcept {
	return current_size;
}

template <typename T, unsigned int buffer_capacity>
constexpr unsigned int
CyclicBuffer<T, buffer_capacity>::capacity() const noexcept {
	return buffer_capacity;
}

template <typename T, unsigned int buffer_capacity>
T &CyclicBuffer<T, buffer_capacity>::operator[](size_type pos) noexcept {
	assert(pos < size());
	return data[calculate_internal_idx(pos)];
}

template <typename T, unsigned int buffer_capacity>
const T &
CyclicBuffer<T, buffer_capacity>::operator[](size_type pos) const noexcept {
	assert(pos < size());
	return data[calculate_internal_idx(pos)];
}

template <typename T, unsigned int buffer_capacity>
T &CyclicBuffer<T, buffer_capacity>::back() noexcept {
	assert(size() > 0);
	return (*this)[size() - 1];
}

template <typename T, unsigned int buffer_capacity>
const T &CyclicBuffer<T, buffer_capacity>::back() const noexcept {
	assert(size() > 0);
	return (*this)[size() - 1];
}

template <typename T, unsigned int buffer_capacity>
void CyclicBuffer<T, buffer_capacity>::increment_start_idx() noexcept {
	++start_idx;
	if (start_idx == buffer_capacity) {
		start_idx = 0;
	}
}

template <typename T, unsigned int buffer_capacity>
unsigned int CyclicBuffer<T, buffer_capacity>::calculate_internal_idx(
    size_type pos) const noexcept {
	return (start_idx + pos) % buffer_capacity;
}

#endif // CYCLIC_BUFFER_HPP