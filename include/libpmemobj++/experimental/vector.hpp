/*
 * Copyright 2018, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * Vector container with std::vector compatible interface.
 */

#ifndef LIBPMEMOBJ_CPP_VECTOR_HPP
#define LIBPMEMOBJ_CPP_VECTOR_HPP

#include <libpmemobj++/detail/common.hpp>
#include <libpmemobj++/detail/iterator_traits.hpp>
#include <libpmemobj++/detail/life.hpp>
#include <libpmemobj++/experimental/contiguous_iterator.hpp>
#include <libpmemobj++/experimental/slice.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pext.hpp>
#include <libpmemobj++/transaction.hpp>
#include <libpmemobj.h>

#include <cassert>
#include <utility>

namespace pmem
{

namespace obj
{

namespace experimental
{

/**
 * pmem::obj::experimental::vector - EXPERIMENTAL persistent container
 * with std::vector compatible interface.
 */
template <typename T>
class vector {
public:
	/* Member types */
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type &;
	using const_reference = const value_type &;
	using pointer = value_type *;
	using const_pointer = const value_type *;
	using iterator = basic_contiguous_iterator<T>;
	using const_iterator = const_contiguous_iterator<T>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	/* Constructors */
	vector();
	vector(size_type count, const value_type &value);
	explicit vector(size_type count);
	template <typename InputIt,
		  typename std::enable_if<
			  detail::is_input_iterator<InputIt>::value &&
				  std::is_constructible<
					  value_type,
					  typename std::iterator_traits<
						  InputIt>::reference>::value,
			  InputIt>::type * = nullptr>
	vector(InputIt first, InputIt last);
	vector(const vector &other);
	vector(vector &&other);
	vector(std::initializer_list<T> init);

	/* Assign operators */
	vector &operator=(const vector &other);
	vector &operator=(vector &&other);
	vector &operator=(std::initializer_list<T> ilist);

	/* Assign methods */
	void assign(size_type count, const T &value);
	template <typename InputIt,
		  typename std::enable_if<
			  detail::is_input_iterator<InputIt>::value &&
				  std::is_constructible<
					  value_type,
					  typename std::iterator_traits<
						  InputIt>::reference>::value,
			  InputIt>::type * = nullptr>
	void assign(InputIt first, InputIt last);
	void assign(std::initializer_list<T> ilist);

	/* Destructor */
	~vector();

	/* Element access */
	reference at(size_type n);
	const_reference at(size_type n) const;
	reference operator[](size_type n);
	const_reference operator[](size_type n) const;
	reference front();
	const_reference front() const;
	reference back();
	const_reference back() const;
	pointer data();
	const_pointer data() const noexcept;

	/* Iterators */
	iterator begin();
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;
	iterator end();
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const noexcept;
	const_reverse_iterator crbegin() const noexcept;
	reverse_iterator rend();
	const_reverse_iterator rend() const noexcept;
	const_reverse_iterator crend() const noexcept;

	/* Capacity */
	constexpr bool empty() const noexcept;
	size_type size() const noexcept;
	constexpr size_type max_size() const noexcept;
	void reserve(size_type capacity_new);
	size_type capacity() const noexcept;
	void shrink_to_fit();

	/* Modifiers */
	// void clear() noexcept;
	void free_data();
	// iterator insert(const_iterator pos, const T &value);
	// iterator insert(const_iterator pos, T &&value);
	// iterator insert(const_iterator pos, size_type count, const T &value);
	// template <typename InputIt>
	// iterator insert(const_iterator pos, InputIt first, typename
	// std::enable_if<detail::is_input_iterator<InputIt>::value,
	// InputIt>::type last); iterator insert(const_iterator pos,
	// std::initializer_list<T> ilist); template <class... Args> iterator
	// emplace(const_iterator pos, Args&&... args); template< class... Args
	// > void emplace_back(Args&&... args); iterator erase(iterator pos);
	// iterator erase(const_iterator pos);
	// iterator erase(iterator first, iterator last);
	// iterator erase(const_iterator first, const_iterator last);
	// void push_back(const T& value);
	// void push_back(T&& value);
	// void pop_back();
	void resize(size_type count);
	void resize(size_type count, const value_type &value);
	void swap(vector &other);

private:
	/* helper functions */
	void _alloc(size_type size);
	void _dealloc();
	void _grow(size_type count, const_reference value);
	void _grow(size_type count);
	template <typename InputIt,
		  typename std::enable_if<
			  detail::is_input_iterator<InputIt>::value &&
				  std::is_constructible<
					  value_type,
					  typename std::iterator_traits<
						  InputIt>::reference>::value,
			  InputIt>::type * = nullptr>
	void _grow(InputIt first, InputIt last);
	void _realloc(size_type size);
	void _shrink(size_type size_new) noexcept;

