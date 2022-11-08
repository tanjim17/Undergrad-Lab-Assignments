#include<bits/stdc++.h>

#define NULL_VALUE -999999
#define INFINITY 999999

using namespace std;

class Edge {
    int u, v;
    double w;

public:
    Edge(int u, int v, double w) {
        this->u = u;
        this->v = v;
        this->w = w;
    }

    void setU(int u) { this->u = u; }
    void setV(int v) { this->v = v; }
    void setW(double w) { this->w = w; }

    int getU() { return u; }
    int getV() { return v; }
    double getW() { return w; }
};

class Graph {
    int nVertices;
	vector<Edge>* adjList;
	double** distanceMatrix;
	int** parentMatrix;

public:
	Graph() {
	    nVertices = 0;
	    adjList = 0;
	    distanceMatrix = 0;
	    parentMatrix = 0;
	}

	void setnVertices(int n) {
	    if (adjList != 0) {
            delete[] adjList;
	    }
        if (distanceMatrix != 0) {
            for (int i = 0; i < nVertices; i++) {
                delete[] distanceMatrix[i];
            }
            delete[] distanceMatrix;
        }
	    if (parentMatrix != 0) {
            for (int i = 0; i < nVertices; i++) {
                delete[] parentMatrix[i];
            }
            delete[] parentMatrix;
        }

        this->nVertices = n;
        adjList = new vector<Edge>[nVertices];
        distanceMatrix = new double*[nVertices];
        parentMatrix = new int*[nVertices];
        for (int i = 0; i < nVertices; i++) {
            distanceMatrix[i] = new double[nVertices];
            parentMatrix[i] = new int[nVertices];
        }

        for (int i = 0; i < nVertices; i++) {
            for (int j = 0; j < nVertices; j++) {
                distanceMatrix[i][j] = INFINITY;
                parentMatrix[i][j] = -1;
            }
        }
    }

	bool addEdge(int u, int v, double w) {
        if (u<0 || v<0 || u>=nVertices || v>=nVertices || w<-10000 || w>10000 || isEdge(u, v)) {
            return false;
        }
        Edge edge(u, v, w);
        adjList[u].push_back(edge);
        return true;
    }

    void printGraph() {
        for (int i = 1; i < nVertices; i++) {
            cout << i << " :";
            for (int j = 0; j < adjList[i].size(); j++) {
                cout << " " << adjList[i][j].getV() << "(" << adjList[i][j].getW() << ") -->";
            }
            cout << "\b\b\b\b    \n";
        }
    }

	void removeEdge(int u, int v) {
        if (u<0 || v<0 || u>=nVertices || v>=nVertices || isEdge(u, v)) {
            return;
        }
        int i;
        for (i = 0; i < adjList[u].size(); i++) {
            if (adjList[u][i].getV() == v) {
                break;
            }
        }
        if (i < adjList[u].size()) {
            adjList[u].erase(adjList[u].begin() + i);
        }
    }

    Edge* searchEdge(int u, int v) {
        if (u<0 || v<0 || u>=nVertices || v>=nVertices) {
            return NULL;
        }
        for (int i = 0; i < adjList[u].size(); i++) {
            if (adjList[u][i].getV() == v) {
                return &adjList[u][i];
            }
        }
        return NULL;
    }

    void reweightEdge(int u, int v, double w) {
        if (u<0 || v<0 || u>=nVertices || v>=nVertices || w<-10000 || w>10000) {
            return;
        }
        Edge* edge = searchEdge(u, v);
        if (edge == NULL) {
            addEdge(u, v, w);
        }
        else {
            edge->setW(w);
        }
    }

    bool isEdge(int u, int v) {
        if (u<0 || v<0 || u>=nVertices || v>=nVertices) {
            return false;
        }
        if (searchEdge(u, v) != NULL) {
            return true;
        }
        return false;
	}

	double getWeight(int u, int v) {
        if (u<0 || v<0 || u>=nVertices || v>=nVertices) {
            return INFINITY;
        }
        Edge* edge = searchEdge(u, v);
        if (edge == NULL) {
            return INFINITY;
        }
        else {
            return edge->getW();
        }
	}

