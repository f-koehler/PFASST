#include "pfasst/sweeper/imex.hpp"

namespace pfasst
{
  template<typename precision, class EncapT>
  IMEX<precision, EncapT>::IMEX()
    :   Sweeper<precision, EncapT>()
      , _s_integrals(0)
      , _expl_rhs(0)
      , _impl_rhs(0)
  {}

  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::setup()
  {
    pfasst::Sweeper<precision, EncapT>::setup();
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

  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::pre_predict()
  {}

  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::predict()
  {}

  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::post_predict()
  {}

  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::pre_sweep()
  {}

  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::sweep()
  {}

  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::post_sweep()
  {}

  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::advance()
  {}

  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::reevaluate(const bool initial_only)
  {
    
  }


  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::integrate_end_state()
  {}

  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::compute_residuals()
  {}

  template<typename precision, class EncapT>
  shared_ptr<EncapT>
  IMEX<precision, EncapT>::evaluate_rhs_expl(const precision& t, const shared_ptr<EncapT> u)
  {}

  template<typename precision, class EncapT>
  shared_ptr<EncapT>
  IMEX<precision, EncapT>::evaluate_rhs_impl(const precision& t, const shared_ptr<EncapT> u)
  {}

  template<typename precision, class EncapT>
  void
  IMEX<precision, EncapT>::implicit_solve(shared_ptr<EncapT> f, shared_ptr<EncapT> u,
                                          const precision& t, const precision& dt,
                                          const shared_ptr<EncapT> rhs)
  {}
}  // ::pfasst
