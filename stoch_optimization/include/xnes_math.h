#pragma once

#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>

#ifdef DEBUG
#define ASSERT(b) { if (! (b)) { char msg[256]; sprintf(msg, "ASSERT failed in file %s, line %d", __FILE__, __LINE__); throw msg; } }
// #define ASSERT(b) { if (! (b)) {*((int*)(NULL)) = 0; } }        // trap the debugger
#else
#define ASSERT(b) { }
#endif

class Matrix
{
public:
	Matrix();
	explicit Matrix(unsigned int rows, unsigned int cols = 1);
	Matrix(const Matrix& other);
	Matrix(double* data, unsigned int rows, unsigned int cols = 1);
	virtual ~Matrix();

	inline bool isValid() const { return (m_data.size() > 0); }
	inline bool isVector() const { return (isColumnVector() || isRowVector()); }
	inline bool isColumnVector() const { return (m_rows == 1); }
	inline bool isRowVector() const { return (m_cols == 1); }
	inline bool isSquare() const { return (m_rows == m_cols); }

	inline unsigned int rows() const { return m_rows; }
	inline unsigned int cols() const { return m_cols; }

	inline unsigned int size() const { return m_data.size(); }

	// type cast for 1x1 matrices
	operator double() const;

	inline double& operator [] (unsigned int index)
	{
		ASSERT(index < m_data.size());
		return m_data[index];
	}
	inline const double& operator [] (unsigned int index) const
	{
		ASSERT(index < m_data.size());
		return m_data[index];
	}

	inline double& operator () (unsigned int index) { return operator [] (index); }
	inline const double& operator () (unsigned int index) const { return operator [] (index); }

	inline double& operator () (unsigned int y, unsigned int x) { return operator [] (y * m_cols + x); }
	inline const double& operator () (unsigned int y, unsigned int x) const { return operator [] (y * m_cols + x); }

	Matrix row(unsigned int r) const;
	Matrix col(unsigned int c) const;

	void resize(unsigned int rows, unsigned int cols = 1);

	const Matrix& operator = (const Matrix& other);

	bool operator == (const Matrix& other);
	bool operator != (const Matrix& other);

	Matrix operator + (const Matrix& other) const;
	Matrix operator - (const Matrix& other) const;
	Matrix operator * (const Matrix& other) const;
	void operator += (const Matrix& other);
	void operator -= (const Matrix& other);
	void operator *= (double scalar);
	void operator /= (double scalar);
	friend Matrix operator * (double scalar, const Matrix& mat);
	Matrix operator / (double scalar) const;

	static Matrix zeros(unsigned int rows, unsigned int cols = 1);
	static Matrix ones(unsigned int rows, unsigned int cols = 1);
	static Matrix eye(unsigned int n);
	static Matrix fromEig(const Matrix& U, const Matrix& lambda);

	void eig(Matrix& U, Matrix& lambda, unsigned int iter = 200, bool ignoreError = true) const;
	Matrix T() const;
	Matrix exp() const;
	Matrix log() const;
	Matrix pow(double e) const;
	inline Matrix power(double e) const { return pow(e); }
	inline Matrix sqrt() const { return pow(0.5); }
	inline Matrix inv() const { return pow(-1.0); }
	double det() const;
	double logdet() const;
	double tr() const;

	double min() const;
	double max() const;

	double norm(double p = 2.0) const;
	double onenorm() const;
	double twonorm() const;
	double twonorm2() const;
	double maxnorm() const;

	Matrix diag() const;

	void print() const;

protected:
	// thread-safe comparison predicate for indices
	class CmpIndex
	{
	public:
		CmpIndex(const Matrix& lambda, std::vector<unsigned int>& index);

		bool operator ()(unsigned int i1, unsigned int i2) const;

	protected:
		const Matrix& m_lambda;
		std::vector<unsigned int>& m_index;
	};

	unsigned int m_rows;
	unsigned int m_cols;
	std::vector<double> m_data;
};

