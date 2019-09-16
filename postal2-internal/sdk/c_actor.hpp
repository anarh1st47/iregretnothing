#pragma once
struct c_pawn;

struct c_actor {
private:
	char pad[0x3c];
public:
	float unk_meme;
	int classid;
	c_pawn* is_player();
};