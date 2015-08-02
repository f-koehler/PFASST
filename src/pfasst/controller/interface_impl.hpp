#include "pfasst/controller/interface.hpp"

#include <cmath>
using namespace std;

#include "pfasst/util.hpp"
#include "pfasst/config.hpp"
#include "pfasst/logging.hpp"


namespace pfasst
{
  template<class TransferT>
  Controller<TransferT>::Controller()
    :   _status(make_shared<Status<typename TransferT::traits::fine_time_type>>())
      , _ready(false)
  {}

  template<class TransferT>
  shared_ptr<Status<typename TransferT::traits::fine_time_type>>
  Controller<TransferT>::status()
  {
    return this->_status;
  }

  template<class TransferT>
  const shared_ptr<Status<typename TransferT::traits::fine_time_type>>
  Controller<TransferT>::get_status() const
  {
    return this->_status;
  }

  template<class TransferT>
  shared_ptr<comm::Communicator>&
  Controller<TransferT>::communicator()
  {
    return this->_comm;
  }

  template<class TransferT>
  const shared_ptr<comm::Communicator>
  Controller<TransferT>::get_communicator() const
  {
    return this->_comm;
  }

  template<class TransferT>
  size_t
  Controller<TransferT>::get_num_levels() const
  {
    size_t num = 0;
    if (this->_coarse_level != nullptr) {
      num++;
    }
    if (this->_fine_level != nullptr) {
      num++;
    }
    return num;
  }

  template<class TransferT>
  size_t
  Controller<TransferT>::get_num_steps() const
  {
    if (this->get_status()->get_t_end() <= 0) {
      CLOG(ERROR, "CONTROL") << "Time end point must be non-zero positive."
        << " NOT " << this->get_status()->get_t_end();
      throw logic_error("time end point must be non-zero positive");
    }

    if (this->get_status()->get_dt() <= 0.0) {
      CLOG(ERROR, "CONTROL") << "Time delta must be non-zero positive."
        << " NOT " << this->get_status()->get_dt();
      throw logic_error("time delta must be non-zero positive");
    }

    const auto div = this->get_status()->get_t_end() / this->get_status()->get_dt();
    CLOG_IF(!almost_equal(div * this->get_status()->get_dt(),
                          (size_t)div * this->get_status()->get_dt()), WARNING, "CONTROL")
      << "End time point not an integral multiple of time delta: "
      << this->get_status()->get_t_end() << " / " << this->get_status()->get_dt()
      << " = " << div << " != " << (size_t)div;

    return (size_t)(this->get_status()->get_t_end() / this->get_status()->get_dt());
  }

  template<class TransferT>
  bool&
  Controller<TransferT>::ready()
  {
    return this->_ready;
  }

  template<class TransferT>
  bool
  Controller<TransferT>::is_ready() const
  {
    return this->_ready;
  }

  template<class TransferT>
  void
  Controller<TransferT>::set_options()
  {
    this->status()->max_iterations() = config::get_value<size_t>("max_iters", this->get_status()->get_max_iterations());
    this->status()->t_end() = config::get_value<typename TransferT::traits::fine_time_type>("t_end", this->get_status()->get_t_end());
  }

  template<class TransferT>
  template<class SweeperT>
  void
  Controller<TransferT>::add_sweeper(shared_ptr<SweeperT> sweeper, const bool as_coarse)
  {
    static_assert(is_same<SweeperT, typename TransferT::traits::fine_sweeper_type>::value
                  || is_same<SweeperT, typename TransferT::traits::coarse_sweeper_type>::value,
                  "Sweeper must be either a Coarse or Fine Sweeper Type.");

    if (as_coarse) {
      if (is_same<SweeperT, typename transfer_type::traits::coarse_sweeper_type>::value) {
        this->_coarse_level = sweeper;
      } else {
        CLOG(ERROR, "CONTROL") << "Type of given Sweeper ("
          << typeid(SweeperT).name() << ") is not applicable as Coarse Sweeper ("
          << typeid(typename transfer_type::traits::coarse_sweeper_type).name() << ").";
        throw logic_error("given sweeper can not be used as coarse sweeper");
      }
    } else {
      if (is_same<SweeperT, typename transfer_type::traits::fine_sweeper_type>::value) {
        this->_fine_level = sweeper;
      } else {
        CLOG(ERROR, "CONTROL") << "Type of given Sweeper ("
          << typeid(SweeperT).name() << ") is not applicable as Fine Sweeper ("
          << typeid(typename transfer_type::traits::fine_sweeper_type).name() << ").";
        throw logic_error("given sweeper can not be used as fine sweeper");
      }
    }
  }

