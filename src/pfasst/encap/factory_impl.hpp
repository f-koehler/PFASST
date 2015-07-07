#include "pfasst/encap/factory.hpp"

#include "pfasst/exceptions.hpp"


namespace pfasst
{
  namespace encap
  {
    template<typename precision, template<typename...> class DataT>
    shared_ptr<Encapsulation<precision, DataT>>
    EncapsulationFactory<precision, DataT>::create()
    {
      throw NotImplementedYet("Encapsulation Factory for generic Encapsulation");
    }
  }  // ::pfasst::encap
}  // ::pfasst
