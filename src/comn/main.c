#include <main.h>

int
main (const int argc,
      const char * const * const argv)
{
    // Check command line arguments.
    if (gn_chck_cmdl_args (argc, argv) != EXIT_SUCCESS) return EXIT_FAILURE;

    /*
     * Contains the address of the Unix socket used for
     * IPC between the master process and the worker processes.
     * If this variable is not NULL the process will be a worker process.
     */
    const char * ipc_addr_str = NULL;

    // Parse command line arguments.
    if (gn_prse_cmdl_args (argc, argv, &ipc_addr_str) != EXIT_SUCCESS) return EXIT_FAILURE;

    /*
     * Open a Unix IPC socket. It will be used by:
     * - The master process to control worker processes.
     * - Worker processes to receive commands, send statistics to the master, etc.
     */
    int ipc_sock = gn_ipc_open ();
    if (ipc_sock < 0) return EXIT_FAILURE;

    // List of server sockets.
    gn_serv_sock_list_t serv_sock_list = { 0 };

    if (ipc_addr_str == NULL) gn_mstr_init (ipc_sock, &serv_sock_list);
    else gn_wrkr_init (ipc_sock, &serv_sock_list, ipc_addr_str);

    // Close server sockets.
    gn_close_serv_socks (&serv_sock_list);

    // Close the IPC socket.
    gn_ipc_close (&ipc_sock);

    return EXIT_SUCCESS; // TODO: Return a variable.
}