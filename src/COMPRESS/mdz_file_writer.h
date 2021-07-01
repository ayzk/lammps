/* -*- c++ -*- ----------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   http://lammps.sandia.gov, Sandia National Laboratories
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

#ifndef LMP_MDZ_FILE_WRITER_H
#define LMP_MDZ_FILE_WRITER_H

#include "file_writer.h"
#include <string>
#include <exception>

namespace LAMMPS_NS {

class MdzFileWriter : public FileWriter {
    std::string outfile;
public:
    MdzFileWriter();
    virtual ~MdzFileWriter();
    virtual void open(const std::string &path) override;
    virtual void close() override;
    virtual void flush() override;
    virtual size_t write(const void * buffer, size_t length) override;
    virtual bool isopen() const override;

    size_t write_float(float * buffer, size_t length);

//    void setCompressionLevel(int level);
//    void setChecksum(bool enabled);
};


}

#endif
#endif
