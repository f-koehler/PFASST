#ifndef _PFASST__CONTROLLER__INTERFACE_HPP_
#define _PFASST__CONTROLLER__INTERFACE_HPP_

#include <deque>
#include <iterator>
#include <memory>
#include <type_traits>
using namespace std;

#include "pfasst/globals.hpp"
#include "pfasst/exceptions.hpp"
#include "pfasst/controller/status.hpp"
#include "pfasst/comm/interface.hpp"


namespace pfasst
{
  template<
    class TransferT
  >
  class Controller
    : public enable_shared_from_this<Controller<TransferT>>
  {
    public:
      typedef          TransferT                             transfer_type;
      typedef typename transfer_type::traits::fine_time_type time_type;

    protected:
      shared_ptr<typename transfer_type::traits::coarse_sweeper_type> _coarse_level;
      shared_ptr<typename transfer_type::traits::fine_sweeper_type>   _fine_level;

      shared_ptr<transfer_type>      _transfer;
      shared_ptr<Status<time_type>>  _status;
      shared_ptr<comm::Communicator> _comm;
      bool                           _ready;

      virtual bool& ready();

    public:
      Controller();
      Controller(const Controller<TransferT>& other) = default;
      Controller(Controller<TransferT>&& other) = default;
      virtual ~Controller() = default;
      Controller<TransferT>& operator=(const Controller<TransferT>& other) = default;
      Controller<TransferT>& operator=(Controller<TransferT>&& other) = default;

      virtual       shared_ptr<Status<typename TransferT::traits::fine_time_type>> status();
      virtual const shared_ptr<Status<typename TransferT::traits::fine_time_type>> get_status() const;

      virtual       shared_ptr<comm::Communicator>& communicator();
      virtual const shared_ptr<comm::Communicator>  get_communicator() const;

      virtual size_t get_num_levels() const;
      virtual size_t get_num_steps() const;
      virtual bool   is_ready() const;

      template<class SweeperT>
      void add_sweeper(shared_ptr<SweeperT> sweeper, const bool as_coarse);
      void add_transfer(shared_ptr<TransferT> transfer);

      shared_ptr<typename TransferT::traits::coarse_sweeper_type> get_coarse();
      shared_ptr<typename TransferT::traits::fine_sweeper_type> get_fine();

      shared_ptr<TransferT> get_transfer();

      virtual void set_options();

      virtual void setup();
      virtual void run();

      virtual bool advance_time(const size_t& num_steps = 1);
      virtual bool advance_iteration();
  };
}  // ::pfasst

#include "pfasst/controller/interface_impl.hpp"

#endif  // _PFASST__CONTROLLER__INTERFACE_HPP_
