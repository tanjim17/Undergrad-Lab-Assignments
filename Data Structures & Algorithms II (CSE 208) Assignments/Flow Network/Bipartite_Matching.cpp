#include<bits/stdc++.h>
#define INFINITY 999999999
#define BLACK 1
#define WHITE 0
using namespace std;

class Graph {
    int numOfVertices;
    bool isDirected;
	vector<int> *adjList;
	int *parent;
	int *color;
	int **capacity;
	int **flow;

public:
	Graph(bool isDirected, int numOfVertices) {
        this->numOfVertices = numOfVertices;
        this->isDirected = isDirected;
        adjList = new vector<int>[numOfVertices];
        parent = new int[numOfVertices];
        color = new int[numOfVertices];
        for(int i = 0; i < numOfVertices; i++) parent[i] = -1;
        capacity = new int*[numOfVertices];
        flow = new int*[numOfVertices];
        for(int i = 0; i < numOfVertices; i++) {
            capacity[i] = new int[numOfVertices];
            flow[i] = new int[numOfVertices];
            for(int j = 0; j < numOfVertices; j++) capacity[i][j] = 0;
            for(int j = 0; j < numOfVertices; j++) flow[i][j] = 0;
        }
	}

	init

	void addEdge(int source, int destination) {
        if(source<0 || destination<0 || source>=numOfVertices || destination>=numOfVertices || isEdge(source, destination)) return;
        adjList[source].push_back(destination);
        if(!isDirected) adjList[destination].push_back(source);
	}

	void setCapacity(int source, int destination, int capacity) {
        if(source<0 || destination<0 || source>=numOfVertices || destination>=numOfVertices || isEdge(source, destination)) return;
        this->capacity[source][destination] = capacity;
	}

	void addNegativeEdges() {
	    if(isDirected) {
            for (int source = 0; source < numOfVertices; source++) {
                for (int j = 0; j < adjList[source].size(); j++) {
                    int destination = adjList[source][j];
                    if(capacity[destination][source] == 0)
                        adjList[destination].push_back(source);
                }
            }
	    }
	}

	void createFlowNetwork() {
        for (int source = 0; source < numOfVertices; source++) {
            if(color[source] == BLACK) {

            }
            for (int j = 0; j < adjList[source].size(); j++) {
                int destination = adjList[source][j];
                if(capacity[destination][source] == 0)
                    addEdge(destination, source);
            }
        }
	}

	bool isEdge(int source, int destination) {
        if (source<0 || destination<0 || source>=numOfVertices || destination>=numOfVertices) return false;
        for (int i = 0; i < adjList[source].size(); i++) {
            if (adjList[source][i] == destination) return true;
        }
        return false;
	}

    bool isBipartite() {
	    vector<bool> isVisited(numOfVertices, false);
        queue<int> q;
        q.push(0);
        color[0] = BLACK;
        while(!q.empty()) {
            int currentNode = q.front();
            q.pop();
            isVisited[currentNode] = true;
            for(int i = 0; i < adjList[currentNode].size(); i++) {
                int destination = adjList[currentNode][i];
                if(!isVisited[destination]) {
                    q.push(destination);
                    color[destination] = !color[currentNode];
                }
                else if(color[source] == color[destination]) return false;
                else continue;
            }
        }
        return true;
	}

	int bfs(int source, int sink) {
	    int pathCapacity[numOfVertices];
        for(int i = 0; i < numOfVertices; i++) parent[i] = -1;
        queue<int> q;
        q.push(source);
        parent[source] = INFINITY;
        pathCapacity[source] = INFINITY;
        while(!q.empty()) {
            int currentNode = q.front();
            q.pop();
            for(int i = 0; i < adjList[currentNode].size(); i++) {
                int destination = adjList[currentNode][i];
                if(parent[destination] == -1 && capacity[currentNode][destination] - flow[currentNode][destination] > 0) {
                    parent[destination] = currentNode;
                    pathCapacity[destination] = min(pathCapacity[currentNode], capacity[currentNode][destination] - flow[currentNode][destination]);
                    if(destination == sink) return pathCapacity[destination];
                    q.push(destination);
                }
            }
        }
        return 0;
	}

	int edmondsKarp(int source, int sink) {
        int maxFlow = 0;
        while(true) {
            int newFlow = bfs(source, sink);
            if (newFlow == 0) break;
            maxFlow += newFlow;
            int currentNode = sink;
            while(currentNode != source) {
                int parentNode = parent[currentNode];
                flow[parentNode][currentNode] += newFlow;
                flow[currentNode][parentNode] -= newFlow;
                currentNode = parentNode;
            }
        }
        return maxFlow;
    }

    void findMaxMatching() {
        if(!isBipartite()) {
            cout << "The graph is not bipartite.\n";
        }
        else {
            createFlowNetwork();
            edmondsKarp();
        }
    }

    void printFlows() {
        for (int source = 0; source < numOfVertices; source++) {
            for (int j = 0; j < adjList[source].size(); j++) {
                int destination = adjList[source][j];
                if(capacity[source][destination] > 0)
                    cout << source << " " << destination << " " << flow[source][destination] << "/" << capacity[source][destination] << "\n";
            }
        }
    }

    void printGraph() {
        for (int i = 0; i < numOfVertices; i++) {
            cout << i << " :";
            for (int j = 0; j < adjList[i].size(); j++) cout << " " << adjList[i][j];
            cout << "\n";
        }
    }

	~Graph() {
        delete [] adjList;
        delete [] parent;
        for(int i = 0; i < numOfVertices; i++) {
            delete[] capacity[i];
            delete[] flow[i];
        }
        delete [] capacity;
        delete [] flow;
        adjList = 0;
        parent = 0;
        capacity = 0 ;
        flow = 0 ;
    }

};

int main() {
    cout << "1. Flow Network/n2.Bipartite\n";
    int option;
    cin >> option;
    if(option == 1) {
        int numOfVertices, numOfEdges;
        int source, sink;

        cin >> numOfVertices >> numOfEdges;
        Graph graph(true, numOfVertices);
        int u, v, capacity;
        for(int i = 0; i < numOfEdges; i++){
            cin >> u >> v >> capacity;
            graph.addEdge(u, v);
            graph.setCapacity(u, v, capacity);
        }
        cin >> source >> sink;

        cout << graph.edmondsKarp(source, sink) << "\n";
        graph.printFlows();
    }
    else if(option == 2){
        int numOfVertices, numOfEdges;

        cin >> numOfVertices >> numOfEdges;
        Graph graph(false, numOfVertices);
        int u, v;
        for(int i = 0; i < numOfEdges; i++){
            cin >> u >> v;
            graph.addEdge(u, v);
        }

        graph.addNegativeEdges();
        cout << graph.edmondsKarp(source, sink) << "\n";
        graph.printFlows();
    }
    else return 1;

    return 0;
}
