/*
 * Maze.hpp
 *
 *  Created on: May 31, 2018
 *      Author: thomas
 */

#ifndef TEST_MODULES_TEST_DESIGN_PATTERN_MAZE_HPP_
#define TEST_MODULES_TEST_DESIGN_PATTERN_MAZE_HPP_

#include <algorithm>
#include <iostream>
#include <list>
#include "Room.hpp"

class Maze
{
public:
	Maze() {};
	void AddRoom(Room* ro)
	{
		std::cout << "Add Room" << ro ->_roomNumber << std::endl;
		if (_rooms.empty()) {
			_rooms.push_front(ro);
		}
		else {
			for (std::list<Room*>::iterator it = _rooms.begin(); it != _rooms.end(); ++it){
				std::cout << "Curr Room " << (*it)->_roomNumber	 << std::endl;
				if (((*it)->_roomNumber) < ro->_roomNumber) {
					_rooms.insert(it, 1, ro);
				}

			}
		}
	}

	void PrintList(void)
	{
		for (std::list<Room*>::iterator it = _rooms.begin(); it != _rooms.end(); ++it) {
			std::cout << "Room Nr " << (*it)->_roomNumber << std::endl;
		}

	}


	Room* RoomNo(int _num) const;
private:
	std::list<Room*> _rooms;
};



#endif /* TEST_MODULES_TEST_DESIGN_PATTERN_MAZE_HPP_ */
