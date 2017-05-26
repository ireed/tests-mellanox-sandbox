// Copyright (c) 2014      Mellanox Technologies, Inc.
//                         All rights reserved
// $COPYRIGHT$

static int TEST_NAME(void)
{
    int rc = TC_PASS;
    int num_proc = 0;
    int my_proc = 0;
    int peer;
    TYPE i;
    TYPE *buf;
    
    num_proc = shmem_n_pes();
    my_proc = shmem_my_pe();
    peer = (my_proc + 1) % num_proc;
    buf = (TYPE *)shmem_malloc(sizeof(TYPE));
    if (!buf)
    {
        log_error(OSH_TC, "shmem_malloc\n");
        return TC_FAIL;
    }
    log_debug(OSH_TC, "%d: buf = %p\n", my_proc, buf);

    i = 0;
    buf[0] = i-1;
    if (setjmp(error_env))
    {
        log_fatal(OSH_TC, "Test failed on i=%d buf[0]=%d\n", i, buf[0]);
        shmem_free(buf);
        return TC_FAIL;
    }
    for (i = 0; i < MAX_COUNT; i++) 
    {
        shmem_barrier_all();
        /* update neighbour */
        log_debug(OSH_TC, "write %d to peer %d\n", i, peer);
        FUNC_PUT(buf, i, peer);
        /* now wait 4 our update */
        alarm(5);
        FUNC_WAIT(buf, i-1);
        alarm(0);
        log_debug(OSH_TC, "i=%d buf=%d\n", i, buf[0]);
        if (buf[0] == i-1)
        {
            log_fatal(OSH_TC, "Test failed on i=%d buf[0]=%d\n", i, buf[0]);
            shmem_free(buf);
            return TC_FAIL;
        }
    }
    shmem_free(buf);
    log_debug(OSH_TC, "passed");
    return rc;
}

#undef TYPE
#undef FUNC_PUT
#undef FUNC_WAIT
#undef TEST_NAME

