#ifndef PATCH_H
#define PATCH_H

#include <Eigen/Eigen>
#include <vector>
#include "mesh.h"
using namespace Eigen;

struct patch{
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	Vector3f p_vs[4];
	Vector3f parameter[4];
	Vector3f Normal[4];
	std::vector<patch*> childs;
	Mesh2D * mesh;
};

#endif