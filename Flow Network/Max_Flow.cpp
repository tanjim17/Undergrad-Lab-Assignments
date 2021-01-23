#include<bits/stdc++.h>
#define INFINITY 999999999
#define BLACK 1
#define WHITE 0
using namespace std;

ifstream fin;
ofstream fout;

class Graph {
    int numOfVertices;
    bool isDirected;
	vector<int> *adjList;
	int *color;
	int *parent;
	int **capacity;
	int **flow;

	void addReverseEdges() {
        for (int source = 0; source < numOfVertices; source++) {
            for (int j = 0; j < adjList[source].size(); j++) {
                int destination = adjList[source][j];
                if(capacity[destination][source] == 0)
                    adjList[destination].push_back(source);
            }
        }
	}

	int findAugmentedFlow(int source, int sink) {
	    for(int i = 0; i < numOfVertices; i++) parent[i] = -1;
	    int pathCapacity[numOfVertices];
        queue<int> q;
        q.push(source);
        parent[source] = INFINITY;
        pathCapacity[source] = INFINITY;
        while(!q.empty()) {
            int currentNode = q.front();
            q.pop();
            for(int i = 0; i < adjList[currentNode].size(); i++) {
                int destination = adjList[currentNode][i];
                int residualCapacity = capacity[currentNode][destination] - flow[currentNode][destination];
                if(parent[destination] == -1 && residualCapacity > 0) {
                    parent[destination] = currentNode;
                    pathCapacity[destination] = min(pathCapacity[currentNode], residualCapacity);
                    if(destination == sink) return pathCapacity[destination];
                    q.push(destination);
                }
            }
        }
        return 0;
	}

    Graph createFlowNetwork(int SOURCE, int SINK) {
	    Graph flowNet(true, numOfVertices + 2); //add source and sink
	    int CAPACITY = 1;
        for (int vertex = 0; vertex < numOfVertices; vertex++) {
            if(color[vertex] == BLACK) { //black nodes will be connected with source
                flowNet.addEdge(SOURCE, vertex, CAPACITY);
                for (int j = 0; j < adjList[vertex].size(); j++) {
                    int destination = adjList[vertex][j];
                    flowNet.addEdge(vertex, destination, CAPACITY);
                }
            }
            else {
                flowNet.addEdge(vertex, SINK, CAPACITY);
            }
        }
        return flowNet;
	}

    void printMatchings(int SOURCE, int SINK) {
        for (int source = 0; source < numOfVertices; source++) {
            for (int j = 0; j < adjList[source].size(); j++) {
                int destination = adjList[source][j];
                if(flow[source][destination] > 0 && source != SOURCE && destination != SINK)
                    //cout << source << " " << destination << "\n";
                    fout << source << " " << destination << "\n";
            }
        }
    }

public:
	Graph(bool isDirected, int numOfVertices) {
        this->numOfVertices = numOfVertices;
        this->isDirected = isDirected;
        adjList = new vector<int>[numOfVertices];
        if(isDirected) { //for flow network calculation
            parent = new int[numOfVertices];
            capacity = new int*[numOfVertices];
            flow = new int*[numOfVertices];
            for(int i = 0; i < numOfVertices; i++) {
                capacity[i] = new int[numOfVertices];
                flow[i] = new int[numOfVertices];
                for(int j = 0; j < numOfVertices; j++) capacity[i][j] = 0;
                for(int j = 0; j < numOfVertices; j++) flow[i][j] = 0;
            }
        }
        else { //for bipartite matching
            color = new int[numOfVertices];
        }
	}

	bool isEdge(int source, int destination) {
        if (source<0 || destination<0 || source>=numOfVertices || destination>=numOfVertices) return false;
        for (int i = 0; i < adjList[source].size(); i++) {
            if (adjList[source][i] == destination) return true;
        }
        return false;
	}

	void addEdge(int source, int destination) {
        if(source<0 || destination<0 || source>=numOfVertices || destination>=numOfVertices || isEdge(source, destination)) return;
        adjList[source].push_back(destination);
        if(!isDirected && source!=destination) adjList[destination].push_back(source);
	}

	void addEdge(int source, int destination, int capacity) {
        if(!isDirected || source<0 || destination<0 || source>=numOfVertices || destination>=numOfVertices || isEdge(source, destination)) return;
        adjList[source].push_back(destination);
        this->capacity[source][destination] = capacity;
	}

