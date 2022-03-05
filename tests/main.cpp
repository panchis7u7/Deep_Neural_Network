#include <include/Matrix.hpp>
#include <gtest/gtest.h>

TEST(MatrixAllocation, Stack)
{
	voxel::Matrix<float> m(2, 2);
	m.randomize();

	EXPECT_TRUE(2 == m.getRows());
}

TEST(MatrixVectorStackAllocation, Stack)
{
	std::vector<float> vec = {1.2, 2.2, 3.4, 4.4};
	voxel::Matrix<float> mat(vec);
	EXPECT_TRUE(mat.getRows() == vec.size() && mat.getColumns() == 1);
	mat.forEach([&](float data, unsigned row, unsigned column) -> void
				{ EXPECT_TRUE(vec.at(row) == data); });
}

TEST(StaticMatrixDotProduct, Operations)
{
	voxel::Matrix<float> *src = new voxel::Matrix<float>(2, 2);
	voxel::Matrix<float> *originalSource = src;
	voxel::Matrix<float> aOperand(2, 3);
	voxel::Matrix<float> bOperand(3, 2);

	voxel::Matrix<float>::dot(src, &aOperand, &bOperand);
	voxel::Matrix<float> *newSoruce = src;
	EXPECT_TRUE(originalSource == newSoruce);

	voxel::Matrix<float> *stdMul = voxel::Matrix<float>::dot(&aOperand, &bOperand);
	float **stdMulData = stdMul->getData();

	src->forEach([&](float data, unsigned row, unsigned column)
				 { EXPECT_TRUE(data == stdMulData[row][column]); });

	delete src;
}