#include "pfasst/sweeper/interface.hpp"

#include <algorithm>
#include <cassert>
using namespace std;

#include "pfasst/exceptions.hpp"
#include "pfasst/logging.hpp"
#include "pfasst/quadrature.hpp"
using pfasst::quadrature::IQuadrature;


namespace pfasst
{
  template<typename precision, class EncapT>
  Sweeper<precision, EncapT>::Sweeper()
    :   _controller(nullptr)
      , _quadrature(nullptr)
      , _factory(make_shared<typename encap_type::factory_type>())
      , _initial_state(nullptr)
      , _states(0)
      , _previous_states(0)
      , _end_state(nullptr)
      , _tau(0)
      , _residuals(0)
      , _abs_residual_tol(0.0)
      , _rel_residual_tol(0.0)
  {}

  template<typename precision, class EncapT>
  shared_ptr<IQuadrature<precision>>&
  Sweeper<precision, EncapT>::quadrature()
  {
    return this->_quadrature;
  }

  template<typename precision, class EncapT>
  const shared_ptr<IQuadrature<precision>>
  Sweeper<precision, EncapT>::get_quadrature() const
  {
    return this->_quadrature;
  }

  template<typename precision, class EncapT>
  shared_ptr<typename EncapT::factory_type>&
  Sweeper<precision, EncapT>::encap_factory()
  {
    return this->_factory;
  }

  template<typename precision, class EncapT>
  const shared_ptr<typename EncapT::factory_type>
  Sweeper<precision, EncapT>::get_encap_factory() const
  {
    return this->_factory;
  }

  template<typename precision, class EncapT>
  shared_ptr<Controller<precision, EncapT>>&
  Sweeper<precision, EncapT>::controller()
  {
    return this->_controller;
  }

  template<typename precision, class EncapT>
  const shared_ptr<Controller<precision, EncapT>>
  Sweeper<precision, EncapT>::get_controller() const
  {
    return this->_controller;
  }

  template<typename precision, class EncapT>
  shared_ptr<EncapT>&
  Sweeper<precision, EncapT>::initial_state()
  {
    return this->_initial_state;
  }

  template<typename precision, class EncapT>
  const shared_ptr<EncapT>
  Sweeper<precision, EncapT>::get_initial_state() const
  {
    return this->_initial_state;
  }

  template<typename precision, class EncapT>
  vector<shared_ptr<EncapT>>&
  Sweeper<precision, EncapT>::states()
  {
    return this->_states;
  }

  template<typename precision, class EncapT>
  const vector<shared_ptr<EncapT>>
  Sweeper<precision, EncapT>::get_states() const
  {
    return this->_states;
  }

  template<typename precision, class EncapT>
  vector<shared_ptr<EncapT>>&
  Sweeper<precision, EncapT>::previous_states()
  {
    return this->_previous_states;
  }

  template<typename precision, class EncapT>
  const vector<shared_ptr<EncapT>>
  Sweeper<precision, EncapT>::get_previous_states() const
  {
    return this->_previous_states;
  }

  template<typename precision, class EncapT>
  shared_ptr<EncapT>&
  Sweeper<precision, EncapT>::end_state()
  {
    return this->_end_state;
  }

  template<typename precision, class EncapT>
  const shared_ptr<EncapT>
  Sweeper<precision, EncapT>::get_end_state() const
  {
    return this->_end_state;
  }

  template<typename precision, class EncapT>
  vector<shared_ptr<EncapT>>&
  Sweeper<precision, EncapT>::tau()
  {
    return this->_tau;
  }

  template<typename precision, class EncapT>
  const vector<shared_ptr<EncapT>>
  Sweeper<precision, EncapT>::get_tau() const
  {
    return this->_tau;
  }

  template<typename precision, class EncapT>
  vector<shared_ptr<EncapT>>&
  Sweeper<precision, EncapT>::residuals()
  {
    return this->_residuals;
  }

