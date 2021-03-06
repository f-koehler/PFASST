#include "fixtures/test_helpers.hpp"

#include <stdexcept>
using namespace std;

#include <pfasst/transfer/transfer.hpp>
using pfasst::Transfer;

#include <pfasst/encap/traits.hpp>
#include <pfasst/encap/vector.hpp>
typedef pfasst::vector_encap_traits<double, double> VectorEncapTrait;

// #include "sweeper/mocks.hpp"
// typedef SweeperMock<double, VectorEncapsulation> Sweeper;
#include "pfasst/sweeper/sweeper.hpp"
typedef pfasst::Sweeper<pfasst::sweeper_traits<VectorEncapTrait>> Sweeper;

typedef ::testing::Types<Transfer<pfasst::transfer_traits<Sweeper, Sweeper, 2>>> TransferTypes;
INSTANTIATE_TYPED_TEST_CASE_P(Transfer, Concepts, TransferTypes);


class Interface
  : public ::testing::Test
{
  protected:
    typedef Transfer<pfasst::transfer_traits<Sweeper, Sweeper, 2>> transfer_type;
    typedef pfasst::encap::VectorEncapsulation<double, double>     encap_type;

    transfer_type          transfer;
    shared_ptr<Sweeper>    coarse_sweeper;
    shared_ptr<Sweeper>    fine_sweeper;
    shared_ptr<encap_type> coarse_encap;
    shared_ptr<encap_type> fine_encap;
};

TEST_F(Interface, no_implementation_of_interpolation_of_initial_value)
{
  EXPECT_THROW(transfer.interpolate_initial(coarse_sweeper, fine_sweeper), runtime_error);
}

TEST_F(Interface, no_implementation_of_interpolation)
{
  EXPECT_THROW(transfer.interpolate(coarse_sweeper, fine_sweeper), runtime_error);
  EXPECT_THROW(transfer.interpolate(coarse_sweeper, fine_sweeper, true), runtime_error);
  EXPECT_THROW(transfer.interpolate(coarse_sweeper, fine_sweeper, false), runtime_error);
}

TEST_F(Interface, no_implementation_of_interpolating_encaps)
{
  EXPECT_THROW(transfer.interpolate_data(coarse_encap, fine_encap), runtime_error);
}

TEST_F(Interface, no_implementation_of_restriction_of_initial_value)
{
  EXPECT_THROW(transfer.restrict_initial(coarse_sweeper, fine_sweeper), runtime_error);
}

TEST_F(Interface, no_implementation_of_restriction)
{
  EXPECT_THROW(transfer.restrict(coarse_sweeper, fine_sweeper), runtime_error);
  EXPECT_THROW(transfer.restrict(coarse_sweeper, fine_sweeper, true), runtime_error);
  EXPECT_THROW(transfer.restrict(coarse_sweeper, fine_sweeper, false), runtime_error);
}

TEST_F(Interface, no_implementation_of_restricting_encaps)
{
  EXPECT_THROW(transfer.restrict_data(coarse_encap, fine_encap), runtime_error);
}

TEST_F(Interface, no_implementation_of_fas_correction)
{
  EXPECT_THROW(transfer.fas(1.0, fine_sweeper, coarse_sweeper), runtime_error);
}


TEST_MAIN()
