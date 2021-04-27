#include <iostream>
#include <cassert>
#include <chrono>
#include <unistd.h>
#include "matrix.hpp"
#define N 2000

int main() {
	// perf
#ifdef debug
	std::cout << "pid: " << getpid() << std::endl;
	getchar();
#endif

	// constructor
	Column_Major_Matrix<int> c1(2, 3);
	Row_Major_Matrix<int>    r1(3, 2);
	std::cout << c1;
	std::cout << r1;

	// matrix multiplication
	auto m1 = c1 * r1;
	auto m2 = r1 * c1;
	std::cout << m1;
	std::cout << m2;

	// copy constructor
	Column_Major_Matrix<int> c2 (c1);
	Row_Major_Matrix<int>    r2 = (r1);
	assert(c1 == c2);
	assert(r1 == r2);

	// move constructor
	Column_Major_Matrix<int> c3 = std::move(c2);
	Row_Major_Matrix<int>    r3 = std::move(r2);
	assert(c2.size() == 0);
	assert(r2.size() == 0);
	assert(c1 == c3);
	assert(r1 == r3);

	// getter, setter
	c3.setter(0, 0, 100);
	assert(c3.getter(0, 0) == 100);

	// implicit type conversion
	Row_Major_Matrix<int> rc1 = c1;
	Column_Major_Matrix<int> cr1 = r1;
	assert(rc1 == c1);
	assert(cr1 == r1);

	// single-threaded matrix multiplication (operator*)
	auto c_sing = Column_Major_Matrix<int>(N, N);
	auto r_sing = Row_Major_Matrix<int>(N, N);
	auto s1 = std::chrono::high_resolution_clock::now();
	auto t1 = c_sing * r_sing;
	auto e1 = std::chrono::high_resolution_clock::now();
	auto d1 = std::chrono::duration<double>(e1 - s1);

	// check multi-thread result
	auto t3 = c_sing % r_sing;
	assert(t1 == t3 and "multithread answer incorrect");
//	std::cout << t1 << std::endl;
//	std::cout << t2 << std::endl;

	// multi-threaded matrix multiplication (operator%)
	auto c_mul = Column_Major_Matrix<int>(N, N);
	auto r_mul = Row_Major_Matrix<int>(N, N);
	auto s2 = std::chrono::high_resolution_clock::now();
	auto t2 = c_mul % r_mul;
	auto e2 = std::chrono::high_resolution_clock::now();
	auto d2 = std::chrono::duration<double>(e2 - s2);

	// result
	std::cout << "Matrix size: " << N << "\n"
		<< "single-threaded: " << d1.count() << "(s)\n"
		<< "multi-threaded: " << d2.count() << "(s)\n"
		<< "speed up: " << d1 / d2 << "\n";

	return 0;
}
