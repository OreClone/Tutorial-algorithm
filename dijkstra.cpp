#include<bits/stdc++.h>
#define fr(i,a,b)		for(int i=a;i<=b;i++)
#define frd(i,a,b)		for(int i=a;i>=b;i--)
#define cl(a)			memset(a,0,sizeof(a))
#define oo				163835000
#define endl			'\n'
#define ii				pair<int,int>
#define X				first
#define Y				second
#define PB				push_back

using namespace std;

const int N=1e5+2;

int n,m,st,fi;
vector<ii> a[N];
priority_queue<ii,vector<ii>,greater<ii> > Q;
int d[N];
vector<int> b;
int tr[N];

void init() {
	fr(i,1,n) d[i]=oo;
	d[st]=0;
	Q.push(ii(0,st));
	cl(tr);
}

void dij() {
	while (!Q.empty()) {
		int u=Q.top().Y;
		int du=Q.top().X;
		Q.pop();
		if (d[u]!=du) continue;
		if (!a[u].empty())
			fr(i,0,a[u].size()-1) {
				int v=a[u][i].Y;
				int uv=a[u][i].X;
				if (d[v]>du+uv) {
					d[v]=du+uv;
					Q.push(ii(d[v],v));
					tr[v]=u;
				}
			}
	}
}

int main() {
	ios::sync_with_stdio(0);
	cin.tie(0);
	freopen("nhap.inp","r",stdin);
	cin>>n>>m;
	st = 1; fi = n;
	int u,v,w;
	fr(i,1,m) {
		cin>>u>>v>>w;
		a[u].PB(ii(w,v));
		a[v].PB(ii(w,u));
	}
	init();
	dij();
	//cout<<d[fi]<<'\n';
	if (d[fi] == oo) { cout << -1; return 0 ; }
	while (fi!=0) {
		b.PB(fi);
		fi=tr[fi];
	}
	frd(i,b.size()-1,0) cout<<b[i]<<" ";
}
