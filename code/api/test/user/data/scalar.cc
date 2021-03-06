#include <gtest/gtest.h>

#include "allscale/api/user/data/scalar.h"
#include "allscale/utils/string_utils.h"

#include "allscale/utils/printer/vectors.h"
#include "allscale/utils/printer/pairs.h"

namespace allscale {
namespace api {
namespace user {
namespace data {

	#include "data_item_test.inl"

	TEST(ScalarRegion,TypeProperties) {

		using namespace detail;

		EXPECT_TRUE(utils::is_value<ScalarRegion>::value);
		EXPECT_TRUE(utils::is_serializable<ScalarRegion>::value);
		EXPECT_TRUE(core::is_region<ScalarRegion>::value);

	}

	TEST(ScalarRegion, LoadStore) {

		using namespace detail;

		// create the two region values
		ScalarRegion off = false;
		ScalarRegion on = true;

		EXPECT_NE(on,off);

		auto a1 = utils::serialize(off);
		auto a2 = utils::serialize(on);

		// restore values
		auto off2 = utils::deserialize<ScalarRegion>(a1);
		auto on2 = utils::deserialize<ScalarRegion>(a2);

		EXPECT_EQ(off,off2);
		EXPECT_EQ(on,on2);

	}

	TEST(ScalarFragment,TypeProperties) {

		using namespace detail;

		EXPECT_TRUE(core::is_fragment<ScalarFragment<int>>::value);

	}

	TEST(ScalarFragment, ExtractInsertTest) {

		using namespace detail;

		core::no_shared_data noSharedData;

		// get the two scalar region constants
		ScalarRegion on = true;
		ScalarRegion off = false;

		// set up three regions
		ScalarFragment<int> src(noSharedData,on);
		ScalarFragment<int> dst1(noSharedData,off);
		ScalarFragment<int> dst2(noSharedData,on);

		// check covered region
		EXPECT_EQ(on, src.getCoveredRegion());
		EXPECT_EQ(off,dst1.getCoveredRegion());
		EXPECT_EQ(on, dst2.getCoveredRegion());


		// set the value in source
		src.mask().set(12);

		// extract data from the source
		auto archiveOn  = extract(src,on);		// this is actually some data

		src.mask().set(14);
		auto archiveOff = extract(src,off);		// this is no data


		// now, import the data in the destination fragments

		// this is supposed to crash, since dst1 is not big enough to accept the input
		EXPECT_DEBUG_DEATH(insert(dst1,archiveOn),".*The region to be imported is not covered by this fragment!.*");

		// but this should work, since no actual data is imported
		insert(dst1,archiveOff);


		// some real import -- this one should work
		EXPECT_NE(12,dst2.mask().get());
		insert(dst2,archiveOn);
		EXPECT_EQ(12,dst2.mask().get());

		// importing the empty data archive should not change anything
		EXPECT_EQ(12,dst2.mask().get());
		insert(dst2,archiveOff);
		EXPECT_EQ(12,dst2.mask().get());

	}

	TEST(Scalar,TypeProperties) {

		using namespace detail;

		EXPECT_TRUE(core::is_data_item<Scalar<int>>::value);

	}

	TEST(Scalar,ExampleUse) {

		using namespace detail;

		ScalarRegion off = false;
		ScalarRegion on  = true;

		// create two fragments
		ScalarFragment<int> fA(core::no_shared_data(),on);
		ScalarFragment<int> fB(core::no_shared_data(),off);

		// write in first fragment
		Scalar<int> sA = fA.mask();
		sA.set(12);

		// check the value
		EXPECT_EQ(12,sA.get());
		EXPECT_EQ(12,const_cast<const Scalar<int>&>(sA).get());

		// -- move data to second fragment --

		fB.resize(on);
		fB.insert(fA,on);

		// write in first fragment
		Scalar<int> sB = fB.mask();
		EXPECT_EQ(12,sB.get());

	}

} // end namespace data
} // end namespace user
} // end namespace api
} // end namespace allscale
