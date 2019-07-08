//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MatrixSparsityTestApp.h"
#include "MooseInit.h"
#include "Moose.h"
#include "MooseApp.h"
#include "AppFactory.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>
#include <string>

#include "libmesh/libmesh.h"
#include "libmesh/dense_matrix.h"
#include "libmesh/petsc_matrix.h"

// Create a performance log
PerfLog Moose::perf_log("MatrixSparsity");

// Begin the main program.
int
main(int argc, char * argv[])
{
  // Initialize MPI, solvers and MOOSE
  MooseInit init(argc, argv);

  const unsigned int n_dofs = 26559;
  unsigned int first_local_index;
  unsigned int last_local_index;
  const unsigned int np = init.comm().size();
  if ( np == 1 )
  {
    first_local_index = 0;
    last_local_index = 26559;
  }
  else if ( np == 2 )
  {
    if ( init.comm().rank() == 0 )
    {
      first_local_index = 0;
      last_local_index = 13911;
    }
    else
    {
      first_local_index = 13911;
      last_local_index = 26559;
    }
  }
  else
  {
    std::cout << "Not implemented for np>2";
    return 1;
  }

  // Read element dof indices from files
  std::vector< std::vector< std::vector< dof_id_type > > > elem_dof_indices(np);
  for ( unsigned int proc_id = 0; proc_id < np; ++proc_id )
  {
    const std::string dof_fname = "dof_indices_np_" + std::to_string(np) +
                                  "_proc_" + std::to_string(proc_id) + ".txt";
    std::string line;
    std::ifstream dof_file(dof_fname);
    if (dof_file.is_open())
    {
      while ( getline (dof_file,line) )
      {
        std::vector< dof_id_type > dof_indices;
        std::stringstream sstream(line);
        std::string token;
        while (std::getline(sstream, token, ' '))
        {
          dof_indices.push_back(std::stoi(token));
        }
        elem_dof_indices[proc_id].push_back(dof_indices);
      }
      dof_file.close();
    }
    else
    {
      std::cout << "Could not open file";
      return 1;
    }
  }

  // Set up the (global) sparsity pattern
  std::vector< std::set< dof_id_type > > sparsity(n_dofs);
  for ( unsigned int proc_id = 0; proc_id < np; ++proc_id )
  {
    for ( auto & dof_indices : elem_dof_indices[proc_id] )
    {
      for ( unsigned int i = 0; i < dof_indices.size(); ++i )
      {
        for ( unsigned int j = 0; j < dof_indices.size(); ++j )
        {
          sparsity[dof_indices[i]].insert(dof_indices[j]);
        }
      }
    }
  }

  // Determine the local nonzeros on this processor
  const unsigned int n_local_dofs = last_local_index - first_local_index;
  std::vector< numeric_index_type > n_nz( n_local_dofs, 0 );
  std::vector< numeric_index_type > n_oz( n_local_dofs, 0 );

  for ( unsigned int i = 0; i < n_local_dofs; ++i )
  {
    for ( auto dof : sparsity[i+first_local_index] )
    {
      if ( ( dof >= first_local_index ) && ( dof < last_local_index ) )
      {
        n_nz[i]++;
      }
      else
      {
        n_oz[i]++;
      }
    }
  }

  // Initialize the matrix
  PetscMatrix< Number > B(init.comm());
  B.init( n_dofs, n_dofs, n_local_dofs, n_local_dofs, n_nz, n_oz );

  // Local element loop
  for ( auto & dof_indices : elem_dof_indices[init.comm().rank()] )
  {
    DenseMatrix< Number >  zero_mat( dof_indices.size(), dof_indices.size() );
    B.add_matrix( zero_mat, dof_indices );
  }

  // Close the matrix
  std::cout << "Close matrix for np = " << np << " ... " << std::endl;
  B.close();
  std::cout << "Matrix successfully closed" << std::endl;

  return 0;
}
