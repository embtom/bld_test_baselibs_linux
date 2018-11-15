/*
 * MapSide.h
 *
 *  Created on: May 31, 2018
 *      Author: thomas
 */

#ifndef TEST_MODULES_TEST_DESIGN_PATTERN_MAPSIDE_BASE_HPP_
#define TEST_MODULES_TEST_DESIGN_PATTERN_MAPSIDE_BASE_HPP_

#include <iostream>

class MapSite {
public:
	MapSite() {std::cout << "Base created" << std::endl;}
	virtual ~MapSite() {std::cout << "Base destroyed" << std::endl;};
	virtual void Enter() = 0;
};



#endif /* TEST_MODULES_TEST_DESIGN_PATTERN_MAPSIDE_BASE_HPP_ */
