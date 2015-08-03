
namespace pfasst
{
  template<class TransferT>
  TwoLevelPfasst<TransferT>::TwoLevelPfasst()
    : Controller<TransferT>()
  {}

  template<class TransferT>
  shared_ptr<comm::Communicator>&
  TwoLevelPfasst<TransferT>::communicator()
  {
    return this->_comm;
  }

  template<class TransferT>
  const shared_ptr<comm::Communicator>
  TwoLevelPfasst<TransferT>::get_communicator() const
  {
    return this->_comm;
  }

  template<class TransferT>
  size_t
  TwoLevelPfasst<TransferT>::get_num_levels() const
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
  template<class SweeperT>
  void
  TwoLevelPfasst<TransferT>::add_sweeper(shared_ptr<SweeperT> sweeper, const bool as_coarse)
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
  const shared_ptr<typename TransferT::traits::coarse_sweeper_type>
  TwoLevelPfasst<TransferT>::get_coarse() const
  {
    return this->_coarse_level;
  }

  template<class TransferT>
  shared_ptr<typename TransferT::traits::coarse_sweeper_type>
  TwoLevelPfasst<TransferT>::get_coarse()
  {
    return this->_coarse_level;
  }

  template<class TransferT>
  const shared_ptr<typename TransferT::traits::fine_sweeper_type>
  TwoLevelPfasst<TransferT>::get_fine() const
  {
    return this->_fine_level;
  }

  template<class TransferT>
  shared_ptr<typename TransferT::traits::fine_sweeper_type>
  TwoLevelPfasst<TransferT>::get_fine()
  {
    return this->_fine_level;
  }

  template<class TransferT>
  void
  TwoLevelPfasst<TransferT>::set_options()
  {
    Controller<TransferT>::set_options();
  }


  template<class TransferT>
  void
  TwoLevelPfasst<TransferT>::setup()
  {
    Controller<TransferT>::setup();

    if (this->get_num_levels() != 2) {
      CLOG(ERROR, "CONTROL") << "Two levels (Sweeper) must have been added for Two-Level-PFASST.";
      throw logic_error("Two-Level-PFASST requires two levels");
    }
  }

  template<class TransferT>
  void
  TwoLevelPfasst<TransferT>::run()
  {
    Controller<TransferT>::run();
  }

  template<class TransferT>
  bool
  TwoLevelPfasst<TransferT>::advance_time(const size_t& num_steps)
  {
    return Controller<TransferT>::advance_time(num_steps);
  }

  template<class TransferT>
  bool
  TwoLevelPfasst<TransferT>::advance_iteration()
  {
    return Controller<TransferT>::advance_iteration();
  }
}  // ::pfasst
