#include "pfasst/controller/two_level_mlsdc.hpp"

#include <cassert>
using namespace std;

#include "pfasst/util.hpp"
#include "pfasst/config.hpp"
#include "pfasst/logging.hpp"


namespace pfasst
{
  template<class TransferT, class CommT>
  TwoLevelMLSDC<TransferT, CommT>::TwoLevelMLSDC()
    : Controller<TransferT, CommT>()
  {
    TwoLevelMLSDC<TransferT, CommT>::init_loggers();
    this->set_logger_id("MLSDC");
  }

  template<class TransferT, class CommT>
  void
  TwoLevelMLSDC<TransferT, CommT>::init_loggers()
  {
    log::add_custom_logger("MLSDC");
    log::add_custom_logger("LVL_COARSE");
    log::add_custom_logger("LVL_FINE");
  }

  template<class TransferT, class CommT>
  size_t
  TwoLevelMLSDC<TransferT, CommT>::get_num_levels() const
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

  template<class TransferT, class CommT>
  template<class SweeperT>
  void
  TwoLevelMLSDC<TransferT, CommT>::add_sweeper(shared_ptr<SweeperT> sweeper, const bool as_coarse)
  {
    static_assert(is_same<SweeperT, typename TransferT::traits::fine_sweeper_type>::value
                  || is_same<SweeperT, typename TransferT::traits::coarse_sweeper_type>::value,
                  "Sweeper must be either a Coarse or Fine Sweeper Type.");

    if (as_coarse) {
      if (is_same<SweeperT, typename transfer_type::traits::coarse_sweeper_type>::value) {
        this->_coarse_level = sweeper;
        this->get_coarse()->set_logger_id("LVL_COARSE");
      } else {
        ML_CLOG(ERROR, this->get_logger_id(), "Type of given Sweeper ("
          << typeid(SweeperT).name() << ") is not applicable as Coarse Sweeper ("
          << typeid(typename transfer_type::traits::coarse_sweeper_type).name() << ").");
        throw logic_error("given sweeper can not be used as coarse sweeper");
      }
    } else {
      if (is_same<SweeperT, typename transfer_type::traits::fine_sweeper_type>::value) {
        this->_fine_level = sweeper;
        this->get_fine()->set_logger_id("LVL_FINE");
      } else {
        ML_CLOG(ERROR, this->get_logger_id(), "Type of given Sweeper ("
          << typeid(SweeperT).name() << ") is not applicable as Fine Sweeper ("
          << typeid(typename transfer_type::traits::fine_sweeper_type).name() << ").");
        throw logic_error("given sweeper can not be used as fine sweeper");
      }
    }
  }

  template<class TransferT, class CommT>
  const shared_ptr<typename TransferT::traits::coarse_sweeper_type>
  TwoLevelMLSDC<TransferT, CommT>::get_coarse() const
  {
    return this->_coarse_level;
  }

  template<class TransferT, class CommT>
  shared_ptr<typename TransferT::traits::coarse_sweeper_type>
  TwoLevelMLSDC<TransferT, CommT>::get_coarse()
  {
    return this->_coarse_level;
  }

  template<class TransferT, class CommT>
  const shared_ptr<typename TransferT::traits::fine_sweeper_type>
  TwoLevelMLSDC<TransferT, CommT>::get_fine() const
  {
    return this->_fine_level;
  }

  template<class TransferT, class CommT>
  shared_ptr<typename TransferT::traits::fine_sweeper_type>
  TwoLevelMLSDC<TransferT, CommT>::get_fine()
  {
    return this->_fine_level;
  }

  template<class TransferT, class CommT>
  void
  TwoLevelMLSDC<TransferT, CommT>::set_options()
  {
    Controller<TransferT, CommT>::set_options();

    this->get_fine()->set_options();
    this->get_coarse()->set_options();
  }


  template<class TransferT, class CommT>
  void
  TwoLevelMLSDC<TransferT, CommT>::setup()
  {
    assert(this->get_transfer() != nullptr);

    Controller<TransferT, CommT>::setup();

    if (this->get_num_levels() != 2) {
      ML_CLOG(ERROR, this->get_logger_id(), "Two levels (Sweeper) must have been added for Two-Level-MLSDC.");
      throw logic_error("Two-Level-MLSDC requires two levels");
    }

    ML_CVLOG(1, this->get_logger_id(), "setting up coarse level");
    this->get_coarse()->status() = this->get_status();
    this->get_coarse()->setup();

    ML_CVLOG(1, this->get_logger_id(), "setting up fine level");
    this->get_fine()->status() = this->get_status();
    this->get_fine()->setup();
  }

