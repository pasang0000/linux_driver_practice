#include <unistd.h>     /* STDOUT_FILENO */
#include <stdio.h>      /* printf() */
#include <string.h>     /* strerror() */
#include <errno.h>      /* errno */
#include <sys/types.h>  /* open() */
#include <sys/stat.h>   /* open() */
#include <sys/fcntl.h>  /* open() */

int
main(int argc, char *argv[])
{
    char    buf[4096];
    int ret, len, i;
    int fd;

    if (argc != 2)
    {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return -EINVAL;
    }

    fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0)
    {
        fprintf(stderr, "Failed to open: %s\n", strerror(errno));
        return -errno;
    }

    while (1)
    {
        ret = read(STDIN_FILENO, buf, sizeof(buf));
        if (ret < 0)
        {
            fprintf(stderr, "Failed to write: %s\n", strerror(errno));
            ret = -errno;
            goto end;
        }
        else if (ret == 0)   /* end of file */
        {
            break;
        }

        for (i = 0, len = ret; i < len; )
        {
            ret = write(fd, buf + i, len - i);
            if (ret < 0)
            {
                fprintf(stderr, "Failed to write: %s\n", strerror(errno));
                ret = -errno;
                goto end;
            }
            i += ret;
        }
    }

    ret = 0;
end:
    close(fd);
    return ret;
}