  template<class TransferT>
  void
  Controller<TransferT>::add_transfer(shared_ptr<TransferT> transfer)
  {
    this->_transfer = transfer;
  }

  template<class TransferT>
  const shared_ptr<typename TransferT::traits::coarse_sweeper_type>
  Controller<TransferT>::get_coarse() const
  {
    return this->_coarse_level;
  }

  template<class TransferT>
  shared_ptr<typename TransferT::traits::coarse_sweeper_type>
  Controller<TransferT>::get_coarse()
  {
    return this->_coarse_level;
  }

  template<class TransferT>
  const shared_ptr<typename TransferT::traits::fine_sweeper_type>
  Controller<TransferT>::get_fine() const
  {
    return this->_fine_level;
  }

  template<class TransferT>
  shared_ptr<typename TransferT::traits::fine_sweeper_type>
  Controller<TransferT>::get_fine()
  {
    return this->_fine_level;
  }

  template<class TransferT>
  const shared_ptr<TransferT>
  Controller<TransferT>::get_transfer() const
  {
    return this->_transfer;
  }

  template<class TransferT>
  shared_ptr<TransferT>
  Controller<TransferT>::get_transfer()
  {
    return this->_transfer;
  }

  template<class TransferT>
  void
  Controller<TransferT>::setup()
  {
    CLOG_IF(this->is_ready(), WARNING, "CONTROL")
      << "Controller has already been setup.";

    if (this->get_num_levels() == 0) {
      CLOG(ERROR, "CONTROL") << "At least one level (Sweeper) must have been added.";
      throw logic_error("no levels defined");
    }

    if (this->get_status()->get_t_end() <= 0.0) {
      CLOG(ERROR, "CONTROL") << "End time point must be larger than zero."
        << " (" << this->get_status()->get_t_end() << ")";
      throw logic_error("end time point must be larger zero");
    }

    const auto num_steps = this->get_num_steps();
    if (num_steps * this->get_status()->get_dt() != this->get_status()->get_t_end()) {
      CLOG(ERROR, "CONTROL") << "End time point not an integral multiple of time delta. "
        << " (" << num_steps << " * " << this->get_status()->get_dt()
        << " = " << num_steps * this->get_status()->get_dt() << " != " << this->get_status()->get_t_end() << ")";
      throw logic_error("time end point is not an integral multiple of time delta");
    }

    CLOG_IF(this->get_status()->get_max_iterations() == 0, WARNING, "CONTROL")
      << "You sould define a maximum number of iterations to avoid endless runs."
      << " (" << this->get_status()->get_max_iterations() << ")";

    this->ready() = true;
  }

  template<class TransferT>
  void
  Controller<TransferT>::run()
  {
    if (!this->is_ready()) {
      CLOG(ERROR, "CONTROL") << "Controller is not ready to run. setup() not called yet.";
      throw logic_error("controller not ready to run");
    }
  }

  template<class TransferT>
  bool
  Controller<TransferT>::advance_time(const size_t& num_steps)
  {
    const time_type delta_time = num_steps * this->get_status()->get_dt();
    const time_type new_time = this->get_status()->get_time() + delta_time;

    if (new_time > this->get_status()->get_t_end()) {
      CLOG(WARNING, "CONTROL") << "Advancing " << num_steps
        << ((num_steps > 1) ? " steps " : " step ")
        << "with dt=" << this->get_status()->get_dt() << " to t=" << new_time
        << " will exceed T_end=" << this->get_status()->get_t_end() << " by "
        << (new_time - this->get_status()->get_t_end());
      return false;
    } else {
      CLOG(INFO, "CONTROL") << "Advancing " << num_steps
        << ((num_steps > 1) ? " steps " : " step ")
        << "with dt=" << this->get_status()->get_dt() << " to t=" << new_time;
      this->status()->time() += delta_time;
      this->status()->step() += num_steps;
      return true;
    }
  }

  template<class TransferT>
  bool
  Controller<TransferT>::advance_iteration()
  {
    if (this->get_status()->get_iteration() + 1 > this->get_status()->get_max_iterations()) {
      CLOG(WARNING, "CONTROL") << "Advancing to next iteration ("
        << (this->get_status()->get_iteration() + 1)
        << ") will exceed maximum number of allowed iterations ("
        << this->get_status()->get_max_iterations() << ")";
      return false;
    } else {
      CLOG(INFO, "CONTROL") << "Advancing to next iteration ("
        << (this->get_status()->get_iteration() + 1) << ")";
      this->status()->iteration()++;
      return true;
    }
  }
}  // ::pfasst
