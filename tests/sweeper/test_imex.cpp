#include "fixtures/test_helpers.hpp"

#include <pfasst/sweeper/imex.hpp>
using pfasst::IMEX;

#include <pfasst/encap/traits.hpp>
#include <pfasst/encap/vector.hpp>
typedef pfasst::vector_encap_traits<double, double> VectorEncapTrait;
typedef pfasst::encap::Encapsulation<VectorEncapTrait> VectorEncapsulation;

#include "quadrature/mocks.hpp"


typedef ::testing::Types<IMEX<double, VectorEncapsulation>> SweeperTypes;
INSTANTIATE_TYPED_TEST_CASE_P(IMEX, Concepts, SweeperTypes);


class Setup
  : public ::testing::Test
{
  protected:
    IMEX<double, VectorEncapsulation> sweeper;

    vector<double> nodes{0.0, 0.5, 1.0};
    shared_ptr<NiceMock<QuadratureMock<double>>> quadrature = make_shared<NiceMock<QuadratureMock<double>>>();

    virtual void SetUp()
    {
      ON_CALL(*(quadrature.get()), get_num_nodes()).WillByDefault(Return(nodes.size()));
      ON_CALL(*(quadrature.get()), get_nodes()).WillByDefault(ReturnRef(nodes));
    }
};

TEST_F(Setup, quadrature_is_required_for_setup)
{
  EXPECT_THAT(sweeper.quadrature(), IsNull());

  EXPECT_CALL(*(quadrature.get()), get_num_nodes()).Times(AtLeast(1));
  EXPECT_CALL(*(quadrature.get()), get_nodes()).Times(AtLeast(1));

  sweeper.quadrature() = quadrature;
  EXPECT_THAT(sweeper.quadrature(), NotNull());

  sweeper.setup();
}

TEST_F(Setup, state_data_initialized_after_setup)
{
  EXPECT_THAT(sweeper.initial_state(), IsNull());
  EXPECT_THAT(sweeper.get_end_state(), IsNull());
  EXPECT_THAT(sweeper.get_states(), IsEmpty());
  EXPECT_THAT(sweeper.get_previous_states(), IsEmpty());
  EXPECT_THAT(sweeper.tau(), IsEmpty());
  EXPECT_THAT(sweeper.get_residuals(), IsEmpty());

  sweeper.quadrature() = quadrature;
  auto num_nodes = quadrature->get_num_nodes();
  sweeper.setup();

  EXPECT_THAT(sweeper.get_initial_state(), NotNull());
  EXPECT_THAT(sweeper.get_end_state(), NotNull());

  EXPECT_THAT(sweeper.get_states(), SizeIs(num_nodes));
  EXPECT_THAT(sweeper.get_states(), Each(NotNull()));
  EXPECT_THAT(sweeper.get_states(), Not(MutuallyEqual()));

  EXPECT_THAT(sweeper.get_previous_states(), SizeIs(num_nodes));
  EXPECT_THAT(sweeper.get_previous_states(), Each(NotNull()));
  EXPECT_THAT(sweeper.get_previous_states(), Not(MutuallyEqual()));

  EXPECT_THAT(sweeper.get_tau(), SizeIs(num_nodes));
  EXPECT_THAT(sweeper.get_tau(), Each(NotNull()));
  EXPECT_THAT(sweeper.get_tau(), Not(MutuallyEqual()));

  EXPECT_THAT(sweeper.get_residuals(), SizeIs(num_nodes + 1));
  EXPECT_THAT(sweeper.get_residuals(), Each(NotNull()));
  EXPECT_THAT(sweeper.get_residuals(), Not(MutuallyEqual()));
}


class DataAccess
  : public ::testing::Test
{
  protected:
    typedef          IMEX<double, VectorEncapsulation> sweeper_type;
    typedef typename sweeper_type::encap_type          encap_type;

    sweeper_type sweeper;

    vector<double> nodes{0.0, 0.5, 1.0};
    shared_ptr<NiceMock<QuadratureMock<double>>> quadrature = make_shared<NiceMock<QuadratureMock<double>>>();

    shared_ptr<encap_type> encap = make_shared<encap_type>(vector<double>{1.0, 2.0, 3.0});

    virtual void SetUp()
    {
      sweeper.encap_factory()->set_size(3);
      ON_CALL(*(quadrature.get()), get_num_nodes()).WillByDefault(Return(nodes.size()));
      ON_CALL(*(quadrature.get()), get_nodes()).WillByDefault(ReturnRef(nodes));
      sweeper.quadrature() = quadrature;
      sweeper.setup();
    }
};

TEST_F(DataAccess, initial_state_for_modification)
{
  sweeper.initial_state() = encap;
  EXPECT_THAT(sweeper.initial_state()->data(), Pointwise(Eq(), encap->data()));

  *(sweeper.get_initial_state().get()) = vector<double>{1.0, 1.0, 1.0};
  EXPECT_THAT(sweeper.initial_state()->data(), Each(1.0));
}

TEST_F(DataAccess, tau_for_modification)
{
  sweeper.tau() = vector<shared_ptr<encap_type>>{encap, encap, encap};
  EXPECT_THAT(sweeper.get_tau(), Each(Eq(encap)));
}

TEST_F(DataAccess, states_after_spreading_initial_state)
{
  sweeper.initial_state() = encap;
  sweeper.spread();
  EXPECT_THAT(sweeper.get_initial_state(), Eq(encap));
  EXPECT_THAT(sweeper.get_initial_state()->data(), Pointwise(Eq(), encap->data()));
  EXPECT_THAT(sweeper.get_states(), Not(MutuallyEqual()));
  for (auto state : sweeper.get_states()) {
    EXPECT_THAT(state->data(), Pointwise(Eq(), encap->data()));
  }
}

TEST_F(DataAccess, previous_states_after_spreading_initial_state_and_saving)
{
  sweeper.initial_state() = encap;
  sweeper.spread();
  sweeper.save();
  EXPECT_THAT(sweeper.get_initial_state(), Eq(encap));
  EXPECT_THAT(sweeper.get_initial_state()->data(), Pointwise(Eq(), encap->data()));
  EXPECT_THAT(sweeper.get_previous_states(), Not(MutuallyEqual()));
  for (auto state : sweeper.get_previous_states()) {
    EXPECT_THAT(state->data(), Pointwise(Eq(), encap->data()));
  }
}


TEST_MAIN()
