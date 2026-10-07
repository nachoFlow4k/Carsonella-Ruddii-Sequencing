#include <bits/stdc++.h>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Debugging

#define dbg(x) { auto t = (x); cout << #x << " = " << t << endl; }
template <class T, class S>
ostream& operator<<(ostream& os, pair<T, S> p) {
    return os << p.first << ':' << p.second;
}
template <class T>
ostream& operator<<(ostream& os, vector<T> v) {
    for (auto x : v) os << x << ' ';
    return os;
}
template <class T, class S, class R>
ostream& operator<<(ostream& os, unordered_map<T, S, R> m) {
    for (auto [x, y] : m) os << x << "->" << y << ' ';
    return os;
}

////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Input

string genome;

void read_input_file() {
    ios::sync_with_stdio(0);
    cin.tie(0); cout.tie(0);
    freopen("CarsonellaRuddii.txt", "r", stdin);
    cin >> noskipws;
    istreambuf_iterator<char> it(cin), end;
    genome = string(it, end);
    genome.erase(remove(genome.begin(), genome.end(), '\n'), genome.end());
    genome.erase(remove(genome.begin(), genome.end(), '\r'), genome.end());
    cin >> skipws;
    
    dbg(genome.size())
    // dbg(genome)
}

////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////
// Grafos
    // Decision: trabajar con grafos representados en naturales, hacer el pasaje 
    // de strings a naturales, despues recuperar los strings una vez obtenido el 
    // camino euleriano 
    // Decision: indexar los vertices de 1 a n
    
int get_vertex(unordered_map<string, int>& m, const string& s) {
    auto it = m.find(s);
    if (it != m.end()) return it->second;
    int v = m.size() + 1;
    m[s] = v;
    return v;
}

