#include<bits/stdc++.h>

#define NULL_VALUE -999999
#define INFINITY 999999

using namespace std;

class CompareBySecond {
public:
    bool operator()(pair<int,int> a, pair<int,int> b) {
        return a.second > b.second;
    }
};

class Disjoint_Sets {
    int size;
    int *parent;
    int *rank;

public:
    Disjoint_Sets(int n) {
        size = n;
        parent = new int[n];
        rank = new int[n];
        for (int i = 0; i < n; i++) {
            parent[i] = -1;
            rank[i] = -1;
        }
    }

    void make_set(int x) {
        if (parent[x] == -1) {
            parent[x] = x;
            rank[x] = 0;
        }
    }

    int find_set(int x) {
        if (parent[x] != x && parent[x] != -1) {
            parent[x] = find_set(parent[x]);
        }
        return parent[x];
    }

    void union_(int x, int y) {
        if (parent[x] == -1) {
            make_set(x);
        }
        if (parent[y] == -1) {
            make_set(y);
        }

        if (x != y) {
            if (rank[x] < rank[y]) {
                parent[x] = y;
            }
            else if (rank[x] > rank[y]) {
                parent[y] = x;
            }
            else {
                parent[y] = x;
                rank[x] ++;
            }
        }
    }

    ~Disjoint_Sets() {
        delete[] parent;
        delete[] rank;
        size = 0;
        parent = 0;
        rank = 0;
    }
};

class Edge {
    int u, v, w;

public:
    Edge(int u, int v, int w) {
        this->u = u;
        this->v = v;
        this->w = w;
    }

    void setU(int u) { this->u = u; }
    void setV(int v) { this->v = v; }
    void setW(int w) { this->w = w; }

    int getU() { return u; }
    int getV() { return v; }
    int getW() { return w; }

    bool operator < (Edge edge2) {
        return w < edge2.getW();
    }
};

class Graph {
    int nVertices;
	vector<pair<int, int>> *adjList;

public:
	Graph() {
	    nVertices = 0;
	    adjList = 0;
	}

	void setnVertices(int n) {
        this->nVertices = n;
        adjList = new vector<pair<int, int>>[nVertices];
    }

	bool addEdge(int u, int v, int w) {
        if (u<0 || v<0 || u>=nVertices || v>=nVertices || isEdge(u, v)) {
            return false;
        }
        adjList[u].push_back(make_pair(v, w));
        adjList[v].push_back(make_pair(u, w));
        return true;
    }

    void printGraph() {
        for (int i = 1; i < nVertices; i++) {
            cout << i << " :";
            for (int j = 0; j < adjList[i].size(); j++) {
                cout << " " << adjList[i][j].first << "(" << adjList[i][j].second << ") -->";
            }
            cout << "\b\b\b\b    \n";
        }
    }

	void removeEdge(int u, int v) {
        if (u<0 || v<0 || u>=nVertices || v>=nVertices) {
            return;
        }
        for (int j = 0; j < adjList[u].size(); j++) {
            if (adjList[u][j].first == v) {
                adjList[u].erase(adjList[u].begin() + j);

                //also remove u from adj list of v, since graph is undirected
                for (int k = 0; k < adjList[v].size(); k++) {
                    if (adjList[v][k].first == u) {
                        adjList[v].erase(adjList[v].begin() + k);
                        break;
                    }
                }
                break;
            }
        }
    }

    bool isEdge(int u, int v) {
        if (u<0 || v<0 || u>=nVertices || v>=nVertices) {
            return false;
        }
        for (int j = 0; j < adjList[u].size(); j++) {
            if (adjList[u][j].first == v) {
                return true;
            }
        }
        return false;
	}

	void prim(ofstream &fout) {
        int source = rand() % nVertices;

        int dist[nVertices], parent[nVertices];
        bool inMST[nVertices];
        priority_queue<pair<int, int>, vector <pair<int, int>>, CompareBySecond> pq;
        for (int i = 0; i < nVertices; i++) {
            dist[i] = INFINITY;
            parent[i] = -1;
            inMST[i] = false;
        }

        dist[source] = 0;
        pq.push(make_pair(source, dist[source]));

        while (!pq.empty()) {
            int u = pq.top().first;
            pq.pop();
            inMST[u] = true;

            for (int j = 0; j < adjList[u].size(); j++) {
                int v = adjList[u][j].first;
                int w = adjList[u][j].second;
                if (inMST[v] == false && w < dist[v]) {
                    dist[v] = w;
                    parent[v] = u;
                    pq.push(make_pair(v, dist[v]));
                }
            }
        }

        /*int total_weight = 0;
        for (int i = 0; i < nVertices; i++) {
            total_weight += dist[i];
        }
        fout << total_weight << endl;*/
        fout << "Prim's Algorithm:\n" << "Root node = " << source << endl;
        for (int u = 0; u < nVertices; u++) {
            if (parent[u] != -1) {
                fout << u << " " << parent[u] << endl;
            }
        }
    }

    void kruskal(ofstream &fout) {
        vector<Edge> edges;
        for (int u = 0; u < nVertices; u++) {
            for (int j = 0; j < adjList[u].size(); j++) {
                int v = adjList[u][j].first;
                int w = adjList[u][j].second;
                Edge edge(u, v, w);
                edges.push_back(edge);
            }
        }
        sort(edges.begin(), edges.end());

        Disjoint_Sets ds(nVertices);
        for (int u = 0; u < nVertices; u++) {
            ds.make_set(u);
        }

        vector<pair<int, int>> mst;
        int total_weight = 0;
        for (int i = 0; i < edges.size(); i++) {
            int u = edges[i].getU();
            int v = edges[i].getV();
            int u_parent = ds.find_set(u);
            int v_parent = ds.find_set(v);

            if (u_parent != v_parent) {
                ds.union_(u_parent, v_parent);
                mst.push_back(make_pair(u, v));
                total_weight += edges[i].getW();
            }
        }

        fout << total_weight << endl << "Kruskal's Algorithm:\n";
        for (int i = 0; i < mst.size(); i++) {
            fout << mst[i].first << " " << mst[i].second << endl;
        }
    }

     ~Graph() {
        delete[] adjList;
        nVertices = 0;
        adjList = 0;
    }
};

int main() {
    int n, m;
    ifstream fin;
    ofstream fout;

    fin.open("input.txt");
    fout.open("output.txt");

    fin >> n >> m;
    Graph g;
    g.setnVertices(n);
    for (int i = 0; i < m; i++) {
        int u, v, w;
        fin >> u >> v >> w;
        g.addEdge(u, v, w);
    }

    g.kruskal(fout);
    g.prim(fout);

    fin.close();
    fout.close();

    return 0;
}
