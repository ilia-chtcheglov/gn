#include <gn_process_conn.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

int
gn_close (int * const fd);

__attribute__((nonnull))
void
gn_open_file (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_open_file (gn_conn_t * const conn)
{
    // Test code.
    const char * const document_root = "/srv/www/testa.local";
    const uint32_t document_root_len = strlen (document_root);

    /*
     * Allocate a buffer to store the absolute path the program has to access.
     * (Document root + URI + NULL byte).
     */
    const size_t abs_path_sz = document_root_len + conn->uri_len + 1;
    char * abs_path = (char *)malloc (abs_path_sz);
    if (abs_path == NULL)
    {
        fprintf (stderr, "Failed to allocate buffer for absolute file path.\n");
        strcpy (conn->send_buf, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
        conn->status = 500;
    }
    else
    {
        memset (abs_path, '\0', abs_path_sz);

        /*
         * Create the absolute path by concatenating the document root and the URI.
         *
         * If the step is OPEN_FILE we add the document root.
         * If step is OPEN_LIST we don't add the document root because the URI is
         * the path to a temporary file containing the directory listing.
         */
        if (conn->step == GN_CONN_STEP_OPEN_FILE) strcpy (abs_path, document_root);
        strcat (abs_path, conn->uri);
        printf ("Absolute path: \"%s\".\n", abs_path);

        // Get the path type and (file) size.
        struct stat st = { 0 };
        const int rstat = stat (abs_path, &st);
        switch (rstat)
        {
            case 0:
            {
                if (S_ISREG (st.st_mode) || S_ISLNK (st.st_mode))
                {
                    // TODO: Check if following symlinks is allowed by server configuration.
                    conn->fd = open (abs_path, O_RDONLY | O_NONBLOCK);
                    if (conn->fd > -1)
                    {
                        printf ("Opened \"%s\".\n", abs_path); // TODO: Remove.

                        conn->status = 200;
                        char tmp[48];
                        sprintf (tmp, "Content-Length: %li\r\n\r\n", st.st_size);
                        strcat (conn->send_buf, tmp);
                    }
                    else
                    {
                        fprintf (stderr, "Failed to open \"%s\". %s.\n", abs_path, strerror (errno));
                        switch (errno)
                        {
                            // TODO: Check other error codes.
                            case ENOENT:
                            {
                                conn->status = 404;
                                break;
                            }
                            default:
                                conn->status = 500;
                        }
                    }
                }
                else if (S_ISDIR (st.st_mode))
                {
                    DIR * ropendir = opendir (abs_path);
                    if (ropendir == NULL)
                    {
                        switch (errno)
                        {
                            case EACCES:
                            {
                                conn->status = 403;
                                break;
                            }
                            case ENOENT:
                            {
                                conn->status = 404;
                                break;
                            }
                            default:
                                conn->status = 500;
                        }
                        break;
                    }

                    printf ("Opened directory \"%s\".\n", abs_path); // TODO: Remove.

                    char tmppath[32];
                    strcpy (tmppath, "/tmp/gndirlstXXXXXX");

                    // Create temporary file to store the directory listing.
                    const int rmkostemp = mkostemp (tmppath, O_RDWR | O_CLOEXEC | O_NONBLOCK);
                    if (rmkostemp > -1)
                    {
                        const char * const bg =
                        "<html>\n"
                        "\t<head>\n"
                        "\t\t<title>Listing</title>\n"
                        "\t</head>\n"
                        "\t<body>\n"
                        "\t\t<ul>\n";
                        ssize_t rwrite = write (rmkostemp, bg, strlen (bg));
                        if (rwrite == strlen (bg))
                        {
                            errno = 0;
                            // This structure may be statically allocated; do not attempt to free(3) it.
                            struct dirent * ent = NULL;
                            while ((ent = readdir (ropendir)) != NULL)
                            {
                                char tmp1[1024];
                                sprintf (tmp1, "<li><a href=\"%s", conn->uri);
                                if (conn->uri[conn->uri_len - 1] != '/') strcat (tmp1, "/");

                                char tmp2[1024];
                                sprintf (tmp2, "%s\">%s</a></li>\n", ent->d_name, ent->d_name);

                                char tmp3[2048];
                                strcpy (tmp3, tmp1);
                                strcat (tmp3, tmp2);
                                (void)! write (rmkostemp, tmp3, strlen (tmp3));
                            }

                            if (errno != 0)
                            {
                                fprintf (stderr, "Directory handle corrupted.\n");
                                ropendir = NULL;
                                conn->status = 500;
                                // TODO: Stop worker process.
                            }
                            else
                            {
                                const char * const en =
                                "\t\t</ul>\n"
                                "\t</body>\n"
                                "</html>";
                                rwrite = write (rmkostemp, en, strlen (en));
                                if (rwrite == strlen (en))
                                {
                                    strcpy (conn->uri, tmppath);
                                    conn->uri_len = (uint32_t)strlen (tmppath);
                                    printf ("New URI (%u) \"%s\".\n", conn->uri_len, conn->uri); // TODO: Remove.

                                    closedir (ropendir);
                                    ropendir = NULL;

                                    // The same function will be called with this step.
                                    conn->step = GN_CONN_STEP_OPEN_LIST;
                                    return;
                                }
                                else conn->status = 500;
                            }
                        }
                        else conn->status = 500;
                    }
                    else
                    {
                        // TODO: Check errno including open() errno values.
                        conn->status = 500;
                    }

                    closedir (ropendir);
                    ropendir = NULL;
                }
                else conn->status = 403;
                break;
            }
            case -1:
            {
                switch (errno)
                {
                    case EACCES:
                    {
                        conn->status = 403;
                        break;
                    }
                    case ENOENT:
                    case ENOTDIR:
                    {
                        conn->status = 404;
                        break;
                    }
                    case ELOOP:
                    case ENAMETOOLONG:
                    case ENOMEM:
                    case EOVERFLOW:
                    {
                        conn->status = 500;
                        break;
                    }
                    case EBADF:
                    case EFAULT:
                    {
                        conn->status = 500;
                        // TODO: Stop worker process.
                        break;
                    }
                    default:
                    {
                        conn->status = 500;
                        // TODO: Stop worker process.
                    }
                }
                break;
            }
            default:
            {
                
                conn->status = 500;
                // TODO: Stop worker process.
            }
        }

        free (abs_path);
        abs_path = NULL;
    }

    switch (conn->status)
    {
        case 200:
        {
            strcpy (conn->send_buf, "HTTP/1.1 200 OK\r\n\r\n");
            break;
        }
        case 403:
        {
            strcpy (conn->send_buf, "HTTP/1.1 403 Forbidden\r\n\r\n");
            break;
        }
        case 404:
        {
            strcpy (conn->send_buf, "HTTP/1.1 404 Not Found\r\n\r\n");
            break;
        }
        case 500:
        {
            strcpy (conn->send_buf, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
            break;
        }
        default:
        {
            fprintf (stderr, "Unexpected response code %i.\n", conn->status);
            conn->status = 500;
            strcpy (conn->send_buf, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
        }
    }

    conn->send_buf_len = (uint32_t)strlen (conn->send_buf);
    conn->step = GN_CONN_STEP_SEND_HDRS;

    printf ("\"%s\" %i.\n", conn->uri, conn->status); // TODO: Remove.
}

__attribute__((nonnull))
void
gn_send_hdrs (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_send_hdrs (gn_conn_t * const conn)
{
    printf ("Sending headers (%p) (%u) \"%s\".\n", conn->send_buf, conn->send_buf_len, conn->send_buf);
    // Test code.
    size_t to_send = conn->send_buf_len;
    // if (to_send > 10) to_send = 10;

    const ssize_t rsend = send (conn->sock, conn->send_buf, to_send, 0);
    switch (rsend)
    {
        // TODO: Check if the client doesn't want to read what we're sending.
        case 0:
            break;
        case -1:
        {
            switch (errno)
            {
                case EAGAIN:
                case EINTR:
                case ENOBUFS:
                case ENOMEM:
                {
                    conn->last_io = time (NULL);
                    break;
                }
                case EBADF:
                case EFAULT:
                case EINVAL:
                case ENOTCONN:
                case ENOTSOCK:
                case EPIPE:
                default:
                {
                    gn_close (&conn->sock);
                    conn->step = GN_CONN_STEP_CLOSE;
                    break;
                }
            }
            break;
        }
        default:
        {
            conn->last_io = time (NULL);
            // Move the rest of the data to the beginning of the send buffer.
            uint32_t i = 0;
            uint32_t j = (uint32_t)rsend;
            while (j < conn->send_buf_len)
            {
                conn->send_buf[i] = conn->send_buf[j];
                i++;
                j++;
            }
            conn->send_buf_len -= (uint32_t)rsend;
            conn->send_buf[conn->send_buf_len] = '\0';
            printf ("Remaining (%u) \"%s\"\n", conn->send_buf_len, conn->send_buf); // TODO: Remove.

            if (conn->send_buf_len == 0)
            {
                printf ("Response headers sent.\n");
                switch (conn->status)
                {
                    case 200:
                    {
                        printf ("Sending file.\n");
                        conn->step = GN_CONN_STEP_SEND_FILE;
                        break;
                    }
                    default:
                    {
                        printf ("Closing connection.\n");
                        conn->step = GN_CONN_STEP_CLOSE;
                    }
                }
            }
        }
    }
}

__attribute__((nonnull))
void
gn_send_file (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_send_file (gn_conn_t * const conn)
{
    const size_t max_read = conn->send_buf_sz - conn->send_buf_len - 1;
    const ssize_t rread = read (conn->fd, &conn->send_buf[conn->send_buf_len], max_read);
    switch (rread)
    {
        case -1:
        {
            /*
             * Documentation says: On error, -1 is returned, and errno is set appropriately.
             * IN THIS CASE it is left unspecified whether the file position (if any) changes.
             */
            switch (errno)
            {
                case EIO: // Could be a low-level I/O error.
                    break;
                case EAGAIN:
                /*
                 * read() can return EAGAIN or EWOULDBLOCK.
                 * But if they have the same value we can use only one of them.
                 */
                #if EAGAIN != EWOULDBLOCK
                case EWOULDBLOCK:
                #endif
                case EINTR:
                {
                    conn->last_io = time (NULL);
                    break;
                }
                case EBADF:
                {
                    fprintf (stderr, "Failed to read file. %s.\n", strerror (errno));

                    /*
                     * If conn->fd is not invalid (> -1) but we get EBADF it means the fd value was changed/corrupted.
                     * Close the connection without closing conn->fd. We don't know what fd refers to now.
                     * If fd is invalid (< 0), I don't know why gn_send_file() was called. Close the connection anyway.
                     */
                    if (conn->fd > -1)
                    {
                        conn->fd = -1;
                        // TODO: Stop the worker process.
                    }
                    conn->step = GN_CONN_STEP_CLOSE;
                    break;
                }
                case EISDIR:
                {
                    fprintf (stderr, "Failed to read file. %s.\n", strerror (errno));

                    /*
                     * This program doesn't open directories. If we get EISDIR it means
                     * the fd value was changed/corrupted. Close the connection without
                     * closing conn->fd. We don't know what fd refers to now.
                     */
                    conn->fd = -1;
                    conn->step = GN_CONN_STEP_CLOSE;
                    break;
                }
                default:
                {
                    fprintf (stderr, "Failed to read file. %s.\n", strerror (errno));
                    conn->step = GN_CONN_STEP_CLOSE;
                }
            }
            break;
        }
        case 0:
        {
            if (max_read > 0)
            {
                conn->step = GN_CONN_STEP_CLOSE;
                break;
            }
            __attribute__((fallthrough));
        }
        default:
        {
            conn->send_buf_len += (uint32_t)rread;
            conn->send_buf[conn->send_buf_len] = '\0';
            gn_send_hdrs (conn);
        }
    }
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
        case GN_CONN_STEP_OPEN_LIST:
        {
            gn_open_file (conn);
            break;
        }
        case GN_CONN_STEP_SEND_HDRS:
        {
            gn_send_hdrs (conn);
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