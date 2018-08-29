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

#ifndef PMEMOBJ_VECTOR_HPP
#define PMEMOBJ_VECTOR_HPP

#include "libpmemobj++/detail/common.hpp"
#include "libpmemobj++/detail/iterator_traits.hpp"
#include "libpmemobj++/detail/life.hpp"
#include "libpmemobj++/experimental/contiguous_iterator.hpp"
#include "libpmemobj++/experimental/slice.hpp"
#include "libpmemobj++/persistent_ptr.hpp"
#include "libpmemobj++/pext.hpp"
#include "libpmemobj++/transaction.hpp"
#include "libpmemobj.h"
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
	vector();						// done
	vector(size_type count, const value_type &value = T()); // done
	template <typename InputIt>
	vector(InputIt first,
	       typename std::enable_if<
		       detail::is_input_iterator<InputIt>::value &&
			       std::is_constructible<
				       value_type,
				       typename std::iterator_traits<
					       InputIt>::reference>::value,
		       InputIt>::type last);   // done
	vector(const vector &other);	   // done
	vector(vector &&other);		       // done
	vector(std::initializer_list<T> init); // done

	/* Assign operators */
	vector &operator=(const vector &other);		   // done
	vector &operator=(vector &&other);		   // done
	vector &operator=(std::initializer_list<T> ilist); // done

	/* Assign methods */
	void assign(size_type count, const T &value); // done
	template <typename InputIt>
	void assign(InputIt first,
		    typename std::enable_if<
			    detail::is_input_iterator<InputIt>::value &&
				    std::is_constructible<
					    value_type,
					    typename std::iterator_traits<
						    InputIt>::reference>::value,
			    InputIt>::type last);    // done
	void assign(std::initializer_list<T> ilist); // done

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
	bool empty() const noexcept;	  // done
	size_type size() const noexcept;      // done
	size_type max_size() const noexcept;  // done
	void reserve(size_type capacity_new); // done
	size_type capacity() const noexcept;  // done
	void shrink_to_fit();		      // done

	/* Modifiers */
	void clear() noexcept;				     // done
	void free_data();				     // done
	iterator insert(const_iterator pos, const T &value); // WIP
	iterator insert(const_iterator pos, T &&value);      // WIP
	iterator insert(const_iterator pos, size_type count,
			const T &value); // WIP
	template <typename InputIt>
	iterator insert(const_iterator pos, InputIt first,
			typename std::enable_if<
				detail::is_input_iterator<InputIt>::value,
				InputIt>::type last); // WIP
	iterator insert(const_iterator pos,
			std::initializer_list<T> ilist); // WIP
	// emplace
	// erase
	// push_back
	// emplace_back
	// pop_back
	// resize
	void swap(vector &other);

private:
	/* helper functions */
	void _alloc(size_type size); // done
	void _dealloc();	     // done
	void _construct_at_end(size_type count,
			       const value_type &value); // done
	template <typename InputIt>
	void _construct_at_end(
		InputIt first,
		typename std::enable_if<
			detail::is_input_iterator<InputIt>::value &&
				std::is_constructible<
					value_type,
					typename std::iterator_traits<
						InputIt>::reference>::value,
			InputIt>::type last);		    // done
	void _destruct_at_end(size_type size_new) noexcept; // done
	pointer _make_ptr(iterator it);			    // done

	/* Underlying array */
	persistent_ptr<T[]> _data = nullptr;

	p<size_type> _size = 0;
	p<size_type> _capacity = 0;
};

/**
 * Default constructor. Constructs an empty container.
 *
 * @throw pmem::pool_error if an object is not in persistent memory.
 */
template <typename T>
vector<T>::vector() : _data(nullptr), _size(0), _capacity(0)
{
	if (nullptr == pmemobj_pool_by_ptr(this))
		throw pool_error("Invalid pool handle.");
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
 * @throw rethrows constructor exception from _construct_at_end.
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

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		_alloc(detail::next_pow_2(count));
		_construct_at_end(count, value);
	});
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
 * @throw rethrows constructor exception from _construct_at_end.
 *
 * @post _size = std::distance(first, last)
 * @post _capacity = detail::next_pow_2(_size)
 */
