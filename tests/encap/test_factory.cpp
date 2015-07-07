#include "fixtures/test_helpers.hpp"

#include <vector>
using namespace std;

#include <pfasst/encap/factory.hpp>
#include <pfasst/encap/vector.hpp>
using pfasst::encap::EncapsulationFactory;


typedef ::testing::Types<EncapsulationFactory<double, vector>> FactoryTypes;
INSTANTIATE_TYPED_TEST_CASE_P(VectorEncapFactory, Concepts, FactoryTypes);


TEST(VectorEncapFactory, set_size_after_initialization)
{
  EncapsulationFactory<double, vector> factory;
  EXPECT_THAT(factory.size(), Eq(0));

  factory.set_size(3);
  EXPECT_THAT(factory.size(), Eq(3));
}

TEST(VectorEncapFactory, takes_fixed_size)
{
  EncapsulationFactory<double, vector> factory(3);
  EXPECT_THAT(factory.size(), Eq(3));
}

TEST(VectorEncapFactory, produces_encapsulated_vectors)
{
  EncapsulationFactory<double, vector> factory(3);
  auto encap = factory.create();
  EXPECT_THAT(encap, NotNull());
  EXPECT_THAT(encap->get_data(), SizeIs(3));
}

TEST_MAIN()
