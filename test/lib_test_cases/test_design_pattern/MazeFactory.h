/*
 * MazeFactory.h
 *
 *  Created on: Jun 3, 2018
 *      Author: thomas
 */

#ifndef TEST_MODULES_TEST_DESIGN_PATTERN_MAZEFACTORY_H_
#define TEST_MODULES_TEST_DESIGN_PATTERN_MAZEFACTORY_H_

#include <Maze.hpp>
#include <Wall.hpp>
#include <Room.hpp>


class MazeFactory {
public:
	MazeFactory();
	virtual ~MazeFactory();
	virtual Maze* MakeMaze() const {
		return new Maze;
	}
	virtual Wall* MakeWall() const {
		return new Wall;
	}
	virtual Room* MakeRoom(int n) const {
		return new Room(n);
	}
	virtual Door* MakeDoor(Room *r1, Room *r2) const {
		return new Door(r1, r2);
	}
};

#endif /* TEST_MODULES_TEST_DESIGN_PATTERN_MAZEFACTORY_H_ */
