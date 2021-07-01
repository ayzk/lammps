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

#ifdef DUMP_CLASS

DumpStyle(mdz,DumpMdz)

#else

#ifndef LMP_DUMP_MDZ_H
#define LMP_DUMP_MDZ_H

#include "dump.h"
#include "mdz_file_writer.h"

namespace LAMMPS_NS {

class DumpMdz : public Dump {
 public:
  DumpMdz(class LAMMPS *, int, char **);
  virtual ~DumpMdz();

 protected:
  MdzFileWriter xwriter,ywriter,zwriter;
  std::vector<float> xbuf,ybuf,zbuf;
  size_t nAtom=-1;
  size_t nFrame=10;
  size_t iFrame=0, iAtom=0;

  void pack(tagint *);
  void init_style();

   void openfile();
   void write_header(bigint);
   void write_data(int, double *);
   void write();

   int modify_param(int, char **);
};

}

#endif
#endif
#endif

/* ERROR/WARNING messages:

E: Dump mdz only writes compressed files

The dump mdz output file name must have a .mdz suffix.

E: Cannot open dump file

Self-explanatory.

*/
