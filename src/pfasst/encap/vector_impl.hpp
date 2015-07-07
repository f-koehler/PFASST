#include "pfasst/encap/vector.hpp"

#include <algorithm>
#include <cassert>
using namespace std;

#include "pfasst/logging.hpp"


namespace pfasst
{
  namespace encap
  {
    template<typename precision>
    Encapsulation<precision, vector>::Encapsulation(const size_t size)
      : Encapsulation<precision, vector>(vector<precision>(size))
    {}

    template<typename precision>
    Encapsulation<precision, vector>::Encapsulation(const vector<precision>& data)
      : Encapsulation<precision, vector>()
    {
      this->data() = data;
    }

    template<typename precision>
    Encapsulation<precision, vector>&
    Encapsulation<precision, vector>::operator=(const vector<precision>& data)
    {
      this->data() = data;
      return *this;
    }

    template<typename precision>
    vector<precision>&
    Encapsulation<precision, vector>::data()
    {
      return this->_data;
    }

    template<typename precision>
    const vector<precision>&
    Encapsulation<precision, vector>::get_data() const
    {
      return this->_data;
    }

    template<typename precision>
    void
    Encapsulation<precision, vector>::zero()
    {
      fill(this->data().begin(), this->data().end(), precision(0.0));
    }

    template<typename precision>
    void
    Encapsulation<precision, vector>::axpy(const precision& a,
                                           const shared_ptr<Encapsulation<precision, vector>> y)
    {
      assert(this->get_data().size() == y->data().size());
      transform(this->get_data().cbegin(), this->get_data().cend(), y->data().cbegin(),
                this->data().begin(),
                [a](const precision& xi, const precision& yi) { return a * xi + yi; });
    }

    template<typename precision>
    precision
    Encapsulation<precision, vector>::norm0() const
    {
      return abs(*(max_element(this->get_data().cbegin(), this->get_data().cend(),
                               [](const precision& a, const precision& b)
                                 { return abs(a) < abs(b); })));
    }

    template<typename precision>
    void
    Encapsulation<precision, vector>::send(shared_ptr<comm::Communicator> comm,
                                           const int dest_rank, const int tag,
                                           const bool blocking)
    {
      if (blocking) {
        comm->send(this->get_data().data(), this->get_data().size(), dest_rank, tag);
      } else {
        comm->isend(this->get_data().data(), this->get_data().size(), dest_rank, tag);
      }
    }

    template<typename precision>
    void
    Encapsulation<precision, vector>::recv(shared_ptr<comm::Communicator> comm,
                                           const int src_rank, const int tag,
                                           const bool blocking)
    {
      if (blocking) {
        comm->recv(this->data().data(), this->get_data().size(), src_rank, tag);
      } else {
        comm->irecv(this->data().data(), this->get_data().size(), src_rank, tag);
      }
    }

    template<typename precision>
    void
    Encapsulation<precision, vector>::bcast(shared_ptr<comm::Communicator> comm,
                                            const int root_rank)
    {
      comm->bcast(this->data().data(), this->get_data().size(), root_rank);
    }


    template<typename precision>
    EncapsulationFactory<precision, vector>::EncapsulationFactory(const size_t size)
      : _size(size)
    {}

    template<typename precision>
    shared_ptr<Encapsulation<precision, vector>>
    EncapsulationFactory<precision, vector>::create()
    {
      return make_shared<Encapsulation<precision, vector>>(this->size());
    }

    template<typename precision>
    void EncapsulationFactory<precision, vector>::set_size(const size_t& size)
    {
      this->_size = size;
    }

    template<typename precision>
    size_t EncapsulationFactory<precision, vector>::size() const
    {
      return this->_size;
    }
  }  // ::pfasst::encap
}  // ::pfasst
