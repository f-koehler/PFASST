#ifndef _PFASST__ENCAP__INTERFACE_HPP_
#define _PFASST__ENCAP__INTERFACE_HPP_

#include <memory>
#include <type_traits>
using namespace std;

#include <leathers/push>
#include <leathers/all>
#include <Eigen/Dense>
#include <leathers/pop>
template<typename precision>
using Matrix = Eigen::Matrix<precision, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

#include "pfasst/globals.hpp"
#include "pfasst/logging.hpp"
#include "pfasst/comm/interface.hpp"


namespace pfasst
{
  namespace encap
  {
    template<
      typename precision,
      template<typename...> class DataT
    >
    class Encapsulation;

    template<
      typename precision,
      template<typename...> class DataT
    >
    class EncapsulationFactory;

    template<
      typename precision,
      template<typename...> class DataT
    >
    shared_ptr<Encapsulation<precision, DataT>>
    axpy(const precision& a,
         const shared_ptr<Encapsulation<precision, DataT>> x,
         const shared_ptr<Encapsulation<precision, DataT>> y);

    template<
      typename precision,
      template<typename...> class DataT
    >
    vector<shared_ptr<Encapsulation<precision, DataT>>>
    mat_apply(const precision& a, const Matrix<precision>& M,
              const vector<shared_ptr<Encapsulation<precision, DataT>>>& x);

    template<
      typename precision,
      template<typename...> class DataT
    >
    precision
    norm0(const shared_ptr<Encapsulation<precision, DataT>> x);


    template<
      typename precision,
      template<typename...> class DataT
    >
    class Encapsulation
      : public enable_shared_from_this<Encapsulation<precision, DataT>>
    {
      static_assert(is_arithmetic<precision>::value,
                    "precision must be an arithmetic type");
      static_assert(is_constructible<DataT<precision>>::value,
                    "Data Type must be constructible");
      static_assert(is_default_constructible<DataT<precision>>::value,
                    "Data Type must be default constructible");
      static_assert(is_destructible<DataT<precision>>::value,
                    "Data Type must be destructible");
      static_assert(is_assignable<DataT<precision>, DataT<precision>>::value,
                    "Data Type must be assignable");

      STATIC_WARNING(is_move_constructible<DataT<precision>>::value,
                     "Data Type should be move constructible");
      STATIC_WARNING(is_copy_constructible<DataT<precision>>::value,
                     "Data Type should be copy constructible");
      STATIC_WARNING(is_move_assignable<DataT<precision>>::value,
                     "Data Type should be move assignable");
      STATIC_WARNING(is_copy_assignable<DataT<precision>>::value,
                     "Data Type should be copy assignable");

      public:
        typedef DataT<precision>                       data_type;
        typedef precision                              precision_type;
        typedef EncapsulationFactory<precision, DataT> factory_type;

      protected:
        data_type _data;

      public:
        Encapsulation() = default;
        Encapsulation(const DataT<precision>& data);
        Encapsulation(const Encapsulation<precision, DataT>& other) = default;
        Encapsulation(Encapsulation<precision, DataT>&& other) = default;
        ~Encapsulation() = default;
        Encapsulation<precision, DataT>& operator=(const DataT<precision>& data);
        Encapsulation<precision, DataT>& operator=(const Encapsulation<precision, DataT>& other) = default;
        Encapsulation<precision, DataT>& operator=(Encapsulation<precision, DataT>&& other) = default;

        virtual       DataT<precision>& data();
        virtual const DataT<precision>& get_data() const;

        virtual void zero();
        virtual void axpy(const precision& a, const shared_ptr<Encapsulation<precision, DataT>> y);

        virtual precision norm0() const;

        virtual void send(shared_ptr<comm::Communicator> comm, const int dest_rank, const int tag,
                          const bool blocking);
        virtual void recv(shared_ptr<comm::Communicator> comm, const int src_rank, const int tag,
                          const bool blocking);
        virtual void bcast(shared_ptr<comm::Communicator> comm, const int root_rank);
    };
  }  // ::encap
}  // ::pfasst

#include "pfasst/encap/factory.hpp"

#include "pfasst/encap/interface_impl.hpp"

#endif  // _PFASST__ENCAP__INTERFACE_HPP_
