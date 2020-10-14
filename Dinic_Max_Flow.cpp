const int maxc = 1000000007;
const int MM = 10000005;
const int N = 200005;

struct edge
{
    int u, v, c, f;
    edge(int u=0, int v=0, int c=0, int f=0) : u(u), v(v), c(c), f(f) {}
}e[MM];

vector<int> a[N];
int cnt = 0;
void add_edge(int u, int v, int w)
{
    a[u].push_back(cnt);
    e[cnt++] = edge(u, v, w, 0);
    a[v].push_back(cnt);
    e[cnt++] = edge(v, u, 0, 0);
}

int dd[N], pos[N], d[N], cur_time;

bool BFS()
{
    dd[s] = ++cur_time;
    queue<int> q;
    q.push(s);
    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        pos[u] = 0;
        for (auto id : a[u])
        {
            int v = e[id].v;
            if (dd[v] == cur_time || e[id].f == e[id].c) continue;
            dd[v] = cur_time;
            q.push(v);
            d[v] = d[u] + 1;
        }
    }
    return dd[t] == cur_time;
}

int DFS(int u, int delta)
{
    if (u == t || delta == 0) return delta;
    for (; pos[u] < a[u].size(); pos[u]++)
    {
        int id = a[u][pos[u]];
        int v = e[id].v;
        if (d[v] != d[u] + 1 || e[id].f == e[id].c)
            continue;
        if (int x = DFS(v, min(delta, e[id].c - e[id].f)))
        {
            e[id].f += x;
            e[id ^ 1].f -= x;
            return x;
        }
    }
    return 0;
}

void buildGraph() {
	
}
void solve()
{
    buildGraph();
    int max_flow = 0;
    while (BFS())
        while (int x = DFS(s, maxc)) max_flow += x;
    return 0;
}
