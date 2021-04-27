#pragma once
#include <vector>
#include <random>
#include <cassert>
#include <thread>
#include <execution>
#define THREAD_CNT 10

//namespace Matrix {

template <typename T>
class Row_Major_Matrix;

template <typename T>
class Column_Major_Matrix;

// #Base class
template <typename T>
class Basic_Matrix {
  public:
//	virtual const T& getter (size_t, size_t) {}

//	virtual void setter (size_t, size_t, const T&) {}

	void random() {
		auto gen = std::mt19937(std::random_device{}());
		auto distrib = std::uniform_int_distribution(0, 9);
		for (auto &vec : data) {
			for (auto &x : vec) {
				x = distrib(gen);
			}
		}
	}

	auto begin() { return data.begin(); }

	auto end() { return data.end(); }

	auto size() { return data.size(); }

	std::string_view name;
	std::vector<std::vector<T>> data;

	size_t row;
	size_t col;

  protected:
//	TODO: make data private
};

// #Column_Major_Matrix
template <typename T>
class Column_Major_Matrix : public Basic_Matrix<T> {
  public:
	// constructor
	Column_Major_Matrix (size_t row_, size_t col_, bool rand = 1) {
		this->data = std::vector(row_, std::vector<T>(col_));
		this->name = __func__;
		this->row = row_;
		this->col = col_;
		if (rand) this->random();
	}

	void setter (size_t r, size_t c, const T& val) {
#ifdef debug
		assert(r < this->row and c < this->col);
#endif
		this->data[r][c] = val;
	}

	const T& getter (size_t r, size_t c) {
#ifdef debug
		assert(r < this->row and c < this->col);
#endif
		return this->data[r][c];
	}

	operator Row_Major_Matrix<T>() {
		auto res = Row_Major_Matrix<T>(this->row, this->col);
		for (size_t i=0; i<this->row; i++) {
			for (size_t j=0; j<this->col; j++) {
				res.setter(i, j, this->getter(i, j));
			}
		}
		return res;
	}

	template <typename U>
	Column_Major_Matrix<T> operator* (Row_Major_Matrix<U>& other) {
		assert(this->col == other.row);
		auto res = Column_Major_Matrix<T>(this->size(), other.size(), 0);

		for (size_t i = 0; i < res.row; i++) {
			for (size_t j = 0; j < res.col; j++) {
//				auto tmp = T{};
//				for (size_t k = 0; k < this->col; k++) {
//					tmp += this->getter(i, k) * other.getter(k, j);
//				}
//				res.setter(i, j, tmp);
				res.setter(i, j, std::transform_reduce(this->data[i].begin(), this->data[i].end(), other.data[j].begin(), 0));
			}
		}
		return res;
	}

	template <typename U>
	Column_Major_Matrix<T> operator% (Row_Major_Matrix<U>& other) {
		assert(this->col == other.row);
		auto res = Column_Major_Matrix<T>(this->size(), other.size(), 0);

		using Mat = std::vector<std::vector<T>>;
		auto cal = [](Mat &m1, Mat &m2, Mat &res, size_t tid) {

			auto [idx_row, idx_col, cnt] = [](size_t r, size_t c, size_t tid) {
				auto total = r * c;
				auto chunk = total / THREAD_CNT + (total % THREAD_CNT != 0);
				auto start = tid * chunk;
				auto end = std::min(start + chunk, total);
				return std::tuple{start / r, start % r, end - start};
			} (m1.size(), m2.size(), tid);
//			std::cout << idx_row << " " << idx_col << " " << cnt << "\n";

			for (size_t i = idx_row, j = idx_col, idx = 0; i < m1.size() and idx < cnt; i++, j = 0) {
				for (; j < m2.size() and idx < cnt; j++, idx++) {
					res[i][j] = std::transform_reduce(m1[i].begin(), m1[i].end(), m2[j].begin(), 0);
				}
			}
		};

		std::vector<std::jthread> threads;
		for (int tid = 0; tid < THREAD_CNT; tid++) {
//			cal(this->data, other.data, res.data, tid);
			threads.emplace_back(cal, std::ref(this->data), std::ref(other.data), std::ref(res.data), tid);
		}

		return res;
	}

//	Column_Major_Matrix(const Column_Major_Matrix&) = default;
//	Column_Major_Matrix(Column_Major_Matrix&&) = default;
//	Column_Major_Matrix& operator=(const Column_Major_Matrix&) = default;
//	Column_Major_Matrix& operator=(Column_Major_Matrix&&) = default;
}; // class Column_Major_Matrix

// #Row_Major_Matrix
template <typename T>
class Row_Major_Matrix : public Basic_Matrix<T> {
  public:
	// constructor
	Row_Major_Matrix (size_t row_, size_t col_, bool rand = 1) {
		this->data = std::vector(col_, std::vector<T>(row_));
		this->name = __func__;
		this->row = row_;
		this->col = col_;
		if (rand) this->random();
	}

	const T& getter (size_t r, size_t c) {
#ifdef debug
		assert(r < this->row and c < this->col);
#endif
		return this->data[c][r];
	}

	void setter (size_t r, size_t c, const T& val) {
#ifdef debug
		assert(r < this->row and c < this->col);
#endif
		this->data[c][r] = val;
	}

	template <typename U>
	Row_Major_Matrix<T> operator* (Column_Major_Matrix<U>& other) {
		assert(this->col == other.row);
		auto res = Row_Major_Matrix<T>(this->row, other.col);
		for (size_t i=0; i<res.row; i++) {
			for (size_t j=0; j<res.col; j++) {
				T tmp = 0;
				for (size_t k=0; k<this->col; k++) {
					tmp += this->getter(i, k) * other.getter(k, j);
				}
				res.setter(i, j, tmp);
			}
		}
		return res;
	}

	operator Column_Major_Matrix<T>() {
		auto res = Column_Major_Matrix<T>(this->row, this->col);
		for (size_t i=0; i<this->row; i++) {
			for (size_t j=0; j<this->col; j++) {
				res.setter(i, j, this->getter(i, j));
			}
		}
		return res;
	}
};

// #Helper function
template <typename T>
concept Is_matrix = requires (T& t) {
	{ t.row } -> std::convertible_to<std::size_t>;
	{ t.col } -> std::convertible_to<std::size_t>;
};

auto& operator<<(std::ostream& os, Is_matrix auto& data) {
	os << data.name << ": (" << data.row << ", " << data.col << ")\n";
	for (size_t i=0; i<data.row; i++) {
		os << "  ";
		for (size_t j=0; j<data.col; j++) {
			os << " ["[j == 0];
			os << data.getter(i, j);
			os << ",]"[j == data.col-1];
		}
		os << "\n";
	}
	return os << std::endl;
}

bool operator== (Is_matrix auto& lhs, Is_matrix auto& rhs) {
	if (lhs.row != rhs.row or lhs.col != lhs.col) return false;
	for (size_t i=0; i<lhs.row; i++) {
		for (size_t j=0; j<lhs.col; j++) {
			if (lhs.getter(i, j) != rhs.getter(i, j)) return false;
		}
	}
	return true;
}


//} // end of Matrix namespace