template <typename T>
template <typename InputIt>
vector<T>::vector(InputIt first,
		  typename std::enable_if<
			  detail::is_input_iterator<InputIt>::value &&
				  std::is_constructible<
					  value_type,
					  typename std::iterator_traits<
						  InputIt>::reference>::value,
			  InputIt>::type last)
{
	auto pop = pmemobj_pool_by_ptr(this);
	if (pop == nullptr)
		throw pool_error("Invalid pool handle.");

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		_alloc(detail::next_pow_2(
			static_cast<size_type>(std::distance(first, last))));
		_construct_at_end(first, last);
	});
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
 * @throw rethrows constructor exception from _construct_at_end.
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

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		_alloc(other._capacity);
		_construct_at_end(other.begin(), other.end());
	});
}

/**
 * Move constructor. Constructs the container with the contents of other using
 * move semantics. After the move, other is guaranteed to be empty().
 *
 * @param[in] other rvalue reference to the vector to be moved.
 *
 * @throw pmem::pool_error if an object is not in persistent memory.
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

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		_data = other._data;
		_capacity = other._capacity;
		_size = other._size;
		other._data = nullptr;
		other._capacity = other._size = 0;
	});
}

/**
 * Constructs the container with the contents of the initializer list init.
 *
 * @param[in] init initializer list with content to be constructed.
 *
 * @throw pmem::pool_error if an object is not in persistent memory.
 * @throw pmem::transaction_alloc_error when allocating memory for underlying
 * array in transaction failed.
 * @throw rethrows constructor exception from _construct_at_end.
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
 * @post _size = other._size
 * @post _capacity = max(next_pow_2(_size), other._capacity)
 */
template <typename T>
vector<T> &
vector<T>::operator=(const vector &other)
{
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
 * @post _size = other._size
 * @post _capacity = other._capacity
 */
template <typename T>
vector<T> &
vector<T>::operator=(vector &&other)
{
	if (this == &other)
		return *this;

	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		_dealloc();
		_data = other._data;
		_capacity = other._capacity;
		_size = other._size;
		other._data = nullptr;
		other._capacity = other._size = 0;
	});

	return *this;
}

/**
 * Replaces the contents with those identified by initializer list ilist.
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
vector<T>::assign(size_type count, const T &value)
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
				_construct_at_end(count - _size, value);
			else
				_destruct_at_end(count);
		} else {
			_dealloc();
			_alloc(detail::next_pow_2(count));
			_construct_at_end(count, value);
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
template <typename InputIt>
void
vector<T>::assign(InputIt first,
		  typename std::enable_if<
			  detail::is_input_iterator<InputIt>::value &&
				  std::is_constructible<
					  value_type,
					  typename std::iterator_traits<
						  InputIt>::reference>::value,
			  InputIt>::type last)
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
				_construct_at_end(mid, last);
			else
				_destruct_at_end(m);
		} else {
			_dealloc();
			_alloc(detail::next_pow_2(size_new));
			_construct_at_end(first, last);
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
 * @throw std::out_of_range if pos is not within the range of the container.
 */
