#include "Maths.hpp"

____MATH__Matrix::Matrix ____MATH__Matrix::Matrix::identity(int row)
{
	____MATH__Matrix::Matrix unitM(row, row);
	for (int i=0; i<row; i++) {
		for (int j=0; j<row; j++) {
			unitM[i][j] = (i==j ? 1.f : 0.f);
		}
	}
	return unitM;
}

std::vector<float>& ____MATH__Matrix::Matrix::operator[](const int i)
{
	return m[i];
}

____MATH__Matrix::Matrix ____MATH__Matrix::Matrix::operator*(const Matrix & a)
{
	____MATH__Matrix::Matrix result(this->nrows, a.ncols);

	for (int i=0; i<nrows; i++) {
		for (int j=0; j<a.ncols; j++) {
			result.m[i][j] = 0.f;
			for (int k=0; k<ncols; k++) {
				result.m[i][j] += m[i][k]*a.m[k][j];
			}
		}
	}
	return result;
}

____MATH__Matrix::Matrix ____MATH__Matrix::Matrix::transpose()
{
	____MATH__Matrix::Matrix result(this->nrows, this->ncols);
	for(int i=0; i<this->nrows; i++)
		for(int j=0; j< this->ncols; j++)
			result[j][i] = m[i][j];
	return result;
}

____MATH__Matrix::Matrix ____MATH__Matrix::Matrix::inverse()
{
	Matrix result(this->nrows, (this->ncols)*2);
	for(int i=0; i<this->nrows; i++)
		for(int j=0; j<this->ncols; j++)
			result[i][j] = m[i][j];
	for(int i=0; i<this->nrows; i++)
		result[i][i+this->ncols] = 1;

	for (int i=0; i<this->nrows-1; i++) {
		for(int j=result.ncols-1; j>=0; j--)
			result[i][j] /= result[i][i];
		for (int k=i+1; k<this->nrows; k++) {
			float c = result[k][i];
			for (int j=0; j<result.ncols; j++) {
				result[k][j] -= result[i][j]*c;
			}
		}
	}

	for(int j=result.ncols-1; j>=this->nrows-1; j--)
		result[this->nrows-1][j] /= result[this->nrows-1][this->nrows-1];

	for (int i=this->nrows-1; i>0; i--) {
		for (int k=i-1; k>=0; k--) {
			float coeff = result[k][i];
			for (int j=0; j<result.ncols; j++) {
				result[k][j] -= result[i][j]*coeff;
			}
		}
	}

	Matrix te(this->nrows, this->ncols);
	for(int i=0; i<this->nrows; i++)
		for(int j=0; j<this->ncols; j++)
			te[i][j] = result[i][j+this->ncols];

	return te;
}
