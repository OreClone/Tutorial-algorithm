#include<bits/stdc++.h>
using namespace std;

void Read(int &val) {
	val = 0; char c;
	do { c = getchar(); } while (!isdigit(c));
	while (isdigit(c)) { val = val * 10 + c - '0'; c = getchar(); }
}

const int N = 2e5 + 4;
int n, m, a[N], Real[N][2];

struct Interval_Tree {
	int node[4*N], lazy[4*N];
	Interval_Tree() { 
		memset(node, 0, sizeof(node)); 
		memset(lazy, 0, sizeof(lazy)); 
	}

	void Down(int i) {
		node[2*i] += lazy[i]; node[2*i+1] += lazy[i];
		lazy[2*i] += lazy[i]; lazy[2*i+1] += lazy[i];
		lazy[i] = 0; 
	}

	void update(int i, int l, int r, int x, int y, int val) {
		if (l > y || x > r) return;
		if (x <= l && r <= y) {
			node[i] += val; lazy[i] += val;
			return;
		}
		int mid = (l + r) / 2;
		Down(i);
		update(2*i, l, mid, x, y, val);
		update(2*i+1, mid+1, r, x, y, val);
		node[i] = min(node[2*i], node[2*i+1]);
	}
} IT;

void sol() {	
	int num = 0;
	for (int i = 1; i <= (int) 1e5; ++i) {
		Real[i][0] = ++num;
		Real[i][1] = ++num;
	}

	int res = -1;
	for (int i = 1; i <= n/2; ++i) {
		IT.update(1, 1, N-1, Real[a[i]][0], N-1, 1);
		IT.update(1, 1, N-1, Real[a[i]][1], N-1, 1);

		IT.update(1, 1, N-1, Real[a[2*i]][0],   N-1, -1);
		IT.update(1, 1, N-1, Real[a[2*i-1]][0], N-1, -1);

		if (IT.node[1] == 0) res = i; 
	}
	cout << res << '\n';
}

int main() {
	if (fopen("input.txt", "r")) {
		freopen("input.txt", "r", stdin);
	}

	Read(n); Read(m);
	for (int i = 1; i <= n; ++i) Read(a[i]);

	sol();

	return 0;
}
