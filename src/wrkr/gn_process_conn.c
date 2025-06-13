#include <gn_process_conn.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_process_conn (gn_conn_t * const conn)
{
    labl_reprocess_conn:
    switch (conn->step)
    {
        case GN_CONN_STEP_EXTR_MTHD:
        {
            gn_extr_mthd (conn);
            if (conn->step != GN_CONN_STEP_EXTR_URI) break;
            __attribute__((fallthrough));
        }
        case GN_CONN_STEP_EXTR_URI:
        {
            gn_extr_uri (conn);
            if (conn->step != GN_CONN_STEP_EXTR_PROT) break;
            __attribute__((fallthrough));
        }
        case GN_CONN_STEP_EXTR_PROT:
        {
            gn_extr_prot (conn);
            if (conn->step != GN_CONN_STEP_EXTR_HDRN) break;
            __attribute__((fallthrough));
        }
        case GN_CONN_STEP_EXTR_HDRN:
        {
            gn_extr_hdrn (conn);
            if (conn->step != GN_CONN_STEP_EXTR_HDRV) break;
            __attribute__((fallthrough));
        }
        case GN_CONN_STEP_EXTR_HDRV:
        {
            gn_extr_hdrv (conn);
            if (conn->step == GN_CONN_STEP_EXTR_HDRN) goto labl_reprocess_conn;
            break;
        }
        case GN_CONN_STEP_OPEN_FILE:
        case GN_CONN_STEP_OPEN_LIST:
        {
            gn_open_file (conn);
            break;
        }
        case GN_CONN_STEP_WRIT_HDRS:
        {
            gn_writ_hdrs (conn);
            break;
        }
        case GN_CONN_STEP_SEND_HDRS:
        {
            gn_send_data (conn);
            break;
        }
        case GN_CONN_STEP_SEND_FILE:
        {
            gn_send_file (conn);
            break;
        }
        case GN_CONN_STEP_RECV_DATA:
        {
            gn_recv_data (conn);
            break;
        }
        case GN_CONN_STEP_FCGI_CONN:
        {
            gn_fcgi_conn (conn);
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