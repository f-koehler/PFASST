#include "pfasst/encap/interface.hpp"

#include <cassert>
using namespace std;

#include "pfasst/exceptions.hpp"
#include "pfasst/logging.hpp"


namespace pfasst
{
  namespace encap
  {
    template<typename precision, template<typename...> class DataT>
    shared_ptr<Encapsulation<precision, DataT>>
    axpy(const precision& a,
         const shared_ptr<Encapsulation<precision, DataT>> x,
         const shared_ptr<Encapsulation<precision, DataT>> y)
    {
      shared_ptr<Encapsulation<precision, DataT>> result = \
        make_shared<Encapsulation<precision, DataT>>(*x);
      result->axpy(a, y);
      return result;
    }

    template<typename precision, template<typename...> class DataT>
    vector<shared_ptr<Encapsulation<precision, DataT>>>
    mat_apply(const precision& a, const Matrix<precision>& mat,
              const vector<shared_ptr<Encapsulation<precision, DataT>>>& x)
    {
      assert((size_t)mat.cols() == x.size());
      const size_t cols = mat.cols();
      const size_t rows = mat.rows();

      // initialize result vector of encaps
      vector<shared_ptr<Encapsulation<precision, DataT>>> result(rows);
      transform(x.cbegin(), x.cend(), result.begin(),
                [](const shared_ptr<Encapsulation<precision, DataT>>& xi) {
                  auto ri = make_shared<Encapsulation<precision, DataT>>(xi->get_data());
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

    template<typename precision, template<typename...> class DataT>
    precision
    norm0(const shared_ptr<Encapsulation<precision, DataT>> x)
    {
      return x->norm0();
    }


    template<typename precision, template<typename...> class DataT>
    Encapsulation<precision, DataT>::Encapsulation(const DataT<precision>& data)
      : Encapsulation<precision, DataT>()
    {
      this->data() = data;
    }

    template<typename precision, template<typename...> class DataT>
    Encapsulation<precision, DataT>&
    Encapsulation<precision, DataT>::operator=(const DataT<precision>& data)
    {
      this->data() = data;
      return *this;
    }

    template<typename precision, template<typename...> class DataT>
    DataT<precision>&
    Encapsulation<precision, DataT>::data()
    {
      return this->_data;
    }

    template<typename precision, template<typename...> class DataT>
    const DataT<precision>&
    Encapsulation<precision, DataT>::get_data() const
    {
      return this->_data;
    }

    template<typename precision, template<typename...> class DataT>
    void
    Encapsulation<precision, DataT>::zero()
    {
      throw NotImplementedYet("zeroing for generic Encapsulation");
    }

    template<typename precision, template<typename...> class DataT>
    void
    Encapsulation<precision, DataT>::axpy(const precision& a,
                                          const shared_ptr<Encapsulation<precision, DataT>> y)
    {
      throw NotImplementedYet("x=a*x+y for generic Encapsulation");
    }

    template<typename precision, template<typename...> class DataT>
    precision
    Encapsulation<precision, DataT>::norm0() const
    {
      throw NotImplementedYet("maximum norm for generic Encapsulation");
    }

    template<typename precision, template<typename...> class DataT>
    void
    Encapsulation<precision, DataT>::send(shared_ptr<comm::Communicator> comm,
                                          const int dest_rank, const int tag,
                                          const bool blocking)
    {
      throw NotImplementedYet("sending of generic Encapsulation");
    }

    template<typename precision, template<typename...> class DataT>
    void
    Encapsulation<precision, DataT>::recv(shared_ptr<comm::Communicator> comm,
                                          const int src_rank, const int tag,
                                          const bool blocking)
    {
      throw NotImplementedYet("receiving of generic Encapsulation");
    }

    template<typename precision, template<typename...> class DataT>
    void
    Encapsulation<precision, DataT>::bcast(shared_ptr<comm::Communicator> comm,
                                           const int root_rank)
    {
      throw NotImplementedYet("braodcasting of generic Encapsulation");
    }
  }  // ::pfasst::encap
}  // ::pfasst
