/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"
#include <stdlib.h> 

// NGP File System (NGPFS) Format
// Sector Index:
// 0: Boot Sector
// 1: NGPFS Header

enum NGPFSVersion
{
	// Version 1.0: First version
	NGPFS_VERSION_1_0 = 0x0100'0000,
};

// This should be in the second sector of the disk
struct alignas(4096) NGPFSv1Header
{
	// Always "NGPF"
	char magic[4];
	// Version of the NGPFS format.
	Word version;
	// Sector size, 4096 bytes in NGPv1
	Word sector_size;
	// Total number of sectors in the disk
	Word total_sectors;

	// Start sector of node storage
	Word file_node_start;

	// Start sector of bitmap storage
	Word bitmap_start;

	// Start sector of file storage
	Word data_start;

	u8 reserved[4068];
};
static_assert(sizeof(NGPFSv1Header) == 4096, "NGPFSv1Header must be 4096 bytes");

enum FileAttributes
{
	FileReadOnly = 0x0001,
	FileWriteOnly = 0x0002,
	FileHidden = 0x0004,
	FileSystem = 0x0008,
	FileDirectory = 0x0010,

	FileReadWrite = FileReadOnly | FileWriteOnly,
	FileSystemDirectory = FileSystem | FileDirectory,
};

#define NGPFSV1_MAX_FILENAME 256

struct alignas(32) NGPFSv1FileNode
{
	// Null terminated file name
	u8 name[NGPFSV1_MAX_FILENAME];
	// Hash of the file name
	Word name_hash;
	// Start sector of the file data
	Word start_sector;
	// Sector count of the file data
	Word sector_count;
	// File size in bytes
	Word size_in_bytes;
	// File attributes
	Word attributes;
	// Creation timestamp
	Word creation_time;
	// Modified timestamp
	Word modified_time;
};
