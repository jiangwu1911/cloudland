#include "unixsock.hpp"
#include "exception.hpp"

UnixSocket::UnixSocket()
    : sockFile("/tmp/cloudland.sock")
{
    socket = ::socket(AF_UNIX, SOCK_DGRAM, 0);
    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sun_family = AF_UNIX;
    strcpy(srvaddr.sun_path, sockFile.c_str());
}

UnixSocket::~UnixSocket()
{
    close(socket);
}

int UnixSocket::init()
{
    int rc = -1;

    unlink(sockFile.c_str());
    rc = bind(socket, (struct sockaddr*)&srvaddr, sizeof(srvaddr));
    if (rc != 0) {
        throw CommonException(CommonException::SOCK_BIND_ERROR);
    }   

    return socket;
}

int UnixSocket::poll(struct Command &cmd)
{
    ssize_t n;
    char *cont;

    n = ::recvfrom(socket, &cmd, sizeof(cmd) - sizeof(void *), 0, NULL, NULL);
    if (n < 0)
        throw CommonException(CommonException::SOCK_RECV_ERROR);
    cont = new char[cmd.size];
    n = ::recvfrom(socket, (void *)cont, (size_t)cmd.size, 0, NULL, NULL);
    if (n < 0)
        throw CommonException(CommonException::SOCK_RECV_ERROR);
    cmd.content = cont;


    return n;
}

int UnixSocket::send(struct Command &cmd)
{
    int n;

    n = sendto(socket, &cmd, sizeof(cmd) - sizeof(void *), 0, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr_un));
    if (n < 0)
        throw CommonException(CommonException::SOCK_SEND_ERROR);
    n = sendto(socket, cmd.content, cmd.size, 0, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr_un));
    if (n < 0)
        throw CommonException(CommonException::SOCK_SEND_ERROR);

    return 0;
}

