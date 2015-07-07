#ifndef _PFASST__SWEEPER__INTERFACE_HPP_
#define _PFASST__SWEEPER__INTERFACE_HPP_

#include <memory>
#include <type_traits>
#include <vector>
using namespace std;

#include "pfasst/comm/interface.hpp"
#include "pfasst/controller/interface.hpp"
#include "pfasst/encap/interface.hpp"
#include "pfasst/quadrature.hpp"
using pfasst::quadrature::IQuadrature;


namespace pfasst
{
  template<
    typename precision,
    class EncapT
  >
  class Sweeper
    : public enable_shared_from_this<Sweeper<precision, EncapT>>
  {
    static_assert(is_arithmetic<precision>::value,
                  "precision type must be arithmetic");
    static_assert(is_constructible<EncapT>::value,
                  "data type must be constructible");
    static_assert(is_destructible<EncapT>::value,
                  "data type must be destructible");

    public:
      typedef precision   precision_type;
      typedef EncapT      encap_type;

    protected:
      shared_ptr<Controller<precision, EncapT>>      _controller;
      shared_ptr<IQuadrature<precision>>             _quadrature;
      shared_ptr<typename encap_type::factory_type>  _factory;

      shared_ptr<encap_type>                         _initial_state;
      vector<shared_ptr<encap_type>>                 _states;
      vector<shared_ptr<encap_type>>                 _previous_states;
      shared_ptr<encap_type>                         _end_state;

      vector<shared_ptr<encap_type>>                 _tau;
      vector<shared_ptr<encap_type>>                 _residuals;

      precision_type                                 _abs_residual_tol;
      precision_type                                 _rel_residual_tol;

      virtual void integrate_end_state();
      virtual void compute_residuals();

      virtual vector<shared_ptr<EncapT>>& states();
      virtual vector<shared_ptr<EncapT>>& previous_states();
      virtual shared_ptr<EncapT>&         end_state();
      virtual vector<shared_ptr<EncapT>>& residuals();

    public:
      explicit Sweeper();
      Sweeper(const Sweeper<precision, EncapT>& other) = default;
      Sweeper(Sweeper<precision, EncapT>&& other) = default;
      virtual ~Sweeper() = default;
      Sweeper<precision, EncapT>& operator=(const Sweeper<precision, EncapT>& other) = default;
      Sweeper<precision, EncapT>& operator=(Sweeper<precision, EncapT>&& other) = default;

      virtual       shared_ptr<IQuadrature<precision>>& quadrature();
      virtual const shared_ptr<IQuadrature<precision>>  get_quadrature() const;

      virtual       shared_ptr<typename EncapT::factory_type>& encap_factory();
      virtual const shared_ptr<typename EncapT::factory_type>  get_encap_factory() const;

      virtual       shared_ptr<Controller<precision, EncapT>>& controller();
      virtual const shared_ptr<Controller<precision, EncapT>>  get_controller() const;

      virtual       shared_ptr<EncapT>&         initial_state();
      virtual       vector<shared_ptr<EncapT>>& tau();

      virtual const shared_ptr<EncapT>          get_initial_state() const;
      virtual const vector<shared_ptr<EncapT>>  get_states() const;
      virtual const vector<shared_ptr<EncapT>>  get_previous_states() const;
      virtual const shared_ptr<EncapT>          get_end_state() const;
      virtual const vector<shared_ptr<EncapT>>  get_tau() const;
      virtual const vector<shared_ptr<EncapT>>  get_residuals() const;

      virtual void set_options();
      virtual void set_abs_residual_tol(const precision& abs_res_tol);
      virtual void set_rel_residual_tol(const precision& rel_res_tol);

      virtual void setup();

      virtual void pre_predict();
      virtual void predict();
      virtual void post_predict();

      virtual void pre_sweep();
      virtual void sweep();
      virtual void post_sweep();

      virtual void advance();

      virtual void spread();
      virtual void save();
      virtual void reevaluate(const bool initial_only=false);
      virtual bool converged();
  };
}

#include "pfasst/sweeper/interface_impl.hpp"

#endif  // _PFASST__SWEEPER__INTERFACE_HPP_
