#ifndef _PFASST__EXAMPLES__QMOSC1D__QMOSC1d_SWEEPER_HPP_
#define _PFASST__EXAMPLES__QMOSC1D__QMOSC1d_SWEEPER_HPP_

#include <complex>
#include <memory>
#include <vector>
using namespace std;

#include <pfasst/sweeper/imex.hpp>
#include <pfasst/contrib/fft.hpp>

// I'd really like to have these as static const variable templates but this is only possible since C++14 ... :-(
#define DEFAULT_EPSILON 1.


namespace pfasst
{
  namespace examples
  {
    namespace qmosc1d
    {
      template<
        class SweeperTrait,
        typename Enabled = void
      >
      class QmOsc1d
        : public IMEX<SweeperTrait, Enabled>
      {
        /* static_assert(is_same< */
        /*                 vector<typename SweeperTrait::time_type>, */
        /*                 typename SweeperTrait::encap_type::data_type */
        /*               >::value, */
        /*               "QmOsc1d Sweeper requires encapsulated vectors"); */

        public:
          typedef          SweeperTrait         traits;
          typedef typename traits::encap_type   encap_type;
          typedef typename traits::time_type    time_type;
          typedef typename traits::spatial_type spatial_type;

          static void init_opts();

        private:
          time_type _t0;
          time_type _epsilon;

          pfasst::contrib::FFT<spatial_type> _fft;
          vector<complex<spatial_type>>      _ddx;
          vector<complex<spatial_type>>      _lap;

        protected:
          virtual shared_ptr<typename SweeperTrait::encap_type> evaluate_rhs_expl(const typename SweeperTrait::time_type& t,
                                                                                  const shared_ptr<typename SweeperTrait::encap_type> u) override;
          virtual shared_ptr<typename SweeperTrait::encap_type> evaluate_rhs_impl(const typename SweeperTrait::time_type& t,
                                                                                  const shared_ptr<typename SweeperTrait::encap_type> u) override;

          virtual void implicit_solve(shared_ptr<typename SweeperTrait::encap_type> f,
                                      shared_ptr<typename SweeperTrait::encap_type> u,
                                      const typename SweeperTrait::time_type& t,
                                      const typename SweeperTrait::time_type& dt,
                                      const shared_ptr<typename SweeperTrait::encap_type> rhs) override;

          virtual vector<shared_ptr<typename SweeperTrait::encap_type>> compute_error(const typename SweeperTrait::time_type& t);
          virtual vector<shared_ptr<typename SweeperTrait::encap_type>> compute_relative_error(const vector<shared_ptr<typename SweeperTrait::encap_type>>& error, const typename SweeperTrait::time_type& t);

        public:
          explicit QmOsc1d(const size_t& ndofs, const typename SweeperTrait::time_type& epsilon = DEFAULT_EPSILON);
          QmOsc1d(const QmOsc1d<SweeperTrait, Enabled>& other) = default;
          QmOsc1d(QmOsc1d<SweeperTrait, Enabled>&& other) = default;
          virtual ~QmOsc1d() = default;
          QmOsc1d<SweeperTrait, Enabled>& operator=(const QmOsc1d<SweeperTrait, Enabled>& other) = default;
          QmOsc1d<SweeperTrait, Enabled>& operator=(QmOsc1d<SweeperTrait, Enabled>&& other) = default;

          virtual void set_options() override;

          virtual shared_ptr<typename SweeperTrait::encap_type> exact(const typename SweeperTrait::time_type& t);

          virtual void post_step() override;

          virtual bool converged() override;

          size_t get_num_dofs() const;
      };
    }  // ::pfasst::examples::qmosc1d
  }  // ::pfasst::examples
}  // ::pfasst

#include "qmosc1d_sweeper_impl.hpp"

#endif  // _PFASST__EXAMPLES__QMOSC__QMOSC_SWEEPER_HPP_
