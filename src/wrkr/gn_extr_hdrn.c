#include <gn_extr_hdrn.h>

__attribute__((nonnull))
void
gn_htbl_dump (gn_htbl_t * const tbl);

__attribute__((nonnull))
void
gn_htbl_dump (gn_htbl_t * const tbl)
{
    for (size_t i = 0; i < tbl->sz; i++)
    {
        printf ("Index %lu, ", i);
        if (tbl->itms[i] == NULL)
        {
            printf ("NULL\n");
        }
        else
        {
            printf (".key (%u) \"%s\", .val (%u) \"%s\"\n",
                    tbl->itms[i]->key_len, tbl->itms[i]->key,
                    tbl->itms[i]->val_len, tbl->itms[i]->val);
            gn_htbl_item_t * next_item = tbl->itms[i]->next;
            for ( ; next_item != NULL; next_item = next_item->next)
            {
                printf ("\t.key (%u) \"%s\", .val (%u) \"%s\"\n",
                        next_item->key_len, next_item->key,
                        next_item->val_len, next_item->val);
            }
        }
    }
    printf ("\n");
}

__attribute__((nonnull))
void
gn_extr_hdrn (gn_conn_t * const conn)
{
    size_t recv_buf_i = 0;
    for ( ;
         recv_buf_i < conn->recv_buf.len &&
         conn->hdrn.len < conn->hdrn.sz - 1 &&
         conn->recv_buf.dat[recv_buf_i] != ':' &&
         conn->recv_buf.dat[recv_buf_i] != '\n';
         recv_buf_i++, conn->hdrn.len++)
    {
        conn->hdrn.dat[conn->hdrn.len] = conn->recv_buf.dat[recv_buf_i];
    }
    conn->hdrn.dat[conn->hdrn.len] = '\0';

    switch (conn->recv_buf.dat[recv_buf_i])
    {
        case ':':
        {
            printf ("Header name (%u) \"%s\".\n", conn->hdrn.len, conn->hdrn.dat);
            // Move the rest of the data to the beginning of the receive buffer.
            size_t i = 0;
            size_t j = (size_t)conn->hdrn.len + 1;
            while (j < conn->recv_buf.len)
            {
                conn->recv_buf.dat[i] = conn->recv_buf.dat[j];
                i++;
                j++;
            }
            conn->recv_buf.len -= (uint32_t)conn->hdrn.len + 1;
            conn->recv_buf.dat[conn->recv_buf.len] = '\0';
            printf ("Remaining (%u) \"%s\"\n", conn->recv_buf.len, conn->recv_buf.dat); // TODO: Remove.

            conn->step = GN_CONN_STEP_EXTR_HDRV; // TODO: Go to next step.
            break;
        }
        case '\n':
        {
            printf ("End of request headers.\n");
            conn->recv_buf.dat[0] = '\0';
            gn_htbl_dump (&conn->req_hdrs);
            conn->step = GN_CONN_STEP_OPEN_FILE;
            break;
        }
        default:
        {
            if (conn->hdrn.len == conn->hdrn.sz - 1)
            {
                fprintf (stderr, "Request header name too long.\n");
                conn->step = GN_CONN_STEP_CLOSE;
            }
            else conn->step = GN_CONN_STEP_RECV_DATA;
        }
    }
}