struct PairHash {
    uint64_t operator()(const pair<string, string>& p) const {
        uint64_t h1 = hash<string>{}(p.first);
        uint64_t h2 = hash<string>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};
using PairMap = unordered_map<pair<string, string>, int, PairHash>;

int get_vertex(PairMap& m, const pair<string, string>& p) {
    auto it = m.find(p);
    if (it != m.end()) return it->second;
    int v = m.size() + 1;
    m[p] = v;
    return v;
}

const int MAXN = 1e6 + 1;
int n;
vector<int> adj[MAXN];
int in_degree[MAXN];
int out_degree[MAXN];
using Edge = pair<int, int>;

void clear() {
    for (int v = 1; v <= n; ++v) {
        adj[v].clear();
        in_degree[v] = 0;
        out_degree[v] = 0;
    }
    n = 0;
}
void load_graph(const vector<Edge>& edges) {
    clear();
    for (auto [u, v] : edges) {
        n = max(u, n);
        n = max(v, n);
        adj[u].push_back(v);
        out_degree[u]++;
        in_degree[v]++;
    }
}

// bool vis[MAXN];
// void dfs(int v) {
//     if (vis[v]) return;
//     vis[v] = true;
//     for (int u : adj[v]) dfs(u);
// }
bool vis[MAXN];
void dfs(int start_v) {
    vector<int> st;
    st.push_back(start_v);
    vis[start_v] = true;
    
    while (!st.empty()) {
        int v = st.back();
        st.pop_back();
        for (int u : adj[v]) {
            if (!vis[u]) {
                vis[u] = true;
                st.push_back(u);
            }
        }
    }
}
bool is_strongly_connected(int s) {
    for (int v = 1; v <= n; ++v) vis[v] = false;
    dfs(s);
    for (int v = 1; v <= n; ++v) if (not vis[v]) return false;
    return true;
}


vector<int> path;
// void hierholzer(int v) {
//     while (not adj[v].empty()) {
//         int u = adj[v].back();
//         adj[v].pop_back();
//         hierholzer(u);
//     }
//     path.push_back(v);
// }
void hierholzer(int start_v) {
    vector<int> st;
    st.push_back(start_v);
    
    while (!st.empty()) {
        int v = st.back();
        if (!adj[v].empty()) {
            int u = adj[v].back();
            adj[v].pop_back();
            st.push_back(u);
        } else {
            path.push_back(v);
            st.pop_back();
        }
    }
}

vector<int> eulerian_path(const vector<Edge>& edges) {
    load_graph(edges);
    // dbg(n)
    int s = 0, t = 0;
    for (int v = 1; v <= n; ++v) {
        // dbg(v)dbg(in_degree[v])dbg(out_degree[v])
        if (abs(in_degree[v] - out_degree[v]) > 1) return {};
        if (in_degree[v] + 1 == out_degree[v]) {
            if (s != 0) return {};
            s = v;
        }
        if (in_degree[v] == out_degree[v] + 1) {
            if (t != 0) return {};
            t = v;
        }
    }
    if (s == 0 and t != 0) return {};
    if (s != 0 and t == 0) return {};
    if (s == 0) s = t = 1;
    if (not is_strongly_connected(s)) return {};
    path.clear();
    hierholzer(s);
    reverse(path.begin(), path.end());
    return path;
}

vector<int> get_unique_path(vector<Edge>& edges) {
    sort(edges.begin(), edges.end());
    vector<int> path1 = eulerian_path(edges);
    reverse(edges.begin(), edges.end());
    vector<int> path2 = eulerian_path(edges);
    // dbg(path1)dbg(path2)
    if (path1 == path2) return path1;
    else return {};
}

////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////
// metodo con k-meros

vector<string> composition_k(const string& text, int k) {
    vector<string> ans;
    for (int i = 0; i + k <= text.size(); ++i) {
        ans.emplace_back(text.substr(i, k));
    }
    return ans;
}

pair<unordered_map<string, int>, vector<Edge>> debruijn_graph(const string& text, int k) {
    // Input: string text and integer k
    // Output: DeBruijn_k(text)
    unordered_map<string, int> string_to_vertex;
    vector<Edge> edges;
    for (auto&& s : composition_k(text, k)) {
        int u = get_vertex(string_to_vertex, s.substr(0, k - 1)),
            v = get_vertex(string_to_vertex, s.substr(1, k - 1));
        edges.push_back({u, v});
    }
    return { string_to_vertex, edges };
}

string path_to_string(const vector<int>& path, const unordered_map<string, int>& string_to_vertex, int k) {
    vector<string> vertex_to_string(n + 1);
    for (auto&& [s, v] : string_to_vertex) vertex_to_string[v] = s;
    vector<char> ans;
    for (int v : path) ans.push_back(vertex_to_string[v].front());
    return string(ans.begin(), ans.end()) + vertex_to_string[path.back()].substr(1, k - 1);
}

bool kmer_assembly(int k) {
    auto [stov, edges] = debruijn_graph(genome, k);
    auto path = get_unique_path(edges);
    if (path.empty()) return false;
    auto sequence = path_to_string(path, stov, k);
    if (sequence.empty()) return false;
    return sequence == genome;
}

////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
// metodo con kdmeros

vector<pair<string, string>> paired_compositions(const string& text, int k, int d) {
    vector<pair<string, string>> ans;
    for (int i = 0; i + 2*k + d - 1 < text.size(); ++i)
        ans.push_back({ text.substr(i, k), text.substr(i + k + d, k) });
    return ans;
}

pair<PairMap, vector<Edge>> debruijn_graph(const string& text, int k, int d) {
    PairMap pair_to_vertex;
    vector<Edge> edges;
    for (auto&& [s, t] : paired_compositions(text, k, d)) {
        int u = get_vertex(pair_to_vertex, { s.substr(0, k-1), t.substr(0, k-1) }),
            v = get_vertex(pair_to_vertex, { s.substr(1, k-1), t.substr(1, k-1) });
        edges.push_back({ u, v });
    }
    return { pair_to_vertex, edges };
}

string path_to_string(const vector<int>& path, const PairMap& pair_to_vertex, int k, int d) {
    vector<pair<string, string>> vertex_to_pair(n + 1);
    for (auto&& [p, v] : pair_to_vertex) vertex_to_pair[v] = p;
    string ans(path.size() + 2 * k + d - 2, '#');
    for (int i = 0; i < path.size(); ++i) {
        int v = path[i];
        const string& s = vertex_to_pair[v].first;
        ans[i] = s.front();
        if (i + 1 == path.size()) {
            for (int j = 1; j < s.size(); ++j)
                ans[i + j] = s[j];
        }
    }
    for (int i = (int)path.size() - 1, j = (int)ans.size() - 1; ans[j] == '#'; --i, --j) {
        int v = path[i];
        const string& s = vertex_to_pair[v].second;
        ans[j] = s.back();
        
        if (i == 0) {
            for (int k = (int)s.size() - 2; k >= 0; --k)
                ans[--j] = s[k];
        }
    }
    
    for (char x : ans) if (x == '#') return "";
    return ans;
}

bool kdmer_assembly(int k, int d) {
    auto [ptov, edges] = debruijn_graph(genome, k, d);
    auto path = get_unique_path(edges);
    if (path.empty()) return false;
    auto sequence = path_to_string(path, ptov, k, d);
    if (sequence.empty()) return false;
    return sequence == genome;
}

////////////////////////////////////////////////////////////////////////////////


map<pair<int, int>, bool> cache;
void load_cache() {
    freopen("cache.txt", "r", stdin);
    int k, d, ans;
    while (cin >> k >> d >> ans) cache[{ k, d }] = ans;
}
void save_cache() {
    ofstream out("cache.txt");
    for (auto [p, ans] : cache) {
        auto [k, d] = p;
        out << k << ' ' << d << ' ' << ans << '\n';
    }
}


int total = 0;
int succeed_cnt = 0;

void test(int k, int d) {
    cout << "k=" << k << " d=" << d << " kdmer assembly ";
    auto it = cache.find({k, d});
    bool success;
    if (it == cache.end()) {
        success = kdmer_assembly(k, d);
        cache[{ k, d }] = success;
    } else {
        success = it->second;
    }
    total++;
    succeed_cnt += success;
    cout << (success ? "succeed" : "failed") << "  \t" << succeed_cnt << '/' << total << endl;
    if (total % 20 == 0) save_cache();
}


int main()
{
    read_input_file();
    // dbg(genome)
    
    
    load_cache();
// para testear, modificar los valores de k y d en los for loops.  Se guardan los resultados en cache.txt para no tener que recalcularlos.
    for (int k = 14; k <= 14; k+=1)
    for (int d = 0; d <= 86000; d+=1000)
     {
        test(k, d);
     }
    save_cache();
    cout << succeed_cnt << '/' << total << endl;



}  

//k=10. No anduvo para ningun k que encontramos x ahora.  Busquemos con precision desde el tope 86942. No anduvo para ninguno
//k=12 No anduvo para ningun k que encontramos

//k=13 El tope de d,86939, no anda. K=13 es muy random cerca del tope,.  Parece que la mitad andan y la mitad no andan.
//k=13 
                                                  //De 0 hacia 86000 con saltos de a 1000, encontramos 5/87 que anduvieron
//el primer d que encontramos que anduvo es 59000.  De 59000 hacia 86000 con saltos de 500, encontramos 9/60 que anduvieron
                                                 // De 85400 hacia 86900 con saltos de 50, encontramos 10/30 que anduvieron 
                                                 // De 86639 hacia 86939 con saltos de 10, encontramos 12/30 que anduvieron
                                                 // De 86909 hacia 86939 con saltos de a 1, enocntramos 11/30 que anduvieron     

//k=14. tope 86938
//De 0 hacia 86000 con saltos de a 1000, encontramos 62/86 que anduvieron.
//De 0 hacia 100 con saltos de a 1, no encontramos ningun caso que anduvo 0/100
//De 0 hacia 1000 con saltos de a 50, encontramos 5/21 que anduvieron
//De 80000 hacia 86938 con saltos de a 50, encontramos 127/139 que anduvieron
//De 50000 hacia 50100 con saltos de a 1, encontramos 3/100 que anduvieron

//k=15 tope 86937
//De 0 hacia 86000, ++1000.  Anduvieron (82/87)
//De 0 hacia 100, saltos de a 1 (58/100)
//De 0 hacia 1000, con saltos de 10 (80/101)
//De 20000 hacia 25000 con saltos de 101.  Anduvieron (48/50)
//De 80000 hacia 86000, con saltos de 50  Anduvieron (120/121)

//k=16 minimo d encontrado es 47000 (buscando con intervalos de 1-- y de 500, todavia no entendemos que determina si anda o no)
//k=17 tope anda
//k=20 minimo d encontrado. Anda para todos los d que buscamos.(intervalos de a 500(47k-80k) y de a 2(0-100))

//k = 






//2k+2d<=173904
//con k=16, el max d que admite es 86936


//k=15 d=7 primer instancia minima encontrada(hasta ahora) donde empieza a andar

//k=16 d=5 empieza a andar bien para el resto de los k=16???

//k=17 d=3 empieza a andar bien para el resto de los k=17???

//k=18 d=1 empieza a andar bien para el resto de los k=18?

//k=19 andan todos desde 0 hasta ...


