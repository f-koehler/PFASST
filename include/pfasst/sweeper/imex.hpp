#ifndef _PFASST__SWEEPER__IMEX_HPP_
#define _PFASST__SWEEPER__IMEX_HPP_

#include "pfasst/sweeper/interface.hpp"


namespace pfasst
{
  template<
    typename precision,
    class EncapT
  >
  class IMEX
    : public Sweeper<precision, EncapT>
  {
    public:
      typedef precision precision_type;
      typedef EncapT    encap_type;

    protected:
      vector<shared_ptr<encap_type>> _s_integrals;
      vector<shared_ptr<encap_type>> _expl_rhs;
      vector<shared_ptr<encap_type>> _impl_rhs;

      size_t num_expl_f_evals;
      size_t num_impl_f_evals;

      virtual void integrate_end_state();
      virtual void compute_residuals();

      virtual shared_ptr<EncapT> evaluate_rhs_expl(const precision& t,
                                                   const shared_ptr<EncapT> u);
      virtual shared_ptr<EncapT> evaluate_rhs_impl(const precision& t,
                                                   const shared_ptr<EncapT> u);

      virtual void implicit_solve(shared_ptr<EncapT> f, shared_ptr<EncapT> u,
                                  const precision& t, const precision& dt,
                                  const shared_ptr<EncapT> rhs);

    public:
      explicit IMEX();
      IMEX(const IMEX<precision, EncapT>& other) = default;
      IMEX(IMEX<precision, EncapT>&& other) = default;
      virtual ~IMEX() = default;
      IMEX<precision, EncapT>& operator=(const IMEX<precision, EncapT>& other) = default;
      IMEX<precision, EncapT>& operator=(IMEX<precision, EncapT>&& other) = default;

      virtual void setup();

      virtual void pre_predict();
      virtual void predict();
      virtual void post_predict();

      virtual void pre_sweep();
      virtual void sweep();
      virtual void post_sweep();

      virtual void advance();
      virtual void reevaluate(const bool initial_only=false);
  };
}  // ::pfasst

#include "pfasst/sweeper/imex_impl.hpp"

#endif  // _PFASST__SWEEPER__IMEX_HPP_
