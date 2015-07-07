#include "fixtures/test_helpers.hpp"

#include <pfasst/controller/interface.hpp>
using pfasst::Controller;

#include <pfasst/encap/vector.hpp>
using pfasst::encap::VectorEncapsulation;
#include <pfasst/sweeper/interface.hpp>
using pfasst::Sweeper;

#include "comm/mocks.hpp"
#include "controller/mocks.hpp"
#include "sweeper/mocks.hpp"


typedef ::testing::Types<Controller<double, VectorEncapsulation<double>>> ControllerTypes;
INSTANTIATE_TYPED_TEST_CASE_P(Controller, Concepts, ControllerTypes);


class Interface
  : public ::testing::Test
{
  protected:
    shared_ptr<Controller<double, VectorEncapsulation<double>>> controller;

    shared_ptr<StatusMock<double>> status;
    shared_ptr<CommMock> comm;

    virtual void SetUp()
    {
      this->controller = make_shared<Controller<double, VectorEncapsulation<double>>>();
      this->status = make_shared<StatusMock<double>>();
      this->comm = make_shared<CommMock>();
    }
};

TEST_F(Interface, has_a_status)
{
  EXPECT_THAT(controller->get_status(), NotNull());
}

TEST_F(Interface, status_can_not_be_assigned)
{
  controller->status() = status;
  EXPECT_THAT(controller->get_status(), Not(Eq(status)));
}

TEST_F(Interface, status_can_be_modified)
{
  controller->status()->time() = 42.0;
  EXPECT_THAT(controller->get_status()->get_time(), Eq(42.0));
}

TEST_F(Interface, has_no_communicator_after_instantiation)
{
  EXPECT_THAT(controller->get_communicator(), IsNull());
}

TEST_F(Interface, communicator_can_be_assigned)
{
  controller->communicator() = comm;
  EXPECT_THAT(controller->get_communicator(), Eq(comm));
}

TEST_F(Interface, has_time_end_point)
{
  EXPECT_THAT(controller->get_t_end(), Eq(0.0));

  controller->t_end() = 42.1;
  EXPECT_THAT(controller->get_t_end(), Eq(42.1));
}

TEST_F(Interface, has_max_iterations_threshold)
{
  EXPECT_THAT(controller->get_max_iterations(), Eq(0));

  controller->max_iterations() = 5;
  EXPECT_THAT(controller->get_max_iterations(), Eq(5));
}

TEST_F(Interface, computes_number_steps_fails_if_tend_or_dt_not_set)
{
  EXPECT_THROW(controller->get_num_steps(), logic_error);

  controller->t_end() = 4.2;
  EXPECT_THROW(controller->get_num_steps(), logic_error);
}

TEST_F(Interface, computes_number_steps)
{
  controller->t_end() = 4.2;
  controller->status()->dt() = 0.1;
  EXPECT_THAT(controller->get_num_steps(), Eq(42));
}


class Setup
  : public ::testing::Test
{
  protected:
    shared_ptr<Controller<double, VectorEncapsulation<double>>> controller;

    shared_ptr<StatusMock<double>> status;
    shared_ptr<CommMock> comm;
    shared_ptr<Sweeper<double, VectorEncapsulation<double>>> sweeper1;
    shared_ptr<Sweeper<double, VectorEncapsulation<double>>> sweeper2;

    virtual void SetUp()
    {
      this->controller = make_shared<Controller<double, VectorEncapsulation<double>>>();
      this->status = make_shared<StatusMock<double>>();
      this->comm = make_shared<CommMock>();
      this->sweeper1 = make_shared<Sweeper<double, VectorEncapsulation<double>>>();
      this->sweeper2 = make_shared<Sweeper<double, VectorEncapsulation<double>>>();
    }
};

TEST_F(Setup, adding_level_adds_backref_to_controller_in_sweeper)
{
  ASSERT_THAT(sweeper1->get_controller(), Not(Eq(controller)));

  controller->add_sweeper(sweeper1);
  EXPECT_THAT(sweeper1->get_controller(), Eq(controller));
}

TEST_F(Setup, adding_coarser_level)
{
  ASSERT_THAT(controller->get_num_levels(), Eq(0));

  controller->add_sweeper(sweeper1);
  EXPECT_THAT(controller->get_num_levels(), Eq(1));

  controller->add_sweeper(sweeper2);
  EXPECT_THAT(controller->get_num_levels(), Eq(2));
}

