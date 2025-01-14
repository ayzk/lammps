/* ----------------------------------------------------------------------
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
   Contributing author: Kai Zhao (UC Riverside)
------------------------------------------------------------------------- */

#ifdef LAMMPS_MDZ

#include "atom.h"
#include "memory.h"
#include "update.h"
#include "error.h"
#include "dump_mdz.h"


#include <cassert>
#include <cstring>

using namespace LAMMPS_NS;

DumpMdz::DumpMdz(LAMMPS *lmp, int narg, char **arg) :
        Dump(lmp, narg, arg) {
    compressed = 1;
    binary = 1;
    multifile = 0;
    multiproc = 0;
    size_one = 3;

//  if (!compressed)
//    error->all(FLERR,"Dump mdz only writes compressed files");
}


/* ---------------------------------------------------------------------- */

DumpMdz::~DumpMdz() {
}

void DumpMdz::init_style() {

}

/* ----------------------------------------------------------------------
   generic opening of a dump file
   ASCII or binary or gzipped
   some derived classes override this function
------------------------------------------------------------------------- */

void DumpMdz::openfile() {
    // each proc with filewriter = 1 opens a file
    if (filewriter) {
        if (append_flag) {
            error->one(FLERR, "dump mdz currently doesn't support append");
        }
        char *filestar = filename;
        char *ptr = strchr(filestar, '*');
        if (ptr == NULL) {
            error->one(FLERR, "dump mdz currently requires * in file name");
        }
        *ptr = '\0';

        try {
            char *filecurrent = new char[strlen(filestar) + 16];
            sprintf(filecurrent, "%s" BIGINT_FORMAT ".%c" "%s",
                    filestar, update->ntimestep, 'x', ptr + 1);
            xwriter.open(filecurrent);
            sprintf(filecurrent, "%s" BIGINT_FORMAT ".%c" "%s",
                    filestar, update->ntimestep, 'y', ptr + 1);
            ywriter.open(filecurrent);
            sprintf(filecurrent, "%s" BIGINT_FORMAT ".%c" "%s",
                    filestar, update->ntimestep, 'z', ptr + 1);
            zwriter.open(filecurrent);
        } catch (FileWriterException &e) {
            error->one(FLERR, e.what());
        }

        *ptr = '*';
    }

}

void DumpMdz::write_header(bigint ndump) {
    if (nAtom == -1) {
        nAtom = ndump;
        xbuf.resize(nAtom * nFrame, 0);
        ybuf.resize(nAtom * nFrame, 0);
        zbuf.resize(nAtom * nFrame, 0);
    }
    iAtom = 0;
//  if (me == 0) {
//    std::string header = fmt::format("{}\n", ndump);
//    header += fmt::format("Atoms. Timestep: {}\n", update->ntimestep);
//    writer.write(header.c_str(), header.length());
//  }
}

void DumpMdz::pack(tagint *ids) {
    int m, n;

    tagint *tag = atom->tag;
    int *type = atom->type;
    int *mask = atom->mask;
    double **x = atom->x;
    int nlocal = atom->nlocal;

    m = n = 0;
    for (int i = 0; i < nlocal; i++)
        if (mask[i] & groupbit) {
//      buf[m++] = tag[i];
//      buf[m++] = type[i];
            buf[m++] = x[i][0];
            buf[m++] = x[i][1];
            buf[m++] = x[i][2];
            if (ids) ids[n++] = tag[i];
        }
}

/* ---------------------------------------------------------------------- */

void DumpMdz::write_data(int n, double *mybuf) {
    size_t idx = iFrame * nAtom + iAtom;
    assert(idx + n <= xbuf.size());
    for (size_t i = 0; i < n; i++) {
        xbuf[idx + i] = mybuf[i * 3];
        ybuf[idx + i] = mybuf[i * 3 + 1];
        zbuf[idx + i] = mybuf[i * 3 + 2];
    }
    iAtom += n;
}

/* ---------------------------------------------------------------------- */

void DumpMdz::write() {
    Dump::write();
    if (nprocs < 3) {
        if (filewriter) {
            iFrame++;
            if (iFrame == nFrame) {
                iFrame = 0;
                openfile();
                int m = xwriter.write_float_compress(xbuf.data(), nFrame, nAtom, -1);
                ywriter.write_float_compress(ybuf.data(), nFrame, nAtom, m);
                zwriter.write_float_compress(zbuf.data(), nFrame, nAtom, m);
                xwriter.close();
                ywriter.close();
                zwriter.close();
            }
        }
    } else {
        int yproc = nprocs / 3;
        int zproc = nprocs / 3 * 2;
        if (filewriter || me == yproc || me == zproc) {
            iFrame++;
            if (iFrame == nFrame) {
                iFrame = 0;
                char *filestar = filename;
                char *ptr = strchr(filestar, '*');
                if (ptr == NULL) {
                    error->one(FLERR, "dump mdz currently requires * in file name");
                }
                *ptr = '\0';
                char *filecurrent = new char[strlen(filestar) + 16];

                try {
                    int tmp;
                    MPI_Status status;
                    MPI_Request request;
                    if (filewriter) {
                        MPI_Recv(&tmp, 0, MPI_INT, yproc, 0, world, MPI_STATUS_IGNORE);
                        MPI_Rsend(ybuf.data(), ybuf.size(), MPI_FLOAT, yproc, 0, world);

                        MPI_Recv(&tmp, 0, MPI_INT, zproc, 0, world, MPI_STATUS_IGNORE);
                        MPI_Rsend(zbuf.data(), zbuf.size(), MPI_FLOAT, zproc, 0, world);

                        sprintf(filecurrent, "%s" BIGINT_FORMAT ".%c" "%s",
                                filestar, update->ntimestep, 'x', ptr + 1);
                        assert(nAtom == ntotal);

                    }
                    nAtom = ntotal;
                    if (me == yproc || me == zproc) {
                        xbuf.resize(nFrame * nAtom);
                        MPI_Irecv(xbuf.data(), xbuf.size(), MPI_FLOAT, 0, 0, world, &request);
                        MPI_Send(&tmp, 0, MPI_INT, 0, 0, world);
                        MPI_Wait(&request, &status);
//                    MPI_Get_count(&status, MPI_DOUBLE, &nlines);

                        sprintf(filecurrent, "%s" BIGINT_FORMAT ".%c" "%s",
                                filestar, update->ntimestep, (me == yproc ? 'y' : 'z'), ptr + 1);
                    }

                    xwriter.open(filecurrent);
                    xwriter.write_float_compress(xbuf.data(), nFrame, nAtom, -1);
                    xwriter.close();
                } catch (FileWriterException &e) {
                    error->one(FLERR, e.what());
                }

                *ptr = '*';
            }
        }
    }
}

/* ---------------------------------------------------------------------- */

int DumpMdz::modify_param(int narg, char **arg) {
    int consumed = Dump::modify_param(narg, arg);
    return consumed;
}

#endif