    bool isBipartite() {
	    vector<bool> isVisited(numOfVertices, false);
        queue<int> q;
        for(int vertex = 0; vertex < numOfVertices; vertex++) {
            if(!isVisited[vertex]) {
                q.push(vertex);
                color[vertex] = BLACK;
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
                        else if(color[currentNode] == color[destination]) return false;
                        else continue;
                    }
                }
            }
        }
        return true;
	}

	int edmondsKarp(int source, int sink) {
	    addReverseEdges();
        int maxFlow = 0;
        while(true) {
            int augmentedFlow = findAugmentedFlow(source, sink);
            if (augmentedFlow == 0) break;
            maxFlow += augmentedFlow;
            int currentNode = sink;
            while(currentNode != source) {
                int parentNode = parent[currentNode];
                flow[parentNode][currentNode] += augmentedFlow;
                flow[currentNode][parentNode] -= augmentedFlow;
                currentNode = parentNode;
            }
        }
        return maxFlow;
    }

    void findMaxBipartiteMatchings() {
        if(!isBipartite()) {
            //cout << "The graph is not bipartite.\n";
            fout << "The graph is not bipartite.\n";
        }
        else {
            int SOURCE = numOfVertices;
            int SINK = numOfVertices + 1;
            Graph flowNet = createFlowNetwork(SOURCE, SINK);
            //cout << flowNet.edmondsKarp(SOURCE, SINK) << "\n";
            fout << flowNet.edmondsKarp(SOURCE, SINK) << "\n";
            flowNet.printMatchings(SOURCE, SINK);
        }
    }

    void printGraph() {
        for (int i = 0; i < numOfVertices; i++) {
            cout << i << " :";
            for (int j = 0; j < adjList[i].size(); j++) cout << " " << adjList[i][j];
            cout << "\n";
        }
    }

    void printFlows() {
        for (int source = 0; source < numOfVertices; source++) {
            for (int j = 0; j < adjList[source].size(); j++) {
                int destination = adjList[source][j];
                if(capacity[source][destination] > 0) {
                    if(flow[source][destination] < 0) {
                        //cout << source << " " << destination << " " << 0 << "/" << capacity[source][destination] << "\n";
                        fout << source << " " << destination << " " << 0 << "/" << capacity[source][destination] << "\n";
                    }
                    else {
                        //cout << source << " " << destination << " " << flow[source][destination] << "/" << capacity[source][destination] << "\n";
                        fout << source << " " << destination << " " << flow[source][destination] << "/" << capacity[source][destination] << "\n";
                    }
                }
            }
        }
    }

	~Graph() {
        delete [] adjList;
        delete [] parent;
        if(!capacity) {
            for(int i = 0; i < numOfVertices; i++) delete[] capacity[i];
            delete [] capacity;
        }
        if(!flow) {
            for(int i = 0; i < numOfVertices; i++) delete[] flow[i];
            delete [] flow;
        }
        delete [] color;
        adjList = 0;
        parent = 0;
        capacity = 0 ;
        flow = 0 ;
        color = 0 ;
    }

};

int main() {
    cout << "1.Flow Network\n2.Bipartite\n";
    int option;
    cin >> option;
    if(option == 1) {
        fin.open("input.txt");
        fout.open("output.txt");
        int numOfVertices, numOfEdges;
        int source, sink;
        //cin >> numOfVertices >> numOfEdges;
        fin >> numOfVertices >> numOfEdges;
        Graph graph(true, numOfVertices);
        int u, v, capacity;
        for(int i = 0; i < numOfEdges; i++){
            //cin >> u >> v >> capacity;
            fin >> u >> v >> capacity;
            graph.addEdge(u, v, capacity);
        }
        //cin >> source >> sink;
        fin >> source >> sink;

        //cout << graph.edmondsKarp(source, sink) << "\n";
        fout << graph.edmondsKarp(source, sink) << "\n";
        graph.printFlows();
    }
    else if(option == 2){
        fin.open("input.txt");
        fout.open("output.txt");
        int numOfVertices, numOfEdges;
        //cin >> numOfVertices >> numOfEdges;
        fin >> numOfVertices >> numOfEdges;
        Graph graph(false, numOfVertices);
        int u, v;
        for(int i = 0; i < numOfEdges; i++){
            //cin >> u >> v;
            fin >> u >> v;
            graph.addEdge(u, v);
        }

        graph.findMaxBipartiteMatchings();
    }
    else return 1;

    fin.close();
    fout.close();

    return 0;
}