TEST_F(Setup, adding_finer_level)
{
  ASSERT_THAT(controller->get_num_levels(), Eq(0));

  controller->add_sweeper(sweeper1);
  EXPECT_THAT(controller->get_num_levels(), Eq(1));

  controller->add_sweeper(sweeper2, false);
  EXPECT_THAT(controller->get_num_levels(), Eq(2));
}

TEST_F(Setup, at_least_one_level_must_be_added)
{
  controller->t_end() = 4.2;
  controller->status()->dt() = 0.1;
  controller->max_iterations() = 1;

  EXPECT_THROW(controller->setup(), logic_error);

  controller->add_sweeper(sweeper1);
  controller->setup();
}

TEST_F(Setup, setup_required_for_running)
{
  controller->t_end() = 4.2;
  controller->status()->dt() = 0.1;
  controller->max_iterations() = 1;
  controller->add_sweeper(sweeper1);

  ASSERT_FALSE(controller->is_ready());
  EXPECT_THROW(controller->run(), logic_error);

  controller->setup();
  EXPECT_TRUE(controller->is_ready());
  controller->run();
}


class Logic
  : public ::testing::Test
{
  protected:
    shared_ptr<Controller<double, VectorEncapsulation<double>>> controller;

    shared_ptr<StatusMock<double>> status;
    shared_ptr<CommMock> comm;
    shared_ptr<Sweeper<double, VectorEncapsulation<double>>> sweeper1;
    shared_ptr<Sweeper<double, VectorEncapsulation<double>>> sweeper2;

    virtual void SetUp()
    {
      this->controller = make_shared<Controller<double, VectorEncapsulation<double>>>();
      this->status = make_shared<StatusMock<double>>();
      this->comm = make_shared<CommMock>();
      this->sweeper1 = make_shared<Sweeper<double, VectorEncapsulation<double>>>();
      this->sweeper2 = make_shared<Sweeper<double, VectorEncapsulation<double>>>();
    }
};

TEST_F(Logic, advance_in_time_with_sufficient_t_end)
{
  controller->status()->dt() = 0.1;
  controller->status()->time() = 1.0;
  controller->status()->step() = 1;
  controller->t_end() = 2.0;

  EXPECT_TRUE(controller->advance_time());
  EXPECT_THAT(controller->get_status()->get_time(), Eq(1.1));
  EXPECT_THAT(controller->get_status()->get_step(), Eq(2));
}

TEST_F(Logic, advance_in_time_with_insufficient_t_end)
{
  controller->status()->dt() = 0.1;
  controller->status()->time() = 1.0;
  controller->status()->step() = 1;
  controller->t_end() = 1.0;

  EXPECT_FALSE(controller->advance_time());
  EXPECT_THAT(controller->get_status()->get_time(), Eq(1.0));
  EXPECT_THAT(controller->get_status()->get_step(), Eq(1));
}

TEST_F(Logic, advance_in_time_multiple_steps_at_once)
{
  controller->status()->dt() = 0.1;
  controller->status()->time() = 1.0;
  controller->status()->step() = 1;
  controller->t_end() = 2.0;

  EXPECT_TRUE(controller->advance_time(3));
  EXPECT_THAT(controller->get_status()->get_time(), Eq(1.3));
  EXPECT_THAT(controller->get_status()->get_step(), Eq(4));
}


TEST_F(Logic, advance_iteration_with_exceeding_max_iteration_threshold)
{
  controller->status()->iteration() = 1;
  controller->max_iterations() = 1;
  ASSERT_THAT(controller->get_status()->get_iteration(), Eq(1));
  ASSERT_THAT(controller->get_max_iterations(), Eq(1));

  EXPECT_FALSE(controller->advance_iteration());
  EXPECT_THAT(controller->get_status()->get_iteration(), Eq(1));
}

TEST_F(Logic, advance_iteration)
{
  controller->status()->iteration() = 1;
  controller->max_iterations() = 5;
  ASSERT_THAT(controller->get_status()->get_iteration(), Eq(1));
  ASSERT_THAT(controller->get_max_iterations(), Eq(5));

  EXPECT_TRUE(controller->advance_iteration());
  EXPECT_THAT(controller->get_status()->get_iteration(), Eq(2));
}


TEST_MAIN()