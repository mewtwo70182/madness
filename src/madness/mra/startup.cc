/*
  This file is part of MADNESS.

  Copyright (C) 2007,2010 Oak Ridge National Laboratory

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  For more information please contact:

  Robert J. Harrison
  Oak Ridge National Laboratory
  One Bethel Valley Road
  P.O. Box 2008, MS-6367

  email: harrisonrj@ornl.gov
  tel:   865-241-3937
  fax:   865-572-0680


  $Id$
*/


/// \file mra/startup.cc

#include <madness/mra/mra.h>
//#include <madness/mra/mraimpl.h> !!!!!!!!!!!!!!!!  NOOOOOOOOOOOOOOOOOOOOOOOOOO !!!!!!!!!!!!!!!!!!!!!!!
#include <iomanip>
#include <cstdlib>

namespace madness {
    void startup(World& world, int argc, char** argv) {
        const char* data_dir = MRA_DATA_DIR;

        // Process command line arguments
        for (int arg=1; arg<argc; ++arg) {
            if (strcmp(argv[arg],"-dx")==0)
                xterm_debug(argv[0], 0);
            else if (strcmp(argv[arg],"-lf")==0)
                redirectio(world);
            else if (strcmp(argv[arg],"-dn") ==0 &&
                     std::atoi(argv[arg+1])==world.rank())
                xterm_debug("world",0);
//             else if (strcmp(argv[arg],"-dam")==0)
//                 world.am.set_debug(true);
//             else if (strcmp(argv[arg],"-dmpi")==0)
//                 world.mpi.set_debug(true);
            else if (strcmp(argv[arg],"-rio")==0)
                redirectio(world);
        }

        // Process environment variables
        if (getenv("MRA_DATA_DIR")) data_dir = getenv("MRA_DATA_DIR");

        // Need to add an RC file ...

        world.gop.fence();

	init_tensor_lapack();

        std::cout << std::boolalpha;  // Pretty printing of booleans
        std::cout << std::scientific;
        std::cout << std::showpoint;
        //std::cout << std::showpos;
        std::cout << std::setprecision(6);

#ifdef FUNCTION_INSTANTIATE_1
        FunctionDefaults<1>::set_defaults(world);
        Displacements<1> d1;
#endif
#ifdef FUNCTION_INSTANTIATE_2
        FunctionDefaults<2>::set_defaults(world);
        Displacements<2> d2;
#endif
#ifdef FUNCTION_INSTANTIATE_3
        FunctionDefaults<3>::set_defaults(world);
        Displacements<3> d3;
#endif
#ifdef FUNCTION_INSTANTIATE_4
        FunctionDefaults<4>::set_defaults(world);
        Displacements<4> d4;
#endif
#ifdef FUNCTION_INSTANTIATE_5
        FunctionDefaults<5>::set_defaults(world);
        Displacements<5> d5;
#endif
#ifdef FUNCTION_INSTANTIATE_6
        FunctionDefaults<6>::set_defaults(world);
        Displacements<6> d6;
#endif

        //if (world.rank() == 0) print("loading coeffs, etc.");

        load_coeffs(world, data_dir);

        //if (world.rank() == 0) print("loading quadrature, etc.");

        load_quadrature(world, data_dir);

        // This to init static data while single threaded
        initialize_legendre_stuff();

        //if (world.rank() == 0) print("testing coeffs, etc.");
        MADNESS_ASSERT(gauss_legendre_test());
        MADNESS_ASSERT(test_two_scale_coefficients());

        // print the configuration options
        if (world.rank() == 0) {
            print("");
            print("--------------------------------------------");
            print("   MADNESS",PACKAGE_VERSION, "multiresolution suite");
            print("--------------------------------------------");
            print("");
            print("   number of processors ...", world.size());
            print("    processor frequency ...", cpu_frequency());
            print("            host system ...", HOST_SYSTEM);
            print("          configured by ...", MADNESS_CONFIGURATION_USER);
            print("          configured on ...", MADNESS_CONFIGURATION_HOST);
            print("          configured at ...", MADNESS_CONFIGURATION_DATE);
            print("                    CXX ...", MADNESS_CONFIGURATION_CXX);
            print("               CXXFLAGS ...", MADNESS_CONFIGURATION_CXXFLAGS);
#ifdef OPTERON_TUNE
            print("             tuning for ...", "opteron");
#elif defined(CORE_DUO_TUNE)
            print("             tuning for ...", "core duo");
#else
            print("             tuning for ...", "default");
#endif
#ifdef BOUNDS_CHECKING
            print(" tensor bounds checking ...", "enabled");
#endif
#ifdef TENSOR_INSTANCE_COUNT
            print("  tensor instance count ...", "enabled");
#endif
#if HAVE_INTEL_TBB
            print("              Intel TBB ...", "yes ");
#else
            print("              Intel TBB ...", "no ");
#endif
           	print("               compiled ...",__TIME__," on ",__DATE__);

            //         print(" ");
            //         IndexIterator::test();
        }


        world.gop.fence();
    }
}
