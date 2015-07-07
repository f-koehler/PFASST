#ifndef _PFASST__ENCAP__VECTOR_HPP_
#define _PFASST__ENCAP__VECTOR_HPP_

#include <memory>
#include <vector>
using namespace std;

#include "pfasst/globals.hpp"
#include "pfasst/logging.hpp"
#include "pfasst/encap/interface.hpp"
#include "pfasst/comm/interface.hpp"


namespace pfasst
{
  namespace encap
  {
    template<
      typename precision
    >
    class Encapsulation<precision, vector>
      : public enable_shared_from_this<Encapsulation<precision, vector>>
    {
      public:
        typedef vector<precision>                       data_type;
        typedef precision                               precision_type;
        typedef EncapsulationFactory<precision, vector> factory_type;

      protected:
        data_type _data;

      public:
        Encapsulation() = default;
        Encapsulation(const size_t size);
        Encapsulation(const vector<precision>& data);
        Encapsulation(const Encapsulation<precision, vector>& other) = default;
        Encapsulation(Encapsulation<precision, vector>&& other) = default;
        virtual ~Encapsulation() = default;
        Encapsulation<precision, vector>& operator=(const vector<precision>& data);
        Encapsulation<precision, vector>& operator=(const Encapsulation<precision, vector>& other) = default;
        Encapsulation<precision, vector>& operator=(Encapsulation<precision, vector>&& other) = default;

        virtual       vector<precision>& data();
        virtual const vector<precision>& get_data() const;

        virtual void zero();
        virtual void axpy(const precision& a, const shared_ptr<Encapsulation<precision, vector>> y);

        virtual precision norm0() const;

        virtual void send(shared_ptr<comm::Communicator> comm, const int dest_rank, const int tag,
                          const bool blocking);
        virtual void recv(shared_ptr<comm::Communicator> comm, const int src_rank, const int tag,
                          const bool blocking);
        virtual void bcast(shared_ptr<comm::Communicator> comm, const int root_rank);
    };

    template<typename precision>
    using VectorEncapsulation = Encapsulation<precision, vector>;


    template<
      typename precision
    >
    class EncapsulationFactory<precision, vector>
    {
      protected:
        size_t _size;

      public:
        explicit EncapsulationFactory(const size_t size=0);
        EncapsulationFactory(const EncapsulationFactory<precision, vector>& other) = default;
        EncapsulationFactory(EncapsulationFactory<precision, vector>&& other) = default;
        virtual ~EncapsulationFactory() = default;
        EncapsulationFactory<precision, vector>&
        operator=(const EncapsulationFactory<precision, vector>& other) = default;
        EncapsulationFactory<precision, vector>&
        operator=(EncapsulationFactory<precision, vector>&& other) = default;

        virtual shared_ptr<Encapsulation<precision, vector>> create();

        virtual void set_size(const size_t& size);
        virtual size_t size() const;
    };
  }  // ::pfasst::encap
}  // ::pfasst

#include "pfasst/encap/vector_impl.hpp"

#endif  // _PFASST__ENCAP__VECTOR_HPP_
