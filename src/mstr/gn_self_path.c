#include <gn_self_path.h>

__attribute__((warn_unused_result))
char *
gn_self_path (void)
{
    char temp_path[1024];
    memset (temp_path, 0, sizeof (temp_path));

    const ssize_t rreadlink = readlink ("/proc/self/exe", temp_path, sizeof (temp_path) - 1);
    if (rreadlink == -1)
    {
        fprintf (stderr, "Failed to read path from \"/proc/self/exe\". %s.\n", strerror (errno));
        return NULL;
    }

    const size_t self_path_len = (size_t)rreadlink;
    temp_path[self_path_len] = '\0';

    char * self_path = (char *)malloc (self_path_len + 1);
    if (self_path == NULL)
    {
        fprintf (stderr, "Failed to allocate buffer for program's self path.\n");
        return NULL;
    }

    strcpy (self_path, temp_path);
    return self_path;
}