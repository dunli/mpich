/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2017 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 *  Portions of this code were written by Intel Corporation.
 *  Copyright (C) 2011-2018 Intel Corporation.  Intel provides this material
 *  to Argonne National Laboratory subject to Software Grant and Corporate
 *  Contributor License Agreement dated February 8, 2012.
 */

#include "mpiimpl.h"

/* generate gentran algo prototypes */
#include "tsp_gentran.h"
#include "ireduce_tsp_tree_algos_prototypes.h"
#include "tsp_undef.h"

int MPIR_Ireduce_intra_gentran_ring(const void *sendbuf, void *recvbuf, int count,
                                    MPI_Datatype datatype, MPI_Op op, int root,
                                    MPIR_Comm * comm_ptr, int chunk_size, int buffer_per_child,
                                    MPIR_Request ** request)
{
    int mpi_errno = MPI_SUCCESS;

    /* Ring algorithm is equivalent to kary tree with k = 1 */
    mpi_errno = MPII_Gentran_Ireduce_intra_tree(sendbuf, recvbuf, count, datatype, op, root,
                                                comm_ptr, request, MPIR_TREE_TYPE_KARY,
                                                1, buffer_per_child, chunk_size);

    return mpi_errno;
}
