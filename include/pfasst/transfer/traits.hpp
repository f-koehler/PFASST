#ifndef _PFASST__TRANSFER__TRAITS_HPP_
#define _PFASST__TRANSFER__TRAITS_HPP_


namespace pfasst
{
  template<
    class CoarseSweeper,
    class FineSweeper,
    class... Ts
  >
  struct transfer_traits
  {
    typedef          CoarseSweeper                       coarse_sweeper_type;
    typedef typename coarse_sweeper_type::traits         coarse_sweeper_traits;
    typedef typename coarse_sweeper_traits::encap_traits coarse_encap_traits;
    typedef typename coarse_sweeper_traits::encap_type   coarse_encap_type;
    typedef typename coarse_sweeper_traits::time_type    coarse_time_type;
    typedef typename coarse_sweeper_traits::spacial_type coarse_spacial_type;

    typedef          FineSweeper                         fine_sweeper_type;
    typedef typename fine_sweeper_type::traits           fine_sweeper_traits;
    typedef typename fine_sweeper_traits::encap_traits   fine_encap_traits;
    typedef typename fine_sweeper_traits::encap_type     fine_encap_type;
    typedef typename fine_sweeper_traits::time_type      fine_time_type;
    typedef typename fine_sweeper_traits::spacial_type   fine_spacial_type;
  };
}  // ::pfasst

#endif  // _PFASST__TRANSFER__TRAITS_HPP_
