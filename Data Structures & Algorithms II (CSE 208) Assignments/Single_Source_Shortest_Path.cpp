#include<bits/stdc++.h>

#define NULL_VALUE -999999
#define INFINITY 999999

using namespace std;

class Graph
{
	int V, E;
	int **matrix;

public:
	Graph(int V);
	~Graph();
	void addEdge(int u, int v, int w);
	void removeEdge(int u, int v);
	bool isEdge(int u, int v);
    int getWeight(int u, int v);
    void printGraph();
    int getMinDistVertex(int dist[], bool isFinalized[]);
    void getShortestPath(int parent[], int vertex, vector<int> &path);
    void makeVerticesPositive();
    vector<int> bellmanford(int source, int dest);
    vector<int> dijkstra(int source, int dest);
};


Graph::Graph(int V)
{
	this->V = V;
	E = 0;
    matrix = new int*[V];
	for(int i=0; i<V; i++)
        matrix[i] = new int[V];

    for(int i=0; i<V; i++) {
        for(int j=0; j<V; j++) {
            matrix[i][j] = NULL_VALUE;
        }
    }
}

void Graph::addEdge(int u, int v, int w)
{
    if(u<0 || u>=V || v<0 || v>=V || isEdge(u, v)) return;

    matrix[u][v] = w;
    E++;
}

void Graph::removeEdge(int u, int v)
{
    if(u<0 || u>=V || v<0 || v>=V) return;

    matrix[u][v] = NULL_VALUE;
    E--;
}

bool Graph::isEdge(int u, int v)
{
    if(u<0 || u>=V || v<0 || v>=V) return false;

    if(matrix[u][v] == NULL_VALUE)
        return true;
    return false;
}

int Graph::getWeight(int u, int v){
     if(u<0 || u>=V || v<0 || v>=V) return NULL_VALUE;
     return matrix[u][v];
}

void Graph::printGraph()
{
    for(int i=0; i<V; i++)
    {
        cout << i << ":";
        for(int j=0; j<V; j++){

            if(!matrix[i][j])
                cout << " " << j;
        }
        cout << endl;
    }
}

Graph::~Graph()
{
    for(int i = 0; i < V; i++) delete[] matrix[i];
    delete[] matrix;

    matrix = 0;
}

void Graph::makeVerticesPositive()
{
    for(int u = 0; u < V; u++){
        for(int v = 0; v < V; v++){
            matrix[u][v] = abs(matrix[u][v]);
        }
    }
}

int Graph::getMinDistVertex(int dist[], bool isFinalized[])
{
	int min = INFINITY, min_index;

	for (int i = 0; i < V; i++){
		if (!isFinalized[i] && dist[i] < min){
            min = dist[i];
            min_index = i;
		}
    }
	return min_index;
}

void Graph::getShortestPath(int parent[], int vertex, vector<int> &path)
{
	if (parent[vertex] == - 1){
        path.push_back(vertex);
        return;
	}
	getShortestPath(parent, parent[vertex], path);

	path.push_back(vertex);
}

vector<int> Graph::bellmanford(int source, int dest)
{
    vector<int> path;

    if(source<0 || source>=V || dest<0 || dest>=V) return path;

	int dist[V];
	int parent[V];

    //initialization
	for (int i = 0; i < V; i++)
	{
	    dist[i] = INFINITY;
		parent[i] = -1;
	}

	dist[source] = 0;

	for (int i = 0; i < V - 1; i++){

	    for(int u = 0; u < V; u++){
            for(int v = 0; v < V; v++){
                //relaxation
                if (isEdge(u, v) && dist[u] != INFINITY && dist[u] + getWeight(u, v) < dist[v]){
                    parent[v] = u;
                    dist[v] = dist[u] + getWeight(u, v);
                }
            }
	    }
	}

	//check negative cycle
	for(int u = 0; u < V; u++){
        for(int v = 0; v < V; v++){
            //relaxation
            if (isEdge(u, v) && dist[u] != INFINITY && dist[u] + getWeight(u, v) < dist[v]){
                return path;
            }
        }
    }

    if(dist[dest] == INFINITY) return path;

	path.push_back(dist[dest]); //path[0] is distance, path starts from index 1
	getShortestPath(parent, dest, path);

	return path;
}

vector<int> Graph::dijkstra(int source, int dest)
{
    vector<int> path;

    if(source<0 || source>=V || dest<0 || dest>=V) return path;

	int dist[V];
	bool isFinalized[V];
	int parent[V];

    //initialization
	for (int i = 0; i < V; i++)
	{
	    dist[i] = INFINITY;
		parent[i] = -1;
		isFinalized[i] = false;
	}

	dist[source] = 0;

	for (int i = 0; i < V - 1; i++)
	{
		int u = getMinDistVertex(dist, isFinalized);

		//remaining edges not reachable
		if(dist[u] == INFINITY) break;

		isFinalized[u] = true;

		for (int v = 0; v < V; v++){
            //relaxation
             if (!isFinalized[v] && getWeight(u, v) && dist[u] + getWeight(u, v) < dist[v]){
				parent[v] = u;
				dist[v] = dist[u] + getWeight(u, v);
			}
		}
	}

	if(dist[dest] == INFINITY) return path;

	path.push_back(dist[dest]);
	getShortestPath(parent, dest, path);

	return path;
}

int main()
{
    int V, E, source, dest;

    ifstream fin;
    ofstream fout;

    fin.open("input.txt");

    //read number of vertices and edges
    fin >> V >> E;

    Graph graph(V);

    //read edges and their weights
    int u, v, w;
    for (int i = 0; i < E; i++){
        fin >> u >> v >> w;
        graph.addEdge(u, v, w);
    }

    //read source and destination
    fin >> source >> dest;

    fin.close();

    fout.open("output.txt");

    vector<int> path;

    //run bellman-ford
    path = graph.bellmanford(source, dest);

    fout << "Bellman-Ford Algorithm:\n";

    if(path.size() > 0){

        fout << path[0] << endl; //path[0] is distance, path starts from index 1

        int i;
        for(i=1; i<path.size()-1; i++)
            fout << path[i] << " -> ";
        fout << path[i] << endl;
    }
    else
        fout << "no shortest path\n";

    //executing |weight| for Dijkstra
    graph.makeVerticesPositive();

    //run Dijkstra
    path = graph.dijkstra(source, dest);

    fout << "Dijkstra Algorithm:\n";

    if(path.size() > 0){

        fout << path[0] << endl; //path[0] is distance, path starts from index 1

        int i;
        for(i=1; i<path.size()-1; i++)
            fout << path[i] << " -> ";
        fout << path[i] << endl;
    }
    else
        fout << "no shortest path\n";

	fout.close();

	return 0;
}

