
#include <stdio.h>
#include <dvd.h>

int
main(int argc, char **argv)
{
    if(argc < 2)
        return 1;

    int err = 0;
    union dvd_entry entry;
    struct dvd_file *file = dvd_file_open(argv[1], &err);

    while (dvd_file_has_next(file)) {
        dvd_file_get_next(file, &entry);

        switch(entry.type) {
            case DVD_ENTRY_TYPE_MISC:
                printf("entry misc\n");
                break;
            default:
                printf("skipping entry\n");
                break;
        }

        dvd_entry_done(&entry);
    }
    dvd_file_close(file);

    return 0;
}

