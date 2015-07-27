#include "pfasst/transfer/interface.hpp"

#include <memory>
using namespace std;

#include "pfasst/exceptions.hpp"


namespace pfasst
{
  template<class CSweeperT, class FSweeperT>
  void
  Transfer<CSweeperT, FSweeperT>::interpolate_initial(const shared_ptr<CSweeperT> coarse,
                                                      shared_ptr<FSweeperT> fine)
  {
    throw NotImplementedYet("interpolation of initial values for generic Sweeper");
  }

  template<class CSweeperT, class FSweeperT>
  void
  Transfer<CSweeperT, FSweeperT>::interpolate(const shared_ptr<CSweeperT> coarse,
                                              shared_ptr<FSweeperT> fine,
                                              const bool initial)
  {
    throw NotImplementedYet("interpolation for generic Sweeper");
  }

  template<class CSweeperT, class FSweeperT>
  void
  Transfer<CSweeperT, FSweeperT>::interpolate(const shared_ptr<typename CSweeperT::encap_type> coarse,
                                              shared_ptr<typename FSweeperT::encap_type> fine)
  {
    throw NotImplementedYet("interpolation for generic Encapsulations");
  }

  template<class CSweeperT, class FSweeperT>
  void
  Transfer<CSweeperT, FSweeperT>::restrict_initial(const shared_ptr<FSweeperT> fine,
                                                   shared_ptr<CSweeperT> coarse)
  {
    throw NotImplementedYet("restriction of initial value for generic Sweeper");
  }

  template<class CSweeperT, class FSweeperT>
  void
  Transfer<CSweeperT, FSweeperT>::restrict(const shared_ptr<FSweeperT> fine,
                                           shared_ptr<CSweeperT> coarse,
                                           const bool initial)
  {
    throw NotImplementedYet("restriction for generic Sweeper");
  }

  template<class CSweeperT, class FSweeperT>
  void
  Transfer<CSweeperT, FSweeperT>::restrict(const shared_ptr<typename FSweeperT::encap_type> fine,
                                           shared_ptr<typename CSweeperT::encap_type> coarse)
  {
    throw NotImplementedYet("restriction for generic Encapsulations");
  }

  template<class CSweeperT, class FSweeperT>
  void
  Transfer<CSweeperT, FSweeperT>::fas(const typename FSweeperT::precision_type& dt,
                                      const shared_ptr<FSweeperT> fine,
                                      shared_ptr<CSweeperT> coarse)
  {
    throw NotImplementedYet("FAS correction for generic Sweeper");
  }
}  // ::pfasst
