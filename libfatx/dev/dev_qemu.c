#ifdef FATX_QEMU
#include "fatx_dev.h"
#include "qemu_block_backend.h"

struct fatx_dev_qemu_data {
    BlockBackend *backend;
    char         *path;
};

size_t fatx_dev_qemu_read(void *device_data, void *buf, off_t offset, size_t size, size_t items)
{
    struct fatx_dev_qemu_data *data;
    int size_r;
    
    data = (struct fatx_dev_qemu_data*) device_data;
    
    size_r = blk_pread(data->backend, offset, buf, size * items);
    if (size_r < 0)
    {
        return 0;
    }

    return size_r / size;
}

size_t fatx_dev_qemu_write(void *device_data, const void *buf, off_t offset, size_t size, size_t items)
{
    struct fatx_dev_qemu_data *data;
    int size_r;

    data = (struct fatx_dev_qemu_data*) device_data;

    size_r = blk_pwrite(data->backend, offset, buf, size * items, 0);
    if (size_r < 0)
    {
        return 0;
    }

    return size_r / size;
}

void fatx_dev_qemu_close(void *device_data)
{
    struct fatx_dev_qemu_data *data;
    data = (struct fatx_dev_qemu_data*) device_data;

    blk_unref(data->backend);
    free(data->path);
}

bool fatx_init_qemu_driver()
{
    Error *error = NULL;
    qemu_init_main_loop(&error);

    if (error)
    {
        error_report_err(error);
        return false;
    }

    bdrv_init();

    return true;
}

struct fatx_dev *fatx_open_qemu_device(const char *path)
{
    struct fatx_dev *dev;
    struct fatx_dev_qemu_data *data;
    Error *error;
    QDict *options;

    dev = malloc(sizeof(struct fatx_dev));
    if (!dev) return NULL;

    data = malloc(sizeof(struct fatx_dev_qemu_data));
    if (!data)
    {
        free(dev);
        return NULL;
    }

    data->path = malloc(strlen(path) + 1);
    strcpy(data->path, path);

    data->backend = blk_new_open(data->path, NULL, NULL, BDRV_O_RDWR, &error);
    if (!data->backend)
    {
        free(data->path);
        free(data);
        free(dev);
        return NULL;
    }

    dev->device_data = (void*) data;
    dev->uses_threads = true;
    dev->read = &fatx_dev_qemu_read;
    dev->write = &fatx_dev_qemu_write;
    dev->close = &fatx_dev_qemu_close;

    return dev;
}

#endif
