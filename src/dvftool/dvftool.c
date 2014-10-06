#include <stdio.h>
#include <errno.h>

#include "dvf_reader.h"

int
main(int argc, char **argv)
{
    int err = 0;
    char *file_name = NULL;
    struct dvf_file *file = NULL;

    if(argc < 2) {
        fprintf(stderr, "missing paramater\n");
        return EINVAL;
    }
    file_name = argv[1];

    file = dvf_file_open(file_name, &err);
    if (!file) {
        fprintf(stderr,
                "error: cannot open file %s: %s (%d)",
                file_name,
                strerror(err),
                err);
        return err;
    }

    struct dvf_container *container = dvf_container_create(file, &err);
    if (!container) {
        fprintf(stderr,
                "error: cannot init dvf container: %s (%d)\n",
                strerror(err),
                err);
        dvf_file_close(file);
        return err;
    }

    dvf_container_destroy(container);
    dvf_file_close(file);

    return 0;
}