  template<typename precision, class EncapT>
  const vector<shared_ptr<EncapT>>
  Sweeper<precision, EncapT>::get_residuals() const
  {
    return this->_residuals;
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::set_options()
  {
    this->_abs_residual_tol = config::get_value<precision>("abs_res_tol", this->_abs_residual_tol);
    this->_rel_residual_tol = config::get_value<precision>("rel_res_tol", this->_rel_residual_tol);
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::set_abs_residual_tol(const precision& abs_res_tol)
  {
    this->_abs_residual_tol = abs_res_tol;
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::set_rel_residual_tol(const precision& rel_res_tol)
  {
    this->_rel_residual_tol = rel_res_tol;
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::setup()
  {
    if (this->get_quadrature() == nullptr) {
      throw runtime_error("Quadrature not yet set.");
    }
    assert(this->get_encap_factory() != nullptr);

    const auto nodes = this->get_quadrature()->get_nodes();
    const auto num_nodes = this->get_quadrature()->get_num_nodes();

    this->initial_state() = this->get_encap_factory()->create();

    this->states().resize(num_nodes);
    generate(this->states().begin(), this->states().end(),
             bind(&encap_type::factory_type::create, this->get_encap_factory()));

    this->previous_states().resize(num_nodes);
    generate(this->previous_states().begin(), this->previous_states().end(),
             bind(&encap_type::factory_type::create, this->get_encap_factory()));

    this->end_state() = this->get_encap_factory()->create();

    this->tau().resize(num_nodes);
    generate(this->tau().begin(), this->tau().end(),
             bind(&encap_type::factory_type::create, this->get_encap_factory()));

    // residuals have one more entry than nodes
    //   the additional is for the end state (which might be a copy of `_states.back()`)
    this->residuals().resize(num_nodes + 1);
    generate(this->residuals().begin(), this->residuals().end(),
             bind(&encap_type::factory_type::create, this->get_encap_factory()));
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::pre_predict()
  {}

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::predict()
  {
    throw NotImplementedYet("prediction logic for base sweeper");
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::post_predict()
  {
    this->integrate_end_state();
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::pre_sweep()
  {}

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::sweep()
  {
    throw NotImplementedYet("sweeping logic for base sweeper");
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::post_sweep()
  {
    this->integrate_end_state();
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::advance()
  {
    throw NotImplementedYet("advancing the sweeper to the next time step");
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::spread()
  {
    assert(this->get_initial_state() != nullptr);

    for_each(this->states().begin(), this->states().end(),
      [this](shared_ptr<encap_type> state)
      { state->data() = this->get_initial_state()->get_data(); });
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::save()
  {
    assert(this->get_quadrature() != nullptr);
    assert(this->get_states().size() == this->get_quadrature()->get_num_nodes());
    assert(this->get_previous_states().size() == this->get_states().size());

    for (size_t m = 0; m < this->get_quadrature()->get_num_nodes(); ++m) {
      this->previous_states()[m]->data() = this->get_states()[m]->get_data();
    }
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::reevaluate(const bool initial_only)
  {
    UNUSED(initial_only);
    throw NotImplementedYet("reevaluation of right-hand-side");
  }

  template<typename precision, class EncapT>
  bool
  Sweeper<precision, EncapT>::converged()
  {
    if (this->_abs_residual_tol > 0.0 || this->_rel_residual_tol > 0.0) {
      this->compute_residuals();
      const size_t num_residuals = this->get_residuals().size();
      vector<precision> abs_norms(num_residuals), rel_norms(num_residuals);
      transform(this->get_residuals().cbegin(), this->get_residuals().cend(),
                abs_norms.begin(),
                [](const shared_ptr<encap_type>& residual)
                { return residual->norm0(); });
      transform(this->get_residuals().cbegin(), this->get_residuals().cend(), abs_norms.cbegin(),
                rel_norms.begin(),
                [](const shared_ptr<encap_type>& residual, const precision& absnorm)
                { return absnorm / residual->norm0(); });
      return (   *(max_element(abs_norms.cbegin(), abs_norms.cend())) < this->_abs_residual_tol
              || *(max_element(rel_norms.cbegin(), rel_norms.cend())) < this->_rel_residual_tol);
    }
    return false;
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::integrate_end_state()
  {
    assert(this->get_quadrature() != nullptr);

    if (this->get_quadrature()->right_is_node()) {
      assert(this->end_state() != nullptr);
      assert(this->get_states().size() > 0);

      this->end_state()->data() = this->get_states().back()->get_data();
    } else {
      throw NotImplementedYet("integration of end state for quadrature not including right time point");
    }
  }

  template<typename precision, class EncapT>
  void
  Sweeper<precision, EncapT>::compute_residuals()
  {
    throw NotImplementedYet("computation of residuals");
  }
}  // ::pfasst
