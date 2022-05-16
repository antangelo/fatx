/*
 * FATX Filesystem Library
 *
 * Copyright (C) 2015  Matt Borgerson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../fatx_internal.h"
#include "fatx_dev.h"

#include <stddef.h>
#include <assert.h>

fatx_dev_open_fn *fatx_dev_open_functions[] = {
    &fatx_open_raw_device,
#ifdef FATX_QEMU
    &fatx_open_qemu_device,
#endif
};

fatx_dev_init_fn *fatx_dev_init_functions[] = {
    &fatx_init_raw_driver,
#ifdef FATX_QEMU
    &fatx_init_qemu_driver,
#endif
};

bool fatx_dev_init()
{
    static bool init_complete = false;

    if (init_complete) return true;

    bool init_success = true;
    assert(ARRAY_SIZE(fatx_dev_open_functions) == ARRAY_SIZE(fatx_dev_init_functions));

    for (size_t i = 0; i < ARRAY_SIZE(fatx_dev_init_functions); ++i)
    {
        init_success &= fatx_dev_init_functions[i]();
    }

    init_complete = init_success;
    return init_success;
}

struct fatx_dev *fatx_dev_open(const char *path, off_t sample_partition_offset)
{
    struct fatx_dev *dev;
    uint32_t signature;

    /* 
     * Use the partition offset to detect raw or qcow2
     * No need for block driver complexity if we have a raw image
     */
    for (size_t i = 0; i < ARRAY_SIZE(fatx_dev_open_functions); ++i)
    {
        dev = fatx_dev_open_functions[i](path);
        if (!dev)
        {
            continue;
        }
        
        if (dev->read(dev->device_data, &signature, sample_partition_offset, sizeof(uint32_t), 1))
        {
            if (signature == FATX_SIGNATURE)
            {
                return dev;
            }
        }

        dev->close(dev->device_data);
    }

    return NULL;
}

void fatx_dev_close(struct fatx_dev *dev)
{
    dev->close(dev->device_data);
    free(dev);
}

/*
 * Seek to a byte offset in the device.
 */
int fatx_dev_seek(struct fatx_fs *fs, uint64_t offset)
{
    fs->seek_position = offset;

    return FATX_STATUS_SUCCESS;
}

/*
 * Seek to a cluster + byte offset in the device.
 */
int fatx_dev_seek_cluster(struct fatx_fs *fs, size_t cluster, off_t offset)
{
    int status;
    uint64_t pos;

    fatx_debug(fs, "fatx_dev_seek_cluster(cluster=%zd, offset=0x%zx)\n", cluster, offset);

    status = fatx_cluster_number_to_byte_offset(fs, cluster, &pos);
    if (status) return status;

    pos += offset;
    fs->seek_position = pos;

    return FATX_STATUS_SUCCESS;
}

/*
 * Read from the device.
 */
size_t fatx_dev_read(struct fatx_fs *fs, void *buf, size_t size, size_t items)
{
    fatx_debug(fs, "fatx_dev_read(buf=0x%p, size=0x%zx, items=0x%zx)\n", buf, size, items);
    return fs->device->read(fs->device->device_data, buf, fs->seek_position, size, items);
}

/*
 * Write to the device.
 */
size_t fatx_dev_write(struct fatx_fs *fs, const void *buf, size_t size, size_t items)
{
    fatx_debug(fs, "fatx_dev_write(buf=0x%p, size=0x%zx, items=0x%zx)\n", buf, size, items);
    return fs->device->write(fs->device->device_data, buf, fs->seek_position, size, items);
}
