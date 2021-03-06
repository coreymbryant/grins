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

#include "grins_config.h"

#include <iostream>

// GRINS
#include "grins/simulation_builder.h"
#include "grins/simulation.h"
#include "grins/math_constants.h"

// libMesh
#include "libmesh/parallel.h"

// SuspendedCable
//#include "suspended_cable_solver_factory.h"

// Function for getting initial temperature field
libMesh::Real initial_values( const libMesh::Point& p, const libMesh::Parameters &params,
		                      const std::string& system_name, const std::string& unknown_name );

int main(int argc, char* argv[])
{
	// Check command line count.
	if( argc < 2 )
	{
		// TODO: Need more consistent error handling.
		std::cerr << "Error: Must specify libMesh input file." << std::endl;
		exit(1); // TODO: something more sophisticated for parallel runs?
	}

	// libMesh input file should be first argument
	std::string libMesh_input_filename = argv[1];

	// Create our GetPot object.
	GetPot libMesh_inputfile( libMesh_input_filename );

	// GetPot doesn't throw an error for a nonexistent file?
	{
		std::ifstream i(libMesh_input_filename.c_str());
		if (!i)
		{
			std::cerr << "Error: Could not read from libMesh input file "
					<< libMesh_input_filename << std::endl;
			exit(1);
		}
	}

	// Initialize libMesh library.
	libMesh::LibMeshInit libmesh_init(argc, argv);

	libMesh::out << "Starting GRINS with command:\n";
	for (int i=0; i != argc; ++i)
		libMesh::out << argv[i] << ' ';
	libMesh::out << std::endl;

	GRINS::SimulationBuilder sim_builder;

        //GRINS::SharedPtr<GRINS::SuspendedCableSolverFactory> cable_factory( new GRINS::SuspendedCableSolverFactory );

//sim_builder.attach_solver_factory( cable_factory );

	GRINS::Simulation grins( libMesh_inputfile,
						     sim_builder,
						     libmesh_init.comm() );

	std::string system_name = libMesh_inputfile( "screen-options/system_name", "GRINS" );
	GRINS::SharedPtr<libMesh::EquationSystems> es = grins.get_equation_system();
	const libMesh::System& system = es->get_system(system_name);

	libMesh::Parameters &params = es->parameters;

	system.project_solution( initial_values, NULL, params );

	grins.run();

	return 0;
}

libMesh::Real initial_values( const libMesh::Point& p, const libMesh::Parameters &/*params*/,
		     const std::string& , const std::string& unknown_name )
{
	libMesh::Real value = 0.0;


	if( unknown_name == "u" )
	{
		value = -35*sin(GRINS::Constants::pi*p(0)/400.0);
	}
	else if( unknown_name == "w" )
	{
		value = -55*sin(GRINS::Constants::pi*p(0)/200.0);
	}
	else
	{
		value = 0.0;
	}

	return value;
}
