#include "fixtures/test_helpers.hpp"

#include <pfasst/sweeper/imex.hpp>
using pfasst::IMEX;

#include <pfasst/encap/traits.hpp>
#include <pfasst/encap/vector.hpp>
typedef pfasst::vector_encap_traits<double, double> VectorEncapTrait;
typedef pfasst::encap::Encapsulation<VectorEncapTrait> VectorEncapsulation;

#include "quadrature/mocks.hpp"
#include "controller/mocks.hpp"


typedef ::testing::Types<IMEX<pfasst::sweeper_traits<VectorEncapTrait>>> SweeperTypes;
INSTANTIATE_TYPED_TEST_CASE_P(IMEX, Concepts, SweeperTypes);


class Setup
  : public ::testing::Test
{
  protected:
    IMEX<pfasst::sweeper_traits<VectorEncapTrait>> sweeper;

    vector<double> nodes{0.0, 0.5, 1.0};
    shared_ptr<NiceMock<QuadratureMock<double>>> quadrature = make_shared<NiceMock<QuadratureMock<double>>>();
    shared_ptr<NiceMock<StatusMock<double>>>     status = make_shared<NiceMock<StatusMock<double>>>();

    virtual void SetUp()
    {
      ON_CALL(*(quadrature.get()), get_num_nodes()).WillByDefault(Return(nodes.size()));
      ON_CALL(*(quadrature.get()), get_nodes()).WillByDefault(ReturnRef(nodes));
    }
};

TEST_F(Setup, quadrature_is_required_for_setup)
{
  ASSERT_THAT(sweeper.quadrature(), IsNull());
  ASSERT_THAT(sweeper.get_quadrature(), IsNull());

  ASSERT_THAT(sweeper.status(), IsNull());
  ASSERT_THAT(sweeper.get_status(), IsNull());

  EXPECT_THROW(sweeper.setup(), runtime_error);

  sweeper.quadrature() = quadrature;
  EXPECT_THAT(sweeper.quadrature(), NotNull());
  EXPECT_THAT(sweeper.get_quadrature(), NotNull());

  sweeper.status() = status;
  EXPECT_THAT(sweeper.status(), NotNull());
  EXPECT_THAT(sweeper.get_status(), NotNull());

  sweeper.setup();
}

TEST_F(Setup, state_data_initialized_after_setup)
{
  EXPECT_THROW(sweeper.get_initial_state(), runtime_error);
  EXPECT_THROW(sweeper.initial_state(), runtime_error);
  EXPECT_THAT(sweeper.get_end_state(), IsNull());
  EXPECT_THAT(sweeper.get_states(), IsEmpty());
  EXPECT_THAT(sweeper.get_previous_states(), IsEmpty());
  EXPECT_THAT(sweeper.tau(), IsEmpty());
  EXPECT_THAT(sweeper.get_residuals(), IsEmpty());

  sweeper.status() = status;
  sweeper.quadrature() = quadrature;
  auto num_nodes = quadrature->get_num_nodes();
  sweeper.setup();

  EXPECT_THAT(sweeper.get_initial_state(), NotNull());
  EXPECT_THAT(sweeper.get_end_state(), NotNull());

  EXPECT_THAT(sweeper.get_states(), SizeIs(num_nodes + 1));
  EXPECT_THAT(sweeper.get_states(), Each(NotNull()));
  EXPECT_THAT(sweeper.get_states(), Not(MutuallyEqual()));

  EXPECT_THAT(sweeper.get_previous_states(), SizeIs(num_nodes + 1));
  EXPECT_THAT(sweeper.get_previous_states(), Each(NotNull()));
  EXPECT_THAT(sweeper.get_previous_states(), Not(MutuallyEqual()));

  EXPECT_THAT(sweeper.get_tau(), SizeIs(num_nodes + 1));
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
    typedef          IMEX<pfasst::sweeper_traits<VectorEncapTrait>> sweeper_type;
    typedef typename sweeper_type::encap_type                       encap_type;

    sweeper_type sweeper;

    vector<double> nodes{0.0, 0.5, 1.0};
    shared_ptr<NiceMock<QuadratureMock<double>>> quadrature = make_shared<NiceMock<QuadratureMock<double>>>();
    shared_ptr<encap_type>                       encap = make_shared<encap_type>(vector<double>{1.0, 2.0, 3.0});
    shared_ptr<NiceMock<StatusMock<double>>>     status = make_shared<NiceMock<StatusMock<double>>>();

    virtual void SetUp()
    {
      sweeper.encap_factory()->set_size(3);
      ON_CALL(*(quadrature.get()), get_num_nodes()).WillByDefault(Return(nodes.size()));
      ON_CALL(*(quadrature.get()), get_nodes()).WillByDefault(ReturnRef(nodes));
      sweeper.quadrature() = quadrature;
      sweeper.status() = status;
      sweeper.setup();
    }
};

