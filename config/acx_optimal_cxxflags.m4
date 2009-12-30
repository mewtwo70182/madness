AC_DEFUN([ACX_OPTIMAL_CXXFLAGS], [
    AC_DETECT_CXX
    AC_LANG_PUSH([C++])

    save_CXXFLAGS="$CXXFLAGS"
    case $CXXVENDOR in
         GNU)
            # Delete trailing -stuff from X.X.X-stuff then parse
            CXXVERSION=[`$CXX -dumpversion | sed -e 's/-.*//'`]
            CXXMAJOR=[`echo $CXXVERSION | sed -e 's/\.[.0-9a-zA-Z\-_]*//'`]
            CXXMINOR=[`echo $CXXVERSION | sed -e 's/[0-9]*\.//' -e 's/\.[0-9]*//'`]
            CXXMICRO=[`echo $CXXVERSION | sed -e 's/[0-9]*\.[0-9]*\.//'`]
            echo "Setting compiler flags for GNU C++ major=$CXXMAJOR minor=$CXXMINOR micro=$CXXMICRO"

            CXXFLAGS="-g -Wall -Wno-strict-aliasing -Wno-deprecated -ansi -O3 -ffast-math"

            if test "x$HAVE_CRAYXT" = xyes; then
                echo "Setting target architecture for GNU compilers to barcelona for CRAYXT"
                CXXFLAGS="$CXXFLAGS -march=barcelona"
            else
                CXXFLAGS_SAVE="$CXXFLAGS"
                echo "TESTING NATIVE"
                CXXFLAGS="$CXXFLAGS -march=native"
                AC_TRY_COMPILE( ,  , [], [CXXFLAGS="$CXXFLAGS_SAVE"])
            fi

            case $host_cpu in
                ?86*)
                   CXXFLAGS="$CXXFLAGS  -mfpmath=sse -msse"
                   ;;
            esac
         ;;

         Pathscale)
            CXXFLAGS="-Wall -Ofast"
            if test "x$HAVE_CRAYXT" = xyes; then
                echo "Setting Pathscale CXX architecture to -barcelona for Cray-XT"
                CXXFLAGS="$CXXFLAGS -march=barcelona"             
            fi
            ;;

         Portland)
            CXXFLAGS="-O3 -fastsse -Mflushz -Mcache_align"    
            echo "Appending -pgf90libs to LIBS so can link against Fortran BLAS/linalg"
            LIBS="$LIBS -pgf90libs"
            if test "x$HAVE_CRAYXT" = xyes; then
                echo "Setting PGI CXX architecture to -tp barcelona-64 for Cray-XT"
                CXXFLAGS="$CXXFLAGS -tp barcelona-64"             
            fi
            ;;

         Intel)
            CXXFLAGS="-Wall -fast -ansi"
            ;;

         unknown)
            echo "UNKNOWN COMPILER?"
            ;;

         *)
            ;;
    esac
    CXXFLAGS="$CXXFLAGS -D_REENTRANT "
    echo "Changing CXXFLAGS from '$save_CXXFLAGS'"
    echo "to '$CXXFLAGS'"

    AC_LANG_POP([C++])
])
