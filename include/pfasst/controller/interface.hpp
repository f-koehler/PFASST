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
  template<typename precision, class EncapT> class Sweeper;

  template<
    typename precision,
    class EncapT
  >
  class Controller
    : public enable_shared_from_this<Controller<precision, EncapT>>
  {
    static_assert(is_arithmetic<precision>::value,
                  "precision type must be arithmetic");

    public:
      typedef precision          precision_type;
      typedef EncapT             encap_type;

      class LevelIterator
        : public std::iterator<random_access_iterator_tag, shared_ptr<Sweeper<precision, EncapT>>,
                   size_t, shared_ptr<Sweeper<precision, EncapT>>,
                   shared_ptr<Sweeper<precision, EncapT>>
                 >
      {
        protected:
          size_t                                    _level;
          shared_ptr<Controller<precision, EncapT>> _controller;

          virtual void assert_bound_to_controller();

        public:
          typedef size_t                                 difference_type;
          typedef shared_ptr<Sweeper<precision, EncapT>> value_type;
          typedef shared_ptr<Sweeper<precision, EncapT>> pointer;
          typedef shared_ptr<Sweeper<precision, EncapT>> reference;
          typedef random_access_iterator_tag             iterator_category;

          LevelIterator();
          explicit LevelIterator(const size_t& level,
                                 shared_ptr<Controller<precision, EncapT>> controller);
          LevelIterator(const LevelIterator& other) = default;
          LevelIterator(LevelIterator&& other) = default;
          virtual ~LevelIterator() = default;
          LevelIterator& operator=(const LevelIterator& other) = default;
          LevelIterator& operator=(LevelIterator&& other) = default;

          virtual size_t& level();
          virtual size_t  get_level() const;

          template<class SweeperT = Sweeper<precision, EncapT>>
          shared_ptr<SweeperT> current();

          template<class SweeperT = Sweeper<precision, EncapT>>
          shared_ptr<SweeperT> finer();

          template<class SweeperT = Sweeper<precision, EncapT>>
          shared_ptr<SweeperT> coarser();

          template<class SweeperT = Sweeper<precision, EncapT>>
          shared_ptr<SweeperT> operator*();

          template<class SweeperT = Sweeper<precision, EncapT>>
          shared_ptr<SweeperT> operator->();

          virtual LevelIterator& operator++();
          virtual bool           operator==(const LevelIterator& iter);
          virtual bool           operator!=(const LevelIterator& iter);
          // required by std::bidirectional_iterator_tag
          virtual LevelIterator& operator--();
          // required by std::random_access_iterator_tag
          virtual LevelIterator  operator- (const difference_type& diff);
          virtual LevelIterator  operator+ (const difference_type& diff);
          virtual bool           operator<=(const LevelIterator& iter);
          virtual bool           operator>=(const LevelIterator& iter);
          virtual bool           operator< (const LevelIterator& iter);
          virtual bool           operator> (const LevelIterator& iter);
      };

    protected:
      // index 0 is coarsest level
      deque<shared_ptr<Sweeper<precision, EncapT>>> _levels;

      shared_ptr<Status<precision>>  _status;
      shared_ptr<comm::Communicator> _comm;

      precision _t_end;
      size_t    _max_iterations;
      bool      _ready;

      virtual bool& ready();

    public:
      Controller();
      Controller(const Controller<precision, EncapT>& other) = default;
      Controller(Controller<precision, EncapT>&& other) = default;
      virtual ~Controller() = default;
      Controller<precision, EncapT>&
      operator=(const Controller<precision, EncapT>& other) = default;
      Controller<precision, EncapT>&
      operator=(Controller<precision, EncapT>&& other) = default;

      virtual       shared_ptr<Status<precision>> status();
      virtual const shared_ptr<Status<precision>> get_status() const;

      virtual       shared_ptr<comm::Communicator>& communicator();
      virtual const shared_ptr<comm::Communicator>  get_communicator() const;

      virtual size_t get_num_levels() const;
      virtual size_t get_num_steps() const;
      virtual bool   is_ready() const;

      virtual precision& t_end();
      virtual precision  get_t_end() const;

      virtual size_t& max_iterations();
      virtual size_t  get_max_iterations() const;

      virtual void add_sweeper(shared_ptr<Sweeper<precision, EncapT>> sweeper,
                               const bool as_coarsest=true);

      template<class SweeperT = Sweeper<precision, EncapT>>
      shared_ptr<SweeperT> get_level(const size_t& level);

      virtual void set_options();

      virtual void setup();
      virtual void run();

      virtual bool advance_time(const size_t& num_steps=1);
      virtual bool advance_iteration();

      virtual LevelIterator finest();
      virtual LevelIterator coarsest();
  };
}  // ::pfasst

#include "pfasst/controller/interface_impl.hpp"

#endif  // _PFASST__CONTROLLER__INTERFACE_HPP_
