#ifndef __DVF_READER_H__
#define __DVF_READER_H__

struct dvf_file;
struct dvf_container;

struct dvf_file *
dvf_file_open(char *file_name, int *err);

int
dvf_file_close(struct dvf_file *file);

struct dvf_container *
dvf_container_create(struct dvf_file *file, int *err);

int
dvf_container_destroy(struct dvf_container *container);

#endif /* __DVF_READER_H__ */
