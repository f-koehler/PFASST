#include "pfasst/encap/interface.hpp"

#include <cassert>
using namespace std;

#include "pfasst/exceptions.hpp"
#include "pfasst/logging.hpp"


namespace pfasst
{
  namespace encap
  {
    template<class EncapsulationTrait>
    shared_ptr<Encapsulation<EncapsulationTrait>>
    axpy(const typename EncapsulationTrait::time_type& a,
         const shared_ptr<Encapsulation<EncapsulationTrait>> x,
         const shared_ptr<Encapsulation<EncapsulationTrait>> y)
    {
      shared_ptr<Encapsulation<EncapsulationTrait>> result = \
        make_shared<Encapsulation<EncapsulationTrait>>(*x);
      result->axpy(a, y);
      return result;
    }

    template<class EncapsulationTrait>
    vector<shared_ptr<Encapsulation<EncapsulationTrait>>>
    mat_apply(const typename EncapsulationTrait::time_type& a,
              const Matrix<typename EncapsulationTrait::time_type>& mat,
              const vector<shared_ptr<Encapsulation<EncapsulationTrait>>>& x)
    {
      assert((size_t)mat.cols() == x.size());
      const size_t cols = mat.cols();
      const size_t rows = mat.rows();

      // initialize result vector of encaps
      vector<shared_ptr<Encapsulation<EncapsulationTrait>>> result(rows);
      transform(x.cbegin(), x.cend(), result.begin(),
                [](const shared_ptr<Encapsulation<EncapsulationTrait>>& xi) {
                  auto ri = make_shared<Encapsulation<EncapsulationTrait>>(xi->get_data());
                  ri->zero();
                  return ri;
                });

      for (size_t n = 0; n < rows; ++n) {
        for (size_t m = 0; m < cols; ++m) {
          result[n] = axpy(a * mat(n, m), x[m], result[n]);
        }
      }
      return result;
    }

    template<class EncapsulationTrait>
    typename EncapsulationTrait::spacial_type
    norm0(const shared_ptr<Encapsulation<EncapsulationTrait>> x)
    {
      return x->norm0();
    }
  }  // ::pfasst::encap
}  // ::pfasst
