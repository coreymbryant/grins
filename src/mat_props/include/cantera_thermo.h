//-----------------------------------------------------------------------bl-
//--------------------------------------------------------------------------
// 
// GRINS - General Reacting Incompressible Navier-Stokes 
//
// Copyright (C) 2010-2012 The PECOS Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the Version 2 GNU General
// Public License as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this library; if not, write to the Free Software
// Foundation, Inc. 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301 USA
//
//-----------------------------------------------------------------------el-
//
// $Id$
//
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

#ifndef GRINS_CANTERA_THERMO_H
#define GRINS_CANTERA_THERMO_H

// libMesh
#include "getpot.h"

// GRINS
#include "chemical_mixture.h"
#include "cached_values.h"
#include "cantera_singleton.h"

#ifdef GRINS_HAVE_CANTERA

namespace GRINS
{
  class CanteraThermodynamics
  {
  public:

    CanteraThermodynamics( const GetPot& input, const ChemicalMixture& chem_mixture );
    ~CanteraThermodynamics();

    Real cp( const CachedValues& cache, unsigned int qp ) const;

    Real cv( const CachedValues& cache, unsigned int qp ) const;
     
    Real h(const CachedValues& cache, unsigned int qp, unsigned int species) const;

    void h(const CachedValues& cache, unsigned int qp, std::vector<Real>& h) const;

    //! This is just a dummy for the GRINS interface. Returns 0.0.
    /*! \todo Should look into actually implementing this because the functionality is probably there. */
    Real h_RT_minus_s_R( const CachedValues& cache, unsigned int qp, unsigned int species ) const;

    //! This is just a dummy for the GRINS interface. Returns 0.0.
    /*! \todo Should look into actually implementing this because the functionality is probably there. */
    void h_RT_minus_s_R( const CachedValues& cache, unsigned int qp,
			 std::vector<Real>& h_RT_minus_s_R) const;

  protected:

    const ChemicalMixture& _chem_mixture;

    Cantera::IdealGasMix& _cantera_gas;

  private:

    CanteraThermodynamics();

  };

  /* ------------------------- Inline Functions -------------------------*/
  inline
  Real CanteraThermodynamics::h_RT_minus_s_R( const CachedValues& /*cache*/,
					      unsigned int /*qp*/, 
					      unsigned int /*species*/ ) const
  {
    return 0.0;
  }

  inline
  void CanteraThermodynamics::h_RT_minus_s_R( const CachedValues& /*cache*/, unsigned int /*qp*/,
					      std::vector<Real>& h_RT_minus_s_R ) const
  {
    std::fill( h_RT_minus_s_R.begin(), h_RT_minus_s_R.end(), 0.0 );
    return;
  }

} // namespace GRINS

#endif //GRINS_HAVE_CANTERA

#endif //GRINS_CANTERA_THERMO_H
