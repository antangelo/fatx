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

#ifndef FATX_DEV_H
#define FATX_DEV_H

#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>

struct fatx_dev {
    void *device_data;
    bool uses_threads;
    size_t (*read)(void *device_data, void *buf, off_t offset, size_t size, size_t items);
    size_t (*write)(void *device_data, const void *buf, off_t offset, size_t size, size_t items);
    void (*close)(void *device_data);
};

bool fatx_dev_init(void);
struct fatx_dev *fatx_dev_open(const char *path, off_t sample_partition_offset);
void fatx_dev_close(struct fatx_dev *dev);

struct fatx_dev *fatx_open_raw_device(const char *path);
bool fatx_init_raw_driver(void);

#ifdef FATX_QEMU
struct fatx_dev *fatx_open_qemu_device(const char *path);
bool fatx_init_qemu_driver(void);
#endif

typedef struct fatx_dev *(fatx_dev_open_fn)(const char *path);
typedef bool (fatx_dev_init_fn)(void);
extern fatx_dev_open_fn *fatx_dev_open_functions[];
extern fatx_dev_init_fn *fatx_dev_init_functions[];
extern size_t fatx_dev_num_types;

#endif
