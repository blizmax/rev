//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Generic matrix
#ifndef _REV_MATH_ALGEBRA_MATRIX_H_
#define _REV_MATH_ALGEBRA_MATRIX_H_

#include <cstddef>
#include <algorithm>
#include <initializer_list>

namespace rev {
	namespace math {

		template<class T_, size_t rows_, size_t cols_>
		struct Matrix {
			using Element = T_;
			static constexpr size_t rows = rows_;
			static constexpr size_t cols = cols_;
			using Transpose = Matrix<T_,cols,rows>;

			// Basic construction
			Matrix() = default;
			Matrix(const Matrix&) = default;
			Matrix(std::initializer_list<T_> _l) {
				auto iter = _l.begin();
				for(size_t i = 0; i < rows; ++i)
					for(size_t j = 0; j < cols; ++j) {
						(*this)(i,j) = *iter++;
					}
			}
			Matrix(T_ _x, T_ _y)
				: m{{_x},
					{_y}}
			{
				static_assert(cols==1, "This constructor is for vectors only");
			}
			Matrix(T_ _x, T_ _y, T_ _z)
				: m[0][0](_x)
				, m[1][0](_y)
				, m[2][0](_z)
			{
				static_assert(cols==1, "This constructor is for vectors only");
			}

			// Smarter construction
			static constexpr Matrix identity();
			static constexpr Matrix zero();
			static constexpr Matrix ones();

			// Element access
			T_&			operator()	(size_t _i);
			T_			operator() 	(size_t _i) const;
			T_&			operator()	(size_t row, size_t col)		{ return m[row][col]; }
			const T_&	operator()	(size_t row, size_t col) const	{ return m[row][col]; }
			T_&	x()			{ return namedElement<0>(); }
			T_	x()	const	{ return namedElement<0>(); }
			T_&	y()			{ return namedElement<1>(); }
			T_	y()	const	{ return namedElement<1>(); }
			T_&	z()			{ return namedElement<2>(); }
			T_	z()	const	{ return namedElement<2>(); }
			T_&	w()			{ return namedElement<3>(); }
			T_	w()	const	{ return namedElement<3>(); }

			// Matrix operations
			T_			trace		() const;
			Transpose	transpose	() const;
			Matrix		inverse		() const;

			// Component wise operations
			template<typename Operator_>
			Matrix cwiseOperator(const Matrix& _b, const Operator_& _operation) const;
			Matrix cwiseProduct(const Matrix& _b) const { return cwiseOperator(_b,[](T_ a, T_ b){ return a*b; }); }
			Matrix cwiseMax(const Matrix& _b) const { return cwiseOperator(_b,[](T_ a, T_ b)->T_ { return std::max(a,b);}); }
			Matrix cwiseMin(const Matrix& _b) const { return cwiseOperator(_b,[](T_ a, T_ b)->T_ { return std::min(a,b);}); }

			// Visitor operations
			template<typename Visitor_>
			Matrix visitorOperator(const Visitor_& _visitor) const;
			Matrix abs() const { return visitorOperator([](T_ a){ return std::abs(a); }); }
			Matrix operator-() const { return visitorOperator([](T_ a){ return -a; }); }

		private:
			template<size_t n_>
			T_& namedElement() { 
				static_assert(cols>n_||rows>n_, "Only vectors have named element access"); return (*this)(n_); 
			}
			template<size_t n_>
			T_  namedElement() const { 
				static_assert(cols>n_||rows>n_, "Only vectors have named element access"); return (*this)(n_); 
			}

			T_ m[rows][cols]; ///< Storage, column-major
		};

