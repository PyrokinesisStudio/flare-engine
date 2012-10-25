/*
Copyright © 2012 Justin Jacobs

This file is part of FLARE.

FLARE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

FLARE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
FLARE.  If not, see http://www.gnu.org/licenses/
*/

/**
 * class EffectManager
 */

#include "Animation.h"
#include "AnimationSet.h"
#include "EffectManager.h"

using namespace std;

EffectManager::EffectManager() {
	bleed_dmg = 0;
}

EffectManager::~EffectManager() {
}

void EffectManager::logic() {
	bleed_dmg = 0;
	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].duration > 0) {
			if (effect_list[i].type == "bleed" && effect_list[i].ticks % 30 == 1) bleed_dmg += 1;
			if (effect_list[i].ticks > 0) effect_list[i].ticks--;
			if (effect_list[i].ticks == 0) removeEffect(i);
		}
		if (effect_list[i].shield_maxhp > 0) {
			if (effect_list[i].shield_hp == 0) removeEffect(i);
		}
	}
}

void EffectManager::addEffect(int _id, int _icon, int _duration, int _shield_hp, std::string _type, std::string _animation) {
	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].id == _id) {
			if (effect_list[i].duration <= _duration)
				effect_list[i].ticks = effect_list[i].duration = _duration;
			if (effect_list[i].shield_maxhp <= _shield_hp)
				effect_list[i].shield_hp = effect_list[i].shield_maxhp = _shield_hp;
			return; // we already have this effect
		}
		// if we're adding an immunity effect, remove all negative effects
		if (_type == "immunity") {
			if (effect_list[i].type == "bleed") removeEffect(i);
			else if (effect_list[i].type == "slow") removeEffect(i);
			else if (effect_list[i].type == "stun") removeEffect(i);
			else if (effect_list[i].type == "immobilize") removeEffect(i);
		}
	}

	// if we're already immune, don't add negative effects
	if (hasImmunity()) {
		if (_type == "bleed") return;
		else if (_type == "slow") return;
		else if (_type == "stun") return;
		else if (_type == "immobilize") return;
	}

	Effect e;

	e.id = _id;
	e.icon = _icon;
	e.ticks = e.duration = _duration;
	e.shield_hp = e.shield_maxhp = _shield_hp;
	e.type = _type;

	if (_animation != "") {
		e.animation = loadAnimation(_animation);
		if (e.animation) {
			anim->increaseCount(_animation);
			e.animation_name = _animation;
		}
	}

	effect_list.push_back(e);
}

void EffectManager::removeEffect(int _id) {
	effect_list.erase(effect_list.begin()+_id);
}

int EffectManager::damageShields(int _dmg) {
	int over_dmg = _dmg;

	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].shield_maxhp > 0) {
			effect_list[i].shield_hp -= _dmg;
			if (effect_list[i].shield_hp < 0) {
				if (abs(effect_list[i].shield_hp) < over_dmg) over_dmg = abs(effect_list[i].shield_hp);
				effect_list[i].shield_hp = 0;
			} else {
				over_dmg = 0;
			}
		}
	}

	return over_dmg;
}

bool EffectManager::hasImmunity() {
	for (unsigned i=0; i<effect_list.size(); i++) {
		if (effect_list[i].type == "immunity") return true;
	}
	return false;
}

Animation* EffectManager::loadAnimation(std::string &s) {
	if (s != "") {
		AnimationSet *animationSet = anim->getAnimationSet(s);
		return animationSet->getAnimation(animationSet->starting_animation);
	}
	return NULL;
}

