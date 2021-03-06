#include "pfasst/encap/vector.hpp"

#include <algorithm>
#include <cassert>
using namespace std;

#include "pfasst/logging.hpp"
#include "pfasst/util.hpp"


namespace pfasst
{
  namespace encap
  {
    template<class EncapsulationTrait>
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::Encapsulation(const size_t size)
      : _data(size)
    {
      this->zero();
    }

    template<class EncapsulationTrait>
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::Encapsulation(const typename EncapsulationTrait::data_type& data)
      : Encapsulation<EncapsulationTrait>(data.size())
    {
      this->data() = data;
    }

    template<class EncapsulationTrait>
    Encapsulation<EncapsulationTrait>&
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::operator=(const typename EncapsulationTrait::data_type& data)
    {
      this->data() = data;
      return *this;
    }

    template<class EncapsulationTrait>
    typename EncapsulationTrait::data_type&
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::data()
    {
      return this->_data;
    }

    template<class EncapsulationTrait>
    const typename EncapsulationTrait::data_type&
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::get_data() const
    {
      return this->_data;
    }

    template<class EncapsulationTrait>
    void
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::zero()
    {
      fill(this->data().begin(), this->data().end(), typename EncapsulationTrait::spatial_type(0.0));
    }

    template<class EncapsulationTrait>
    void
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::scaled_add(const typename EncapsulationTrait::time_type& a,
                                   const shared_ptr<Encapsulation<EncapsulationTrait>> y)
    {
      assert(this->get_data().size() == y->data().size());

      transform(this->get_data().cbegin(), this->get_data().cend(), y->data().cbegin(),
                this->data().begin(),
                [a](const typename EncapsulationTrait::spatial_type& xi,
                    const typename EncapsulationTrait::spatial_type& yi) { return xi + a * yi; });
    }

    template<class EncapsulationTrait>
    typename EncapsulationTrait::spatial_type
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::norm0() const
    {
      return abs(*(max_element(this->get_data().cbegin(), this->get_data().cend(),
                               [](const typename EncapsulationTrait::spatial_type& a,
                                  const typename EncapsulationTrait::spatial_type& b)
                                 { return abs(a) < abs(b); })));
    }

    template<class EncapsulationTrait>
    template<class CommT>
    void
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::send(shared_ptr<CommT> comm, const int dest_rank,
                              const int tag, const bool blocking)
    {
      ML_CVLOG(2, "ENCAP", "sending data: " << this->get_data());
      if (blocking) {
        comm->send(this->get_data().data(), this->get_data().size(), dest_rank, tag);
      } else {
        comm->isend(this->get_data().data(), this->get_data().size(), dest_rank, tag);
      }
    }

    template<class EncapsulationTrait>
    template<class CommT>
    void
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::recv(shared_ptr<CommT> comm, const int src_rank,
                              const int tag, const bool blocking)
    {
      if (blocking) {
        comm->recv(this->data().data(), this->get_data().size(), src_rank, tag);
      } else {
        comm->irecv(this->data().data(), this->get_data().size(), src_rank, tag);
      }
      ML_CVLOG(2, "ENCAP", "received data: " << this->get_data());
    }

    template<class EncapsulationTrait>
    template<class CommT>
    void
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::bcast(shared_ptr<CommT> comm, const int root_rank)
    {
      comm->bcast(this->data().data(), this->get_data().size(), root_rank);
    }

    template<class EncapsulationTrait>
    void
    Encapsulation<
      EncapsulationTrait, 
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::log(el::base::type::ostream_t& os) const
    {
      os << "Vector" << pfasst::join(this->get_data(), ", ");
    }


    template<class EncapsulationTrait>
    EncapsulationFactory<
      EncapsulationTrait,
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::EncapsulationFactory(const size_t size)
      : _size(size)
    {}

    template<class EncapsulationTrait>
    shared_ptr<Encapsulation<EncapsulationTrait>>
    EncapsulationFactory<
      EncapsulationTrait,
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::create() const
    {
      return make_shared<Encapsulation<EncapsulationTrait>>(this->size());
    }

    template<class EncapsulationTrait>
    void
    EncapsulationFactory<
      EncapsulationTrait,
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::set_size(const size_t& size)
    {
      this->_size = size;
    }

    template<class EncapsulationTrait>
    size_t
    EncapsulationFactory<
      EncapsulationTrait,
      typename enable_if<
                 is_same<
                   vector<typename EncapsulationTrait::spatial_type>,
                   typename EncapsulationTrait::data_type
                 >::value
               >::type>::size() const
    {
      return this->_size;
    }
  }  // ::pfasst::encap
}  // ::pfasst
