#ifndef MATRIXOPERATIONS_HPP
	#define MATRIXOPERATIONS_HPP
	
	#include "vec3.hpp"
	#include <vector>
	
	/**
		matrix addition
		@param: mat1, mat2
		@return: new mat
	*/
	
	template <class T> class Matrix{
		public:
			Matrix(void){}
			
			Matrix(std::vector<std::vector<T> > mat){
				this->mat = mat;
				this->rows = mat.size();
				this->cols = mat[0].size();
			}
			
			Matrix(Vec3 n, float theta){
				Matrix<float> eye = Matrix<float>({{1,0,0},{0,1,0},{0,0,1}});
				Matrix<float> n_x = Matrix<float>({
								{n.p[0]*n.p[0],n.p[0]*n.p[1],n.p[0]*n.p[2]},
								{n.p[1]*n.p[0],n.p[1]*n.p[1],n.p[1]*n.p[2]},
								{n.p[2]*n.p[0],n.p[2]*n.p[1],n.p[2]*n.p[2]}
							});
				Matrix<float> n_t = Matrix<float>({
									{0,-n.p[2],n.p[1]},
									{n.p[2],0,-n.p[0]},
									{-n.p[1],n.p[0],0}
								});
				this->mat = (cos(theta)*eye+sin(theta)*n_t+(1-cos(theta))*n_x).mat;
				this->rows = this->mat.size();
				this->cols = this->mat[0].size();
			}
			
			std::vector<std::vector<T> > mat;
			unsigned int rows, cols;
			
			void push_back(const std::vector<T>& row){
				mat.push_back(row);
				rows = mat.size();
				cols = mat[0].size();
			}
			
			void print(void) const{
				std::cout << std::endl;
				for(unsigned int i = 0; i < rows; ++i){
					for(unsigned int j = 0; j < cols; ++j){
						std::cout << mat[i][j] << ' ';
					}
					std::cout << std::endl;
				}
				std::cout << std::endl;
			}
			
			inline std::vector<T> &operator[](unsigned int i) {
				return mat[i];
			}
			
			inline const std::vector<T> &operator[](unsigned int i) const {
				return mat[i];
			}
			
			friend Matrix<T> operator+(const Matrix<T>& mat1,const Matrix<T>& mat2) {
				Matrix<T> result;
				if(mat1.rows == mat2.rows && mat1.cols == mat2.cols){
					for(unsigned int i = 0; i < mat1.rows; i++){
						std::vector<T> row;
						for(unsigned int j = 0; j < mat1.cols; j++){
							row.push_back(mat1[i][j] + mat2[i][j]);
						}
						result.push_back(row);
					}
				} else {
					std::cerr << "Matrix dimensions must agree!" << std::endl;
				}
				return result;
			}
			
			/**
				matrix subtraction
				@param: mat1, mat2
				@return: new mat
			*/
			
			friend Matrix<T> operator-(const Matrix<T>& mat1, const Matrix<T>& mat2) {
				Matrix<T> result;
				if(mat1.rows == mat2.rows && mat2.cols == mat2.cols){
					for(unsigned int i = 0; i < mat1.rows; i++){
						std::vector<T> row;
						for(unsigned int j = 0; j < mat1.cols; j++){
							row.push_back(mat1[i][j] - mat2[i][j] );
						}
						result.push_back(row);
					}
				} else {
					std::cerr << "Matrix dimensions must agree!" << std::endl;
				}
				return result;
			}
			
			/**
				matrix multiplication
				@param: mat1, mat2
				@return: new mat
			*/
			
			friend Matrix<T> operator*(const Matrix<T>& mat1, const Matrix<T>& mat2) {
				Matrix<T> result;
				if(mat1.cols == mat2.rows){
					for(unsigned int i = 0; i < mat1.rows; i++){
						std::vector<T> row;
						for(unsigned int j = 0; j < mat2.cols; j++){
							T element = mat1[0][0] - mat1[0][0];
							for(unsigned int k = 0; k < mat1.cols;k++){
								element = element + (mat1[i][k] * mat2[k][j]);
							}
							row.push_back(element);
						}
						result.push_back(row);
					}
				} else {
					std::cerr << "Matrix dimensions must agree!" << std::endl;
				}
				return result;
			}
			
			/**
				scales a matrix
				@param: mat1, double scalar
				@return: new mat
			*/
			
			friend Matrix<T> operator*(const Matrix<T>& mat1, double scalar) {
				Matrix<T> result;
				for(unsigned int i = 0; i < mat1.rows; i++){
					std::vector<T> row;
					for(unsigned int j = 0; j < mat1.cols; j++){
						row.push_back(mat1[i][j] * scalar);
					}
					result.push_back(row);
				}
				return result;
			}
			
			friend inline Matrix<T> operator*(double scalar, const Matrix<T>& mat1) {
				return mat1*scalar;
			}
			
			/**
				matrix multiplication (component wise)
				@param: mat1, mat2
				@return: new mat
			*/
			
			friend Matrix<T> operator%(const Matrix<T>& mat1, const Matrix<T>& mat2) {
				Matrix<T> result;
				if(mat1.rows == mat2.rows && mat1.cols == mat2.cols){
					for(unsigned int i = 0; i < mat1.rows; i++){
						std::vector<T> row;
						for(unsigned int j = 0; j < mat1.cols; j++){
							row.push_back(mat1[i][j] * mat2[i][j]);
						}
						result.push_back(row);
					}
				} else {
					std::cerr << "Matrix dimensions must agree!" << std::endl;
				}
				return result;
			}
			
			/**
				matrix division (component wise)
				@param: mat1, mat2
				@return: new mat
			*/
			
			friend Matrix<T> operator/(const Matrix<T>& mat1, const Matrix<T>& mat2) {
				Matrix<T> result;
				if(mat1.rows == mat2.rows && mat1.cols == mat2.cols){
					for(unsigned int i = 0; i < mat1.rows; i++){
						std::vector<T> row;
						for(unsigned int j = 0; j < mat1.cols; j++){
							row.push_back(mat1[i][j] / mat2[i][j] );
						}
						result.push_back(row);
					}
				} else {
					std::cerr << "Matrix dimensions must agree!" << std::endl;
				}
				return result;
			}
			
			/**
				matrix transponation
				@param: mat
				@return: new mat
			*/
			
			friend Matrix<T> operator~(const Matrix<T>& mat1) {
				Matrix<T> result;
				for(unsigned int i = 0; i < mat1.rows; i++){
					std::vector<T> row;
					for(unsigned int j = 0; j < mat1.cols; j++){
						row.push_back(mat1[j][i]);
					}
					result.push_back(row);
				}
				return result;
			}
			
			friend Matrix<float> operator!(const Matrix<T>& mat1) {
				Matrix<float> inverse({{0,0,0},{0,0,0},{0,0,0}});
				if(mat1.rows == 3 && mat1.cols == 3){
					float det3 = 0;
					for(unsigned int i = 0; i < mat1.rows; ++i)
						det3 += (mat1[0][i]*(mat1[1][(i+1)%3]*mat1[2][(i+2)%3] - mat1[1][(i+2)%3]*mat1[2][(i+1)%3]));
					
					for(unsigned int i = 0; i < mat1.rows; ++i)
						for(unsigned int j = 0; j < mat1.cols; ++j)
							inverse[j][i] = ((mat1[(i+1)%3][(j+1)%3] * mat1[(i+2)%3][(j+2)%3]) - (mat1[(i+1)%3][(j+2)%3]*mat1[(i+2)%3][(j+1)%3]))/det3;

					return inverse;
				}
				std::cerr << "Matrix dimensions must agree!" << std::endl;
				return inverse;
			}
			
			friend inline Vec3 operator*(const Matrix<T>& mat1, const Vec3& v3) {
				if(mat1.rows == 3 && mat1.cols == 3){
					Matrix<float> v({{v3.p[0]},{v3.p[1]},{v3.p[2]}});
					Matrix<float> result(mat1*v);
					return Vec3(result[0][0],result[1][0],result[2][0]);
				}
				std::cerr << "Matrix dimensions must agree!" << std::endl;
				return Null3;
			}
	};
	
#endif