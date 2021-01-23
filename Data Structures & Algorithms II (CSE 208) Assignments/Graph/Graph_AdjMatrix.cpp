#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>
#include<fstream>
#include <chrono>

#define NULL_VALUE -999999
#define INFINITY 999999
#define WHITE 1
#define GRAY 2
#define BLACK 3

using namespace std;

class Queue
{
    int queueInitSize ;
    int queueMaxSize;
    int * data;
    int length;
    int front;
    int rear;
public:
    Queue();
    ~Queue();
    void enqueue(int item); //insert item in the queue
    int dequeue(); //returns the item according to FIFO
    bool empty(); //return true if Queue is empty
};

Queue::Queue()
{
    queueInitSize = 2 ;
    queueMaxSize = queueInitSize;
    data = new int[queueMaxSize] ; //allocate initial memory
    length = 0 ;
    front = 0;
    rear = 0;
}


void Queue::enqueue(int item)
{
	if (length == queueMaxSize)
	{
		int * tempData ;
		//allocate new memory space for tempList
		queueMaxSize = 2 * queueMaxSize ;
		tempData = new int[queueMaxSize] ;
		int i, j;
		j = 0;
		for( i = rear; i < length ; i++ )
		{
			tempData[j++] = data[i] ; //copy items from rear
		}
		for( i = 0; i < rear ; i++ )
		{
			tempData[j++] = data[i] ; //copy items before rear
		}
		rear = 0 ;
		front = length ;
		delete[] data ; //free the memory allocated before
		data = tempData ; //make list to point to new memory
	}

	data[front] = item ; //store new item
	front = (front + 1) % queueMaxSize ;
	length++ ;
}


bool Queue::empty()
{
	if(length == 0) return true ;
	else return false ;
}


int Queue::dequeue()
{
	if(length == 0) return NULL_VALUE ;
	int item = data[rear] ;
	rear = (rear + 1) % queueMaxSize ;  // circular queue implementation
	length-- ;
	return item ;
}


Queue::~Queue()
{
    if(data) delete[] data; //deallocate memory
    data = 0; //set to NULL
}

//****************Queue class ends here************************


//******************Graph class starts here**************************
class Graph
{
	int nVertices, nEdges ;
	bool directed ;
	int ** matrix ;
	int *dist, *color, *parent;
    void dfs_visit(int u);

public:
	Graph(bool dir = false);
	~Graph();
	void setnVertices(int n);
	bool addEdge(int u, int v);
	void removeEdge(int u, int v);
	bool isEdge(int u, int v);
    int getInDegree(int u);
    int getOutDegree(int u);
    bool hasCommonAdjacent(int u, int v);
    int getDist(int u, int v);
    int getnEdges();
    void printGraph();
	void bfs(int source);
	void dfs();
};


Graph::Graph(bool dir)
{
	nVertices = 0 ;
	nEdges = 0 ;
	matrix = 0 ;
	directed = dir ;
	dist = 0;
	color = 0;
	parent = 0;
}

void Graph::setnVertices(int n)
{
	this->nVertices = n ;

	//allocate space for the matrix
    matrix = new int*[nVertices];
    dist = new int[nVertices];
    color = new int[nVertices];
	parent = new int[nVertices];

    for(int i=0;i<nVertices;i++)
    {
        matrix[i] = new int[nVertices];
        for(int j=0;j<nVertices;j++)
            matrix[i][j] = 0; //initialize the matrix cells to 0
    }

}

bool Graph::addEdge(int u, int v)
{
    if(u<0 || u>=nVertices || v<0 || v>=nVertices || isEdge(u, v)) return false;

    nEdges++;
    matrix[u][v] = 1;
    if(!directed) matrix[v][u] = 1;
    return true;
}

void Graph::removeEdge(int u, int v)
{
    if(u<0 || u>=nVertices || v<0 || v>=nVertices) return;

    nEdges--;
    matrix[u][v] = 0;
    if(!directed) matrix[v][u] = 0;
}

bool Graph::isEdge(int u, int v)
{
    if(u<0 || u>=nVertices || v<0 || v>=nVertices) return false;
    if(matrix[u][v])
        return true;
    return false;
}

int Graph::getInDegree(int u)
{
    int degree = 0;

    if(u<0 || u>=nVertices) return NULL_VALUE;

    for(int i = 0; i < nVertices; i++)
        if(isEdge(i, u)) degree++;

    return degree;
}

int Graph::getOutDegree(int u)
{
    int degree = 0;

    if(u<0 || u>=nVertices) return NULL_VALUE;

    for(int i = 0; i < nVertices; i++)
        if(isEdge(u, i)) degree++;

    return degree;
}

bool Graph::hasCommonAdjacent(int u, int v)
{
    if(u<0 || u>=nVertices || v<0 || v>=nVertices) return false;

    for(int i = 0; i < nVertices; i++)
        if(isEdge(u, i) && isEdge(v, i)) return true;

    if(directed){
        for(int i = 0; i < nVertices; i++)
            if(isEdge(i, u) && isEdge(i, v)) return true;
    }
    return false;
}