  template<class TransferT, class CommT>
  void
  TwoLevelMLSDC<TransferT, CommT>::run()
  {
    Controller<TransferT, CommT>::run();

    do {
      ML_CLOG(INFO, this->get_logger_id(), "");
      ML_CLOG(INFO, this->get_logger_id(), "Time Step " << (this->get_status()->get_step() + 1)
                                        << " of " << this->get_status()->get_num_steps());

      this->status()->state() = State::PREDICTING;

      // iterate on each time step
      do {
        if (this->get_status()->get_state() == State::PREDICTING) {
          ML_CLOG(INFO, this->get_logger_id(), "");
          ML_CLOG(INFO, this->get_logger_id(), "MLSDC Prediction step");

          assert(this->get_status()->get_iteration() == 0);

          // restrict fine initial condition ...
          this->get_transfer()->restrict_initial(this->get_fine(), this->get_coarse());
          // ... and spread it to all nodes on the coarse level
          this->get_coarse()->spread();
          this->get_coarse()->save();

          this->predict_coarse();

          this->cycle_up();
          this->sweep_fine();

        } else {
          ML_CLOG(INFO, this->get_logger_id(), "");
          ML_CLOG(INFO, this->get_logger_id(), "Iteration " << this->get_status()->get_iteration());

          this->cycle_down();
          this->sweep_coarse();

          this->cycle_up();
          this->sweep_fine();
        }
      } while(this->advance_iteration());
    } while(this->advance_time());
  }

  template<class TransferT, class CommT>
  bool
  TwoLevelMLSDC<TransferT, CommT>::advance_time(const size_t& num_steps)
  {
    if (Controller<TransferT, CommT>::advance_time(num_steps)) {
      this->get_fine()->advance(num_steps);
      this->get_coarse()->advance(num_steps);
      return true;
    } else {
      return false;
    }
  }

  template<class TransferT, class CommT>
  bool
  TwoLevelMLSDC<TransferT, CommT>::advance_iteration()
  {
    this->get_coarse()->converged();
    if (this->get_fine()->converged()) {
      ML_CLOG(INFO, this->get_logger_id(), "FINE sweeper has converged.");
      return false;
    } else if (Controller<TransferT, CommT>::advance_iteration()) {
      ML_CLOG(INFO, this->get_logger_id(), "FINE sweeper has not yet converged and additional iterations to do.");
      this->get_fine()->save();
      this->get_coarse()->save();
      return true;
    } else {
      ML_CLOG(INFO, this->get_logger_id(), "FINE sweeper has not yet converged and no more iterations to do.");
      return false;
    }
  }


  template<class TransferT, class CommT>
  void
  TwoLevelMLSDC<TransferT, CommT>::predict_coarse()
  {
    ML_CLOG(INFO, this->get_logger_id(), "Predicting on COARSE level");

    this->status()->state() = State::PRE_ITER_COARSE;
    this->get_coarse()->pre_predict();

    this->status()->state() = State::ITER_COARSE;
    this->get_coarse()->predict();

    this->status()->state() = State::POST_ITER_COARSE;
    this->get_coarse()->post_predict();

    this->status()->state() = State::PREDICTING;
  }

  template<class TransferT, class CommT>
  void
  TwoLevelMLSDC<TransferT, CommT>::predict_fine()
  {
    ML_CLOG(INFO, this->get_logger_id(), "Predicting on FINE level");

    this->status()->state() = State::PRE_ITER_FINE;
    this->get_fine()->pre_predict();

    this->status()->state() = State::ITER_FINE;
    this->get_fine()->predict();

    this->status()->state() = State::POST_ITER_FINE;
    this->get_fine()->post_predict();

    this->status()->state() = State::PREDICTING;
  }

  template<class TransferT, class CommT>
  void
  TwoLevelMLSDC<TransferT, CommT>::sweep_coarse()
  {
    ML_CLOG(INFO, this->get_logger_id(), "Sweeping on COARSE level");

    this->status()->state() = State::PRE_ITER_COARSE;
    this->get_coarse()->pre_sweep();

    this->status()->state() = State::ITER_COARSE;
    this->get_coarse()->sweep();

    this->status()->state() = State::POST_ITER_COARSE;
    this->get_coarse()->post_sweep();

    this->status()->state() = State::ITERATING;
  }

  template<class TransferT, class CommT>
  void
  TwoLevelMLSDC<TransferT, CommT>::sweep_fine()
  {
    ML_CLOG(INFO, this->get_logger_id(), "Sweeping on FINE level");

    this->status()->state() = State::PRE_ITER_FINE;
    this->get_fine()->pre_sweep();

    this->status()->state() = State::ITER_FINE;
    this->get_fine()->sweep();

    this->status()->state() = State::POST_ITER_FINE;
    this->get_fine()->post_sweep();

    this->status()->state() = State::ITERATING;
  }

  template<class TransferT, class CommT>
  void
  TwoLevelMLSDC<TransferT, CommT>::cycle_down()
  {
    ML_CLOG(INFO, this->get_logger_id(), "Restrict onto coarse level");

    this->get_transfer()->restrict(this->get_fine(), this->get_coarse(), true);
    this->get_transfer()->fas(this->get_status()->get_dt(), this->get_fine(), this->get_coarse());
    this->get_coarse()->save();
  }

  template<class TransferT, class CommT>
  void
  TwoLevelMLSDC<TransferT, CommT>::cycle_up()
  {
    ML_CLOG(INFO, this->get_logger_id(), "Interpolate onto fine level");

    this->get_transfer()->interpolate(this->get_coarse(), this->get_fine(), true);
  }
}  // ::pfasst
