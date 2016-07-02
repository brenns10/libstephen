# Tries to find libedit headers and libraries
#
# Usage of this module as follows:
#
#     find_package(Libedit)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  LIBEDIT_ROOT_DIR   Set this variable to the root installation of
#                     editline if the module has problems finding
#                     the proper installation path.
#
# Variables defined by this module:
#
#  LIBEDIT_FOUND              System has Editline libs/headers
#  LIBEDIT_LIBRARIES          The Editline libraries
#  LIBEDIT_INCLUDE_DIR        The location of Editline headers

find_path(LIBEDIT_INCLUDE_DIR
          NAMES histedit.h
          HINTS ${LIBEDIT_ROOT_DIR}/include)

find_library(LIBEDIT_LIBRARIES
             NAMES edit
             HINTS ${LIBEDIT_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Libedit
                                  DEFAULT_MSG
                                  LIBEDIT_LIBRARIES
                                  LIBEDIT_INCLUDE_DIR)

mark_as_advanced(LIBEDIT_ROOT_DIR
                 LIBEDIT_LIBRARIES
                 LIBEDIT_INCLUDE_DIR)
