#include <gn_send_serv_sock.h>

void
gn_send_serv_sock (const int ipc_sock, const gn_serv_sock_t * const serv_sock)
{
    char buf[1024];
    memset (buf, 0, sizeof (buf));
    snprintf (buf, sizeof (buf), "%s@%i\n", serv_sock->addr, serv_sock->port);

    struct iovec io_vec;
    memset (&io_vec, 0, sizeof (io_vec));
    io_vec.iov_base = buf;
    io_vec.iov_len = strlen (buf);

    struct msghdr msg_hdr;
    memset (&msg_hdr, 0, sizeof (msg_hdr));
    msg_hdr.msg_iov = &io_vec;
    msg_hdr.msg_iovlen = 1;

    char control[CMSG_SPACE (sizeof (int))];
    memset (control, 0, sizeof (control));
    msg_hdr.msg_control = control;
    msg_hdr.msg_controllen = sizeof (control);

    struct cmsghdr * cmsg_hdr = CMSG_FIRSTHDR (&msg_hdr);
    cmsg_hdr->cmsg_level = SOL_SOCKET;
    cmsg_hdr->cmsg_type = SCM_RIGHTS;
    cmsg_hdr->cmsg_len = CMSG_LEN (sizeof (int));
    memcpy (CMSG_DATA (cmsg_hdr), &serv_sock->fd, sizeof (int));

    const ssize_t rsendmsg = sendmsg (ipc_sock, &msg_hdr, MSG_NOSIGNAL);
    if (rsendmsg == -1)
    {
        fprintf (stderr, "Failed to send server socket to worker process. %s.\n", strerror (errno));
    }
    else if ((size_t)rsendmsg == strlen (buf))
    {

    }
    else
    {
        const size_t sent = (size_t)rsendmsg;
        fprintf (stderr, "Failed to send server socket to worker process. Sent %lu/%lu bytes.\n", sent, strlen (buf));
    }
}