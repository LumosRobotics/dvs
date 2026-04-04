#ifndef COMMON_SOCKET_COMPAT_H_
#define COMMON_SOCKET_COMPAT_H_

/* Portable socket includes and shims.
 * Include this instead of the POSIX socket headers in any file that does
 * network I/O.  On Windows/MinGW the standard BSD socket functions are
 * provided by winsock2; named inline wrappers bridge the remaining
 * differences without polluting the global namespace with macros that clash
 * with C++ standard-library method names (std::ostream::write, etc.).
 *
 * Use socket_send() / socket_recv() / socket_close() instead of the bare
 * POSIX write() / read() / close() on socket file descriptors.
 */

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>   /* socket(), connect(), bind(), send(), recv(), … */
#include <ws2tcpip.h>   /* inet_addr(), socklen_t, … */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* winsock2.h/windows.h pollutes the global namespace with macros like ERROR
 * (winerror.h: #define ERROR 0) which clash with enums in third-party code.
 * Undefine the worst offenders immediately after the system includes. */
#undef ERROR    /* winerror.h: #define ERROR 0  — clashes with logging enums   */
#undef DELETE   /* winnt.h:   #define DELETE …  — clashes with C++ delete     */
#undef BOOL     /* windef.h:  typedef int BOOL  — prefer the C++ bool         */
#undef IGNORE   /* winbase.h: #define IGNORE 0  — clashes with project macros */

/* bzero: not provided by MinGW */
#ifndef bzero
#define bzero(b, len) memset((b), 0, (len))
#endif

/* ssize_t: MinGW-w64 already provides this via corecrt; do not redefine. */

/* usleep(microseconds): approximate with Windows Sleep (millisecond granularity) */
#ifndef usleep
#define usleep(us) Sleep((DWORD)((us) / 1000U))
#endif

/* Named wrappers so that code using write()/read()/close() on socket fds
 * can be ported without introducing macros that break C++ stdlib headers. */
static inline int socket_close(int s) { return closesocket((SOCKET)s); }
static inline int socket_send(int s, const void* buf, size_t len)
{
    return send((SOCKET)s, (const char*)buf, (int)len, 0);
}
static inline ssize_t socket_recv(int s, void* buf, size_t len)
{
    return (ssize_t)recv((SOCKET)s, (char*)buf, (int)len, 0);
}

#else  /* POSIX */

#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static inline int socket_close(int s) { return close(s); }
static inline int socket_send(int s, const void* buf, size_t len)
{
    return (int)write(s, buf, len);
}
static inline ssize_t socket_recv(int s, void* buf, size_t len)
{
    return read(s, buf, len);
}

#endif  /* _WIN32 */

#endif  /* COMMON_SOCKET_COMPAT_H_ */
