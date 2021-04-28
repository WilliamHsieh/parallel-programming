#include <bits/stdc++.h>
#define all(x) begin(x),end(x)
#define what_is(x) std::cout << "[what] " << #x << " is " << x << std::endl

using namespace std;
using pii = pair<int, int>;

// #Helper
ostream& operator<< (ostream &os, vector<vector<int>> &dp) {
	for (size_t i = 0; i < dp.size(); i++) {
		for (size_t j = 0; j < dp[i].size(); j++) {
			os << setw(3) << dp[i][j] << ' ';
		}
		os << endl;
	}
	return os;
}

// #Penalty
struct Penalty {
	int match;
	int mismatch;
	int gap;
};

// #Blast
struct Blast {

// ##constructor
	Blast(vector<vector<int>> &dp, Penalty p, string_view ref, string_view seq) {
		auto IDX = pair{0, 0};
		for (int i = 1, M = dp.size(); i < M; i++) {
			for (int j = 1, N = dp.front().size(); j < N; j++) {
				if (auto [x, y] = IDX; dp[i][j] > dp[x][y]) {
					IDX = pair{i, j};
				}
			}
		}
		pos_seq.second = IDX.first - 1;
		pos_ref.second = IDX.second - 1;

		// backtracking
		for (auto [x, y] = IDX; dp[x][y] != 0; ) {
			int n_x = x, n_y = y;
			char c_seq, c_ref;
			int M = -1;

			// insert
			if (dp[x-1][y] + p.gap == dp[x][y] and dp[x-1][y] > M) {
				M = dp[x-1][y];
				n_x = x - 1;

				c_seq = seq[x-1];
				c_ref = '-';
			}

			// delete
			if (dp[x][y-1] + p.gap == dp[x][y] and dp[x][y-1] > M) {
				M = dp[x][y-1];
				n_y = y - 1;

				c_seq = '-';
				c_ref = ref[y-1];
			}

			// match or mismatch
			if (dp[x-1][y-1] + (seq[x-1] == ref[y-1] ? p.match : p.mismatch) == dp[x][y] and dp[x-1][y-1] > M) {
				M = dp[x-1][y-1];
				n_x = x - 1;
				n_y = y - 1;

				c_seq = seq[x-1];
				c_ref = ref[y-1];
			}

			aligned_seq += c_seq;
			aligned_ref += c_ref;

			x = n_x;
			y = n_y;

			if (dp[x][y] != 0) {
				pos_seq.first = x - 1;
				pos_ref.first = y - 1;
			}
		}

		reverse(all(aligned_ref));
		reverse(all(aligned_seq));
	}

// ##output
	friend ostream& operator<< (ostream &os, Blast &obj) {
//		auto [s_ref, e_ref] = obj.pos_ref;
//		auto [s_seq, e_seq] = obj.pos_seq;
		auto width = log10(max(obj.pos_ref.first, obj.pos_seq.first) + 1) + 5;

		auto print = [&os, &width](string_view title, string &s, pii &pos) {
			int space = 4;
			os << title << setw(width) << pos.first << string(space, ' ');
			os << s << string(space, ' ') << pos.second << '\n';

			if (title == "Seq1:") {
				os << string(space + width + 5, ' ');
			}
		};

		// show first sequence
		print("Seq1:", obj.aligned_ref, obj.pos_ref);

		// show match / mismatch
		for (size_t i = 0; i < obj.aligned_seq.size(); i++) {
			if (obj.aligned_seq[i] == '-' or obj.aligned_ref[i] == '-') {
				os << ' ';
			} else if (obj.aligned_seq[i] == obj.aligned_ref[i]) {
				os << '|';
			} else {
				os << '*';
			}
		}
		os << '\n';

		// show first sequence
		print("Seq2:", obj.aligned_seq, obj.pos_seq);

		return os;
	}

// ##member
	pair<int, int> pos_ref;
	pair<int, int> pos_seq;
	string aligned_seq;
	string aligned_ref;
};

// #Banded Smith Waterman
auto banded(string_view ref, string_view seq, Penalty p = {3, -3, -2}) {
	int M = seq.size() + 1;
	int N = ref.size() + 1;
	auto dp = vector(M, vector(N, 0));

	// Smith Waterman
	for (int i = 1; i < M; i++) {
		for (int j = 1; j < N; j++) {
			int match = dp[i-1][j-1] + (seq[i-1] == ref[j-1] ? p.match : p.mismatch);
			int indel = max(dp[i][j-1], dp[i-1][j]) + p.gap;

			dp[i][j] = max({0, match, indel});
		}
	}
//	cout << dp;

	return Blast(dp, p, ref, seq);
}

// #Main function
int main() {
	auto ref = "TTTTTTTCCAATGCCACAAAACATCTGTCTCTAACTGGTGTGTGTGTGGGGGGG"s;
	auto seq = "AAAACCAGCCCAAAATCTGTTTTAATGGTGGATTTGTGTAAAAAAA"s;

//	auto ref = "TGTTACGG"s;
//	auto seq = "GGTTGACTA"s;

	cout << ref.size() << "   " << ref << endl;
	cout << seq.size() << "   " << seq << endl << endl;

	auto result = banded(ref, seq);
	cout << result;
}