	/* Underlying array */
	persistent_ptr<T[]> _data;

	p<size_type> _size;
	p<size_type> _capacity;
};

/**
 * Default constructor. Constructs an empty container.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @throw pmem::pool_error if an object is not in persistent memory.
 * @throw pmem::transaction_error if function wasn't called in transaction.
 */
template <typename T>
vector<T>::vector()
{
	auto pop = pmemobj_pool_by_ptr(this);
	if (pop == nullptr)
		throw pool_error("Invalid pool handle.");

	if (pmemobj_tx_stage() != TX_STAGE_WORK)
		throw transaction_error(
			"Default constructor called out of transaction scope.");

	_data = nullptr;
	_size = 0;
	_capacity = 0;
}

/**
 * Constructs the container with count copies of elements with value value.
 *
 * @param[in] count number of elements to construct.
 * @param[in] value value of all constructed elements.
 *
 * @throw pmem::pool_error if an object is not in persistent memory.
 * @throw pmem::transaction_alloc_error when allocating memory for underlying
 * array in transaction failed.
 * @throw pmem::transaction_error if function wasn't called in transaction.
 * @throw rethrows constructor exception from _grow.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @post _size = count
 * @post _capacity = detail::next_pow_2(_size)
 */
template <typename T>
vector<T>::vector(size_type count, const value_type &value)
{
	auto pop = pmemobj_pool_by_ptr(this);
	if (pop == nullptr)
		throw pool_error("Invalid pool handle.");

	if (pmemobj_tx_stage() != TX_STAGE_WORK)
		throw transaction_error(
			"Default constructor called out of transaction scope.");

	_data = nullptr;
	_size = 0;
	_alloc(detail::next_pow_2(count));
	_grow(count, value);
}

/**
 * Constructs the container with count copies of T default constructed values.
 *
 * @param[in] count number of elements to construct.
 *
 * @throw pmem::pool_error if an object is not in persistent memory.
 * @throw pmem::transaction_alloc_error when allocating memory for underlying
 * array in transaction failed.
 * @throw pmem::transaction_error if function wasn't called in transaction.
 * @throw rethrows constructor exception from _grow.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @post _size = count
 * @post _capacity = detail::next_pow_2(_size)
 */
template <typename T>
vector<T>::vector(size_type count)
{
	auto pop = pmemobj_pool_by_ptr(this);
	if (pop == nullptr)
		throw pool_error("Invalid pool handle.");

	if (pmemobj_tx_stage() != TX_STAGE_WORK)
		throw transaction_error(
			"Default constructor called out of transaction scope.");

	_data = nullptr;
	_size = 0;
	_alloc(detail::next_pow_2(count));
	_grow(count);
}

/**
 * Constructs the container with the contents of the range [first, last). The
 * first and last arguments must satisfy InputIterator requirements. This
 * overload only participates in overload resolution if InputIt satisfies
 * InputIterator, to avoid ambiguity with the overload of count-value
 * constructor.
 *
 * @param[in] first first iterator.
 * @param[in] last last iterator.
 *
 * @throw pmem::pool_error if an object is not in persistent memory.
 * @throw pmem::transaction_alloc_error when allocating memory for underlying
 * array in transaction failed.
 * @throw pmem::transaction_error if function wasn't called in transaction.
 * @throw rethrows constructor exception from _grow.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @post _size = std::distance(first, last)
 * @post _capacity = detail::next_pow_2(_size)
 */
template <typename T>
template <typename InputIt,
	  typename std::enable_if<
		  detail::is_input_iterator<InputIt>::value &&
			  std::is_constructible<
				  T,
				  typename std::iterator_traits<
					  InputIt>::reference>::value,
		  InputIt>::type *>
vector<T>::vector(InputIt first, InputIt last)
{
	auto pop = pmemobj_pool_by_ptr(this);
	if (pop == nullptr)
		throw pool_error("Invalid pool handle.");

	if (pmemobj_tx_stage() != TX_STAGE_WORK)
		throw transaction_error(
			"Default constructor called out of transaction scope.");

	_data = nullptr;
	_size = 0;
	_alloc(detail::next_pow_2(
		static_cast<size_type>(std::distance(first, last))));
	_grow(first, last);
}

/**
 * Copy constructor. Constructs the container with the copy of the
 * contents of other.
 *
 * @param[in] other reference to the vector to be copied.
 *
 * @throw pmem::pool_error if an object is not in persistent memory.
 * @throw pmem::transaction_alloc_error when allocating memory for underlying
 * array in transaction failed.
 * @throw pmem::transaction_error if function wasn't called in transaction.
 * @throw rethrows constructor exception from _grow.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @post _size = other._size
 * @post _capacity = other._capacity
 */
template <typename T>
vector<T>::vector(const vector &other)
{
	auto pop = pmemobj_pool_by_ptr(this);
	if (pop == nullptr)
		throw pool_error("Invalid pool handle.");

	if (pmemobj_tx_stage() != TX_STAGE_WORK)
		throw transaction_error(
			"Default constructor called out of transaction scope.");

	_data = nullptr;
	_size = 0;
	_alloc(other._capacity);
	_grow(other.begin(), other.end());
}

/**
 * Move constructor. Constructs the container with the contents of other using
 * move semantics. After the move, other is guaranteed to be empty().
 *
 * @param[in] other rvalue reference to the vector to be moved.
 *
 * @throw pmem::pool_error if an object is not in persistent memory.
 * @throw pmem::transaction_error if function wasn't called in transaction.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @post _size = other._size
 * @post _capacity = other._capacity
 * @post _data = other._data
 * @post other._data = nullptr;
 * @post other._capacity = other._size = 0;
 */
template <typename T>
vector<T>::vector(vector &&other)
{
	auto pop = pmemobj_pool_by_ptr(this);
	if (pop == nullptr)
		throw pool_error("Invalid pool handle.");

	if (pmemobj_tx_stage() != TX_STAGE_WORK)
		throw transaction_error(
			"Default constructor called out of transaction scope.");

	_data = other._data;
	_capacity = other._capacity;
	_size = other._size;
	other._data = nullptr;
	other._capacity = other._size = 0;
}

/**
 * Constructs the container with the contents of the initializer list init.
 *
 * @param[in] init initializer list with content to be constructed.
 *
 * @throw pmem::pool_error if an object is not in persistent memory.
 * @throw pmem::transaction_alloc_error when allocating memory for underlying
 * array in transaction failed.
 * @throw pmem::transaction_error if function wasn't called in transaction.
 * @throw rethrows constructor exception from _grow.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @post _size = init.size()
 * @post _capacity = detail::next_pow_2(_size)
 */
template <typename T>
vector<T>::vector(std::initializer_list<T> init)
    : vector(init.begin(), init.end())
{
}

/**
 * Copy assignment operator. Replaces the contents with a copy of the contents
 * of other.
 *
 * @throw pmem::transaction_alloc_error when allocating new memory failed.
 * @throw pmem::transaction_free_error when freeing old underlying array failed.
 * @throw rethrows constructor exception.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @post _size = other._size
 * @post _capacity = max(next_pow_2(_size), other._capacity)
 */
template <typename T>
vector<T> &
vector<T>::operator=(const vector &other)
{
	assert(pmemobj_tx_stage() != TX_STAGE_WORK);
	if (this != &other)
		assign(other.begin(), other.end());
	return *this;
}

/**
 * Move assignment operator. Replaces the contents with those of other using
 * move semantics (i.e. the data in other is moved from other into this
 * container). other is in a valid but empty state afterwards.
 *
 * @throw pmem::transaction_free_error when freeing underlying array failed.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @post _size = other._size
 * @post _capacity = other._capacity
 */
template <typename T>
vector<T> &
vector<T>::operator=(vector &&other)
{
	assert(pmemobj_tx_stage() != TX_STAGE_WORK);
	if (this == &other)
		return *this;

	_dealloc();
	_data = other._data;
	_capacity = other._capacity;
	_size = other._size;
	other._data = nullptr;
	other._capacity = other._size = 0;

	return *this;
}

/**
 * Replaces the contents with those identified by initializer list ilist.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @throw std::length_error if ilist.size() > max_size().
 * @throw pmem::transaction_alloc_error when allocating new memory failed.
 * @throw pmem::transaction_free_error when freeing old underlying array failed.
 * @throw rethrows constructor exception.
 */
template <typename T>
vector<T> &
vector<T>::operator=(std::initializer_list<T> ilist)
{
	assert(pmemobj_tx_stage() != TX_STAGE_WORK);
	assign(ilist.begin(), ilist.end());
	return *this;
}

/**
 * Replaces the contents with count copies of value value. All iterators,
 * pointers and references to the elements of the container are invalidated. The
 * past-the-end iterator is also invalidated.
 *
 * @param[in] count number of elements to construct.
 * @param[in] value value of all constructed elements.
 *
 * @throw std::length_error if detail::next_pow_2(count) > max_size().
 * @throw pmem::transaction_alloc_error when allocating new memory failed.
 * @throw pmem::transaction_free_error when freeing old underlying array failed.
 * @throw rethrows constructor exception.
 *
 * @post _size = count
 * @post _capacity = max(next_pow_2(_size), _capacity)
 */
template <typename T>
void
vector<T>::assign(size_type count, const_reference value)
{
	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);
	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		if (count <= capacity()) {
			std::fill_n(
				begin(),
				std::min(count, static_cast<size_type>(_size)),
				value);
			if (count > _size)
				_grow(count - _size, value);
			else
				_shrink(count);
		} else {
			_dealloc();
			_alloc(detail::next_pow_2(count));
			_grow(count, value);
		}
	});
}

