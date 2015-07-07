#ifndef _PFASST__ENCAP__FACTORY_HPP_
#define _PFASST__ENCAP__FACTORY_HPP_

#include <memory>
using namespace std;

#include "pfasst/encap/interface.hpp"


namespace pfasst
{
  namespace encap
  {
    template<
      typename precision,
      template<typename...> class DataT
    >
    class EncapsulationFactory
    {
      public:
        typedef          Encapsulation<precision, DataT> encap_type;
        typedef typename encap_type::data_type           data_type;

        EncapsulationFactory() = default;
        EncapsulationFactory(const EncapsulationFactory<precision, DataT>& other) = default;
        EncapsulationFactory(EncapsulationFactory<precision, DataT>&& other) = default;
        virtual ~EncapsulationFactory() = default;
        EncapsulationFactory<precision, DataT>&
        operator=(const EncapsulationFactory<precision, DataT>& other) = default;
        EncapsulationFactory<precision, DataT>&
        operator=(EncapsulationFactory<precision, DataT>&& other) = default;

        shared_ptr<Encapsulation<precision, DataT>> create();
    };
  }  // ::pfasst::encap
}  // ::pfasst

#include "pfasst/encap/factory_impl.hpp"

#endif  // _PFASST__ENCAP__FACTORY_HPP_
