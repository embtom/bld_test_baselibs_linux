/*
 * Room.hpp
 *
 *  Created on: May 31, 2018
 *      Author: thomas
 */

#ifndef TEST_MODULES_TEST_DESIGN_PATTERN_ROOM_HPP_
#define TEST_MODULES_TEST_DESIGN_PATTERN_ROOM_HPP_

#include "MapSide_Base.hpp"

enum Direction {North, South, East, West};

class Room : public MapSite {
public:
	Room(int roomNo) : _roomNumber(roomNo) {};
	~Room() {};
	MapSite* GetSide(Direction) const;
	void SetSide (Direction _dir, MapSite *_side)
	{
		_sides[_dir] = _side;
	};
	virtual void Enter() {};
public:
	MapSite* _sides[4];
	int _roomNumber;
};



#endif /* TEST_MODULES_TEST_DESIGN_PATTERN_ROOM_HPP_ */
