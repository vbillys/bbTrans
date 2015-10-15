/* Copyright (c) 2015, Julian Straub <jstraub@csail.mit.edu> Licensed
 * under the MIT license. See the license file LICENSE.
 */
#pragma once

#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include "optRot/combinations.h"
#include "optRot/tetrahedron.h"

namespace OptRot {

std::vector<Tetrahedron4D> TessellateS3();

}