template <typename T>
typename vector<T>::reference
vector<T>::at(size_type n)
{
	if (n >= _size)
		throw std::out_of_range("vector::at");

	detail::conditional_add_to_tx(_data.get() + n);

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
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::reference vector<T>::operator[](size_type n)
{
	detail::conditional_add_to_tx(_data + n);

	return _data[static_cast<difference_type>(n)];
}

/**
 * Access element at specific index. No bounds checking is performed.
 */
template <typename T>
typename vector<T>::const_reference vector<T>::operator[](size_type n) const
{
	return _data[static_cast<difference_type>(n)];
}

/**
 * Access the first element and add this element to a transaction.
 *
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::reference
vector<T>::front()
{
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
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::reference
vector<T>::back()
{
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
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::pointer
vector<T>::data()
{
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
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::iterator
vector<T>::begin()
{
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
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::iterator
vector<T>::end()
{
	return iterator(_data.get() + static_cast<difference_type>(_size));
}

/**
 * Returns a const iterator to the end.
 */
template <typename T>
typename vector<T>::const_iterator
vector<T>::end() const noexcept
{
	return const_iterator(_data.get() +
			      static_cast<difference_type>(_size));
}

/**
 * Returns a const iterator to the end.
 */
template <typename T>
typename vector<T>::const_iterator
vector<T>::cend() const noexcept
{
	return const_iterator(_data.get() +
			      static_cast<difference_type>(_size));
}

/**
 * Returns a reverse iterator to the beginning.
 *
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::reverse_iterator
vector<T>::rbegin()
{
	return reverse_iterator(
		iterator(_data.get() + static_cast<difference_type>(_size)));
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
 * @throw pmem::transaction_error when adding the object to the transaction
 * failed.
 */
template <typename T>
typename vector<T>::reverse_iterator
vector<T>::rend()
{
	return reverse_iterator(iterator(_data.get()));
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
 * @return true if container is empty, 0 othwerwise.
 */
template <typename T>
bool
vector<T>::empty() const noexcept
{
	return !static_cast<bool>(_size);
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
typename vector<T>::size_type
vector<T>::max_size() const noexcept
{
	return PMEMOBJ_MAX_ALLOC_SIZE / sizeof(value_type);
}

/**
 * Increase the capacity of the vector to a value that's next power of 2 or
 * equal to new_cap. If new_cap is greater than the current capacity(), new
 * storage is allocated, otherwise the method does nothing. If new_cap is
 * greater than capacity(), all iterators, including the past-the-end iterator,
 * and all references to the elements are invalidated. Otherwise, no iterators
 * or references are invalidated.
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
	if (capacity_new <= _capacity)
		return;

	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] {
		persistent_ptr<T[]> _data_old = _data;
		size_type _size_old = _size;

		_data = nullptr;
		_size = _capacity = 0;
		_alloc(detail::next_pow_2(capacity_new));
		_construct_at_end(
			iterator(_data_old.get()),
			iterator(_data_old.get() +
				 static_cast<difference_type>(_size_old)));

		/* destruct and free old data */
		for (unsigned i = 0; i < _size_old; ++i)
			detail::destroy<value_type>(_data_old[i]);

		if (pmemobj_tx_free(*_data_old.raw_ptr()) != 0)
			throw transaction_free_error(
				"failed to delete persistent memory object");
	});

	return;
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
		persistent_ptr<T[]> _data_old = _data;
		size_type _size_old = _size;

		_data = nullptr;
		_size = _capacity = 0;
		_alloc(capacity_new);
		_construct_at_end(
			iterator(_data_old.get()),
			iterator(_data_old.get() +
				 static_cast<difference_type>(_size_old)));

		/* destruct and free old data */
		for (unsigned i = 0; i < _size_old; ++i)
			detail::destroy<value_type>(_data_old[i]);

		if (pmemobj_tx_free(*_data_old.raw_ptr()) != 0)
			throw transaction_free_error(
				"failed to delete persistent memory object");
	});
}

/**
 * Clears the content of a vector in transaction.
 *
 * @post _size = 0
 */
template <typename T>
void
vector<T>::clear() noexcept
{
	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] { _destruct_at_end(0); });
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
	if (_data == nullptr)
		return;

	auto pop = pmemobj_pool_by_ptr(this);
	assert(pop != nullptr);

	pool_base pb = pool_base(pop);
	transaction::run(pb, [&] { _dealloc(); });
}

/**
 * Inserts element value before pos.
 *
 * @param[in] pos iterator before which the content will be inserted. pos may be
 * the end() iterator.
 * @param[in] value element value to insert.
 *
 * @return Iterator pointing to the inserted value.
 *
 * @throw
 */
template <typename T>
typename vector<T>::iterator
vector<T>::insert(const_iterator pos, const value_type &value)
{
}

/**
 * Inserts element value before pos. Uses move semantics.
 *
 * @param[in] pos iterator before which the content will be inserted. pos may be
 * the end() iterator.
 * @param[in] value element value to insert.
 *
 * @return Iterator pointing to the inserted value.
 *
 * @throw
 */
template <typename T>
typename vector<T>::iterator
vector<T>::insert(const_iterator pos, value_type &&value)
{
}

/**
 * Inserts count copies of the value before pos.
 *
 * @param[in] pos iterator before which the content will be inserted. pos may be
 * the end() iterator.
 * @param[in] count number of elements to be insterted.
 * @param[in] value element value to insert.
 *
 * @return Iterator pointing to the first element inserted, or pos if count==0.
 *
 * @throw
 */
template <typename T>
typename vector<T>::iterator
vector<T>::insert(const_iterator pos, size_type count, const value_type &value)
{
}

/**
 * Inserts elements from range [first, last) before pos. This overload has the
 * same effect as overload pos-count-value if InputIt is an integral type.
 *
 * @param[in] pos iterator before which the content will be inserted. pos may be
 * the end() iterator.
 * @param[in] first beginning of the range of elements to insert, can't be
 * iterator into container for which insert is called.
 * @param[in] last end of the range of elements to insert, can't be iterator
 * into container for which insert is called.
 *
 * @return Iterator pointing to the first element inserted, or pos if
 * first==last.
 *
 * @throw
 */
