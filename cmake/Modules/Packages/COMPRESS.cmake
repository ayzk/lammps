find_package(ZLIB REQUIRED)
target_link_libraries(lammps PRIVATE ZLIB::ZLIB)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Zstd IMPORTED_TARGET libzstd>=1.4)
pkg_check_modules(ZSTD IMPORTED_TARGET libzstd>=1.4)

if(Zstd_FOUND)
    target_compile_definitions(lammps PRIVATE -DLAMMPS_ZSTD)
    target_link_libraries(lammps PRIVATE PkgConfig::Zstd)
endif()

find_package(SZ3 PATHS $ENV{HOME}/code/sz3/install)
if (SZ3_FOUND AND Zstd_FOUND)
    target_link_libraries(lammps PRIVATE SZ3::SZ3)
    target_compile_definitions(lammps PRIVATE -DLAMMPS_MDZ)
endif()