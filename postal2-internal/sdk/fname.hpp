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

typedef int FName;

enum EFindName
{
	FNAME_Find,			// Find a name; return 0 if it doesn't exist.
	FNAME_Add,			// Find a name or add it if it doesn't exist.
	FNAME_Intrinsic,	// Find a name or add it intrinsically if it doesn't exist.
};