/**
 * Replaces the contents with copies of those in the range [first, last). This
 * overload only participates in overload resolution if InputIt satisfies
 * InputIterator. All iterators, pointers and references to the elements of the
 * container are invalidated. The past-the-end iterator is also invalidated.
 *
 * @param[in] first first iterator.
 * @param[in] last last iterator.
 *
 * @throw std::length_error if std::distance(first, last) > max_size().
 * @throw pmem::transaction_alloc_error when allocating new memory failed.
 * @throw pmem::transaction_free_error when freeing old underlying array failed.
 * @throw rethrows constructor exception.
 *
 * @post _size = std::distance(first, last)
 * @post _capacity = max(next_pow_2(_size), _capacity)
 */
template <typename T>
template <typename InputIt,
	  typename std::enable_if<
		  detail::is_input_iterator<InputIt>::value &&
			  std::is_constructible<
				  T,
				  typename std::iterator_traits<
					  InputIt>::reference>::value,
		  InputIt>::type *>
void
vector<T>::assign(InputIt first, InputIt last)
{
	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);
	size_type size_new = static_cast<size_type>(std::distance(first, last));
	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		if (size_new <= capacity()) {
			InputIt mid = last;
			bool growing = false;
			if (size_new > _size) {
				growing = true;
				mid = first;
				std::advance(mid, _size);
			}
			pointer m = std::copy(first, mid, begin());
			if (growing)
				_grow(mid, last);
			else
				_shrink(m);
		} else {
			_dealloc();
			_alloc(detail::next_pow_2(size_new));
			_grow(first, last);
		}
	});
}

