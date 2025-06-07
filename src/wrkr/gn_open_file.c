#include <gn_open_file.h>

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
    const size_t abs_path_sz = document_root_len + conn->uri.len + 1;
    char * abs_path = (char *)malloc (abs_path_sz);
    if (abs_path == NULL)
    {
        fprintf (stderr, "Failed to allocate buffer for absolute file path.\n");
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
        strcat (abs_path, conn->uri.dat);
        const size_t abs_path_len = strlen (abs_path);
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
                    if (abs_path_len > 3 &&
                        abs_path[abs_path_len - 4] == '.' && abs_path[abs_path_len - 3] == 'p' &&
                        abs_path[abs_path_len - 2] == 'h' && abs_path[abs_path_len - 1] == 'p')
                    {
                        printf ("Detected PHP request.\n");
                        conn->step = GN_CONN_STEP_FCGI_CONN;
                        return;
                    }

                    // TODO: Check if following symlinks is allowed by server configuration.
                    conn->fd = open (abs_path, O_RDONLY | O_NONBLOCK);
                    if (conn->fd > -1)
                    {
                        printf ("Opened \"%s\".\n", abs_path); // TODO: Remove.

                        conn->status = 200;
                        char tmp[48];
                        sprintf (tmp, "Content-Length: %li\r\n\r\n", st.st_size);
                        strcat (conn->send_buf.dat, tmp);
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
                                sprintf (tmp1, "\t\t\t<li><a href=\"%s", conn->uri.dat);
                                if (conn->uri.dat[conn->uri.len - 1] != '/') strcat (tmp1, "/");

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
                                    strcpy (conn->uri.dat, tmppath);
                                    conn->uri.len = (gn_str_len_t)strlen (tmppath);
                                    printf ("New URI (%u) \"%s\".\n", conn->uri.len, conn->uri.dat); // TODO: Remove.

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

    conn->send_buf.len = (uint32_t)strlen (conn->send_buf.dat);
    conn->step = GN_CONN_STEP_WRIT_HDRS;

    printf ("\"%s\" %i.\n", conn->uri.dat, conn->status); // TODO: Remove.
}