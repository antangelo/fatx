#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "fatx_dev.h"

size_t fatx_dev_raw_read(void *device_data, void *buf, off_t offset, size_t size, size_t items)
{
    FILE *fd = (FILE*) device_data;
    fseek(fd, offset, SEEK_SET);
    return fread(buf, size, items, fd);
}

size_t fatx_dev_raw_write(void *device_data, const void *buf, off_t offset, size_t size, size_t items)
{
    FILE *fd = (FILE*) device_data;
    fseek(fd, offset, SEEK_SET);
    return fwrite(buf, size, items, fd);
}

void fatx_dev_raw_close(void *device_data)
{
    FILE *fd;
    fd = (FILE*) device_data;
    fclose(fd);
}

bool fatx_init_raw_driver()
{
    /* No init needed */
    return true;
}

struct fatx_dev *fatx_open_raw_device(const char *path)
{
    struct fatx_dev *dev;
    FILE *fd;

    dev = malloc(sizeof(struct fatx_dev));
    if (!dev) return dev;

    fd = fopen(path, "r+b");
    if (!fd)
    {
        free(dev);
        return NULL;
    }

    dev->device_data = (void*) fd;
    dev->uses_threads = false;
    dev->read = &fatx_dev_raw_read;
    dev->write = &fatx_dev_raw_write;
    dev->close = &fatx_dev_raw_close;

    return dev;
}
