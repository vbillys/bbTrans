/* Copyright (c) 2015, Julian Straub <jstraub@csail.mit.edu> Licensed
 * under the MIT license. See the license file LICENSE.
 */
#include "optRot/s3_tessellation.h" 

namespace OptRot {

std::vector<Tetrahedron4D> TessellateS3() {
  std::vector<Tetrahedron4D> tetrahedra;
  tetrahedra.reserve(600);
  
  Eigen::Matrix<double, 4, 120> vertices;
  vertices.fill(0.);
  uint32_t i = 0;
  for (double a=0; a<2; ++a) 
    for (double b=0; b<2; ++b) 
      for (double c=0; c<2; ++c) 
        for (double d=0; d<2; ++d) {
          vertices(0,i) = a*2. - 1.;
          vertices(1,i) = b*2. - 1.;
          vertices(2,i) = c*2. - 1.;
          vertices(3,i++) = d*2. - 1.;
        }
  for (uint32_t j=0; j<4; ++j) {
    vertices(j,i++) = 2.;
    vertices(j,i++) = -2.;
  }
  // Golden Ratio 
  double phi = 0.5 * (1. + sqrt(5.));
  // All even permutations
  // http://mathworld.wolfram.com/EvenPermutation.html
  Eigen::Matrix<uint32_t, 12, 4> evenPerms;
  evenPerms << 
    0,1,2,3,
    0,2,3,1,
    0,3,1,2,
    1,0,3,2,
    1,2,0,3,
    1,3,2,0,
    2,0,1,3,
    2,1,3,0,
    2,3,0,1,
    3,0,2,1,
    3,1,0,2,
    3,2,1,0;
  for (uint32_t j=0; j<12; ++j) 
    for (double a=0; a<2; ++a) 
      for (double b=0; b<2; ++b) 
        for (double c=0; c<2; ++c) {
          vertices(evenPerms(j,0),i) = (a*2.-1.)*phi;
          vertices(evenPerms(j,1),i) = (b*2.-1.);
          vertices(evenPerms(j,2),i) = (c*2.-1.)/phi;
          vertices(evenPerms(j,3),i++) = 0.;
        }
  vertices *= 0.5;
  assert(i == 120);
  // Filter out half of the sphere.
  Eigen::Vector4d north;
  north << 1., 0., 0., 0.;
  uint32_t j = 0;
  for (i = 0; i < 120; ++i) {
    if (acos(north.transpose() * vertices.col(i)) <= 120.*M_PI/180.){
      vertices.col(j++) = vertices.col(i); 
    }
  }

  uint32_t n_vertices = j;
  std::cout << "Have " << n_vertices << " filtered vertices." << std::endl;
  // Precompute all pairwise tetrahedron edges based on the known
  // angular distance btween any two: 72 deg.
  Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic> G(n_vertices, n_vertices);
  for (i = 0; i < n_vertices; ++i) 
    for (j = 0; j < n_vertices; ++j) {
      double ang = acos(vertices.col(i).transpose() * vertices.col(j)); 
      if (fabs(ang - 36.*M_PI/180.) < 1e-6) {
        G(i,j) = 1;
      } else {
        G(i,j) = 0;
      }
    }
  std::cout << G.cast<uint32_t>().sum() << std::endl;

  // Compute all combinations "n_vertices choose 4"
  std::vector<uint32_t> comb_work(4);
  std::vector<uint32_t> combinations;
  Combinations(comb_work, 0, 0, n_vertices, 4, &combinations);
  // Find tetrahedra by looking at all possible vertex combinations and
  // selecting only the ones which have edges of the correct length
  // between them.
  std::cout << "Found " << combinations.size()/4 << " combinations." << std::endl;
  for (i = 0; i < combinations.size()/4; ++i) {
    Eigen::Map<Eigen::Matrix<uint32_t, 1, 4>> p(&(combinations[i*4]));
    //std::cout << "checking combination: " << p << std::endl;
    if (G(p(0),p(1))==1 && G(p(0),p(2))==1 && G(p(0),p(3))==1 
        && G(p(1),p(2))==1 && G(p(1),p(3))==1 && G(p(2),p(3))==1) {
//      std::cout << " found tetrahedron" << std::endl;
      tetrahedra.push_back(Tetrahedron4D(
            vertices.col(p(0)),
            vertices.col(p(1)),
            vertices.col(p(2)),
            vertices.col(p(3))));
    }
  }
  return tetrahedra;
}
}