TEST_F(DataAccess, initial_state_for_modification)
{
  sweeper.initial_state() = encap;
  EXPECT_THAT(sweeper.get_initial_state()->data(), Pointwise(Eq(), encap->data()));

  *(sweeper.get_initial_state().get()) = vector<double>{1.0, 1.0, 1.0};
  EXPECT_THAT(sweeper.get_initial_state()->data(), Each(1.0));
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


class Logic
  : public ::testing::Test
{
  protected:
    typedef          IMEX<pfasst::sweeper_traits<VectorEncapTrait>> sweeper_type;
    typedef typename sweeper_type::encap_type                       encap_type;

    sweeper_type sweeper;

    vector<double> nodes{0.0, 0.5, 1.0};
    shared_ptr<NiceMock<QuadratureMock<double>>> quadrature = make_shared<NiceMock<QuadratureMock<double>>>();
    shared_ptr<NiceMock<StatusMock<double>>> status = make_shared<NiceMock<StatusMock<double>>>();

    shared_ptr<encap_type> encap = make_shared<encap_type>(vector<double>{1.0, 2.0, 3.0});
    Matrix<double> b_mat = Matrix<double>::Zero(1, 4);

    virtual void SetUp()
    {
      sweeper.encap_factory()->set_size(3);
      ON_CALL(*(quadrature.get()), get_num_nodes()).WillByDefault(Return(nodes.size()));
      ON_CALL(*(quadrature.get()), get_nodes()).WillByDefault(ReturnRef(nodes));

      for(size_t n = 1; n < 4; ++n) {
        b_mat(0, n) = 1.0 / 3.0;
      }
      ON_CALL(*(quadrature.get()), get_b_mat()).WillByDefault(ReturnRef(b_mat));
      sweeper.quadrature() = quadrature;
      ON_CALL(*(status.get()), get_dt()).WillByDefault(Return(1.0));
      sweeper.status() = status;
      sweeper.setup();

    }
};

TEST_F(Logic, pre_predict_copies_initial_state_if_left_is_node)
{
  ON_CALL(*(quadrature.get()), left_is_node()).WillByDefault(Return(true));

  sweeper.initial_state() = encap;
  sweeper.pre_predict();

  EXPECT_THAT(sweeper.get_states().front()->data(), Pointwise(Eq(), encap->data()));
}

TEST_F(Logic, DISABLED_pre_predict_failes_initial_state_if_left_is_node)
{
  ON_CALL(*(quadrature.get()), left_is_node()).WillByDefault(Return(false));

  sweeper.initial_state() = encap;
  sweeper.pre_predict();
}

TEST_F(Logic, post_predict_finishes_end_state_if_right_is_node)
{
  ON_CALL(*(quadrature.get()), right_is_node()).WillByDefault(Return(true));

  sweeper.initial_state() = encap;
  sweeper.spread();

  EXPECT_THAT(sweeper.get_end_state(), NotNull());
  EXPECT_THAT(sweeper.get_end_state()->data(), Pointwise(Not(Eq()), encap->data()));

  sweeper.post_predict();
  EXPECT_THAT(sweeper.get_end_state()->data(), Pointwise(Eq(), encap->data()));
}

TEST_F(Logic, post_predict_finishes_end_state_if_right_is_not_node)
{
  ON_CALL(*(quadrature.get()), right_is_node()).WillByDefault(Return(false));

  sweeper.initial_state() = encap;
  sweeper.spread();

  EXPECT_THAT(sweeper.get_end_state(), NotNull());
  EXPECT_THAT(sweeper.get_end_state()->data(), Pointwise(Not(Eq()), encap->data()));

  sweeper.post_predict();
  EXPECT_THAT(sweeper.get_end_state()->data(), Pointwise(Eq(), encap->data()));
}

TEST_F(Logic, post_sweep_finishes_end_state_if_right_is_node)
{
  ON_CALL(*(quadrature.get()), right_is_node()).WillByDefault(Return(true));

  sweeper.initial_state() = encap;
  sweeper.spread();

  EXPECT_THAT(sweeper.get_end_state(), NotNull());
  EXPECT_THAT(sweeper.get_end_state()->data(), Pointwise(Not(Eq()), encap->data()));

  sweeper.post_sweep();
  EXPECT_THAT(sweeper.get_end_state()->data(), Pointwise(Eq(), encap->data()));
}

TEST_F(Logic, post_sweep_finishes_end_state_if_right_is_not_node)
{
  ON_CALL(*(quadrature.get()), right_is_node()).WillByDefault(Return(false));

  sweeper.initial_state() = encap;
  sweeper.spread();

  EXPECT_THAT(sweeper.get_end_state(), NotNull());
  EXPECT_THAT(sweeper.get_end_state()->data(), Pointwise(Not(Eq()), encap->data()));

  sweeper.post_sweep();
  EXPECT_THAT(sweeper.get_end_state()->data(), Pointwise(Eq(), encap->data()));
}


TEST_MAIN()
