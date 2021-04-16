#!/usr/bin/env bash

# © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause

if [ $# -ne 2 ]
then
    echo "$0 debug|run elf_image"
    exit 0
fi

if [ "$1" == "debug" ]
then
    qemu-system-aarch64 -machine virt -cpu cortex-a72 -serial mon:stdio -nographic -m 1057 -smp 2 -kernel $2 -s -S
else
    qemu-system-aarch64 -machine virt -cpu cortex-a72 -serial mon:stdio -nographic -m 1057 -smp 2 -kernel $2
fi
