#include "pfasst/controller/status.hpp"

#include <sstream>
#include <vector>
using namespace std;

#include <pfasst/logging.hpp>


namespace pfasst
{
#ifdef WITH_MPI
  template<typename precision>
  void
  Status<precision>::create_mpi_datatype()
  {
    const int COUNT = 10;
    int blocks[COUNT] = {
      sizeof(State),     // state
      sizeof(size_t),    // step
      sizeof(size_t),    // num_steps
      sizeof(size_t),    // iteration
      sizeof(size_t),    // max_iterations
      sizeof(precision), // time
      sizeof(precision), // dt
      sizeof(precision), // t_end
      sizeof(precision), // abs_res_norm
      sizeof(precision)  // rel_res_norm
    };
    MPI_Aint displ[COUNT] = {
      offsetof(StatusDetail<precision>, state),
      offsetof(StatusDetail<precision>, step),
      offsetof(StatusDetail<precision>, num_steps),
      offsetof(StatusDetail<precision>, iteration),
      offsetof(StatusDetail<precision>, max_iterations),
      offsetof(StatusDetail<precision>, time),
      offsetof(StatusDetail<precision>, dt),
      offsetof(StatusDetail<precision>, t_end),
      offsetof(StatusDetail<precision>, abs_res_norm),
      offsetof(StatusDetail<precision>, rel_res_norm)
    };
    MPI_Datatype types[COUNT] = {
      MPI_BYTE,
      MPI_BYTE,
      MPI_BYTE,
      MPI_BYTE,
      MPI_BYTE,
      MPI_BYTE,
      MPI_BYTE,
      MPI_BYTE,
      MPI_BYTE,
      MPI_BYTE
    };

    VLOG(1) << "creating MPI Data Type for Status";
    int err = MPI_Type_create_struct(COUNT, blocks, displ, types, &(status_data_type));
    assert(err == MPI_SUCCESS);
    err = MPI_Type_commit(&(status_data_type));
    assert(err == MPI_SUCCESS);
  }

  template<typename precision>
  void
  Status<precision>::free_mpi_datatype()
  {
    int err = MPI_Type_free(&(status_data_type));
    assert(err == MPI_SUCCESS);
  }
#endif

  template<typename precision>
  void
  Status<precision>::clear()
  {
    this->_detail = StatusDetail<precision>();
  }

  template<typename precision>
  size_t
  Status<precision>::get_step() const
  {
    return this->_detail.step;
  }

  template<typename precision>
  size_t&
  Status<precision>::step()
  {
    return this->_detail.step;
  }

  template<typename precision>
  size_t
  Status<precision>::get_num_steps() const
  {
    return this->_detail.num_steps;
  }

  template<typename precision>
  size_t&
  Status<precision>::num_steps()
  {
    return this->_detail.num_steps;
  }

  template<typename precision>
  size_t
  Status<precision>::get_iteration() const
  {
    return this->_detail.iteration;
  }

  template<typename precision>
  size_t& Status<precision>::iteration()
  {
    return this->_detail.iteration;
  }

  template<typename precision>
  size_t
  Status<precision>::get_max_iterations() const
  {
    return this->_detail.max_iterations;
  }

  template<typename precision>
  size_t&
  Status<precision>::max_iterations()
  {
    return this->_detail.max_iterations;
  }

  template<typename precision>
  precision
  Status<precision>::get_time() const
  {
    return this->_detail.time;
  }

  template<typename precision>
  precision&
  Status<precision>::time()
  {
    return this->_detail.time;
  }

  template<typename precision>
  precision
  Status<precision>::get_dt() const
  {
    return this->_detail.dt;
  }

  template<typename precision>
  precision&
  Status<precision>::dt()
  {
    return this->_detail.dt;
  }

  template<typename precision>
  precision
  Status<precision>::get_t_end() const
  {
    return this->_detail.t_end;
  }

  template<typename precision>
  precision&
  Status<precision>::t_end()
  {
    return this->_detail.t_end;
  }

  template<typename precision>
  State
  Status<precision>::get_state() const
  {
    return this->_detail.state;
  }

  template<typename precision>
  State&
  Status<precision>::state()
  {
    return this->_detail.state;
  }

  template<typename precision>
  precision
  Status<precision>::get_abs_res_norm() const
  {
    return this->_detail.abs_res_norm;
  }

  template<typename precision>
  precision&
  Status<precision>::abs_res_norm()
  {
    return this->_detail.abs_res_norm;
  }

  template<typename precision>
  precision
  Status<precision>::get_rel_res_norm() const
  {
    return this->_detail.rel_res_norm;
  }

  template<typename precision>
  precision&
  Status<precision>::rel_res_norm()
  {
    return this->_detail.rel_res_norm;
  }

  template<typename precision>
  template<typename CommT>
  void
  Status<precision>::send(shared_ptr<CommT> comm, const int dest_rank, const int tag,
                          const bool blocking)
  {
    if (blocking) {
      comm->send_status(&(this->_detail), 1, dest_rank, tag);
    } else {
      comm->isend_status(&(this->_detail), 1, dest_rank, tag);
    }
  }

  template<typename precision>
  template<typename CommT>
  void
  Status<precision>::recv(shared_ptr<CommT> comm, const int src_rank, const int tag,
                          const bool blocking)
  {
    if (blocking) {
      comm->recv_status(&(this->_detail), 1, src_rank, tag);
    } else {
      comm->irecv_status(&(this->_detail), 1, src_rank, tag);
    }
  }

  template<typename precision>
  vector<string>
  Status<precision>::summary() const
  {
    vector<string> out;
    out.push_back("Number Iterations: " + to_string(this->get_iteration()));
    {
      stringstream s;
      s << "Absolute Residual: " << LOG_FLOAT << this->get_abs_res_norm();
      out.push_back(s.str());
    }
    {
      stringstream s;
      s << "Relative Residual: " << LOG_FLOAT << this->get_rel_res_norm();
      out.push_back(s.str());
    }
    return out;
  }

  template<typename precision>
  void
  Status<precision>::log(el::base::type::ostream_t& os) const
  {
    os << LOG_FIXED << "Status("
       << "t=" << this->get_time()
       << ", dt=" << this->get_dt()
       << ", t_end=" << this->get_t_end()
       << ", step=" << this->get_step()
       << ", num_steps=" << this->get_num_steps()
       << ", iter=" << this->get_iteration()
       << ", iter_max=" << this->get_max_iterations()
       << ", state=" << this->get_state()
       << ", abs_res=" << LOG_FLOAT << this->get_abs_res_norm()
       << ", rel_res=" << LOG_FLOAT << this->get_rel_res_norm()
       << ")";
  }
}  // ::pfasst
