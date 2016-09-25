#include "pch.h"

#include "LevelInfo.h"

LevelInfo::LevelInfo(int xpPerLevel) {
	this->xpPerLevel = xpPerLevel;
}

int LevelInfo::getLevel() {
	return xp / xpPerLevel;
}

void LevelInfo::addExperience(int xp) {
	this->xp += xp;
}
