#!/bin/bash
for ROOTFS_ARCHIVE in `find . -name "*.c"`
do
	echo $ROOTFS_ARCHIVE
done
