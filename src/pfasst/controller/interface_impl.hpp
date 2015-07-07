#include "pfasst/controller/interface.hpp"

#include <cmath>
using namespace std;

#include "pfasst/util.hpp"
#include "pfasst/config.hpp"
#include "pfasst/logging.hpp"


namespace pfasst
{
  template<typename precision, class EncapT>
  Controller<precision, EncapT>::Controller()
    :   _levels(0)
      , _status(make_shared<Status<precision>>())
      , _t_end(0)
      , _max_iterations(0)
      , _ready(false)
  {}

  template<typename precision, class EncapT>
  shared_ptr<Status<precision>>
  Controller<precision, EncapT>::status()
  {
    return this->_status;
  }

  template<typename precision, class EncapT>
  const shared_ptr<Status<precision>>
  Controller<precision, EncapT>::get_status() const
  {
    return this->_status;
  }

  template<typename precision, class EncapT>
  shared_ptr<comm::Communicator>&
  Controller<precision, EncapT>::communicator()
  {
    return this->_comm;
  }

  template<typename precision, class EncapT>
  const shared_ptr<comm::Communicator>
  Controller<precision, EncapT>::get_communicator() const
  {
    return this->_comm;
  }

  template<typename precision, class EncapT>
  size_t
  Controller<precision, EncapT>::get_num_levels() const
  {
    return this->_levels.size();
  }

  template<typename precision, class EncapT>
  size_t
  Controller<precision, EncapT>::get_num_steps() const
  {
    if (this->get_t_end() <= 0) {
      CLOG(ERROR, "CONTROL") << "Time end point must be non-zero positive."
        << " NOT " << this->get_t_end();
      throw logic_error("time end point must be non-zero positive");
    }

    if (this->get_status()->get_dt() <= 0.0) {
      CLOG(ERROR, "CONTROL") << "Time delta must be non-zero positive."
        << " NOT " << this->get_status()->get_dt();
      throw logic_error("time delta must be non-zero positive");
    }

    const auto div = this->get_t_end() / this->get_status()->get_dt();
    CLOG_IF(!almost_equal(div * this->get_status()->get_dt(),
                          (size_t)div * this->get_status()->get_dt()), WARNING, "CONTROL")
      << "End time point not an integral multiple of time delta: "
      << this->get_t_end() << " / " << this->get_status()->get_dt()
      << " = " << div << " != " << (size_t)div;

    return (size_t)(this->get_t_end() / this->get_status()->get_dt());
  }

  template<typename precision, class EncapT>
  bool&
  Controller<precision, EncapT>::ready()
  {
    return this->_ready;
  }

  template<typename precision, class EncapT>
  bool
  Controller<precision, EncapT>::is_ready() const
  {
    return this->_ready;
  }

  template<typename precision, class EncapT>
  precision&
  Controller<precision, EncapT>::t_end()
  {
    return this->_t_end;
  }

  template<typename precision, class EncapT>
  precision
  Controller<precision, EncapT>::get_t_end() const
  {
    return this->_t_end;
  }

  template<typename precision, class EncapT>
  size_t&
  Controller<precision, EncapT>::max_iterations()
  {
    return this->_max_iterations;
  }
  template<typename precision, class EncapT>
  size_t
  Controller<precision, EncapT>::get_max_iterations() const
  {
    return this->_max_iterations;
  }

  template<typename precision, class EncapT>
  void
  Controller<precision, EncapT>::set_options()
  {
    this->max_iterations() = config::get_value<size_t>("max_iters", this->get_max_iterations());
    this->t_end() = config::get_value<precision>("t_end", this->get_t_end());
  }

  template<typename precision, class EncapT>
  void
  Controller<precision, EncapT>::add_sweeper(shared_ptr<Sweeper<precision, EncapT>> sweeper,
                                             const bool as_coarsest)
  {
    if (as_coarsest) {
      this->_levels.push_front(sweeper);
    } else {
      this->_levels.push_back(sweeper);
    }

    sweeper->controller() = this->shared_from_this();
  }

