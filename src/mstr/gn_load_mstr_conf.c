#include <gn_load_mstr_conf.h>

void
gn_load_mstr_conf (gn_mstr_conf_t * const mstr_conf)
{
    json_object * root = json_object_from_file ("/etc/gn/master.conf");
    if (root == NULL)
    {
        fprintf (stderr, "Failed to parse master configuration file.\n");
        return;
    }

    json_object_object_foreach(root, key, val)
    {
        if (strcmp (key, "workers") == 0)
        {
            if (json_object_get_type (val) != json_type_int)
            {
                fprintf (stderr, "The value of directive \"%s\" must be an integer.\n", key);
                break;
            }

            const int32_t num = json_object_get_int (val);
            if (num < 0)
            {
                fprintf (stderr, "Workers count cannot be lesser than zero.\n");
                break;
            }
            if (num > UINT8_MAX)
            {
                fprintf (stderr, "Workers count cannot be greater than %i.\n", UINT8_MAX);
                break;
            }

            mstr_conf->workers = (uint8_t)num;

            printf ("\"%s\": %i\n", key, mstr_conf->workers);
        }
        else if (strcmp (key, "connection_acceptance_threads") == 0)
        {
            if (json_object_get_type (val) != json_type_int)
            {
                fprintf (stderr, "The value of directive \"%s\" must be an integer.\n", key);
                break;
            }

            const int32_t num = json_object_get_int (val);
            if (num < 0)
            {
                fprintf (stderr, "Connection acceptance threads count cannot be lesser than zero.\n");
                break;
            }
            if (num > UINT8_MAX)
            {
                fprintf (stderr, "Connection acceptance threads count cannot be greater than %i.\n", UINT8_MAX);
                break;
            }

            mstr_conf->connection_acceptance_threads = (uint8_t)num;

            printf ("\"%s\": %i\n", key, mstr_conf->connection_acceptance_threads);
        }
        else if (strcmp (key, "connection_management_threads") == 0)
        {
            if (json_object_get_type (val) != json_type_int)
            {
                fprintf (stderr, "The value of directive \"%s\" must be an integer.\n", key);
                break;
            }

            const int32_t num = json_object_get_int (val);
            if (num < 0)
            {
                fprintf (stderr, "Connection management threads count cannot be lesser than zero.\n");
                break;
            }
            if (num > UINT8_MAX)
            {
                fprintf (stderr, "Connection management threads count cannot be greater than %i.\n", UINT8_MAX);
                break;
            }

            mstr_conf->connection_management_threads = (uint8_t)num;

            printf ("\"%s\": %i\n", key, mstr_conf->connection_management_threads);
        }
        else
        {
            fprintf (stderr, "Unsupported configuration directive \"%s\".\n", key);
            break;
        }
    }

    json_object_put (root);
}