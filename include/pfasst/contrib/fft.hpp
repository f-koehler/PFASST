#ifndef _PFASST__CONTRIB__FFTW_HPP_
#define _PFASST__CONTRIB__FFTW_HPP_

#include <complex>
using std::real;
#include <map>
#include <memory>
#include <utility>
using namespace std;

#include <leathers/push>
#include <leathers/all>
#include <fftw3.h>
#include <leathers/pop>

#include "pfasst/encap/vector.hpp"
using namespace pfasst::encap;


namespace pfasst
{
  namespace contrib
  {
    //! TODO: rewrite to get rid of side-effects and use real RAII
    template<
      class precision
    >
    class FFT
    {
        struct workspace {
          fftw_plan           ffft;
          fftw_plan           ifft;
          fftw_complex*       wk;
          complex<precision>* z;
        };

        map<size_t, shared_ptr<workspace>> workspaces;

      public:
        virtual ~FFT()
        {
          for (auto& x : workspaces) {
            shared_ptr<workspace> wk = std::get<1>(x);
            fftw_free(wk->wk);
            fftw_destroy_plan(wk->ffft);
            fftw_destroy_plan(wk->ifft);
          }
          workspaces.clear();
        }

        shared_ptr<workspace> get_workspace(size_t ndofs)
        {
          if (workspaces.find(ndofs) == workspaces.end()) {
            shared_ptr<workspace> wk = make_shared<workspace>();
            wk->wk = fftw_alloc_complex(ndofs);
            wk->ffft = fftw_plan_dft_1d(ndofs, wk->wk, wk->wk, FFTW_FORWARD, FFTW_ESTIMATE);
            wk->ifft = fftw_plan_dft_1d(ndofs, wk->wk, wk->wk, FFTW_BACKWARD, FFTW_ESTIMATE);
            wk->z = reinterpret_cast<complex<precision>*>(wk->wk);
            workspaces.insert(pair<size_t, shared_ptr<workspace>>(ndofs, wk));
          }

          return workspaces[ndofs];
        }

        template<
          class time_precision
        >
        complex<precision>* forward(const shared_ptr<VectorEncapsulation<time_precision, precision>> x)
        {
          const size_t ndofs = x->data().size();
          shared_ptr<workspace> wk = get_workspace(ndofs);
          for (size_t i = 0; i < ndofs; i++) {
            wk->z[i] = x->data()[i];
          }
          fftw_execute_dft(wk->ffft, wk->wk, wk->wk);
          return wk->z;
        }

        void backward(shared_ptr<VectorEncapsulation<time_precision, precision>> x)
        {
          const size_t ndofs = x->data().size();
          shared_ptr<workspace> wk = get_workspace(ndofs);
          fftw_execute_dft(wk->ifft, wk->wk, wk->wk);
          for (size_t i = 0; i < ndofs; i++) {
            x->data()[i] = real(wk->z[i]);
          }
        }
    };
  }  // ::pfasst::contrib
}  // ::pfasst

#endif  // _PFASST__CONTRIB__FFTW_HPP_
