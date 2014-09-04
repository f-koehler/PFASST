#ifndef _PFASST__QUADRATURE__GAUSS_RADAU_HPP_
#define _PFASST__QUADRATURE__GAUSS_RADAU_HPP_

#include <cassert>
#include <vector>

#include <Eigen/Dense>

#include "../interfaces.hpp"
#include "polynomial.hpp"
#include "interface.hpp"
#include "traits.hpp"

template<typename scalar>
using Matrix = Eigen::Matrix<scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

template<typename scalar>
using Index = typename Matrix<scalar>::Index;

using namespace std;


namespace pfasst
{
  namespace quadrature
  {
    template<typename precision = pfasst::time_precision>
    class GaussRadau;

    template<typename precision>
    struct quadrature_traits<GaussRadau<precision>>
    {
      typedef pfasst::quadrature::gauss_radau integral_constant;
      static const bool left_is_node = false;
      static const bool right_is_node = true;
    };

    template<typename precision>
    class GaussRadau
      : public IQuadrature<precision>
    {
      public:
        //! @{
        GaussRadau(const size_t num_nodes)
          : IQuadrature<precision>(num_nodes)
        {
          if (this->m_num_nodes < 2) {
            throw invalid_argument("Gauss-Radau quadrature requires at least two quadrature nodes.");
          }
          this->compute_nodes();
          this->compute_weights();
          this->compute_delta_nodes();
        }

        GaussRadau()
          : IQuadrature<precision>()
        {}

        GaussRadau(const GaussRadau<precision>& other)
          : IQuadrature<precision>(other)
        {}

        GaussRadau(GaussRadau<precision>&& other)
          : GaussRadau<precision>()
        {
          swap(*this, other);
        }

        virtual ~GaussRadau()
        {}
        //! @}

        //! @{
        virtual bool left_is_node() const
        { return quadrature_traits<GaussRadau<precision>>::left_is_node; }

        virtual bool right_is_node() const
        { return quadrature_traits<GaussRadau<precision>>::right_is_node; }
        //! @}

        //! @{
        GaussRadau<precision>& operator=(GaussRadau<precision> other)
        {
          swap(*this, other);
          return *this;
        }
        //! @}

      protected:
        //! @{
        virtual void compute_nodes() override
        {
          this->m_nodes = vector<precision>(this->m_num_nodes, precision(0.0));
          auto l   = Polynomial<precision>::legendre(this->m_num_nodes);
          auto lm1 = Polynomial<precision>::legendre(this->m_num_nodes - 1);

          for (size_t i = 0; i < this->m_num_nodes; i++) {
            l[i] += lm1[i];
          }
          auto roots = l.roots();
          for (size_t j = 1; j < this->m_num_nodes; j++) {
            this->m_nodes[j - 1] = 0.5 * (1.0 - roots[this->m_num_nodes - j]);
          }
          this->m_nodes.back() = 1.0;
        }
        //! @}
    };
  }  // ::pfasst::quadrature
}  // ::pfasst

#endif  // _PFASST__QUADRATURE__GAUSS_RADAU_HPP_
