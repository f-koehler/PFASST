#include <memory>
using namespace std;

#include <mpi.h>

#include <pfasst.hpp>
#include <pfasst/quadrature.hpp>
#include <pfasst/encap/vector.hpp>
#include <pfasst/comm/mpi_p2p.hpp>
#include <pfasst/controller/two_level_pfasst.hpp>
#include <pfasst/transfer/spectral_1d.hpp>

#include "heat1d_sweeper.hpp"

using pfasst::encap::VectorEncapsulation;
using pfasst::quadrature::quadrature_factory;
using pfasst::quadrature::QuadratureType;
using pfasst::Spectral1DTransfer;
using pfasst::TwoLevelPfasst;
typedef pfasst::comm::MpiP2P CommType;

using pfasst::examples::heat1d::Heat1D;

typedef VectorEncapsulation<double, double>                        EncapType;
typedef Heat1D<pfasst::sweeper_traits<typename EncapType::traits>> SweeperType;
typedef pfasst::transfer_traits<SweeperType, SweeperType, 2>       TransferTraits;
typedef Spectral1DTransfer<TransferTraits>                         TransferType;


namespace pfasst
{
  namespace examples
  {
    namespace heat1d
    {
      void run(const size_t& ndofs, const size_t& nnodes, const QuadratureType& quad_type,
               const double& t_0, const double& dt, const double& t_end, const size_t& niter)
      {
        TwoLevelPfasst<TransferType, CommType> pfasst;
        pfasst.communicator() = make_shared<CommType>(MPI_COMM_WORLD);

        auto coarse = make_shared<SweeperType>(ndofs);
        coarse->quadrature() = quadrature_factory<double>(nnodes, quad_type);
        auto fine = make_shared<SweeperType>(ndofs * 2);
        fine->quadrature() = quadrature_factory<double>(nnodes, quad_type);

        auto transfer = make_shared<TransferType>();

        pfasst.add_sweeper(coarse, true);
        pfasst.add_sweeper(fine, false);
        pfasst.add_transfer(transfer);
        pfasst.set_options();

        pfasst.status()->time() = t_0;
        pfasst.status()->dt() = dt;
        pfasst.status()->t_end() = t_end;
        pfasst.status()->max_iterations() = niter;

        pfasst.setup();

        coarse->initial_state() = coarse->exact(pfasst.get_status()->get_time());
        fine->initial_state() = fine->exact(pfasst.get_status()->get_time());

        pfasst.run();
      }
    }  // ::pfasst::examples::advec_diff
  } // ::pfasst::examples
}  // ::pfasst


int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);

  pfasst::init(argc, argv,
               SweeperType::init_opts);

  const size_t ndofs = pfasst::config::get_value<size_t>("num_dofs", 4);
  const size_t nnodes = pfasst::config::get_value<size_t>("num_nodes", 3);
  const pfasst::quadrature::QuadratureType quad_type = pfasst::quadrature::QuadratureType::GaussRadau;
//   const pfasst::quadrature::QuadratureType quad_type = pfasst::config::get_value<pfasst::quadrature::QuadratureType>("quad_type", pfasst::quadrature::QuadratureType::GaussRadau);
  const double t_0 = 0.0;
//   const double t_0 = pfasst::config::get_value<double>("t_0", 0.0);
  const double dt = pfasst::config::get_value<double>("dt", 0.01);
  double t_end = pfasst::config::get_value<double>("tend", -1);
  size_t nsteps = pfasst::config::get_value<size_t>("num_steps", 0);
  if (t_end == -1 && nsteps == 0) {
    CLOG(ERROR, "USER") << "Either t_end or num_steps must be specified.";
    throw runtime_error("either t_end or num_steps must be specified");
  } else if (t_end != -1 && nsteps != 0) {
    if (!pfasst::almost_equal(t_0 + nsteps * dt, t_end)) {
      CLOG(ERROR, "USER") << "t_0 + nsteps * dt != t_end ("
                          << t_0 << " + " << nsteps << " * " << dt << " = " << (t_0 + nsteps * dt)
                          << " != " << t_end << ")";
      throw runtime_error("t_0 + nsteps * dt != t_end");
    }
  } else if (nsteps != 0) {
    t_end = t_0 + dt * nsteps;
  }
  const size_t niter = pfasst::config::get_value<size_t>("num_iters", 5);

  pfasst::examples::heat1d::run(ndofs, nnodes, quad_type, t_0, dt, t_end, niter);

  MPI_Finalize();
}