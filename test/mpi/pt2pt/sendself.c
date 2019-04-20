/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2006 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "mpitest.h"
#include "dtpools.h"

/*
static char MTEST_Descrip[] = "Test of sending to self (with a preposted receive)";
*/



int main(int argc, char *argv[])
{
    int errs = 0, err;
    int rank, size;
    int i, j, len, seed, testsize;
    MPI_Aint sendcount, recvcount, count[2];
    MPI_Aint maxbufsize;
    MPI_Comm comm;
    MPI_Datatype sendtype, recvtype;
    MPI_Request req;
    DTP_pool_s dtp;
    DTP_obj_s send_obj, recv_obj;
    void *sendbuf, *recvbuf;
    char *basic_type;

    MTest_Init(&argc, &argv);

    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    MTestArgList *head = MTestArgListCreate(argc, argv);
    seed = MTestArgListGetInt(head, "seed");
    testsize = MTestArgListGetInt(head, "testsize");
    count[0] = MTestArgListGetLong(head, "sendcnt");
    count[1] = MTestArgListGetLong(head, "recvcnt");
    maxbufsize = MTestArgListGetLong(head, "maxbufsize");
    basic_type = MTestArgListGetString(head, "type");

    err = DTP_pool_create(basic_type, count[0], seed, &dtp);
    if (err != DTP_SUCCESS) {
        fprintf(stderr, "Error while creating send pool (%s,%ld)\n", basic_type, count[0]);
        fflush(stderr);
    }

    MTestArgListDestroy(head);

    /* To improve reporting of problems about operations, we
     * change the error handler to errors return */
    MPI_Comm_set_errhandler(comm, MPI_ERRORS_RETURN);

    for (i = 0; i < testsize; i++) {
        err = DTP_obj_create(dtp, &send_obj, maxbufsize);
        if (err != DTP_SUCCESS) {
            errs++;
        }

        err = DTP_obj_create(dtp, &recv_obj, maxbufsize);
        if (err != DTP_SUCCESS) {
            errs++;
        }

        sendbuf = malloc(send_obj.DTP_bufsize);
        if (sendbuf == NULL) {
            errs++;
            break;
        }

        recvbuf = malloc(recv_obj.DTP_bufsize);
        if (recvbuf == NULL) {
            errs++;
            break;
        }

        err = DTP_obj_buf_init(send_obj, sendbuf, 0, 1, count[0]);
        if (err != DTP_SUCCESS) {
            errs++;
            break;
        }

        err = DTP_obj_buf_init(recv_obj, recvbuf, -1, -1, count[0]);
        if (err != DTP_SUCCESS) {
            errs++;
            break;
        }

        sendcount = send_obj.DTP_type_count;
        sendtype = send_obj.DTP_datatype;

        recvcount = recv_obj.DTP_type_count;
        recvtype = recv_obj.DTP_datatype;

        err =
            MPI_Irecv(recvbuf + recv_obj.DTP_buf_offset, recvcount, recvtype, rank, 0, comm, &req);
        if (err) {
            errs++;
            if (errs < 10) {
                MTestPrintError(err);
            }
        }

        err = MPI_Send(sendbuf + send_obj.DTP_buf_offset, sendcount, sendtype, rank, 0, comm);
        if (err) {
            errs++;
            if (errs < 10) {
                MTestPrintError(err);
            }
        }

        err = MPI_Wait(&req, MPI_STATUS_IGNORE);
        err = DTP_obj_buf_check(recv_obj, recvbuf, 0, 1, count[0]);
        if (err != DTP_SUCCESS) {
            if (errs < 10) {
                fprintf(stderr,
                        "Data in target buffer did not match for destination datatype %s and source datatype %s, count = %ld\n",
                        recv_obj.DTP_description, send_obj.DTP_description, count[0]);
                fflush(stderr);
            }
            errs++;
        }

        err = DTP_obj_buf_init(recv_obj, recvbuf, -1, -1, count[0]);
        if (err != DTP_SUCCESS) {
            errs++;
            break;
        }

        err =
            MPI_Irecv(recvbuf + recv_obj.DTP_buf_offset, recvcount, recvtype, rank, 0, comm, &req);
        if (err) {
            errs++;
            if (errs < 10) {
                MTestPrintError(err);
            }
        }

        err = MPI_Ssend(sendbuf + send_obj.DTP_buf_offset, sendcount, sendtype, rank, 0, comm);
        if (err) {
            errs++;
            if (errs < 10) {
                MTestPrintError(err);
            }
        }

        err = MPI_Wait(&req, MPI_STATUS_IGNORE);
        err = DTP_obj_buf_check(recv_obj, recvbuf, 0, 1, count[0]);
        if (err != DTP_SUCCESS) {
            if (errs < 10) {
                fprintf(stderr,
                        "Data in target buffer did not match for destination datatype %s and source datatype %s, count = %ld\n",
                        recv_obj.DTP_description, send_obj.DTP_description, count[0]);
                fflush(stderr);
            }
            errs++;
        }

        err = DTP_obj_buf_init(recv_obj, recvbuf, -1, -1, count[0]);
        if (err != DTP_SUCCESS) {
            errs++;
            break;
        }

        err =
            MPI_Irecv(recvbuf + recv_obj.DTP_buf_offset, recvcount, recvtype, rank, 0, comm, &req);
        if (err) {
            errs++;
            if (errs < 10) {
                MTestPrintError(err);
            }
        }

        err = MPI_Rsend(sendbuf + send_obj.DTP_buf_offset, sendcount, sendtype, rank, 0, comm);
        if (err) {
            errs++;
            if (errs < 10) {
                MTestPrintError(err);
            }
        }

        err = MPI_Wait(&req, MPI_STATUS_IGNORE);
        err = DTP_obj_buf_check(recv_obj, recvbuf, 0, 1, count[0]);
        if (err != DTP_SUCCESS) {
            if (errs < 10) {
                fprintf(stderr,
                        "Data in target buffer did not match for destination datatype %s and source datatype %s, count = %ld\n",
                        recv_obj.DTP_description, send_obj.DTP_description, count[0]);
                fflush(stderr);
            }
            errs++;
        }

        free(sendbuf);
        free(recvbuf);
        DTP_obj_free(send_obj);
        DTP_obj_free(recv_obj);
    }

    DTP_pool_free(dtp);

    MTest_Finalize(errs);
    return MTestReturnValue(errs);
}
