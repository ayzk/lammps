/* -*- c++ -*- ----------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   https://lammps.sandia.gov/, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------
   Contributing author: Richard Berger (Temple U)
------------------------------------------------------------------------- */

#ifdef LAMMPS_MDZ

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cassert>
#include "mdz_file_writer.h"
#include <stdio.h>
//#include "fmt/format.h"

using namespace LAMMPS_NS;

MdzFileWriter::MdzFileWriter() : FileWriter(){
}

/* ---------------------------------------------------------------------- */

MdzFileWriter::~MdzFileWriter() {
}

/* ---------------------------------------------------------------------- */

void MdzFileWriter::open(const std::string &path) {
    outfile = path;
}

/* ---------------------------------------------------------------------- */

size_t MdzFileWriter::write(const void *buffer, size_t length) {

}

/* ---------------------------------------------------------------------- */
size_t MdzFileWriter::write_float(float *buffer, size_t length) {
    std::ofstream fout(outfile, std::ios::binary);
    fout.write(reinterpret_cast<const char *>(&buffer[0]), length * sizeof(float));
    fout.close();
}

/* ---------------------------------------------------------------------- */

void MdzFileWriter::close() {
}

/* ---------------------------------------------------------------------- */

bool MdzFileWriter::isopen() const {
}

/* ---------------------------------------------------------------------- */

//void MdzFileWriter::setCompressionLevel(int level)
//{
//  if (isopen())
//    throw FileWriterException("Compression level can not be changed while file is open");
//
//  const int min_level = ZSTD_minCLevel();
//  const int max_level = ZSTD_maxCLevel();
//
//  if(level < min_level || level > max_level)
//    throw FileWriterException(fmt::format("Compression level must in the range of [{}, {}]", min_level, max_level));
//
//  compression_level = level;
//}
//
///* ---------------------------------------------------------------------- */
//
//void MdzFileWriter::setChecksum(bool enabled)
//{
//  if (isopen())
//    throw FileWriterException("Checksum flag can not be changed while file is open");
//  checksum_flag = enabled ? 1 : 0;
//}

#endif
