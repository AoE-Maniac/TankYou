#pragma once

class LevelInfo {
public:
	LevelInfo(int xpPerLevel);
	int getLevel();
	void addExperience(int xp);

private:
	int xpPerLevel;
	int xp;
};