	int getMinDistVertex(int source, bool isFinalized[]) {
        int min = INFINITY, min_index;

        for (int i = 0; i < nVertices; i++){
            if (!isFinalized[i] && distanceMatrix[source][i] < min){
                min = distanceMatrix[source][i];
                min_index = i;
            }
        }
        return min_index;
    }

	void floydWarshall() {
        Edge *edge;
        for (int i = 0; i < nVertices; i++) {
            for (int j = 0; j < nVertices; j++) {
                if (i == j) {
                    distanceMatrix[i][j] = 0;
                }
                else {
                    edge = searchEdge(i, j);
                    if (edge != NULL) {
                        distanceMatrix[i][j] = edge->getW();
                        parentMatrix[i][j] = i;
                    }
                }
            }
        }

        for (int k = 0; k < nVertices; k++) {
            for (int i = 0; i < nVertices; i++) {
                for (int j = 0; j < nVertices; j++) {
                    if (distanceMatrix[i][k] != INFINITY && distanceMatrix[k][j] != INFINITY
                        && distanceMatrix[i][k] + distanceMatrix[k][j] < distanceMatrix[i][j]) {
                            distanceMatrix[i][j] = distanceMatrix[i][k] + distanceMatrix[k][j];
                            parentMatrix[i][j] = parentMatrix[k][j];
                    }
                }
            }
        }
	}

	bool bellmanford(int source) {
	    if(source < 0 || source >= nVertices) {
            return false;
	    }

        distanceMatrix[source][source] = 0;

        int v;
        double w;
        for (int i = 0; i < nVertices - 1; i++) {
            for (int u = 0; u < nVertices; u++) {
                for (int j = 0; j < adjList[u].size(); j++) {
                    //relaxation
                    v = adjList[u][j].getV();
                    w = adjList[u][j].getW();
                    if (distanceMatrix[source][u] != INFINITY && distanceMatrix[source][u] + w < distanceMatrix[source][v]) {
                        distanceMatrix[source][v] = distanceMatrix[source][u] + w;
                    }
                }
            }
        }

        //check negative cycle
        for (int u = 0; u < nVertices; u++) {
            for (int j = 0; j < adjList[u].size(); j++) {
                //relaxation
                v = adjList[u][j].getV();
                w = adjList[u][j].getW();
                if (distanceMatrix[source][u] != INFINITY && distanceMatrix[source][u] + w < distanceMatrix[source][v]) {
                    return true;
                }
            }
        }
        return false;
    }

    void dijkstra(int source) {
        if (source < 0 || source >= nVertices) {
            return;
        }

        bool isFinalized[nVertices]; //array for checking if relaxation of every edge from a vertex is done

        //initialization
        for (int i = 0; i < nVertices; i++) {
            isFinalized[i] = false;
        }

        distanceMatrix[source][source] = 0;

        for (int i = 0; i < nVertices - 1; i++) {
            int u = getMinDistVertex(source, isFinalized);

            //if remaining edges are reachable i.e. if graph is connected
            if (distanceMatrix[source][u] == INFINITY) {
                break;
            }

            isFinalized[u] = true;

            int v;
            double w;
            for (int j = 0; j < adjList[u].size(); j++) {
                //relaxation
                v = adjList[u][j].getV();
                w = adjList[u][j].getW();
                if (!isFinalized[v] && distanceMatrix[source][u] + w < distanceMatrix[source][v]) {
                    parentMatrix[source][v] = u;
                    distanceMatrix[source][v] = distanceMatrix[source][u] + w;
                }
            }
        }
    }

    void johnson() {
        //add 0 weighted edge to every other vertex from vertex 0
        for (int v = 1; v < nVertices; v++) {
            addEdge(0, v, 0);
        }

        //check for negative cycle
        if (bellmanford(0)) {
            cout << "There is a negative-weight cycle.\n";
            return;
        }

        //re-weight this graph
        int v;
        double w;
        for (int u = 0; u < nVertices; u++) {
            for (int j = 0; j < adjList[u].size(); j++) {
                v = adjList[u][j].getV();
                w = adjList[u][j].getW();
                reweightEdge(u, v, w + distanceMatrix[0][u] - distanceMatrix[0][v]);
            }
        }

        //run Dijkstra for every vertex
        for (int u = 1; u < nVertices; u++) {
            dijkstra(u);
            for (int v = 1; v < nVertices; v++) {
                distanceMatrix[u][v] = distanceMatrix[u][v] - distanceMatrix[0][u] + distanceMatrix[0][v];
            }
        }

        //re-weight graph to get back to original form
        for (int u = 0; u < nVertices; u++) {
            for (int j = 0; j < adjList[u].size(); j++) {
                v = adjList[u][j].getV();
                w = adjList[u][j].getW();
                reweightEdge(u, v, w - distanceMatrix[0][u] + distanceMatrix[0][v]);
            }
        }

        //remove edges from vertex 0
        for (int v = 1; v < nVertices; v++) {
            removeEdge(0, v);
            distanceMatrix[0][v] = INFINITY;
        }
    }

