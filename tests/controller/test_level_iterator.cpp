#include "fixtures/test_helpers.hpp"

#include <pfasst/controller/interface.hpp>
using pfasst::Controller;

#include <pfasst/encap/traits.hpp>
#include <pfasst/encap/vector.hpp>
typedef pfasst::vector_encap_traits<double, double> VectorEncapTrait;
typedef pfasst::encap::Encapsulation<VectorEncapTrait> VectorEncapsulation;

#include <pfasst/sweeper/interface.hpp>
using pfasst::Sweeper;

#include "sweeper/mocks.hpp"

typedef typename Controller<double, VectorEncapsulation>::LevelIterator LevelIter;

typedef ::testing::Types<LevelIter> LevelIteratorTypes;
INSTANTIATE_TYPED_TEST_CASE_P(LevelIterator, Concepts, LevelIteratorTypes);


class Interface
  : public ::testing::Test
{
  protected:
    LevelIter level_iter;

    shared_ptr<Controller<double, VectorEncapsulation>> controller;

    virtual void SetUp()
    {
      this->controller = make_shared<Controller<double, VectorEncapsulation>>();
    }
};

TEST_F(Interface, default_construction)
{
  EXPECT_THAT(level_iter.get_level(), Eq(0));
  EXPECT_THROW(level_iter.current(), logic_error);
}

TEST_F(Interface, construct_with_level_and_controller)
{
  level_iter = LevelIter(0, controller);

  EXPECT_THAT(level_iter.get_level(), Eq(0));
  EXPECT_THROW(level_iter.current(), out_of_range);
}

TEST_F(Interface, current_level_can_be_set)
{
  ASSERT_THAT(level_iter.get_level(), Eq(0));

  level_iter.level() = 1;
  EXPECT_THAT(level_iter.get_level(), Eq(1));
}

TEST_F(Interface, order_inducing_comparison)
{
  auto other_level_iter = level_iter;

  EXPECT_TRUE(other_level_iter == level_iter);

  other_level_iter.level() = 1;
  EXPECT_TRUE(other_level_iter != level_iter);
  EXPECT_TRUE(other_level_iter >= level_iter);
  EXPECT_TRUE(other_level_iter >  level_iter);
  EXPECT_TRUE(level_iter <= other_level_iter);
  EXPECT_TRUE(level_iter <  other_level_iter);
}


class Iterating
  : public ::testing::Test
{
  protected:
    LevelIter level_iter;

    shared_ptr<Controller<double, VectorEncapsulation>> controller;
    shared_ptr<Sweeper<double, VectorEncapsulation>> sweeper_finest;
    shared_ptr<Sweeper<double, VectorEncapsulation>> sweeper_intermediate;
    shared_ptr<Sweeper<double, VectorEncapsulation>> sweeper_coarsest;

    virtual void SetUp()
    {
      this->controller = make_shared<Controller<double, VectorEncapsulation>>();
      this->sweeper_finest = make_shared<Sweeper<double, VectorEncapsulation>>();
      this->sweeper_intermediate = make_shared<Sweeper<double, VectorEncapsulation>>();
      this->sweeper_coarsest = make_shared<Sweeper<double, VectorEncapsulation>>();
      this->controller->add_sweeper(this->sweeper_finest);
      this->controller->add_sweeper(this->sweeper_intermediate);
      this->controller->add_sweeper(this->sweeper_coarsest);

      this->level_iter = LevelIter(1, this->controller);
    }
};

TEST_F(Iterating, access_to_finer)
{
  ASSERT_THAT(level_iter.current(), Eq(sweeper_intermediate));
  EXPECT_THAT(level_iter.finer(), Eq(sweeper_finest));
}

TEST_F(Iterating, access_to_coarser)
{
  ASSERT_THAT(level_iter.current(), Eq(sweeper_intermediate));
  EXPECT_THAT(level_iter.coarser(), Eq(sweeper_coarsest));
}

TEST_F(Iterating, iterate_to_finer)
{
  ASSERT_THAT(level_iter.current(), Eq(sweeper_intermediate));

  ++level_iter;
  EXPECT_THAT(level_iter.current(), Eq(sweeper_finest));
}

TEST_F(Iterating, iterate_to_coarser)
{
  ASSERT_THAT(level_iter.current(), Eq(sweeper_intermediate));

  --level_iter;
  EXPECT_THAT(level_iter.current(), Eq(sweeper_coarsest));
}

TEST_F(Iterating, skip_arbitrary_levels)
{
  level_iter.level() = 0;
  ASSERT_THAT(level_iter.current(), Eq(sweeper_coarsest));

  auto skipped = level_iter + 2;
  EXPECT_THAT(skipped.current(), Eq(sweeper_finest));

  auto skipped2 = skipped - 2;
  EXPECT_THAT(skipped2.current(), Eq(sweeper_coarsest));
}


TEST_MAIN()
