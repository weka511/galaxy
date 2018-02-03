/**
 * Copyright (C) 2018 Greenweaves Software Limited
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>
 */
 
#ifndef _CENTRE_OF_MASS_H
#define _CENTRE_OF_MASS_H

#include "treecode.h"

/**
 * Used to calculate centre of mass for Internal and External Nodes
 */
class CentreOfMassCalculator : public Node::Visitor {
  public:
	CentreOfMassCalculator(std::vector<Particle*> particles);
	Node::Visitor::Status visit(Node * node);
	virtual void propagate(Node * node,Node * child);
	virtual bool depart(Node * node);
	void display();
	
  private:
	std::vector<Particle*> _particles;
	std::vector<bool> indices;
};

#endif