/**
 * Replaces the contents with the elements from the initializer list ilist. All
 * iterators, pointers and references to the elements of the container are
 * invalidated. The past-the-end iterator is also invalidated.
 *
 * @param[in] ilist initializer list with content to be constructed.
 *
 * @throw std::length_error if std::distance(first, last) > max_size().
 * @throw pmem::transaction_alloc_error when allocating new memory failed.
 * @throw pmem::transaction_free_error when freeing old underlying array failed.
 * @throw rethrows constructor exception.
 *
 * @post _size = std::distance(ilist.begin(), ilist.end())
 * @post _capacity = max(next_pow_2(_size), _capacity)
 */
template <typename T>
void
vector<T>::assign(std::initializer_list<T> ilist)
{
	assign(ilist.begin(), ilist.end());
}

/**
 * Destructor.
 * Note that free_data may throw an transaction_free_error when freeing
 * underlying array failed. It is recommended to call free_data manually before
 * object destruction.
 */
template <typename T>
vector<T>::~vector()
{
	free_data();
}

/**
 * Access element at specific index with bounds checking and add it to a
 * transaction.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @throw std::out_of_range if pos is not within the range of the container.
 */
template <typename T>
typename vector<T>::reference
vector<T>::at(size_type n)
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	if (n >= _size)
		throw std::out_of_range("vector::at");
	detail::conditional_add_to_tx(&_data[n]);
	return _data[n];
}