		//------------------------------------------------------------------------------------------------------------------
		// Inline implementation
		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		constexpr Matrix<T_, rows_, cols_> Matrix<T_, rows_, cols_>::identity() {
			static_assert(rows_==cols_, "Matrix must be square. Otherwise identity isn't well defined");
			Matrix result;
			for(auto j = 0; j < cols; ++j) {
				for(auto i = 0; i < rows; ++i) {
					result(i,j) = T_(i==j? 1 : 0);
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		constexpr Matrix<T_, rows_, cols_> Matrix<T_, rows_, cols_>::zero() {
			Matrix result;
			for(auto j = 0; j < cols; ++j) {
				for(auto i = 0; i < rows; ++i) {
					result(i,j) = T_(0);
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		constexpr Matrix<T_, rows_, cols_> Matrix<T_, rows_, cols_>::ones() {
			Matrix result;
			for(auto j = 0; j < cols; ++j) {
				for(auto i = 0; i < rows; ++i) {
					result(i,j) = T_(1);
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		// External operators
		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		T_&	Matrix<T_,rows_,cols_>::operator()	(size_t _i)	{ 
			static_assert(cols==1||rows==1, "Only vectors have single coefficient element access");
			return (*this)(0,_i); 
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		T_	Matrix<T_,rows_,cols_>::operator()	(size_t _i)	const { 
			static_assert(cols==1||rows==1, "Only vectors have single coefficient element access");
			return (*this)(0,_i); 
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		Matrix<T_,rows_,cols_> operator+(
			const Matrix<T_,rows_,cols_>& _a,
			const Matrix<T_,rows_,cols_>& _b
			)
		{
			return _a.cwiseOperator(_b, [](T_ a, T_ b) { return a+b; });
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		Matrix<T_,rows_,cols_> operator-(
			const Matrix<T_,rows_,cols_>& _a,
			const Matrix<T_,rows_,cols_>& _b
			)
		{
			return _a.cwiseOperator(_b, [](T_ a, T_ b) { return a-	b; });
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t m_, size_t n_, size_t l_>
		Matrix<T_,m_,l_> operator*(
			const Matrix<T_,m_,n_>& _a,
			const Matrix<T_,n_,l_>& _b
			)
		{
			Matrix<T_,m_,l_> result;
			for(auto i = 0; i < m_; ++i) { // for each row in _a
				for(auto k = 0; k < l_; ++k) { // for each column in _b
					result(i,k) = T_(0);
					for(auto j = 0; j < n_; ++j) { // for each element
						result(i,j) += _a(i,j) * _b(j,k);
					}
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		// Dot product
		template<class T_, size_t rows_>
		T_ operator*(const Matrix<T_, rows_, 1>& _a, const Matrix<T_, rows_, 1>& _b)
		{
			T_ res(0);
			for(auto i = 0; i < rows_; ++i)
				res += _a(i) * _b(i);
			return res;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		Matrix<T_, rows_, cols_> operator*(const Matrix<T_, rows_, cols_>& m, T_ k)
		{
			Matrix<T_, rows_, cols_> result;
			for(auto j = 0; j < cols_; ++j) {
				for(auto i = 0; i < rows_; ++i) {
					result(i,j) = m(i,j) * k;
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		Matrix<T_, rows_, cols_> operator*(T_ k, const Matrix<T_, rows_, cols_>& m)
		{
			return m*k;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		template<typename Operator_>
		auto Matrix<T_, rows_, cols_>::cwiseOperator(
			const Matrix<T_, rows_, cols_>& _b,
			const Operator_& _operation
		) const -> Matrix
		{
			Matrix result;
			for(auto j = 0; j < cols; ++j) {
				for(auto i = 0; i < rows; ++i) {
					result(i,j) = _operation((*this)(i,j), _b(i,j));
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		template<typename Visitor_>
		auto Matrix<T_, rows_, cols_>::visitorOperator(
			const Visitor_& _operation
		) const -> Matrix
		{
			Matrix result;
			for(auto j = 0; j < cols; ++j) {
				for(auto i = 0; i < rows; ++i) {
					result(i,j) = _operation((*this)(i,j));
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		bool operator==(const Matrix<T_,rows_,cols_>& _a, const Matrix<T_,rows_,cols_>& _b)
		{
			for(auto j = 0; j < cols_; ++j) {
				for(auto i = 0; i < rows_; ++i) {
					if(_a(i,j) != _b(i,j))
						return false;
				}
			}
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------
		// Useful aliases
		template<typename T_>
		using Matrix22 = Matrix<T_,2,2>;
		template<typename T_>
		using Matrix33 = Matrix<T_,3,3>;
		template<typename T_>
		using Matrix34 = Matrix<T_,3,4>;
		template<typename T_>
		using Matrix44 = Matrix<T_,4,4>;

		using Mat22f = Matrix22<float>;
		using Mat33f = Matrix33<float>;
		using Mat34f = Matrix34<float>;
		using Mat44f = Matrix44<float>;
	}	// namespace math
}	// namespace rev

#endif // _REV_MATH_ALGEBRA_MATRIX_H_