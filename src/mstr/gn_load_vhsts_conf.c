#include <gn_load_vhsts_conf.h>

void
gn_load_vhsts_conf (gn_vhst_conf_list_t * const vhst_conf_list)
{
    (void)vhst_conf_list; // TODO: Remove.

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
        if (strcmp (ent->d_name, ".conf") == 0)
        {
            fprintf (stderr, "Invalid virtual host configuration file name \".conf\".\n");
            break;
        }
        if (strlen (ent->d_name) < 6 ||
            strcmp (&ent->d_name[strlen (ent->d_name) - 5], ".conf") != 0)
        {
            fprintf (stderr, "Virtual host configuration file \"%s\" doesn't end with \".conf\".\n", ent->d_name);
            break;
        }

        printf ("d_name == \"%s\"\n", ent->d_name); // TODO: Remove.
        char * path = (char *)malloc (strlen (GN_VHSTS_CONF_DIR_PATH) + strlen (ent->d_name) + 1);
        if (path == NULL)
        {
            // TODO: Display error message.
            break;
        }

        strcpy (path, GN_VHSTS_CONF_DIR_PATH);
        strcat (path, ent->d_name);

        json_object * root = json_object_from_file (path);
        if (root == NULL)
        {
            fprintf (stderr, "Failed to parse virtual host configuration file \"%s\".\n", path);
            free (path);
            path = NULL;
            break;
        }

        json_object_put (root);
        root = NULL;

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