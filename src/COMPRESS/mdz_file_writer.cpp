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
#include "mdz_file_writer.h"
//#include "fmt/format.h"

using namespace LAMMPS_NS;

MdzFileWriter::MdzFileWriter() : FileWriter() {
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
    return 0;
}

void MdzFileWriter::write_float_compress(float *data, size_t nFrame, size_t nAtom) {
    auto conf = SZ::Config<float, 2>({nFrame, nAtom});
    conf.eb = 1E-2 * 30;
    conf.block_size = 128;
    conf.stride = 128;
    conf.quant_state_num = 1024;

    bool firsttime = (cmpr_iter == 0);
    if (firsttime) {
        data_ts0 = std::vector<float>(data, data + conf.dims[1]);
        size_t sample_num = 0.1 * conf.dims[1];
        sample_num = std::min(sample_num, (size_t) 20000);
        sample_num = std::max(sample_num, std::min((size_t) 5000, conf.dims[1]));
        SZ::get_cluster(data, conf.dims[1], level_start, level_offset, level_num, sample_num);
        if (level_num > conf.dims[1] * 0.25) {
            level_num = 0;
        }
        if (level_num != 0) {
            printf("start = %.3f , level_offset = %.3f, nlevel=%d\n", level_start, level_offset, level_num);
        }
    }

    if (cmpr_method_update_interval > 0 && cmpr_iter % cmpr_method_update_interval == 0) {
        cmpr_method = select_compressor<float, 2>(conf, data, firsttime, level_start, level_offset, level_num,
                                                  data_ts0.data());
    }
    const char *compressor_names[] = {"VQ", "VQT", "MT", "LR", "TS"};
    printf("%lu Compressor = %s\n", cmpr_iter, compressor_names[cmpr_method]);

    size_t compressed_size;
    SZ::uchar *comp_data = compress<float, 2>(conf, data, cmpr_method, compressed_size, level_start, level_offset,
                                              level_num, data_ts0.data());

    std::ofstream fout(outfile, std::ios::binary);
    fout.write(reinterpret_cast<const char *>(&comp_data[0]), compressed_size * sizeof(SZ::uchar));
    fout.close();

    delete[]comp_data;
    cmpr_iter++;
}

/* ---------------------------------------------------------------------- */
void MdzFileWriter::write_float(float *buffer, size_t length) {
    std::ofstream fout(outfile, std::ios::binary);
    fout.write(reinterpret_cast<const char *>(&buffer[0]), length * sizeof(float));
    fout.close();
}

/* ---------------------------------------------------------------------- */

void MdzFileWriter::close() {
}

void MdzFileWriter::flush() {
}

/* ---------------------------------------------------------------------- */

bool MdzFileWriter::isopen() const {
    return 1;
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
