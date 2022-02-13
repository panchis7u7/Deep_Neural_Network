#include <include/Matrix.hpp>
#include <gtest/gtest.h>

TEST(MatrixAllocation, Stack) {
	voxel::Matrix<float> m(2, 2);
	m.randomize();

	EXPECT_TRUE(2 == m.getRows());
}