/**
 * Access element at specific index with bounds checking.
 *
 * @throw std::out_of_range if pos is not within the range of the container.
 */
template <typename T>
typename vector<T>::const_reference
vector<T>::at(size_type n) const
{
	if (n >= _size)
		throw std::out_of_range("vector::at");
	return _data[n];
}

/**
 * Access element at specific index and add it to a transaction.
 * No bounds checking is performed.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::reference vector<T>::operator[](size_type n)
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	detail::conditional_add_to_tx(&_data[n]);
	return _data[n];
}

/**
 * Access element at specific index. No bounds checking is performed.
 */
template <typename T>
typename vector<T>::const_reference vector<T>::operator[](size_type n) const
{
	return _data[n];
}

/**
 * Access the first element and add this element to a transaction.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::reference
vector<T>::front()
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	detail::conditional_add_to_tx(&_data[0]);
	return _data[0];
}

/**
 * Access the first element.
 */
template <typename T>
typename vector<T>::const_reference
vector<T>::front() const
{
	return _data[0];
}

/**
 * Access the last element and add this element to a transaction.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::reference
vector<T>::back()
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	detail::conditional_add_to_tx(&_data[size() - 1]);
	return _data[size() - 1];
}

/**
 * Access the last element.
 */
template <typename T>
typename vector<T>::const_reference
vector<T>::back() const
{
	return _data[size() - 1];
}

/**
 * Returns raw pointer to the underlying data and adds entire array to a
 * transaction.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::pointer
vector<T>::data()
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	detail::conditional_add_to_tx(this);
	return _data;
}

/**
 * Returns const raw pointer to the underlying data.
 */
template <typename T>
typename vector<T>::const_pointer
vector<T>::data() const noexcept
{
	return _data;
}

/**
 * Returns an iterator to the beginning.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::iterator
vector<T>::begin()
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	return iterator(_data.get());
}

/**
 * Returns const iterator to the beginning.
 */
template <typename T>
typename vector<T>::const_iterator
vector<T>::begin() const noexcept
{
	return const_iterator(_data.get());
}

/**
 * Returns const iterator to the beginning.
 */
template <typename T>
typename vector<T>::const_iterator
vector<T>::cbegin() const noexcept
{
	return const_iterator(_data.get());
}

/**
 * Returns an iterator to the end.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::iterator
vector<T>::end()
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	return iterator(_data.get() + static_cast<std::ptrdiff_t>(_size));
}

/**
 * Returns a const iterator to the end.
 */
template <typename T>
typename vector<T>::const_iterator
vector<T>::end() const noexcept
{
	return const_iterator(_data.get() + static_cast<std::ptrdiff_t>(_size));
}

/**
 * Returns a const iterator to the end.
 */
template <typename T>
typename vector<T>::const_iterator
vector<T>::cend() const noexcept
{
	return const_iterator(_data.get() + static_cast<std::ptrdiff_t>(_size));
}

/**
 * Returns a reverse iterator to the beginning.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::reverse_iterator
vector<T>::rbegin()
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	return reverse_iterator(end());
}

/**
 * Returns a const reverse iterator to the beginning.
 */
template <typename T>
typename vector<T>::const_reverse_iterator
vector<T>::rbegin() const noexcept
{
	return const_reverse_iterator(cend());
}

/**
 * Returns a const reverse iterator to the beginning.
 */
template <typename T>
typename vector<T>::const_reverse_iterator
vector<T>::crbegin() const noexcept
{
	return const_reverse_iterator(cend());
}

