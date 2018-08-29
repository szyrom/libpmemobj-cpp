//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Copyright 2018, Intel Corporation
//
// Modified to test pmem::obj containers
//

#include "test_iterators.h"
#include "test_macros.h"
#include "unittest.hpp"

#include <cstring>
#include <vector>

#include <libpmemobj++/experimental/vector.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

namespace nvobj = pmem::obj;
namespace pmem_exp = nvobj::experimental;

using vector_type = pmem_exp::vector<int>;

struct root {
	nvobj::persistent_ptr<vector_type> vector_pptr;
};

template <typename Iterator>
void
test_case(nvobj::pool<struct root> &pop, Iterator first, Iterator last)
{
	auto r = pop.root();

	try {
		nvobj::transaction::run(pop, [&] {
			r->vector_pptr = nvobj::make_persistent<vector_type>(
				first, last);
		});
		std::ptrdiff_t a =
			static_cast<std::ptrdiff_t>(r->vector_pptr->size());
		std::ptrdiff_t b = std::distance(first, last);
		UT_ASSERT(a == b);
		UT_ASSERT(static_cast<std::ptrdiff_t>(r->vector_pptr->size()) ==
			  std::distance(first, last));

		for (vector_type::const_iterator i = r->vector_pptr->cbegin(),
						 e = r->vector_pptr->cend();
		     i != e; ++i, ++first)
			UT_ASSERT(*i == *first);

	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl
			  << std::strerror(nvobj::transaction::error())
			  << std::endl;
		UT_ASSERT(0);
	}
}

void
basic_test_cases(nvobj::pool<struct root> &pop)
{
	int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 1, 0};
	int *an = a + sizeof(a) / sizeof(a[0]);
	test_case(pop, input_iterator<const int *>(a),
		  input_iterator<const int *>(an));
	test_case(pop, forward_iterator<const int *>(a),
		  forward_iterator<const int *>(an));
	test_case(pop, bidirectional_iterator<const int *>(a),
		  bidirectional_iterator<const int *>(an));
	test_case(pop, random_access_iterator<const int *>(a),
		  random_access_iterator<const int *>(an));
	test_case(pop, a, an);
}

int
main(int argc, char *argv[])
{
	START();

	if (argc < 2) {
		std::cerr << "usage: " << argv[0] << " file-name" << std::endl;
		return 1;
	}

	auto path = argv[1];

	auto pop = nvobj::pool<root>::create(
		path, "VectorTest", PMEMOBJ_MIN_POOL * 10, S_IWUSR | S_IRUSR);

	basic_test_cases(pop);

	pop.close();
}
