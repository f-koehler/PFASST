#ifndef _PFASST__TRANSFER__INTERFACE_HPP_
#define _PFASST__TRANSFER__INTERFACE_HPP_

#include <memory>
#include <vector>
using namespace std;


namespace pfasst
{
  template<
    class CoarseSweeperT,
    class FineSweeperT = CoarseSweeperT
  >
  class Transfer
  {
    static_assert(is_same<
                    typename CoarseSweeperT::precision_type,
                    typename FineSweeperT::precision_type
                  >::value, "precision of coarse and fine Sweeper must be the same");

    public:
      typedef          CoarseSweeperT                      coarse_sweeper_type;
      typedef          FineSweeperT                        fine_sweeper_type;
      typedef typename coarse_sweeper_type::encap_type     coarse_encap_type;
      typedef typename fine_sweeper_type::encap_type       fine_encap_type;
      typedef typename coarse_sweeper_type::precision_type time_type;

    public:
      Transfer() = default;
      Transfer(const Transfer<CoarseSweeperT, FineSweeperT>& other) = default;
      Transfer(Transfer<CoarseSweeperT, FineSweeperT>&& other) = default;
      virtual ~Transfer() = default;
      Transfer<CoarseSweeperT, FineSweeperT>& operator=(const Transfer<CoarseSweeperT, FineSweeperT>& other) = default;
      Transfer<CoarseSweeperT, FineSweeperT>& operator=(Transfer<CoarseSweeperT, FineSweeperT>&& other) = default;

      virtual void interpolate_initial(const shared_ptr<CoarseSweeperT> coarse,
                                       shared_ptr<FineSweeperT> fine);
      virtual void interpolate(const shared_ptr<CoarseSweeperT> coarse,
                               shared_ptr<FineSweeperT> fine,
                               const bool initial = false);
      virtual void interpolate(const shared_ptr<typename CoarseSweeperT::encap_type> coarse,
                               shared_ptr<typename FineSweeperT::encap_type> fine);

      virtual void restrict_initial(const shared_ptr<FineSweeperT> fine,
                                    shared_ptr<CoarseSweeperT> coarse);
      virtual void restrict(const shared_ptr<FineSweeperT> fine,
                            shared_ptr<CoarseSweeperT> coarse,
                            const bool initial = false);
      virtual void restrict(const shared_ptr<typename FineSweeperT::encap_type> fine,
                            shared_ptr<typename CoarseSweeperT::encap_type> coarse);

      virtual void fas(const typename FineSweeperT::precision_type& dt,
                       const shared_ptr<FineSweeperT> fine,
                       shared_ptr<CoarseSweeperT> coarse);
  };
}  // ::pfasst

#include "pfasst/transfer/interface_impl.hpp"

#endif  // _PFASST__TRANSFER__INTERFACE_HPP_