/**
 * Returns a reverse iterator to the end.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::reverse_iterator
vector<T>::rend()
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	return reverse_iterator(begin());
}

/**
 * Returns a const reverse iterator to the end.
 */
template <typename T>
typename vector<T>::const_reverse_iterator
vector<T>::rend() const noexcept
{
	return const_reverse_iterator(cbegin());
}

/**
 * Returns a const reverse iterator to the beginning.
 */
template <typename T>
typename vector<T>::const_reverse_iterator
vector<T>::crend() const noexcept
{
	return const_reverse_iterator(cbegin());
}

/**
 * Checks whether the container is empty.
 *
 * @return true if container is empty, 0 otherwise.
 */
template <typename T>
constexpr bool
vector<T>::empty() const noexcept
{
	return _size == 0;
}

/**
 * @return number of elements.
 */
template <typename T>
typename vector<T>::size_type
vector<T>::size() const noexcept
{
	return _size;
}

/**
 * @return maximum number of elements the container is able to hold due to PMDK
 * limitations.
 */
template <typename T>
constexpr typename vector<T>::size_type
vector<T>::max_size() const noexcept
{
	return PMEMOBJ_MAX_ALLOC_SIZE / sizeof(value_type);
}

/**
 * Increase the capacity of the vector to a value that's next power of 2 or
 * equal to capacity_new. If capacity_new is greater than the current
 * capacity(), new storage is allocated, otherwise the method does nothing. If
 * capacity_new is greater than capacity(), all iterators, including the
 * past-the-end iterator, and all references to the elements are invalidated.
 * Otherwise, no iterators or references are invalidated.
 *
 * @param[in] capacity_new new capacity indicator.
 *
 * @throw std::length_error if new_cap > max_size().
 * @throw pmem::transaction_alloc_error when allocaaaating new memory failed.
 * @throw pmem::transaction_free_error when freeing old underlying array failed.
 *
 * @post _capacity = max(_capacity, detail::next_pow_2(capacity_new))
 */
template <typename T>
void
vector<T>::reserve(size_type capacity_new)
{
	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);

	capacity_new = detail::next_pow_2(capacity_new);
	if (capacity_new <= _capacity)
		return;

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		detail::conditional_add_to_tx(begin().get_ptr(), _size);
		_realloc(capacity_new);
	});
}

/**
 * @return number of elements that can be held in currently allocated storage
 */
template <typename T>
typename vector<T>::size_type
vector<T>::capacity() const noexcept
{
	return _capacity;
}

/**
 * Requests transactional removal of unused capacity. New capacity will be set
 * to the lowest power of 2, not smaller than current size. If reallocation
 * occurs, all iterators, including the past the end iterator, and all
 * references to the elements are invalidated. If no reallocation takes place,
 * no iterators or references are invalidated.
 *
 * @throw pmem::transaction_alloc_error when reallocating failed.
 * @throw pmem::transaction_free_error when freeing old underlying array failed.
 * @throw rethrows constructor exception.
 */
template <typename T>
void
vector<T>::shrink_to_fit()
{
	size_type capacity_new = detail::next_pow_2(_size);
	if (_capacity == capacity_new)
		return;

	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);
	pool_base pb = pool_base(pop);

	transaction::run(pb, [&] {
		detail::conditional_add_to_tx(begin().get_ptr(), _size);
		_realloc(capacity_new);
	});
}

/**
 * Clears the content of a vector and frees all allocated persitent memory for
 * data in transaction.
 *
 * @throw pmem::transaction_free_error when freeing underlying array failed.
 *
 * @post _size = _capacity = 0
 * @post _data = nullptr
 */
template <typename T>
void
vector<T>::free_data()
{
	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);

	if (_data == nullptr)
		return;

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		detail::conditional_add_to_tx(begin().get_ptr(), _size);
		_dealloc();
	});
}

/**
 * Resizes the container to contain count elements. If the current size is
 * greater than count, the container is reduced to its first count elements. If
 * the current size is less than count, additional default-inserted elements are
 * appended
 *
 * @param[in] count new size of the container
 *
 * @throw rethrows constructor exception.
 * @throw pmem::transaction_free_error when freeing old underlying array failed.
 *
 * @post _capacity = count
 * @post _sice = std::min(_size, count)
 */
