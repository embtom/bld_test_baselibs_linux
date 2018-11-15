/*
 * Wall.hpp
 *
 *  Created on: May 31, 2018
 *      Author: thomas
 */

#ifndef TEST_MODULES_TEST_DESIGN_PATTERN_WALL_HPP_
#define TEST_MODULES_TEST_DESIGN_PATTERN_WALL_HPP_

#include "MapSide_Base.hpp"
#include "Room.hpp"


class Wall : public MapSite {
public:
	Wall() {};
	virtual void Enter() {};
};


class Door : public MapSite {
public:
	Door(Room* ro1, Room* ro2) : _room1(ro1), _room2(ro2), _isOpen(false) {};
	~Door() {};
	virtual void Enter() {};
	Room* OtherSideFrom(Room*);
public:
	Room* _room1;
	Room* _room2;
	bool _isOpen;
};



#endif /* TEST_MODULES_TEST_DESIGN_PATTERN_WALL_HPP_ */
