#pragma once
#include <cstdint>

enum { NAME_SIZE = 1024 };

/** Implementation detail exposed for debug visualizers */
struct FNameEntryHeader
{
	int unk0;
	int unk1;
	int unk2;
};

/**
 * A global deduplicated name stored in the global name table.
 */
struct FNameEntry
{
	FNameEntryHeader Header;
	union
	{
		char	AnsiName[NAME_SIZE];
		wchar_t	WideName[NAME_SIZE];
	};
};