template <typename T>
void
vector<T>::resize(size_type count)
{
	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);

	if (_capacity == count)
		return;

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		detail::conditional_add_to_tx(begin().get_ptr(), _size);
		_realloc(count);

		if (_size < _capacity)
			_grow(_capacity - _size);
	});
}

/**
 * Resizes the container to contain count elements. If the current size is
 * greater than count, the container is reduced to its first count elements. If
 * the current size is less than count, additional copies of value are appended.
 *
 * @param[in] count new size of the container.
 * @param[in] value the value to initialize the new elements with.
 *
 * @throw rethrows constructor exception.
 * @throw pmem::transaction_free_error when freeing old underlying array failed.
 *
 * @post _capacity = count
 * @post _sice = std::min(_size, count)
 */
template <typename T>
void
vector<T>::resize(size_type count, const value_type &value)
{
	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);

	if (_capacity == count)
		return;

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		detail::conditional_add_to_tx(begin().get_ptr(), _size);
		_realloc(count);

		if (_size < _capacity)
			_grow(_capacity - _size, value);
	});
}

/**
 * Transactionally exchanges the contents of the container with those of other.
 */
template <typename T>
void
vector<T>::swap(vector &other)
{
	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);
	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		std::swap(this->_data, other._data);
		std::swap(this->_size, other._size);
		std::swap(this->_capacity, other._capacity);
	});
}

/**
 * Private helper function. Must be called during transaction. Allocates memory
 * for given number of elements.
 *
 * @param[in] capacity_new capacity of new underlying array
 *
 * @throw std::length_error if new size exceeds biggest possible pmem
 * allocation.
 * @throw pmem::transaction_alloc_error when allocating memory for underlying
 * array in transaction failed.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 * @pre _data == nullptr;
 * @pre _size == 0
 *
 * @post _capacity = capacity_new
 */
template <typename T>
void
vector<T>::_alloc(size_type capacity_new)
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	assert(_data == nullptr);
	assert(_size == 0);

	if (capacity_new > max_size())
		throw std::length_error("New capacity exceeds max size.");

	_capacity = capacity_new;

	if (capacity_new == 0)
		return;

	_data = pmemobj_tx_alloc(sizeof(value_type) * capacity_new,
				 detail::type_num<value_type>());

	if (_data == nullptr)
		throw transaction_alloc_error(
			"Failed to allocate persistent memory object");
}

/**
 * Private helper function. Must be called during transaction. Deallocates
 * underlying array.
 *
 * @throw pmem::transaction_free_error when freeing old underlying array
 * failed.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 *
 * @post _size = _capacity = 0
 * @post _data = nullptr
 */
template <typename T>
void
vector<T>::_dealloc()
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);

	if (_data != nullptr) {
		_shrink(0);
		if (pmemobj_tx_free(*_data.raw_ptr()) != 0)
			throw transaction_free_error(
				"failed to delete persistent memory object");
		_data = nullptr;
		_capacity = 0;
	}
}

/**
 * Private helper function. Must be called during transaction. Assumes that
 * there is enough space for additional elements. Copy constructs elements at
 * the end of underlying array based on given parameters.
 *
 * @param[in] count number of elements to construct.
 * @param[in] value value of all constructed elements.
 *
 * @throw rethrows constructor exception.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 * @pre if initialized, range [end(), end() + count) must be snapshotted in
 * current transaction
 * @pre _capacity >= count + _size
 * @pre value is valid argument for value_type copy constructor
 *
 * @post _size = _size + count
 */
template <typename T>
void
vector<T>::_grow(size_type count, const_reference value)
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	assert(_capacity >= count + _size);

	pointer dest = _data.get() + static_cast<size_type>(_size);
	const_pointer end = dest + count;
	for (; dest != end; ++dest)
		detail::create<value_type, const_reference>(dest, value);
	_size += count;
}

/**
 * Private helper function. Must be called during transaction. Assumes that
 * there is enough space for additional elements. Constructs default elements at
 * the end of underlying array based on given parameters.
 *
 * @param[in] count number of elements to construct.
 *
 * @throw rethrows constructor exception.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 * @pre if initialized, range [end(), end() + count) must be snapshotted in
 * current transaction
 * @pre _capacity >= count + _size
 * @pre value is valid argument for value_type copy constructor
 *
 * @post _size = _size + count
 */
