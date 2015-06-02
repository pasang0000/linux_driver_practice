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
    int ret;
    int fd;

    if (argc != 2)
    {
        fprintf(stderr, "Invalid usage\n");
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return -EINVAL;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        fprintf(stderr, "Failed to open: %s\n", strerror(errno));
        return -errno;
    }

    while (1)
    {
        ret = read(fd, buf, sizeof(buf));
        if (ret < 0)
        {
            fprintf(stderr, "Failed to read: %s\n", strerror(errno));
            ret = -errno;
            goto end;
        }
        else if (ret == 0)   /* end of file */
        {
            break;
        }

        ret = write(STDOUT_FILENO, buf, ret);
        if (ret < 0)
        {
            fprintf(stderr, "Failed to write: %s\n", strerror(errno));
            ret = -errno;
            goto end;
        }
    }

    ret = 0;
end:
    close(fd);
    return ret;
}
