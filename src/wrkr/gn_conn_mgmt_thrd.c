#include <gn_conn_mgmt_thrd.h>

#include <arpa/inet.h> // TODO: Remove.
#include <errno.h>
#include <stdlib.h>
#include <string.h>

__attribute__((nonnull))
void
gn_recv_data (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_recv_data (gn_conn_t * const conn)
{
    /*
     * Receive data from connection.
     * The received data will be stored after the data already present in the receive buffer.
     * The recv() function will receive (buffer size - 1) - (data already in the buffer).
     *
     * If buffer size is 10 bytes and length (data inside) is 4, recv() will receive 10 - 4 - 1 = 5.
     * The final buffer length will be 4 + 5 = 9 which is less than 10, with room for the NULL byte.
     */
    const ssize_t rrecv = recv (conn->fd, &conn->recv_buf[conn->recv_buf_len],
                                conn->recv_buf_sz - conn->recv_buf_len - 1, 0);
    switch (rrecv)
    {
        case -1: // Error occurred.
        {
            switch (errno)
            {
                case EINTR:
                {
                    // recv() was interrupted by a signal. Do nothing.
                    break;
                }
                case EAGAIN:
                {
                    // recv() received nothing. Check if connection is idle.
                    const time_t t = time (NULL);
                    if (t == conn->last_io + 10) // Timeout after 10 seconds.
                    {
                        // Connection is idle. Close it.
                        conn->step = GN_CONN_STEP_CLOSE;
                        printf ("Connection timeout.\n"); // TODO: Remove.
                    }
                    break;
                }
                // TODO: Check for other errno values such as EBADF.
                default:
                {
                    // Other errors. Close connection.
                    conn->step = GN_CONN_STEP_CLOSE;
                    fprintf (stderr, "Failed to receive HTTP client data. %s.\n", strerror (errno));
                }
            }
            break;
        }
        case 0: // Client disconnected.
        {
            printf ("Client disconnected.\n"); // TODO: Remove.
            conn->step = GN_CONN_STEP_CLOSE;
            break;
        }
        default: // Data received.
        {
            // Update the receive buffer length.
            conn->recv_buf_len += (uint32_t)rrecv;
            // NULL terminate the receive buffer.
            conn->recv_buf[conn->recv_buf_len] = '\0';
            // Go back to the previous connection step.
            conn->step = conn->prev_step;
            printf ("Received (%u) \"%s\"\n", conn->recv_buf_len, conn->recv_buf); // TODO: Remove.
        }
    }
}

void
gn_extr_mthd (gn_conn_t * const conn);

void
gn_extr_mthd (gn_conn_t * const conn)
{
    size_t recv_buf_i = 0;
    for ( ;
         recv_buf_i < conn->recv_buf_len &&
         conn->mthd_len < conn->mthd_sz - 1 &&
         conn->recv_buf[recv_buf_i] != ' ';
         recv_buf_i++, conn->mthd_len++)
    {
        conn->mthd[conn->mthd_len] = conn->recv_buf[recv_buf_i];
    }

    conn->mthd[conn->mthd_len] = '\0';
    if (conn->recv_buf[recv_buf_i] == ' ')
    {
        printf ("Request method (%u) \"%s\".\n", conn->mthd_len, conn->mthd); // TODO: Remove.
        // Move the rest of the data to the beginning of the receive buffer.
        size_t i = 0;
        size_t j = conn->mthd_len + 1;
        while (j < conn->recv_buf_len)
        {
            conn->recv_buf[i] = conn->recv_buf[j];
            i++;
            j++;
        }
        conn->recv_buf_len -= conn->mthd_len + 1;
        conn->recv_buf[conn->recv_buf_len] = '\0';
        printf ("Remaining (%u) \"%s\"\n", conn->recv_buf_len, conn->recv_buf); // TODO: Remove.

        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_EXTR_URI; // TODO: Go to next step.
    }
    else
    {
        if (conn->mthd_len == conn->mthd_sz - 1)
        {
            fprintf (stderr, "Request method too long.\n"); // TODO: Remove.
            conn->step = GN_CONN_STEP_CLOSE;
        }
        else conn->step = GN_CONN_STEP_RECV_DATA;
    }
}

void
gn_extr_uri (gn_conn_t * const conn);

void
gn_extr_uri (gn_conn_t * const conn)
{
    size_t recv_buf_i = 0;
    for ( ;
         recv_buf_i < conn->recv_buf_len &&
         conn->uri_len < conn->uri_sz - 1 &&
         conn->recv_buf[recv_buf_i] != ' ';
         recv_buf_i++, conn->uri_len++)
    {
        conn->uri[conn->uri_len] = conn->recv_buf[recv_buf_i];
    }

    conn->uri[conn->uri_len] = '\0';
    if (conn->recv_buf[recv_buf_i] == ' ')
    {
        printf ("Request URI (%u) \"%s\".\n", conn->uri_len, conn->uri);
        // Move the rest of the data to the beginning of the receive buffer.
        size_t i = 0;
        size_t j = conn->uri_len + 1;
        while (j < conn->recv_buf_len)
        {
            conn->recv_buf[i] = conn->recv_buf[j];
            i++;
            j++;
        }
        conn->recv_buf_len -= conn->uri_len + 1;
        conn->recv_buf[conn->recv_buf_len] = '\0';
        printf ("Remaining (%u) \"%s\"\n", conn->recv_buf_len, conn->recv_buf); // TODO: Remove.

        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_EXTR_PROT; // TODO: Go to next step.
    }
    else
    {
        if (conn->uri_len == conn->uri_sz - 1)
        {
            fprintf (stderr, "Request URI too long.\n");
            conn->step = GN_CONN_STEP_CLOSE;
        }
        else conn->step = GN_CONN_STEP_RECV_DATA;
    }
}

void
gn_extr_prot (gn_conn_t * const conn);

void
gn_extr_prot (gn_conn_t * const conn)
{
    size_t recv_buf_i = 0;
    for ( ;
         recv_buf_i < conn->recv_buf_len &&
         conn->prot_len < conn->prot_sz - 1 &&
         conn->recv_buf[recv_buf_i] != '\n';
         recv_buf_i++, conn->prot_len++)
    {
        conn->prot[conn->prot_len] = conn->recv_buf[recv_buf_i];
    }

    conn->prot[conn->prot_len] = '\0';
    if (conn->recv_buf[recv_buf_i] == '\n')
    {
        // Move the rest of the data to the beginning of the receive buffer.
        size_t i = 0;
        size_t j = (size_t)conn->prot_len + 1;
        while (j < conn->recv_buf_len)
        {
            conn->recv_buf[i] = conn->recv_buf[j];
            i++;
            j++;
        }
        conn->recv_buf_len -= (uint32_t)conn->prot_len + 1;
        conn->recv_buf[conn->recv_buf_len] = '\0';
        printf ("Remaining (%u) \"%s\"\n", conn->recv_buf_len, conn->recv_buf); // TODO: Remove.

        // Must end with CRLF.
        if (conn->prot_len > 0 && conn->prot[conn->prot_len - 1] == '\r')
        {
            conn->prot[conn->prot_len - 1] = '\0';
            conn->prot_len--;
        }
        printf ("Request protocol (%u) \"%s\".\n", conn->prot_len, conn->prot);

        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_EXTR_HDRN; // TODO: Go to next step.
    }
    else
    {
        if (conn->prot_len == conn->prot_sz - 1)
        {
            fprintf (stderr, "Request protocol too long.\n");
            conn->step = GN_CONN_STEP_CLOSE;
        }
        else conn->step = GN_CONN_STEP_RECV_DATA;
    }
}

void
gn_extr_hdrn (gn_conn_t * const conn);

void
gn_extr_hdrn (gn_conn_t * const conn)
{
    size_t recv_buf_i = 0;
    for ( ;
         recv_buf_i < conn->recv_buf_len &&
         conn->hdrn_len < conn->hdrn_sz - 1 &&
         conn->recv_buf[recv_buf_i] != ':';
         recv_buf_i++, conn->hdrn_len++)
    {
        conn->hdrn[conn->hdrn_len] = conn->recv_buf[recv_buf_i];
    }

    conn->hdrn[conn->hdrn_len] = '\0';
    if (conn->recv_buf[recv_buf_i] == ':')
    {
        printf ("Header name (%u) \"%s\".\n", conn->hdrn_len, conn->hdrn);
        // Move the rest of the data to the beginning of the receive buffer.
        size_t i = 0;
        size_t j = (size_t)conn->hdrn_len + 1;
        while (j < conn->recv_buf_len)
        {
            conn->recv_buf[i] = conn->recv_buf[j];
            i++;
            j++;
        }
        conn->recv_buf_len -= (uint32_t)conn->hdrn_len + 1;
        conn->recv_buf[conn->recv_buf_len] = '\0';
        printf ("Remaining (%u) \"%s\"\n", conn->recv_buf_len, conn->recv_buf); // TODO: Remove.

        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_EXTR_HDRV; // TODO: Go to next step.
    }
    else
    {
        if (conn->hdrn_len == conn->hdrn_sz - 1)
        {
            fprintf (stderr, "Request header name too long.\n");
            conn->step = GN_CONN_STEP_CLOSE;
        }
        else conn->step = GN_CONN_STEP_RECV_DATA;
    }
}

void
gn_extr_hdrv (gn_conn_t * const conn);

void
gn_extr_hdrv (gn_conn_t * const conn)
{
    size_t recv_buf_i = 0;
    for ( ;
         recv_buf_i < conn->recv_buf_len &&
         conn->hdrv_len < conn->hdrv_sz - 1 &&
         conn->recv_buf[recv_buf_i] != '\n';
         recv_buf_i++, conn->hdrv_len++)
    {
        conn->hdrv[conn->hdrv_len] = conn->recv_buf[recv_buf_i];
    }

    conn->hdrv[conn->hdrv_len] = '\0';
    if (conn->recv_buf[recv_buf_i] == '\n')
    {
        // Move the rest of the data to the beginning of the receive buffer.
        size_t i = 0;
        size_t j = (size_t)conn->hdrv_len + 1;
        while (j < conn->recv_buf_len)
        {
            conn->recv_buf[i] = conn->recv_buf[j];
            i++;
            j++;
        }
        conn->recv_buf_len -= (uint32_t)conn->hdrv_len + 1;
        conn->recv_buf[conn->recv_buf_len] = '\0';
        printf ("Remaining (%u) \"%s\"\n", conn->recv_buf_len, conn->recv_buf); // TODO: Remove.

        // Must end with CRLF.
        if (conn->hdrv_len > 0 && conn->hdrv[conn->hdrv_len - 1] == '\r')
        {
            conn->hdrv[conn->hdrv_len - 1] = '\0';
            conn->hdrv_len--;
        }
        printf ("Header value (%u) \"%s\".\n", conn->hdrv_len, conn->hdrv);

        conn->hdrn_len = 0;
        conn->hdrv_len = 0;

        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_EXTR_HDRN; // TODO: Go to next step.
    }
    else
    {
        if (conn->hdrn_len == conn->hdrn_sz - 1)
        {
            fprintf (stderr, "Request header value too long.\n");
            conn->step = GN_CONN_STEP_CLOSE;
        }
        else conn->step = GN_CONN_STEP_RECV_DATA;
    }
}

int
gn_close (int * const fd);

void
gn_close_conn (gn_conn_t * const conn);

void
gn_close_conn (gn_conn_t * const conn)
{
    gn_close (&conn->fd);
    free (conn->hdrv);
    conn->hdrv = NULL;
    free (conn->hdrn);
    conn->hdrn = NULL;
    free (conn->prot);
    conn->prot = NULL;
    free (conn->uri);
    conn->uri = NULL;
    free (conn->mthd);
    conn->mthd = NULL;
    free (conn->recv_buf);
    conn->recv_buf = NULL;
    free (conn->saddr);
    conn->saddr = NULL;
}

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_process_conn (gn_conn_t * const conn);

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_process_conn (gn_conn_t * const conn)
{
    switch (conn->step)
    {
        case GN_CONN_STEP_EXTR_MTHD:
        {
            gn_extr_mthd (conn);
            break;
        }
        case GN_CONN_STEP_EXTR_URI:
        {
            gn_extr_uri (conn);
            break;
        }
        case GN_CONN_STEP_EXTR_PROT:
        {
            gn_extr_prot (conn);
            break;
        }
        case GN_CONN_STEP_EXTR_HDRN:
        {
            gn_extr_hdrn (conn);
            break;
        }
        case GN_CONN_STEP_EXTR_HDRV:
        {
            gn_extr_hdrv (conn);
            break;
        }
        case GN_CONN_STEP_RECV_DATA:
        {
            gn_recv_data (conn);
            break;
        }
        case GN_CONN_STEP_CLOSE:
        {
            printf ("Closing connection %p\n", (void *)conn);
            gn_close_conn (conn);
            return true;
        }
        case GN_CONN_STEP_INVALID:
        default:
        {
            fprintf (stderr, "Invalid connection step %u.\n", conn->step);
            conn->step = GN_CONN_STEP_CLOSE;
        }
    }

    return false;
}

void *
gn_conn_mgmt_thrd (void * const p)
{
    gn_conn_mgmt_thrd_data_t * const data = (gn_conn_mgmt_thrd_data_t *)p;
    atomic_store_explicit (&data->state, GN_CONN_MGMT_THRD_STATE_RUNNING, memory_order_release);

    // Connections list.
    gn_conn_list_t conn_list = { 0 };

    bool stop = false;
    bool main_loop = true;
    while (main_loop)
    {
        // Process connections one by one.
        gn_conn_t * conn = conn_list.head;
        for (uint32_t i = 0; i < conn_list.len; i++)
        {
            printf ("Processing connection %p\n", (void *)conn);
            if (stop) conn->step = GN_CONN_STEP_CLOSE;
            if (!gn_process_conn (conn))
            {
                conn = conn->next;
                continue;
            }

            gn_conn_list_remove (&conn_list, conn);
            gn_conn_t * next_conn;
            if (conn_list.len > 0) next_conn = conn->next;
            else next_conn = NULL;

            printf ("Freeing connection %p\n", (void *)conn);
            gn_free_conn (&conn);
            conn = next_conn;
        }

        gn_get_new_conns (data, &conn_list, stop);

        /*
         * If the local stop variable isn't true we check the atomic stop variable.
         * If the atomic one is true we set the local one to true and signal that the thread is stopping.
         * If the thread doesn't have to stop it waits 100 milliseconds to avoid 100% CPU usage.
         */
        if (!stop)
        {
            if (atomic_load_explicit (&data->stop, memory_order_relaxed))
            {
                printf ("Connection management thread %lu stopping.\n", data->tid);
                atomic_store_explicit (&data->state, GN_CONN_MGMT_THRD_STATE_STOPPING, memory_order_release);
                stop = true;
            }
            else gn_sleep_ms (100);
        }
        else if (conn_list.len == 0)
        {
            // If the thread has to stop and the connection list is empty we leave the main loop.
            main_loop = false;
            break;
        }
    }

    // Signal that the thread is stopped.
    atomic_store_explicit (&data->state, GN_CONN_MGMT_THRD_STATE_STOPPED, memory_order_release);
    return NULL;
}