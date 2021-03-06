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
#include "grins/variable_factory.h"

// GRINS
#include "grins/materials_parsing.h"
#include "grins/single_variable.h"
#include "grins/multicomponent_variable.h"
#include "grins/multi_component_vector_variable.h"

namespace GRINS
{
  libMesh::UniquePtr<FEVariablesBase> VariableFactoryAbstract::create()
  {
    // Make sure all necessary state has been setup
    this->check_create_state();

    libMesh::UniquePtr<FEVariablesBase> func;

    func = this->build_fe_var( *_var_names, *_var_indices, *_subdomain_ids );

    // Reset state for error checking during next construction
    this->reset_create_state();

    return func;
  }

  std::vector<std::string> VariableFactoryAbstract::build_var_names( const std::string& name )
  {
    check_build_parse_state();

    VariableFactoryAbstract& factory = get_factory_subclass<VariableFactoryAbstract>(name);

    std::vector<std::string> var_names;
    var_names = factory.parse_var_names( *_input, _var_section );

    reset_build_parse_state();

    return var_names;
  }

  std::string VariableFactoryAbstract::parse_fe_family( const std::string& name )
  {
    check_build_parse_state();

    VariableFactoryAbstract& factory = get_factory_subclass<VariableFactoryAbstract>(name);

    std::string fe_family;
    fe_family = factory.parse_fe_family_impl( *_input, _var_section );

    reset_build_parse_state();

    return fe_family;
  }

  std::string VariableFactoryAbstract::parse_fe_order( const std::string& name )
  {
    check_build_parse_state();

    VariableFactoryAbstract& factory = get_factory_subclass<VariableFactoryAbstract>(name);

    std::string order;
    order = factory.parse_fe_order_impl( *_input, _var_section );

    reset_build_parse_state();

    return order;
  }

  void VariableFactoryAbstract::check_create_state() const
  {
    if( !this->_var_names )
      libmesh_error_msg("ERROR: must call set_var_names() before building FEVariablesBase!");

    if( !this->_var_indices )
      libmesh_error_msg("ERROR: must call set_var_indices() before building FEVariablesBase!");

    if( !this->_subdomain_ids )
      libmesh_error_msg("ERROR: must call set_subdomain_ids() before building FEVariablesBase!");
  }

  void VariableFactoryAbstract::reset_create_state()
  {
    _var_names = NULL;
    _var_indices = NULL;
    _subdomain_ids = NULL;
  }

  void VariableFactoryAbstract::check_build_parse_state()
  {
    if( !_input )
      libmesh_error_msg("ERROR: Must call set_getpot() before calling VariableFactoryAbstract::build_var_names!");

    if( _var_section == std::string("DIE!") )
      libmesh_error_msg("ERROR: Must call set_var_section() before calling VariableFactoryAbstract::build_var_names!");
  }

  void VariableFactoryAbstract::reset_build_parse_state()
  {
    _input = NULL;
    _var_section = std::string("DIE!");
  }

  template<typename VariableType>
  std::vector<std::string> VariableFactoryBasic<VariableType>::parse_var_names( const GetPot& input,
                                                                                const std::string& var_section )
  {
    std::vector<std::string> var_names;

    std::string input_sec = var_section+"/names";

    // Make sure the names are present
    if( !input.have_variable(input_sec) )
      libmesh_error_msg("ERROR: Could not find input parameter "+input_sec);

    unsigned int n_names = input.vector_variable_size(input_sec);

    var_names.resize(n_names);
    for( unsigned int i = 0; i < n_names; i++ )
      var_names[i] = input(input_sec,std::string("DIE!"),i);

    return var_names;
  }

  template<typename VariableType>
  std::vector<std::string> SpeciesVariableFactory<VariableType>::parse_var_names( const GetPot& input,
                                                                                  const std::string& var_section )
  {
    // Make sure the prefix is present
    std::string prefix_sec = var_section+"/names";
    if( !input.have_variable(prefix_sec) )
      libmesh_error_msg("ERROR: Could not find input parameter "+prefix_sec+" for species prefix!");

    // Make sure the material is present
    std::string material_sec = var_section+"/material";
    if( !input.have_variable(material_sec) )
      libmesh_error_msg("ERROR: Could not find input parameter "+material_sec+" for species material!");

    this->_prefix = input(prefix_sec,std::string("DIE!"));
    this->_material = input(material_sec,std::string("DIE!"));

    std::vector<std::string> var_names;
    MaterialsParsing::parse_species_varnames(input,this->_material,this->_prefix,var_names);

    return var_names;
  }

  // Full specialization for the Factory<FEVariablesBase>
  template<>
  std::map<std::string, FactoryAbstract<FEVariablesBase>*>&
  FactoryAbstract<FEVariablesBase>::factory_map()
  {
    static std::map<std::string, FactoryAbstract<FEVariablesBase>*> _map;
    return _map;
  }

  // Definition of static members
  template<>
  const GetPot* FactoryWithGetPot<FEVariablesBase>::_input = NULL;
  const std::vector<std::string>* VariableFactoryAbstract::_var_names = NULL;
  const std::vector<VariableIndex>* VariableFactoryAbstract::_var_indices = NULL;
  std::string VariableFactoryAbstract::_var_section = std::string("DIE!");
  const std::set<libMesh::subdomain_id_type>* VariableFactoryAbstract::_subdomain_ids = NULL;

  VariableFactoryBasic<DisplacementVariable>
  grins_factory_disp_fe_var(VariablesParsing::displacement_section());

  VariableFactoryBasic<SingleVariable>
  grins_factory_single_var(VariablesParsing::single_var_section());

  VariableFactoryBasic<PressureFEVariable>
  grins_factory_press_fe_var(VariablesParsing::pressure_section());

  VariableFactoryBasic<PrimitiveTempFEVariables>
  grins_factory_temp_fe_var(VariablesParsing::temperature_section());

  SpeciesVariableFactory<SpeciesMassFractionsVariable>
  grins_factory_species_mass_frac_fe_var(VariablesParsing::species_mass_fractions_section());

  ScalarVariableFactory<ThermoPressureVariable>
  grins_factory_thermo_press_fe_var(VariablesParsing::thermo_pressure_section());

  VariableFactoryBasic<TurbulenceFEVariables>
  grins_factory_turb_fe_var(VariablesParsing::turbulence_section());

  VariableFactoryBasic<VelocityVariable>
  grins_factory_velocity_fe_var(VariablesParsing::velocity_section());

  ScalarVariableFactory<ScalarVariable>
  grins_factory_scalar_var(VariablesParsing::scalar_var_section());

} // end namespace GRINS