  template<typename precision, class EncapT>
  template<class SweeperT>
  shared_ptr<SweeperT>
  Controller<precision, EncapT>::get_level(const size_t& level)
  {
    return this->_levels.at(level);
  }

  template<typename precision, class EncapT>
  void
  Controller<precision, EncapT>::setup()
  {
    CLOG_IF(this->is_ready(), WARNING, "CONTROL")
      << "Controller has already been setup.";

    if (this->get_num_levels() == 0) {
      CLOG(ERROR, "CONTROL") << "At least one level (Sweeper) must have been added.";
      throw logic_error("no levels defined");
    }

    const auto num_steps = this->get_num_steps();
    if (num_steps * this->get_status()->get_dt() != this->get_t_end()) {
      CLOG(ERROR, "CONTROL") << "End time point not an integral multiple of time delta. "
        << " (" << num_steps << " * " << this->get_status()->get_dt()
        << " = " << num_steps * this->get_status()->get_dt() << " != " << this->get_t_end() << ")";
      throw logic_error("time end point is not an integral multiple of time delta");
    }

    CLOG_IF(this->get_max_iterations() == 0, WARNING, "CONTROL")
      << "You sould define a maximum number of iterations to avoid endless runs."
      << " (" << this->get_max_iterations() << ")";

    this->ready() = true;
  }

  template<typename precision, class EncapT>
  void
  Controller<precision, EncapT>::run()
  {
    if (!this->is_ready()) {
      CLOG(ERROR, "CONTROL") << "Controller is not ready to run. setup() not called yet.";
      throw logic_error("controller not ready to run");
    }
  }

