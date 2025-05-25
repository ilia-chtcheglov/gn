#include <gn_load_vhsts_conf.h>

int
gn_vhst_conf_list_push_back (gn_vhst_conf_list_t * const list, gn_vhst_conf_t * const conf);

int
gn_vhst_conf_list_push_back (gn_vhst_conf_list_t * const list, gn_vhst_conf_t * const conf)
{
    switch (list->len)
    {
        case 0:
        {
            list->head = list->tail = conf->prev = conf->next = conf;
            break;
        }
        case UINT32_MAX:
        {
            return 1;
        }
        default:
        {
            list->tail->next = conf;
            conf->prev = list->tail;
            list->head->prev = conf;
            conf->next = list->head;
            list->tail = conf;
        }
    }

    list->len++;
    return 0;
}

void
gn_load_vhsts_conf (gn_vhst_conf_list_t * const vhst_conf_list)
{
    DIR * ropendir = opendir (GN_VHSTS_CONF_DIR_PATH);
    if (ropendir == NULL)
    {
        fprintf (stderr, "Failed to open virtual hosts configuration directory. %s.\n", strerror (errno));
        return;
    }

    errno = 0;
    struct dirent * ent = NULL;
    while ((ent = readdir (ropendir)) != NULL)
    {
        if (strcmp (ent->d_name, ".") == 0 || strcmp (ent->d_name, "..") == 0) continue;
        // TODO: Check ent->d_type.

        if (strlen (ent->d_name) < 6 ||
            strcmp (&ent->d_name[strlen (ent->d_name) - 5], ".conf") != 0)
        {
            continue;
        }

        char * path = (char *)malloc (strlen (GN_VHSTS_CONF_DIR_PATH) + strlen (ent->d_name) + 1);
        if (path == NULL)
        {
            // TODO: Display error message.
            break;
        }

        strcpy (path, GN_VHSTS_CONF_DIR_PATH);
        strcat (path, ent->d_name);

        gn_vhst_conf_t * vhst_conf = (gn_vhst_conf_t *)malloc (sizeof (gn_vhst_conf_t));
        if (vhst_conf == NULL)
        {
            fprintf (stderr, "Failed to allocate structure for virtual host configuration.\n");
            free (path);
            path = NULL;
            break;
        }

        json_object * root = json_object_from_file (path);
        if (root == NULL)
        {
            fprintf (stderr, "Failed to parse virtual host configuration file \"%s\".\n", path);
            free (vhst_conf);
            vhst_conf = NULL;

            free (path);
            path = NULL;
            break;
        }

        json_object_object_foreach(root, key, val)
        {
            if (strcmp (key, "document_root") == 0)
            {
                if (json_object_get_type (val) != json_type_string)
                {
                    fprintf (stderr, "The value of directive \"%s\" must be a string.\n", key);
                    break;
                }

                vhst_conf->document_root = strdup (json_object_get_string (val));
                printf ("\"%s\": \"%s\"\n", key, vhst_conf->document_root);
            }
            else
            {
                /*
                fprintf (stderr, "Unsupported configuration directive \"%s\" in \"%s\".\n", key, path);
                break;
                */
            }
        }

        gn_vhst_conf_list_push_back (vhst_conf_list, vhst_conf);

        json_object_put (root);
        root = NULL;

        /*
        free (vhst_conf);
        vhst_conf = NULL;
        */

        free (path);
        path = NULL;
    }

    if (errno != 0)
    {
        fprintf (stderr, "Error while reading virtual hosts configuration directory. %s.\n", strerror (errno));
        // TODO: if (errno == XYZ)...
        ropendir = NULL;
    }

    // TODO: Check closedir() return value and errno.
    closedir (ropendir);
    ropendir = NULL;
}