    double getShortestPathWeight(int u ,int v) {
        if (u<0 || v<0 || u>=nVertices || v>=nVertices) {
            return INFINITY;
        }
        return distanceMatrix[u][v];
    }

    void printShortestPath(int u, int v) {
        if (parentMatrix[u][v] == u) {
            cout << u;
            cout << " --> " << v << "(" << getWeight(u, v) << ")";
            return;
        }
        else {
            printShortestPath(u, parentMatrix[u][v]);
            cout << " --> " << v << "(" << getWeight(parentMatrix[u][v], v) << ")";
        }
    }

    void printDistanceMatrix() {
        for (int i = 1; i < nVertices; i++) {
            for (int j = 1; j < nVertices; j++) {
                if (distanceMatrix[i][j] != INFINITY) {
                    cout << distanceMatrix[i][j] << " ";
                }
                else {
                    cout << "INF ";
                }
            }
            cout << endl;
        }
    }

    void printParentMatrix() {
        for (int i = 1; i < nVertices; i++) {
            for (int j = 1; j < nVertices; j++) {
                if (parentMatrix[i][j] != -1) {
                    cout << parentMatrix[i][j] << " ";
                }
                else {
                    cout << "NIL ";
                }
            }
            cout << endl;
        }
    }

    void cleanSPInfo() {
        for (int i = 0; i < nVertices; i++) {
            for (int j = 0; j < nVertices; j++) {
                distanceMatrix[i][j] = INFINITY;
                parentMatrix[i][j] = -1;
            }
        }
    }

    ~Graph() {
        if (adjList != 0) {
            delete[] adjList;
        }
        if (distanceMatrix != 0) {
            for (int i = 0; i < nVertices; i++) {
                delete[] distanceMatrix[i];
            }
            delete[] distanceMatrix;
        }
	    if (parentMatrix != 0) {
            for (int i = 0; i < nVertices; i++) {
                delete[] parentMatrix[i];
            }
            delete[] parentMatrix;
        }
        nVertices = 0;
        adjList = 0;
        distanceMatrix = 0;
        parentMatrix = 0;
    }
};

int main() {
    int n, m, u, v, c;
    double w;
    cin >> n >> m;
    Graph g;
    g.setnVertices(n + 1); //vertex number starts from 1
    for (int i = 0; i < m; i++) {
        cin >> u >> v >> w;
        g.addEdge(u, v, w);
    }
    cout << "Graph Created.\n";

    while (true) {
        cout << "1. Clear APSP matrices 2. Floyd-Warshall 3. Johnson 4. Print shortest path\n";
        cout << "5. Print graph 6. Print distance matrix 7. Print parent matrix\n";
        cin >> c;
        if (c == 1) {
            g.cleanSPInfo();
            cout << "APSP matrices cleared.\n";
        }
        else if (c == 2) {
            g.floydWarshall();
            cout << "Floyd-Warshall algorithm implemented.\n";
        }
        else if (c == 3) {
            g.johnson();
            cout << "Johnson algorithm implemented.\n";
        }
        else if (c == 4) {
            cout << "Give vertices:\n";
            cin >> u >> v;
            cout << "Shortest Path Weight: " << g.getShortestPathWeight(u, v) << endl;
            cout << "Path: ";
            g.printShortestPath(u, v);
            cout << endl;
        }
        else if (c == 5) {
            g.printGraph();
        }
        else if (c == 6) {
            g.printDistanceMatrix();
        }
        else if (c == 7) {
            g.printParentMatrix();
        }
        else {
            break;
        }
    }
}

