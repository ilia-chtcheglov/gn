#include <gn_process_conn.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int
gn_close (int * const fd);

__attribute__((nonnull))
void
gn_open_file (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_open_file (gn_conn_t * const conn)
{
    const char * const document_root = "/srv/www/testa.local";
    const uint32_t document_root_len = strlen (document_root);

    char * abs_path = (char *)malloc (document_root_len + conn->uri_len + 1);
    if (abs_path != NULL)
    {
        strcpy (abs_path, document_root);
        strcat (abs_path, conn->uri);
        printf ("Absolute path: \"%s\".\n", abs_path);

        conn->fd = open (abs_path, O_RDONLY | O_NONBLOCK);
        if (conn->fd > -1)
        {
            printf ("Opened \"%s\".\n", abs_path);
            // conn->step = GN_CONN_STEP_SEND_FILE;
        }
        else fprintf (stderr, "Failed to open \"%s\". %s.\n", abs_path, strerror (errno));

        free (abs_path);
        abs_path = NULL;
    }
    else fprintf (stderr, "Failed to allocate buffer for absolute file path.\n");

    conn->step = GN_CONN_STEP_CLOSE;
}

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
        case GN_CONN_STEP_OPEN_FILE:
        {
            gn_open_file (conn);
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