template <typename T>
void
vector<T>::_grow(size_type count)
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	assert(_capacity >= count + _size);

	pointer dest = _data.get() + static_cast<size_type>(_size);
	const_pointer end = dest + count;
	for (; dest != end; ++dest)
		detail::create<value_type>(dest);
	_size += count;
}

/**
 * Private helper function. Must be called during transaction. Assumes that
 * there is enough space for additional elements and input arguments satisfy
 * InputIterator requirements. Constructs elements in underlying array with the
 * contents of the range [first, last). The first and last arguments must
 * satisfy InputIterator requirements. This overload participates in overload
 * resolution only if InputIt satisfies InputIterator.
 *
 * @param[in] first first iterator.
 * @param[in] last last iterator.
 *
 * @throw rethrows constructor exception.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 * @pre if initialized, range [end(), end() + std::distance(first, last)) must
 * be snapshotted in current transaction
 * @pre _capacity >= std::distance(first, last) + _size
 * @pre InputIt is InputIterator
 * @pre InputIt::reference is valid argument for value_type copy constructor
 *
 * @post _size = _size + std::distance(first, last)
 */
template <typename T>
template <typename InputIt,
	  typename std::enable_if<
		  detail::is_input_iterator<InputIt>::value &&
			  std::is_constructible<
				  T,
				  typename std::iterator_traits<
					  InputIt>::reference>::value,
		  InputIt>::type *>
void
vector<T>::_grow(InputIt first, InputIt last)
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	difference_type diff = std::distance(first, last);
	assert(diff >= 0);
	assert(_capacity >= static_cast<size_type>(diff) + _size);

	pointer dest = _data.get() + static_cast<size_type>(_size);
	_size += static_cast<size_type>(diff);
	while (first != last)
		detail::create<value_type>(dest++, *first++);
}

/**
 * Private helper function. Must be called during transaction. Allocates new
 * memory for capacity_new number of elements, than copies old elements to new
 * memory area. If the current size is greater than capacity_new, the container
 * is reduced to its first capacity_new elements.
 *
 * param[in] capacity_new new capacity.
 *
 * @throw rethrows constructor exception.
 * @throw pmem::transaction_free_error when freeing old underlying array failed.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 * @pre if not initialized, range [begin(), end()) must be snapshotted in
 * current transaction
 *
 * @post _capacity = capacity_new
 */
template <typename T>
void
vector<T>::_realloc(size_type capacity_new)
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);

	iterator cache_begin = begin();
	iterator cache_end = end();
	size_type cache_size = _size;
	_data = nullptr;
	_size = _capacity = 0;
	_alloc(capacity_new);
	iterator grow_end = std::advance(
		cache_begin,
		std::min(capacity_new, static_cast<size_type>(_size)));
	_grow(cache_begin, grow_end);

	/* destruct and free cached data */
	for (iterator it = cache_begin; it != cache_end; ++it)
		detail::destroy<value_type>(*it);
	if (pmemobj_tx_free(cache_begin.get_ptr()) != 0)
		throw transaction_free_error(
			"failed to delete persistent memory object");
}

/**
 * Private helper function. Must be called during transaction. Destroys
 * elements in underlying array beginning from position size_new.
 *
 * @param[in] size_new new size
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 * @pre if initialized, range [begin(), end()) must be snapshotted in current
 * transaction
 * @pre size_new <= _size
 *
 * @post _size = size_new
 */
template <typename T>
void
vector<T>::_shrink(size_type size_new) noexcept
{
	assert(pmemobj_tx_stage() == TX_STAGE_WORK);
	assert(size_new <= _size);

	for (size_type i = size_new; i < _size; ++i)
		// XXX: to remove static_cast when sign-detection in
		// persistent_ptr will be available
		detail::destroy<value_type>(
			_data[static_cast<difference_type>(i)]);
	_size = size_new;
}

} /* namespace experimental */

} /* namespace obj */

} /* namespace pmem */

#endif /* LIBPMEMOBJ_CPP_VECTOR_HPP */
