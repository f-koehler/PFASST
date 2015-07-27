#include "fixtures/test_helpers.hpp"

#include <pfasst/transfer/interface.hpp>
using pfasst::Transfer;

#include <pfasst/encap/traits.hpp>
#include <pfasst/encap/vector.hpp>
typedef pfasst::vector_encap_traits<double, double> VectorEncapTrait;
typedef pfasst::encap::Encapsulation<VectorEncapTrait> VectorEncapsulation;

#include "sweeper/mocks.hpp"
typedef SweeperMock<double, VectorEncapsulation> Sweeper;

typedef ::testing::Types<Transfer<Sweeper, Sweeper>> TransferTypes;
INSTANTIATE_TYPED_TEST_CASE_P(Transfer, Concepts, TransferTypes);


class Interface
  : public ::testing::Test
{
  protected:
    typedef Transfer<Sweeper, Sweeper> transfer_type;
    typedef VectorEncapsulation        encap_type;

    transfer_type                   transfer;
    shared_ptr<Sweeper>             coarse_sweeper;
    shared_ptr<Sweeper>             fine_sweeper;
    shared_ptr<VectorEncapsulation> coarse_encap;
    shared_ptr<VectorEncapsulation> fine_encap;
};

TEST_F(Interface, no_implementation_of_interpolation_of_initial_value)
{
  EXPECT_THROW(transfer.interpolate_initial(coarse_sweeper, fine_sweeper), pfasst::NotImplementedYet);
}

TEST_F(Interface, no_implementation_of_interpolation)
{
  EXPECT_THROW(transfer.interpolate(coarse_sweeper, fine_sweeper), pfasst::NotImplementedYet);
  EXPECT_THROW(transfer.interpolate(coarse_sweeper, fine_sweeper, true), pfasst::NotImplementedYet);
  EXPECT_THROW(transfer.interpolate(coarse_sweeper, fine_sweeper, false), pfasst::NotImplementedYet);
}

TEST_F(Interface, no_implementation_of_interpolating_encaps)
{
  EXPECT_THROW(transfer.interpolate(coarse_encap, fine_encap), pfasst::NotImplementedYet);
}

TEST_F(Interface, no_implementation_of_restriction_of_initial_value)
{
  EXPECT_THROW(transfer.restrict_initial(coarse_sweeper, fine_sweeper), pfasst::NotImplementedYet);
}

TEST_F(Interface, no_implementation_of_restriction)
{
  EXPECT_THROW(transfer.restrict(coarse_sweeper, fine_sweeper), pfasst::NotImplementedYet);
  EXPECT_THROW(transfer.restrict(coarse_sweeper, fine_sweeper, true), pfasst::NotImplementedYet);
  EXPECT_THROW(transfer.restrict(coarse_sweeper, fine_sweeper, false), pfasst::NotImplementedYet);
}

TEST_F(Interface, no_implementation_of_restricting_encaps)
{
  EXPECT_THROW(transfer.restrict(coarse_encap, fine_encap), pfasst::NotImplementedYet);
}

TEST_F(Interface, no_implementation_of_fas_correction)
{
  EXPECT_THROW(transfer.fas(1.0, fine_sweeper, coarse_sweeper), pfasst::NotImplementedYet);
}


TEST_MAIN()
