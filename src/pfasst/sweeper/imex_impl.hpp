#include "pfasst/sweeper/imex.hpp"

namespace pfasst
{
  template<class SweeperTrait, typename Enabled>
  IMEX<SweeperTrait, Enabled>::IMEX()
    :   Sweeper<SweeperTrait, Enabled>()
      , _s_integrals(0)
      , _expl_rhs(0)
      , _impl_rhs(0)
  {}

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::setup()
  {
    pfasst::Sweeper<SweeperTrait, Enabled>::setup();
    const auto num_nodes = this->quadrature()->get_num_nodes();

    this->_s_integrals.resize(num_nodes);
    generate(this->_s_integrals.begin(), this->_s_integrals.end(),
             bind(&encap_type::factory_type::create, this->encap_factory()));

    this->_expl_rhs.resize(num_nodes);
    generate(this->_expl_rhs.begin(), this->_expl_rhs.end(),
             bind(&encap_type::factory_type::create, this->encap_factory()));

    this->_impl_rhs.resize(num_nodes);
    generate(this->_impl_rhs.begin(), this->_impl_rhs.end(),
             bind(&encap_type::factory_type::create, this->encap_factory()));
  }

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::pre_predict()
  {}

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::predict()
  {}

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::post_predict()
  {}

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::pre_sweep()
  {}

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::sweep()
  {}

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::post_sweep()
  {}

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::advance()
  {
    this->initial_state() = this->get_end_state();

    if (this->get_quadrature()->left_is_node() && this->get_quadrature()->right_is_node()) {
      this->_expl_rhs.front()->data() = this->_expl_rhs.back()->get_data();
      this->_impl_rhs.front()->data() = this->_impl_rhs.back()->get_data();
    } else {
      // TODO: this might not be necessary as it is probably dealt with in pre_predict and pre_sweep
      throw NotImplementedYet("advancing IMEX for nodes not containing left and right interval borders");
    }
  }

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::reevaluate(const bool initial_only)
  {
    const time_type t0 = this->get_status()->get_time();

    if (initial_only) {
      if (this->get_quadrature()->left_is_node()) {
        this->_expl_rhs.front() = this->evaluate_rhs_expl(t0, this->_expl_rhs.front());
        this->_impl_rhs.front() = this->evaluate_rhs_impl(t0, this->_impl_rhs.front());
      } else {
        throw NotImplementedYet("reevaluation for quadrature not containing left interval boundary");
      }
    } else {
      const time_type dt = this->get_status()->get_dt();
      const auto nodes = this->get_quadrature()->get_nodes();
      for (size_t m = 0; this->get_quadrature()->get_num_nodes(); ++m) {
        const time_type t = t0 + dt * nodes[m];
        this->_expl_rhs[m] = this->evaluate_rhs_expl(t, this->_expl_rhs[m]);
        this->_impl_rhs[m] = this->evaluate_rhs_impl(t, this->_impl_rhs[m]);
      }
    }
  }


  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::integrate_end_state(const typename SweeperTrait::time_type& dt)
  {
    try {
      Sweeper<SweeperTrait, Enabled>::integrate_end_state(dt);
    } catch (NotImplementedYet niy) {
      if (this->get_quadrature()->right_is_node()) {
        this->end_state() = this->get_initial_state();
        this->end_state()->scale_add(1.0, encap::mat_mul_vec(dt, this->get_quadrature()->get_b_mat(), this->_expl_rhs)[0]);
        this->end_state()->scale_add(1.0, encap::mat_mul_vec(dt, this->get_quadrature()->get_b_mat(), this->_impl_rhs)[0]);
      } else {
        throw niy;
      }
    }
  }

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::compute_residuals()
  {
    const time_type& dt = this->get_status()->get_dt();

    for (size_t m = 0; this->get_quadrature()->get_num_nodes(); ++m) {
      this->residuals()[m] = this->get_initial_state();
      this->residuals()[m]->scale_add(-1.0, this->get_states()[m]);

      for (size_t n = 0; n <= m; ++n) {
        this->residuals()[m]->scale_add(1.0, this->get_tau()[n]);
      }
    }

    encap::mat_apply(this->residuals(), dt, this->get_quadrature()->get_q_mat(), this->_expl_rhs, false);
    encap::mat_apply(this->residuals(), dt, this->get_quadrature()->get_q_mat(), this->_impl_rhs, false);
  }

  template<class SweeperTrait, typename Enabled>
  shared_ptr<typename SweeperTrait::encap_type>
  IMEX<SweeperTrait, Enabled>::evaluate_rhs_expl(const typename SweeperTrait::time_type& t,
                                                 const shared_ptr<typename SweeperTrait::encap_type> u)
  {}

  template<class SweeperTrait, typename Enabled>
  shared_ptr<typename SweeperTrait::encap_type>
  IMEX<SweeperTrait, Enabled>::evaluate_rhs_impl(const typename SweeperTrait::time_type& t,
                                                 const shared_ptr<typename SweeperTrait::encap_type> u)
  {}

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::implicit_solve(shared_ptr<typename SweeperTrait::encap_type> f,
                                              shared_ptr<typename SweeperTrait::encap_type> u,
                                              const typename SweeperTrait::time_type& t,
                                              const typename SweeperTrait::time_type& dt,
                                              const shared_ptr<typename SweeperTrait::encap_type> rhs)
  {}
}  // ::pfasst