void Graph::bfs(int source)
{
    if(source<0 || source>=nVertices) return;

    Queue q;
    int u;

    for(int i = 0; i < nVertices; i++){
        dist[i] = INFINITY;
        color[i] = WHITE;
        parent[i] = NULL_VALUE;
    }

    dist[source] = 0;
    cout << source << " ";
    color[source] = GRAY;
    q.enqueue(source);

    while(!q.empty()){

        u = q.dequeue();

        for(int v = 0; v < nVertices; v++){

            if(matrix[u][v] && color[v] == WHITE){

                cout << v << " ";
                color[v] = GRAY;
                dist[v] = dist[u] + 1;
                parent[v] = u;
                q.enqueue(v);
            }
        }
        color[u] = BLACK;
    }
    cout << endl;
}

void Graph::dfs()
{
    for(int i = 0; i < nVertices; i++){
        color[i] = WHITE;
        parent[i] = NULL_VALUE;
    }

    for(int i = 0; i < nVertices; i++){
        if(color[i] == WHITE)
            dfs_visit(i);
    }
    cout << endl;
}

void Graph::dfs_visit(int u)
{
    cout << u << " ";
    color[u] = GRAY;

    int v;
    for(int v = 0; v < nVertices; v++){
        if(matrix[u][v] && color[v] == WHITE){
            parent[v] = u;
            dfs_visit(v);
        }
    }
    color[u] = BLACK;
}

int Graph::getDist(int u, int v)
{
    if(u<0 || u>=nVertices || v<0 || v>=nVertices) return NULL_VALUE;

    bfs(u);

    return dist[v];
}

int Graph::getnEdges(){
     return nEdges;
}

void Graph::printGraph()
{
    for(int i=0;i<nVertices;i++)
    {
        printf("%d:", i);
        for(int j=0; j<nVertices;j++)
        {
            if(matrix[i][j]==1)
                printf(" %d", j);
        }
        printf("\n");
    }
}

Graph::~Graph()
{
    if(matrix){
        for(int i = 0; i < nVertices; i++) delete[] matrix[i];
        delete[] matrix;
    }
    if(dist) delete [] dist;
    if(color) delete [] color;
    matrix = 0;
    dist = 0 ;
    color = 0 ;
}
//**********************Graph class ends here******************************


//******main function to test your code*************************
double count_bfs_runtime(int nVertices, int nEdges)
{
    Graph g;
    g.setnVertices(nVertices);

    int u, v;
    while(true){
        if(g.getnEdges()==nEdges) break;
        u = rand() % nVertices;
        v = rand() % nVertices;
        g.addEdge(u,v);
    }

    int source;
    chrono::duration<double> diff;
    double total_time = 0, avg_time;

    for(int i=0; i<10; i++){
        source = rand() % nVertices;

        auto start = chrono::high_resolution_clock::now();
        g.bfs(source);
        auto finish = chrono::high_resolution_clock::now();

        diff = finish-start;
        auto diff_ms = chrono::duration_cast<chrono::milliseconds>(finish-start);
        total_time += diff_ms.count();
    }
    avg_time = total_time/10;

    return avg_time;
}

int main()
{
    int n;
    int choice;
    bool dir;
    printf("Enter your choice:\n");
    printf("1. directed graph   2. undirected graph\n");
    scanf("%d",&choice);
    if(choice == 1)dir = true;
    else if(choice == 2)dir = false;

    Graph g(dir);
    printf("Enter number of vertices: ");
    scanf("%d", &n);
    g.setnVertices(n);

    while(1)
    {
        printf("1. Add edge  2. Remove edge  3. Print BFS  4. Print DFS  5. isEdge  6. getInDegree\n");
        printf("7. getOutDegree  8. hasCommonAdjacent  9. getDist  10. Print Graph  11. Runtime analysis  12.Exit\n");

        int ch, u, v;
        scanf("%d",&ch);

        if(ch==1)
        {
            scanf("%d%d", &u, &v);
            if(!g.addEdge(u, v))
                cout << "input out of range/edge already exists\n";
        }
        else if(ch==2)
        {
            scanf("%d%d", &u, &v);
            g.removeEdge(u, v);
        }
        else if(ch==3)
        {
            scanf("%d", &u);
            g.bfs(u);
        }
        else if(ch==4)
        {
            g.dfs();
        }
        else if(ch==5)
        {
            scanf("%d%d", &u, &v);
            cout << g.isEdge(u, v) << endl;
        }
        else if(ch==6)
        {
            scanf("%d", &u);
            cout << g.getInDegree(u) << endl;
        }
        else if(ch==7)
        {
            scanf("%d", &u);
            cout << g.getOutDegree(u) << endl;
        }
        else if(ch==8)
        {
            scanf("%d%d", &u, &v);
            cout << g.hasCommonAdjacent(u, v) << endl;
        }
        else if(ch==9)
        {
            scanf("%d%d", &u, &v);
            cout << g.getDist(u, v) << endl;
        }
        else if(ch==10)
        {
            g.printGraph();
        }
        else if(ch==11)
        {
            ofstream out;
            out.open("adjacency_matrix_bfs_runtime.txt");

            for(int i=1000; i<=16000; i*=2){
                for(int j=i; j<=(pow(i,2)-i)/8; j*=2){
                    out << "|V| = " << i << " |E| = " << j << " time: " << count_bfs_runtime(i, j) << " ms\n";
                }
            }

            out.close();
        }
        else if(ch==12)
        {
            break;
        }
    }

}
