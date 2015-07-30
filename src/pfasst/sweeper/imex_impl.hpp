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
  {}

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::reevaluate(const bool initial_only)
  {
    // TODO
  }


  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::integrate_end_state()
  {}

  template<class SweeperTrait, typename Enabled>
  void
  IMEX<SweeperTrait, Enabled>::compute_residuals()
  {}

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
