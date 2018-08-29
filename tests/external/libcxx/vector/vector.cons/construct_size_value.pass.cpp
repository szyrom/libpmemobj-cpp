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

#include "unittest.hpp"

#include <cstring>
#include <libpmemobj++/experimental/vector.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

namespace nvobj = pmem::obj;
namespace pmem_exp = nvobj::experimental;

static const std::size_t TEST_ELEMENTS_NUMBER = 666;
static const double TEST_ELEMENT_VALUE = 6.66;

using vector_type = pmem_exp::vector<double>;

struct foo {
	vector_type v1 = vector_type(TEST_ELEMENTS_NUMBER, TEST_ELEMENT_VALUE);
};

struct root {
	nvobj::persistent_ptr<vector_type> vector_pptr;
	nvobj::persistent_ptr<foo> foo_pptr;
};

void
test_size_value_ctor(nvobj::pool<struct root> &pop)
{
	auto r = pop.root();

	try {
		nvobj::transaction::run(pop, [&] {
			r->vector_pptr = nvobj::make_persistent<vector_type>(
				TEST_ELEMENTS_NUMBER, TEST_ELEMENT_VALUE);
			r->foo_pptr = nvobj::make_persistent<foo>();
		});

		UT_ASSERT(r->vector_pptr->size() == TEST_ELEMENTS_NUMBER);
		UT_ASSERT(r->foo_pptr->v1.size() == TEST_ELEMENTS_NUMBER);

		for (vector_type::const_iterator i = r->vector_pptr->cbegin(),
						 e = r->vector_pptr->cend();
		     i != e; ++i)
			UT_ASSERT(*i == TEST_ELEMENT_VALUE);

		for (vector_type::const_iterator i = r->foo_pptr->v1.cbegin(),
						 e = r->foo_pptr->v1.cend();
		     i != e; ++i)
			UT_ASSERT(*i == TEST_ELEMENT_VALUE);

	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl
			  << std::strerror(nvobj::transaction::error())
			  << std::endl;
		UT_ASSERT(0);
	}
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

	test_size_value_ctor(pop);

	pop.close();
}