template <typename T>
template <typename InputIt>
typename vector<T>::iterator
vector<T>::insert(
	const_iterator pos, InputIt first,
	typename std::enable_if<detail::is_input_iterator<InputIt>::value,
				InputIt>::type last)
{
}

/**
 * Inserts elements from initializer list ilist before pos.
 *
 * @param[in] pos iterator before which the content will be inserted. pos may be
 * the end() iterator.
 * @param[in] ilist initializer list to insert the values from.
 *
 * @return Iterator pointing to the first element inserted, or pos if ilist is
 * empty.
 *
 * @throw
 */
template <typename T>
typename vector<T>::iterator
vector<T>::insert(const_iterator pos, std::initializer_list<value_type> ilist)
{
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
 *
 * @post _capacity = capacity_new
 */
template <typename T>
void
vector<T>::_alloc(size_type capacity_new)
{
	_capacity = capacity_new;

	if (capacity_new == 0) {
		_data = nullptr;
		return;
	}

	if (capacity_new > max_size())
		throw std::length_error("New capacity exceeds max size.");

	_data = pmemobj_tx_alloc(sizeof(value_type) * capacity_new,
				 detail::type_num<value_type>());

	if (_data == nullptr)
		throw transaction_alloc_error(
			"failed to allocate persistent memory object");
}

/**
 * Private helper function. Must be called during transaction. Dellocates
 * underlying array.
 *
 * @throw pmem::transaction_free_error when freeing old underlying array failed.
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
	if (_data != nullptr) {
		clear();
		if (pmemobj_tx_free(*_data.raw_ptr()) != 0)
			throw transaction_free_error(
				"failed to delete persistent memory "
				"object");
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
 * @pre _capacity >= count + _size
 * @pre value is valid argument for value_type copy constructor
 *
 * @post _size = _size + count
 */
template <typename T>
void
vector<T>::_construct_at_end(size_type count, const value_type &value)
{
	pointer dest = _data.get() + static_cast<difference_type>(_size);
	pointer end = dest + count;
	for (; dest != end; ++dest)
		detail::create<value_type, const value_type &>(dest, value);
	_size += count;
}

/**
 * Private helper function. Must be called during transaction. Assumes that
 * there is enough space for additional elements and input arguments satisfies
 * InputIterator requirements. Constructs elements in underlying array with the
 * contents of the range [first, last). The first and last arguments must
 * satisfy InputIterator requirements. This overload only participates in
 * overload resolution if InputIt satisfies InputIterator.
 *
 * @param[in] first first iterator.
 * @param[in] last last iterator.
 *
 * @throw rethrows constructor exception.
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 * @pre _capacity >= std::distance(first, last)
 * @pre InputIt is InputIterator
 * @pre InputIt::reference is valid argument for value_type copy constructor
 *
 * @post _size = _size + std::distance(first, last)
 */
template <typename T>
template <typename InputIt>
void
vector<T>::_construct_at_end(
	InputIt first,
	typename std::enable_if<
		detail::is_input_iterator<InputIt>::value &&
			std::is_constructible<
				value_type,
				typename std::iterator_traits<
					InputIt>::reference>::value,
		InputIt>::type last)
{
	pointer dest = _data.get() + static_cast<difference_type>(_size);
	_size += static_cast<size_type>(std::distance(first, last));
	for (; first != last; ++dest)
		detail::create<value_type, const value_type &>(dest, *first++);
}

/**
 * Private helper function. Must be called during transaction. Destructs
 * elements in underlying array beginning from position size_new.
 *
 * @param[in] size_new new size
 *
 * @pre pmemobj_tx_stage() == TX_STAGE_WORK
 * @pre size_new <= _size
 *
 * @post _size = size_new
 */
template <typename T>
void
vector<T>::_destruct_at_end(size_type size_new) noexcept
{
	for (unsigned i = size_new; i < _size; ++i)
		detail::destroy<value_type>(_data[i]);
	_size = size_new;
}

/**
 * Private helper function. Returns raw vale_type pointer from given iterator.
 */
template <typename T>
typename vector<T>::pointer
vector<T>::_make_ptr(iterator it)
{
	return _data.get() + std::distance(begin(), it);
}

/* Non-member functions */

/**
 * Non-member equal operator.
 */
template <typename T>
inline bool
operator==(const vector<T> &lhs, const vector<T> &rhs)
{
	return lhs.size() == rhs.size() &&
		std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

/**
 * Non-member not-equal operator.
 */
template <typename T>
inline bool
operator!=(const vector<T> &lhs, const vector<T> &rhs)
{
	return !(lhs == rhs);
}

/**
 * Non-member less than operator.
 */
template <typename T>
inline bool
operator<(const vector<T> &lhs, const vector<T> &rhs)
{
	return std::lexicographical_compare(lhs.cbegin(), lhs.cend(),
					    rhs.cbegin(), rhs.cend());
}

/**
 * Non-member greater than operator.
 */
template <typename T>
inline bool
operator>(const vector<T> &lhs, const vector<T> &rhs)
{
	return rhs < lhs;
}

/**
 * Non-member greater or equal operator.
 */
template <typename T>
inline bool
operator>=(const vector<T> &lhs, const vector<T> &rhs)
{
	return !(lhs < rhs);
}

/**
 * Non-member less or equal operator.
 */
template <typename T>
inline bool
operator<=(const vector<T> &lhs, const vector<T> &rhs)
{
	return !(lhs > rhs);
}

/**
 * Non-member cbegin.
 */
template <typename T>
typename pmem::obj::experimental::vector<T>::const_iterator
cbegin(const pmem::obj::experimental::vector<T> &a)
{
	return a.cbegin();
}

/**
 * Non-member cend.
 */
template <typename T>
typename pmem::obj::experimental::vector<T>::const_iterator
cend(const pmem::obj::experimental::vector<T> &a)
{
	return a.cend();
}

/**
 * Non-member crbegin.
 */
template <typename T>
typename pmem::obj::experimental::vector<T>::const_reverse_iterator
crbegin(const pmem::obj::experimental::vector<T> &a)
{
	return a.crbegin();
}

/**
 * Non-member crend.
 */
template <typename T>
typename pmem::obj::experimental::vector<T>::const_reverse_iterator
crend(const pmem::obj::experimental::vector<T> &a)
{
	return a.crend();
}

/**
 * Non-member begin.
 */
template <typename T>
typename pmem::obj::experimental::vector<T>::iterator
begin(pmem::obj::experimental::vector<T> &a)
{
	return a.begin();
}

/**
 * Non-member end.
 */
template <typename T>
typename pmem::obj::experimental::vector<T>::iterator
end(pmem::obj::experimental::vector<T> &a)
{
	return a.end();
}

/**
 * Non-member rbegin.
 */
template <typename T>
typename pmem::obj::experimental::vector<T>::reverse_iterator
rbegin(pmem::obj::experimental::vector<T> &a)
{
	return a.rbegin();
}

/**
 * Non-member rend.
 */
template <typename T>
typename pmem::obj::experimental::vector<T>::reverse_iterator
rend(pmem::obj::experimental::vector<T> &a)
{
	return a.rend();
}

/**
 * Non-member swap function.
 */
template <typename T>
inline void
swap(pmem::obj::experimental::vector<T> &lhs,
     pmem::obj::experimental::vector<T> &rhs)
{
	lhs.swap(rhs);
}

/**
 * Non-member get function.
 */
template <size_t I, typename T>
T &
get(pmem::obj::experimental::vector<T> &v)
{
	static_assert(I < v.size(),
		      "Index out of bounds in std::get<> "
		      "(pmem::obj::vector)");
	return v.at(I);
}

/**
 * Non-member get function.
 */
template <size_t I, typename T>
T &&
get(pmem::obj::experimental::vector<T> &&v)
{
	static_assert(I < v.size(),
		      "Index out of bounds in std::get<> "
		      "(pmem::obj::vector)");
	return std::move(v.at(I));
}

/**
 * Non-member get function.
 */
template <size_t I, typename T>
const T &
get(const pmem::obj::experimental::vector<T> &v) noexcept
{
	static_assert(I < v.size(),
		      "Index out of bounds in std::get<> "
		      "(pmem::obj::vector)");
	return v.at(I);
}

/**
 * Non-member get function.
 */
template <size_t I, typename T>
const T &&
get(const pmem::obj::experimental::vector<T> &&v) noexcept
{
	static_assert(I < v.size(),
		      "Index out of bounds in std::get<> "
		      "(pmem::obj::vector)");
	return std::move(v.at(I));
}

} /* namespace experimental */

} /* namespace obj */

} /* namespace pmem */

#endif /* PMEMOBJ_VECTOR_HPP */
