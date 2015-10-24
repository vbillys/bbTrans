/* Copyright (c) 2015, Julian Straub <jstraub@csail.mit.edu> Licensed
 * under the MIT license. See the license file LICENSE.
 */

#include <iostream>
#include <random>
#include <vector>
#include "optRot/node.h"
#include "optRot/lower_bound_R3.h"
#include "optRot/upper_bound_indep_R3.h"

using namespace OptRot;

Eigen::Vector3d RandomTranslation() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<> N(0,1);
  Eigen::Vector3d t(N(gen), N(gen), N(gen));
  return t*3.;
}

int main(int argc, char ** argv) {
  Eigen::Vector3d t_true;
  t_true << 1.,1.,1.;
  Eigen::Quaterniond q;
  q.setIdentity();

  Eigen::Vector3d muA1, muA2;
  muA1 << 1.,0.,0.;
  muA2 << 0.,1.,0.;
  Eigen::Vector3d muB1, muB2;
  muB1 = muA1 + t_true;
  muB2 = muA2 + t_true;

  Eigen::Matrix3d Sigma1 = Eigen::Matrix3d::Identity();
  Eigen::Matrix3d Sigma2 = Eigen::Matrix3d::Identity();
  Sigma1 *= 0.01;
  Sigma2 *= 0.001;

  std::vector<Normal<3>> gmmA;
  gmmA.push_back(Normal<3>(muA1,Sigma1,0.3));
  gmmA.push_back(Normal<3>(muA2,Sigma2,0.7));

  std::vector<Normal<3>> gmmB;
  gmmB.push_back(Normal<3>(muB1,Sigma1,0.3));
  gmmB.push_back(Normal<3>(muB2,Sigma2,0.7));

  std::vector<Normal<3>> gmmT;
  ComputeGmmT(gmmA, gmmB, gmmT, q);

  std::cout << "GMM A" << std::endl;
  for (auto& g: gmmA) g.Print();
  std::cout << "GMM B" << std::endl;
  for (auto& g: gmmB) g.Print();
  std::cout << "GMM T" << std::endl;
  for (auto& g: gmmT) g.Print();
  
  Eigen::Vector3d min, max;
  min << 0.,0.,0.;
  max << 2.,2.,2.;
  std::list<NodeR3> nodes = GenerateNotesThatTessellateR3(min, max, 0.3);

  LowerBoundR3 lower_bound(gmmA, gmmB, q);
  UpperBoundIndepR3 upper_bound(gmmA, gmmB, q);
  
  Eigen::VectorXd lbs(nodes.size());
  Eigen::VectorXd ubs(nodes.size());
  Eigen::VectorXd ubCs(nodes.size());
  std::size_t i=0;
  for (NodeR3& node : nodes) {
    std::cout << "---- box " << std::endl;
    std::cout << node.GetBox().GetCenter().transpose() << std::endl;

    std::cout << " ------ " << std::endl;
    lbs[i] = lower_bound.Evaluate(node);
    std::cout << "lower bound: " << lbs[i] << std::endl;
//    std::cout << " ------ " << std::endl;
//    ubCs[i] = upper_bound_convexity.Evaluate(node);
//    std::cout << "upper bound C: " << ubCs[i] << std::endl;
//    if (ubCs[i] - lbs[i] < -1.) 
//      std::cout << " !! large deviation !!" << std::endl;
//    std::cout << " ------ " << std::endl;
    ubs[i] = upper_bound.Evaluate(node);
    std::cout << "upper bound: " << ubs[i] << std::endl;
    ++i;
  }
  std::cout << "Lower Bounds: " << std::endl;
  std::cout << lbs.transpose() << std::endl;
  std::cout << "Upper Bounds: " << std::endl;
  std::cout << ubs.transpose() << std::endl;
  std::cout << "# upper < lower " << (ubs.array() < lbs.array()).transpose()
    << std::endl;
//  std::cout << "Upper Bounds Convexity: " << std::endl;
//  std::cout << ubCs.transpose() << std::endl;
//  std::cout << "# upper < lower " << (ubCs.array() < lbs.array()).transpose()
//    << std::endl;
//  std::cout << "# upper < lower " << (ubCs.array() < lbs.array()-1e-6).transpose()
//    << std::endl;
//  std::cout << "# upper - lower " << (ubCs.array() - lbs.array()).transpose()
//    << std::endl;
  return 0;
}