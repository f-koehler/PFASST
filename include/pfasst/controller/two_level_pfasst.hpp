#ifndef _PFASST__CONTROLLER__TWO_LEVEL_PFASST_HPP_
#define _PFASST__CONTROLLER__TWO_LEVEL_PFASST_HPP_

#include <memory>
using namespace std;

#include "pfasst/controller/interface.hpp"
#include "pfasst/comm/interface.hpp"


namespace pfasst
{
  template<
    class TransferT
  >
  class TwoLevelPfasst
    : public Controller<TransferT>
  {
    public:
      typedef          TransferT                             transfer_type;
      typedef typename transfer_type::traits::fine_time_type time_type;

    protected:
      shared_ptr<typename transfer_type::traits::coarse_sweeper_type> _coarse_level;
      shared_ptr<typename transfer_type::traits::fine_sweeper_type>   _fine_level;

      shared_ptr<comm::Communicator> _comm;

    public:
      TwoLevelPfasst();
      TwoLevelPfasst(const TwoLevelPfasst<TransferT>& other) = default;
      TwoLevelPfasst(TwoLevelPfasst<TransferT>&& other) = default;
      virtual ~TwoLevelPfasst() = default;
      TwoLevelPfasst<TransferT>& operator=(const TwoLevelPfasst<TransferT>& other) = default;
      TwoLevelPfasst<TransferT>& operator=(TwoLevelPfasst<TransferT>&& other) = default;

      virtual       shared_ptr<comm::Communicator>& communicator();
      virtual const shared_ptr<comm::Communicator>  get_communicator() const;

      virtual size_t get_num_levels() const override;

      template<class SweeperT>
      void add_sweeper(shared_ptr<SweeperT> sweeper, const bool as_coarse);

      virtual const shared_ptr<typename TransferT::traits::coarse_sweeper_type> get_coarse() const;
      virtual       shared_ptr<typename TransferT::traits::coarse_sweeper_type> get_coarse();
      virtual const shared_ptr<typename TransferT::traits::fine_sweeper_type> get_fine() const;
      virtual       shared_ptr<typename TransferT::traits::fine_sweeper_type> get_fine();

      virtual void set_options() override;

      virtual void setup() override;
      virtual void run() override;

      virtual bool advance_time(const size_t& num_steps = 1) override;
      virtual bool advance_iteration() override;
  };
}  // ::pfasst

#include "pfasst/controller/two_level_pfasst_impl.hpp"

#endif  // _PFASST__CONTROLLER__TWO_LEVEL_PFASST_HPP_
