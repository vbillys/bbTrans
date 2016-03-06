/* Copyright (c) 2015, Julian Straub <jstraub@csail.mit.edu> Licensed
 * under the MIT license. See the license file LICENSE.
 */
#pragma once

#include <stdint.h>
#include <vector>
#include <list>
#include <sstream>
#include <string>

#include "manifold/S.h"
#include "bbTrans/node_AA.h"
#include "bbTrans/node_R3.h"
#include "bbTrans/node_S3.h"
#include "bbTrans/box.h"
#include "bbTrans/tetrahedron.h"

namespace bb {

class NodeTpS3 : public NodeAA {
 public:
  NodeTpS3(const Box& box, std::vector<uint32_t> ids);
  NodeTpS3(const NodeTpS3& node);
  virtual ~NodeTpS3() = default;

 protected:
  virtual Tetrahedron4D TetraFromBox(const Box& box, uint32_t i0, uint32_t i1,
    uint32_t i2, uint32_t i3);
};
}