  template<typename precision, class EncapT>
  bool
  Controller<precision, EncapT>::advance_time(const size_t& num_steps)
  {
    const precision delta_time = num_steps * this->get_status()->get_dt();
    const precision new_time = this->get_status()->get_time() + delta_time;

    if (new_time > this->get_t_end()) {
      CLOG(WARNING, "CONTROL") << "Advancing " << num_steps
        << ((num_steps > 1) ? " steps " : " step ")
        << "with dt=" << this->get_status()->get_dt() << " to t=" << new_time
        << " will exceed T_end=" << this->get_t_end() << " by " << (new_time - this->get_t_end());
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

  template<typename precision, class EncapT>
  bool
  Controller<precision, EncapT>::advance_iteration()
  {
    if (this->get_status()->get_iteration() + 1 > this->get_max_iterations()) {
      CLOG(WARNING, "CONTROL") << "Advancing to next iteration ("
        << (this->get_status()->get_iteration() + 1)
        << ") will exceed maximum number of allowed iterations ("
        << this->get_max_iterations() << ")";
      return false;
    } else {
      CLOG(INFO, "CONTROL") << "Advancing to next iteration ("
        << (this->get_status()->get_iteration() + 1) << ")";
      this->status()->iteration()++;
      return true;
    }
  }

  template<typename precision, class EncapT>
  typename Controller<precision, EncapT>::LevelIterator
  Controller<precision, EncapT>::finest()
  {
    return LevelIterator(this->_levels.size() - 1, this->shared_from_this());
  }

  template<typename precision, class EncapT>
  typename Controller<precision, EncapT>::LevelIterator
  Controller<precision, EncapT>::coarsest()
  {
    return LevelIterator(0, this->shared_from_this());
  }


  template<typename precision, class EncapT>
  Controller<precision, EncapT>::LevelIterator::LevelIterator()
    :   _level(0)
      , _controller(nullptr)
  {}

  template<typename precision, class EncapT>
  Controller<precision, EncapT>::LevelIterator::LevelIterator(const size_t& level,
                                                              shared_ptr<Controller<precision, EncapT>> controller)
    :   _level(level)
      , _controller(controller)
  {}

  template<typename precision, class EncapT>
  void
  Controller<precision, EncapT>::LevelIterator::assert_bound_to_controller()
  {
    if (this->_controller == nullptr) {
      CLOG(ERROR, "CONTROL") << "No Controller is bound to this LevelIterator.";
      throw logic_error("no controller bound to LevelIterator");
    }
  }

  template<typename precision, class EncapT>
  size_t&
  Controller<precision, EncapT>::LevelIterator::level()
  {
    return this->_level;
  }

  template<typename precision, class EncapT>
  size_t
  Controller<precision, EncapT>::LevelIterator::get_level() const
  {
    return this->_level;
  }

  template<typename precision, class EncapT>
  template<class SweeperT>
  shared_ptr<SweeperT>
  Controller<precision, EncapT>::LevelIterator::current()
  {
    this->assert_bound_to_controller();
    return this->_controller->template get_level<SweeperT>(this->_level);
  }

  template<typename precision, class EncapT>
  template<class SweeperT>
  shared_ptr<SweeperT>
  Controller<precision, EncapT>::LevelIterator::finer()
  {
    this->assert_bound_to_controller();
    return this->_controller->template get_level<SweeperT>(this->_level + 1);
  }

  template<typename precision, class EncapT>
  template<class SweeperT>
  shared_ptr<SweeperT>
  Controller<precision, EncapT>::LevelIterator::coarser()
  {
    this->assert_bound_to_controller();
    return this->_controller->template get_level<SweeperT>(this->_level - 1);
  }

  template<typename precision, class EncapT>
  template<class SweeperT>
  shared_ptr<SweeperT>
  Controller<precision, EncapT>::LevelIterator::operator*()
  {
    return this->template current<SweeperT>();
  }

  template<typename precision, class EncapT>
  template<class SweeperT>
  shared_ptr<SweeperT>
  Controller<precision, EncapT>::LevelIterator::operator->()
  {
    return this->template current<SweeperT>();
  }

  template<typename precision, class EncapT>
  typename Controller<precision, EncapT>::LevelIterator&
  Controller<precision, EncapT>::LevelIterator::operator++()
  {
    this->_level++;
    return *this;
  }

  template<typename precision, class EncapT>
  bool
  Controller<precision, EncapT>::LevelIterator::operator==(const typename Controller<precision, EncapT>::LevelIterator& iter)
  {
    return this->_level == iter.get_level();
  }

  template<typename precision, class EncapT>
  bool
  Controller<precision, EncapT>::LevelIterator::operator!=(const typename Controller<precision, EncapT>::LevelIterator& iter)
  {
    return !(*this == iter);
  }

  template<typename precision, class EncapT>
  typename Controller<precision, EncapT>::LevelIterator&
  Controller<precision, EncapT>::LevelIterator::operator--()
  {
    this->_level--;
    return *this;
  }

  template<typename precision, class EncapT>
  typename Controller<precision, EncapT>::LevelIterator
  Controller<precision, EncapT>::LevelIterator::operator-(const typename Controller<precision, EncapT>::LevelIterator::difference_type& diff)
  {
    this->assert_bound_to_controller();
    return typename Controller<precision, EncapT>::LevelIterator(this->_level - diff, this->_controller);
  }

  template<typename precision, class EncapT>
  typename Controller<precision, EncapT>::LevelIterator
  Controller<precision, EncapT>::LevelIterator::operator+(const typename Controller<precision, EncapT>::LevelIterator::difference_type& diff)
  {
    this->assert_bound_to_controller();
    return typename Controller<precision, EncapT>::LevelIterator(this->_level + diff, this->_controller);
  }

  template<typename precision, class EncapT>
  bool
  Controller<precision, EncapT>::LevelIterator::operator<=(const typename Controller<precision, EncapT>::LevelIterator& iter)
  {
    return this->_level <= iter.get_level();
  }

  template<typename precision, class EncapT>
  bool
  Controller<precision, EncapT>::LevelIterator::operator>=(const typename Controller<precision, EncapT>::LevelIterator& iter)
  {
    return this->_level >= iter.get_level();
  }

  template<typename precision, class EncapT>
  bool
  Controller<precision, EncapT>::LevelIterator::operator<(const typename Controller<precision, EncapT>::LevelIterator& iter)
  {
    return this->_level < iter.get_level();
  }

  template<typename precision, class EncapT>
  bool
  Controller<precision, EncapT>::LevelIterator::operator>(const typename Controller<precision, EncapT>::LevelIterator& iter)
  {
    return this->_level > iter.get_level();
  }
}  // ::pfasst
