#pragma once
struct c_pawn;

struct c_actor {
	char pad[0x3c];
	float unk_meme;
	int classid;
	c_pawn* is_player();
};