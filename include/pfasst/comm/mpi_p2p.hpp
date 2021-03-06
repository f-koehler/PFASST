#ifndef _PFASST__COMM__MPI_P2P_HPP_
#define _PFASST__COMM__MPI_P2P_HPP_

#ifndef WITH_MPI
  #error "You need MPI enabled for using the MPI P2P communicator"
#endif

#include <list>
#include <map>
#include <memory>
#include <type_traits>
#include <utility>
using namespace std;

#include <leathers/push>
#include <leathers/all>
#include <mpi.h>
#include <leathers/pop>

#include "pfasst/comm/communicator.hpp"
#include "pfasst/controller/status.hpp"


#ifndef NON_CONST_MPI
  template<typename T>
  T* mpi_const_cast(const T* input) {
      return const_cast<T*>(input);
  }
  template<typename T>
  T& mpi_const_cast(const T& input) {
      return const_cast<T&>(input);
  }
#else
  template<typename T>
  const T* mpi_const_cast(const T* input) {
      return input;
  }
  template<typename T>
  const T& mpi_const_cast(const T& input) {
      return input;
  }
#endif


namespace pfasst
{
  namespace comm
  {
    static string error_from_code(const int err_code);
    static MPI_Status MPI_Status_factory();
    static void check_mpi_error(const int err_code);


    class MpiP2P
      : public Communicator
    {
      protected:
        int      _size = -1;
        int      _rank = -1;
        string   _name = "";

        MPI_Comm _comm;

        list<MPI_Status>                 _stati;
        map<pair<int, int>, MPI_Request> _requests;

      public:
        explicit MpiP2P(MPI_Comm comm = MPI_COMM_WORLD);
        MpiP2P(const MpiP2P& other) = default;
        MpiP2P(MpiP2P&& other) = default;
        virtual ~MpiP2P();
        MpiP2P& operator=(const MpiP2P& other) = default;
        MpiP2P& operator=(MpiP2P&& other) = default;

        virtual size_t get_size() const override;
        virtual size_t get_rank() const override;

        virtual string get_name() const;

        virtual bool is_first() const override;
        virtual bool is_last() const override;

        virtual void cleanup() override;
        virtual void abort(const int& err_code) override;

        virtual void send(const double* const data, const int count, const int dest_rank, const int tag) override;
        virtual void send_status(const StatusDetail<double>* const data, const int count, const int dest_rank, const int tag) override;

        virtual void isend(const double* const data, const int count, const int dest_rank, const int tag) override;
        virtual void isend_status(const StatusDetail<double>* const data, const int count, const int dest_rank, const int tag) override;

        virtual void recv(double* data, const int count, const int src_rank, const int tag) override;
        virtual void recv_status(StatusDetail<double>* data, const int count, const int src_rank, const int tag) override;

        virtual void irecv(double* data, const int count, const int src_rank, const int tag) override;
        virtual void irecv_status(StatusDetail<double>* data, const int count, const int src_rank, const int tag) override;

        virtual void bcast(double* data, const int count, const int root_rank) override;
    };
  }  // ::pfasst::comm
}  // ::pfasst

#include "pfasst/comm/mpi_p2p_impl.hpp"

#endif  // _PFASST__COMM__MPI_P2P_HPP_
