#ifndef MATRIXOPERATIONS_HPP
	#define MATRIXOPERATIONS_HPP
	
	#include "vec3.hpp"
	#include <vector>
	
	/**
		matrix addition
		@param: mat1, mat2
		@return: new mat
	*/
	
	template <typename T> std::vector<std::vector<T> > operator+(std::vector<std::vector<T> > mat1, std::vector<std::vector<T> > mat2){
		std::vector<std::vector<T> > result;
		if(mat1.size() == mat2.size() && mat1[0].size() == mat2[0].size()){
			for(unsigned int i = 0; i < mat1.size(); i++){
				std::vector<T> row;
				for(unsigned int j = 0; j < mat1[i].size(); j++){
					row.push_back(mat1[i][j] + mat2[i][j] );
				}
				result.push_back(row);
			}
		} else {
			std::cerr << "Matrix dimensions must agree!" << std::endl;
		}
		return result;
	};
	
	/**
		matrix subtraction
		@param: mat1, mat2
		@return: new mat
	*/
	
	template <typename T> std::vector<std::vector<T> > operator-(std::vector<std::vector<T> > mat1, std::vector<std::vector<T> > mat2){
		std::vector<std::vector<T> > result;
		if(mat1.size() == mat2.size() && mat1[0].size() == mat2[0].size()){
			for(unsigned int i = 0; i < mat1.size(); i++){
				std::vector<T> row;
				for(unsigned int j = 0; j < mat1[i].size(); j++){
					row.push_back(mat1[i][j] - mat2[i][j] );
				}
				result.push_back(row);
			}
		} else {
			std::cerr << "Matrix dimensions must agree!" << std::endl;
		}
		return result;
	};
	
	/**
		matrix multiplication
		@param: mat1, mat2
		@return: new mat
	*/
	
	template <typename T> std::vector<std::vector<T> > operator*(std::vector<std::vector<T> > mat1, std::vector<std::vector<T> > mat2){
		std::vector<std::vector<T> > result;
		if(mat1[0].size() == mat2.size()){
			for(unsigned int i = 0; i < mat1.size(); i++){
				std::vector<T> row;
				for(unsigned int j = 0; j < mat2[i].size(); j++){
					T element = mat1[0][0] - mat1[0][0];
					for(unsigned int k = 0; k < mat1[j].size();k++){
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
	};
	
	/**
		scales a matrix
		@param: mat1, double scalar
		@return: new mat
	*/
	
	template <typename T> std::vector<std::vector<T> > operator*(std::vector<std::vector<T> > mat, double scalar){
		std::vector<std::vector<T> > result;
		for(unsigned int i = 0; i < mat.size(); i++){
			std::vector<T> row;
			for(unsigned int j = 0; j < mat[i].size(); j++){
				row.push_back(mat[i][j] * scalar);
			}
			result.push_back(row);
		}
		return result;
	};
	
	/**
		matrix multiplication (component wise)
		@param: mat1, mat2
		@return: new mat
	*/
	
	template <typename T> std::vector<std::vector<T> > operator%(std::vector<std::vector<T> > mat1, std::vector<std::vector<T> > mat2){
		std::vector<std::vector<T> > result;
		if(mat1.size() == mat2.size() && mat1[0].size() == mat2[0].size()){
			for(unsigned int i = 0; i < mat1.size(); i++){
				std::vector<T> row;
				for(unsigned int j = 0; j < mat1[i].size(); j++){
					row.push_back(mat1[i][j] * mat2[i][j] );
				}
				result.push_back(row);
			}
		} else {
			std::cerr << "Matrix dimensions must agree!" << std::endl;
		}
		return result;
	};
	
	/**
		matrix division (component wise)
		@param: mat1, mat2
		@return: new mat
	*/
	
	template <typename T> std::vector<std::vector<T> > operator/(std::vector<std::vector<T> > mat1, std::vector<std::vector<T> > mat2){
		std::vector<std::vector<T> > result;
		if(mat1.size() == mat2.size() && mat1[0].size() == mat2[0].size()){
			for(unsigned int i = 0; i < mat1.size(); i++){
				std::vector<T> row;
				for(unsigned int j = 0; j < mat1[i].size(); j++){
					row.push_back(mat1[i][j] / mat2[i][j] );
				}
				result.push_back(row);
			}
		} else {
			std::cerr << "Matrix dimensions must agree!" << std::endl;
		}
		return result;
	};
	
	/**
		matrix transponation
		@param: mat
		@return: new mat
	*/
	
	template <typename T> std::vector<std::vector<T> > operator~(std::vector<std::vector<T> > mat){
		std::vector<std::vector<T> > result;
		for(unsigned int i = 0; i < mat.size(); i++){
			std::vector<T> row;
			for(unsigned int j = 0; j < mat[i].size(); j++){
				row.push_back(mat[j][i]);
			}
			result.push_back(row);
		}
		return result;
	};
	
	inline std::vector<std::vector<float> > operator!(std::vector<std::vector<float> > mat33){
		std::vector<std::vector<float> > inverse = {{0,0,0},{0,0,0},{0,0,0}};
		if(mat33.size() == 3 && mat33[0].size() == 3){
			float det3 = 0;
			for(unsigned int i = 0; i < mat33.size(); ++i)
				det3 += (mat33[0][i]*(mat33[1][(i+1)%3]*mat33[2][(i+2)%3] - mat33[1][(i+2)%3]*mat33[2][(i+1)%3]));
			
			for(unsigned int i = 0; i < mat33.size(); ++i)
				for(unsigned int j = 0; j < mat33[0].size(); ++j)
					inverse[j][i] = ((mat33[(i+1)%3][(j+1)%3] * mat33[(i+2)%3][(j+2)%3]) - (mat33[(i+1)%3][(j+2)%3]*mat33[(i+2)%3][(j+1)%3]))/det3;

			return inverse;
		}
		std::cerr << "Matrix dimensions must agree!" << std::endl;
		return inverse;
	};
	
	inline Vec3 operator*(std::vector<std::vector<float> > mat33, const Vec3& v3){
		if(mat33.size() == 3 && mat33[0].size() == 3){
			std::vector<std::vector<float> > v,result;
			v = {{v3.p[0]},{v3.p[1]},{v3.p[2]}};
			result = mat33*v;
			return Vec3(result[0][0],result[1][0],result[2][0]);
		}
		std::cerr << "Matrix dimensions must agree!" << std::endl;
		return Null3;
	};
	
	// inline Vec3 operator|(std::vector<std::vector<float> > mat33, const Vec3& v3){
		// if(mat33.size() == 3 && mat33[0].size() == 3){
			// std::vector<std::vector<float> > v,result;
			// std::vector<std::vector<float> > inverse = {{0,0,0},{0,0,0},{0,0,0}};
			// float det3 = 0;
			// for(unsigned int i = 0; i < mat33.size(); ++i)
				// det3 += (mat33[0][i]*(mat33[1][(i+1)%3]*mat33[2][(i+2)%3] - mat33[1][(i+2)%3]*mat33[2][(i+1)%3]));
			
			// for(unsigned int i = 0; i < mat33.size(); ++i)
				// for(unsigned int j = 0; j < mat33[0].size(); ++j)
					// inverse[j][i] = ((mat33[(i+1)%3][(j+1)%3] * mat33[(i+2)%3][(j+2)%3]) - (mat33[(i+1)%3][(j+2)%3]*mat33[(i+2)%3][(j+1)%3]))/det3;
			
			// v = {{v3.p[0]},{v3.p[1]},{v3.p[2]}};
			// result = inverse*v;
			// return Vec3(result[0][0],result[1][0],result[2][0]);
		// }
		// std::cerr << "Matrix dimensions must agree!" << std::endl;
		// return Null3;
	// };
	
#endif