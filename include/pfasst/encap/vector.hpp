#ifndef _PFASST__ENCAP__VECTOR_HPP_
#define _PFASST__ENCAP__VECTOR_HPP_

#include <memory>
#include <type_traits>
#include <vector>
using namespace std;

#include "pfasst/globals.hpp"
#include "pfasst/logging.hpp"
#include "pfasst/encap/encapsulation.hpp"


namespace pfasst
{
  namespace encap
  {
    /**
     * Specialization of Encapsulation for `std::vector`.
     */
    template<
      class EncapsulationTrait
    >
    class Encapsulation<EncapsulationTrait,
                        typename enable_if<
                                   is_same<
                                     vector<typename EncapsulationTrait::spatial_type>,
                                     typename EncapsulationTrait::data_type
                                   >::value>::type>
      :   public enable_shared_from_this<Encapsulation<EncapsulationTrait>>
        , public el::Loggable
    {
      public:
        typedef          EncapsulationTrait           traits;
        typedef typename traits::time_type            time_type;
        typedef typename traits::spatial_type         spatial_type;
        typedef typename traits::data_type            data_type;
        typedef          EncapsulationFactory<traits> factory_type;

      protected:
        data_type _data;

      public:
        explicit Encapsulation(const size_t size = 0);
        Encapsulation(const typename EncapsulationTrait::data_type& data);
        Encapsulation<EncapsulationTrait>& operator=(const typename EncapsulationTrait::data_type& data);

        virtual       typename EncapsulationTrait::data_type& data();
        virtual const typename EncapsulationTrait::data_type& get_data() const;

        virtual void zero();
        virtual void scaled_add(const typename EncapsulationTrait::time_type& a,
                               const shared_ptr<Encapsulation<EncapsulationTrait>> y);

        virtual typename EncapsulationTrait::spatial_type norm0() const;

        template<class CommT>
        void send(shared_ptr<CommT> comm, const int dest_rank, const int tag, const bool blocking);
        template<class CommT>
        void recv(shared_ptr<CommT> comm, const int src_rank, const int tag, const bool blocking);
        template<class CommT>
        void bcast(shared_ptr<CommT> comm, const int root_rank);

        virtual void log(el::base::type::ostream_t& os) const override;
    };

    /**
     * Shortcut for encapsulation of `std::vector` data types.
     */
    template<
      typename time_precision,
      typename spatial_precision
    >
    using VectorEncapsulation = Encapsulation<vector_encap_traits<time_precision, spatial_precision>>;


    template<
      class EncapsulationTrait
    >
    class EncapsulationFactory<EncapsulationTrait,
                               typename enable_if<
                                          is_same<
                                            vector<typename EncapsulationTrait::spatial_type>,
                                            typename EncapsulationTrait::data_type
                                          >::value>::type>
      : public enable_shared_from_this<EncapsulationFactory<EncapsulationTrait>>
    {
      protected:
        size_t _size;

      public:
        explicit EncapsulationFactory(const size_t size = 0);
        EncapsulationFactory(const EncapsulationFactory<EncapsulationTrait>& other);
        EncapsulationFactory(EncapsulationFactory<EncapsulationTrait>&& other);
        virtual ~EncapsulationFactory() = default;
        EncapsulationFactory<EncapsulationTrait>& operator=(const EncapsulationFactory<EncapsulationTrait>& other);
        EncapsulationFactory<EncapsulationTrait>& operator=(EncapsulationFactory<EncapsulationTrait>&& other);

        virtual shared_ptr<Encapsulation<EncapsulationTrait>> create() const;

        virtual void set_size(const size_t& size);
        virtual size_t size() const;
    };
  }  // ::pfasst::encap
}  // ::pfasst


#include "pfasst/encap/vector_impl.hpp"

#endif  // _PFASST__ENCAP__VECTOR_HPP_
