############################
# This configuration file defines some cmake variables:
# HPR_MCMC_INCLUDE_DIRS: list of include directories for the HPR-MCMC library
# HPR_MCMC_LIBRARIES: libraries needed 
# HPR_MCMC_CXX_DEFINITIONS: definitions to enable the requested interfaces
# HPR_MCMC_MAJOR_VERSION: major version 
# HPR_MCMC_MINOR_VERSION: minor version 
# HPR_MCMC_PATH_VERSION: patch version
#
#supported components:
#

unset(HPR_MCMC_LIBRARIES )
unset(HPR_MCMC_CXX_DEFINITIONS )
unset(HPR_MCMC_INCLUDE_DIRS )

set(HPR_MCMC_MAJOR_VERSION 0)
set(HPR_MCMC_MINOR_VERSION 1)
set(HPR_MCMC_PATCH_VERSION 1)

if (MSVC)
    add_definitions(/wd4522) # multiple assignment ops for single type, to be investigated further if avoidable
endif()

if (USE_ASSERTS)
  list(APPEND HPR_MCMC_CXX_DEFINITIONS "-DHPR_MCMC_ASSERT_FOR_THROW")
endif()

if(EXISTS ${HPR_MCMC_DIR}/include/hprmcmc.hpp)
	list(APPEND HPR_MCMC_INCLUDE_DIRS "${HPR_MCMC_DIR}/include")
else()
	message(ERROR " HPR_MCMC was not found")
	# assuming a dev tree where the repos are stored along side each other, i.e.
	# .../dev/clones/universal
	# .../dev/clones/mtl4
	# .../dev/clones/hpr-blas
	# .../dev/clones/hpr-dsp
	# .../dev/clones/hpr-mcmc
	list(APPEND HPR_MCMC_INCLUDE_DIRS "${HPR_MCMC_DIR}/../hpr-mcmc/include")
endif(EXISTS ${HPR_MCMC_DIR}/include/hprmcmc.hpp)

macro(hpr_mcmc_check_cxx_compiler_flag FLAG RESULT)
  # counts entirely on compiler's return code, maybe better to combine it with check_cxx_compiler_flag
  file(WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.cxx" "int main() { return 0;}\n")
  try_compile(${RESULT}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.cxx
    COMPILE_DEFINITIONS ${FLAG})  
endmacro()

