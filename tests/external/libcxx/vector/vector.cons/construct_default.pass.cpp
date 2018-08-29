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

struct foo {
	pmem_exp::vector<double> v1;
	pmem_exp::vector<double> v2 = {};
};

struct root {
	nvobj::persistent_ptr<pmem_exp::vector<int>> vector_pptr;
	nvobj::persistent_ptr<foo> foo_pptr;
};

void
test_default_ctor(nvobj::pool<struct root> &pop)
{
	auto r = pop.root();

	try {
		nvobj::transaction::run(pop, [&] {
			r->vector_pptr =
				nvobj::make_persistent<pmem_exp::vector<int>>();
			r->foo_pptr = nvobj::make_persistent<foo>();
		});

		UT_ASSERT(r->vector_pptr->empty() == 1);
		UT_ASSERT(r->foo_pptr->v1.empty() == 1);
		UT_ASSERT(r->foo_pptr->v2.empty() == 1);

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
		path, "VectorTest", PMEMOBJ_MIN_POOL, S_IWUSR | S_IRUSR);

	test_default_ctor(pop);

	pop.close();
}
