//-----------------------------------------------------------------------bl-
//--------------------------------------------------------------------------
//
// GRINS - General Reacting Incompressible Navier-Stokes
//
// Copyright (C) 2014-2016 Paul T. Bauman, Roy H. Stogner
// Copyright (C) 2010-2013 The PECOS Development Team
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the Version 2.1 GNU Lesser General
// Public License as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc. 51 Franklin Street, Fifth Floor,
// Boston, MA  02110-1301  USA
//
//-----------------------------------------------------------------------el-

// This class
#include "grins/gas_recombination_catalytic_wall_neumann_bc_factory_impl.h"

// GRINS
#include "grins/string_utils.h"

#ifdef GRINS_HAVE_CANTERA
#include "grins/cantera_mixture.h"
#endif

#ifdef GRINS_HAVE_ANTIOCH
#include "grins/antioch_chemistry.h"
#endif

namespace GRINS
{
  // To avoid compiler warnings without GRINS or Cantera
#if defined(GRINS_HAVE_ANTIOCH) || defined(GRINS_HAVE_CANTERA)
  SharedPtr<NeumannBCAbstract>
  GasRecombinationCatalyticWallNeumannBCFactoryImpl::build_catalytic_wall
  ( const GetPot& input, const std::string& reaction,SharedPtr<CatalycityBase>& gamma_ptr,
    const std::vector<VariableIndex>& species_vars,const std::string& material,
    VariableIndex T_var,libMesh::Real p0,const std::string& thermochem_lib )
#else
  SharedPtr<NeumannBCAbstract>
  GasRecombinationCatalyticWallNeumannBCFactoryImpl::build_catalytic_wall
  ( const GetPot& /*input*/, const std::string& reaction,SharedPtr<CatalycityBase>& /*gamma_ptr*/,
    const std::vector<VariableIndex>& /*species_vars*/,const std::string& /*material*/,
    VariableIndex /*T_var*/,libMesh::Real /*p0*/,const std::string& thermochem_lib )
#endif
  {
    std::string reactant;
    std::string product;
    this->parse_reactant_and_product(reaction,reactant,product);

    // Now construct the Neumann BC
    SharedPtr<NeumannBCAbstract> catalytic_wall;

    if( thermochem_lib == "cantera" )
      {
#ifdef GRINS_HAVE_CANTERA
        this->build_wall_ptr<CanteraMixture>(input,material,gamma_ptr,reactant,product,
                                             species_vars,T_var,p0,catalytic_wall);
#else
         libmesh_error_msg("Error: Cantera not enabled in this configuration. Reconfigure using --with-cantera option.");
#endif
      }
    else if( thermochem_lib == "antioch" )
      {
#ifdef GRINS_HAVE_ANTIOCH
       this->build_wall_ptr<AntiochChemistry>(input,material,gamma_ptr,reactant,product,
                                             species_vars,T_var,p0,catalytic_wall);
#else
         libmesh_error_msg("Error: Antioch not enabled in this configuration. Reconfigure using --with-antioch option.");
#endif
      }
    else
      libmesh_error_msg("ERROR: Invalid thermochemistry library "+thermochem_lib+"!");

    return catalytic_wall;
  }

  void GasRecombinationCatalyticWallNeumannBCFactoryImpl::parse_reactant_and_product( const std::string& reaction,
                                                                                      std::string& reactant,
                                                                                      std::string& product ) const
  {
    // Split each reaction into reactants and products
    std::vector<std::string> partners;
    StringUtilities::split_string(reaction, "->", partners);

    /*! \todo We currently can only handle reactions of the type R -> P
      e.g. not R1+R2 -> P, etc. */
    if( partners.size() != 2 )
      libmesh_error_msg("ERROR: Must have only one reactant and one product for GasRecombinationCatalyticWall!");

    reactant = partners[0];
    product = partners[1];
  }

} // end namespace GRINS
