# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "D:/ProjektyWCplusplus/BattleDuel/cmake-build-release-visual-studio/_deps/miniaudio-src")
  file(MAKE_DIRECTORY "D:/ProjektyWCplusplus/BattleDuel/cmake-build-release-visual-studio/_deps/miniaudio-src")
endif()
file(MAKE_DIRECTORY
  "D:/ProjektyWCplusplus/BattleDuel/cmake-build-release-visual-studio/_deps/miniaudio-build"
  "D:/ProjektyWCplusplus/BattleDuel/cmake-build-release-visual-studio/_deps/miniaudio-subbuild/miniaudio-populate-prefix"
  "D:/ProjektyWCplusplus/BattleDuel/cmake-build-release-visual-studio/_deps/miniaudio-subbuild/miniaudio-populate-prefix/tmp"
  "D:/ProjektyWCplusplus/BattleDuel/cmake-build-release-visual-studio/_deps/miniaudio-subbuild/miniaudio-populate-prefix/src/miniaudio-populate-stamp"
  "D:/ProjektyWCplusplus/BattleDuel/cmake-build-release-visual-studio/_deps/miniaudio-subbuild/miniaudio-populate-prefix/src"
  "D:/ProjektyWCplusplus/BattleDuel/cmake-build-release-visual-studio/_deps/miniaudio-subbuild/miniaudio-populate-prefix/src/miniaudio-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/ProjektyWCplusplus/BattleDuel/cmake-build-release-visual-studio/_deps/miniaudio-subbuild/miniaudio-populate-prefix/src/miniaudio-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/ProjektyWCplusplus/BattleDuel/cmake-build-release-visual-studio/_deps/miniaudio-subbuild/miniaudio-populate-prefix/src/miniaudio-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
