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
    void enqueue(int item);
    int dequeue();
    bool empty();
};

Queue::Queue()
{
    queueInitSize = 2 ;
    queueMaxSize = queueInitSize;
    data = new int[queueMaxSize] ;
    length = 0 ;
    front = 0;
    rear = 0;
}


void Queue::enqueue(int item)
{
	if (length == queueMaxSize)
	{
		int * tempData ;

		queueMaxSize = 2 * queueMaxSize ;
		tempData = new int[queueMaxSize] ;
		int i, j;
		j = 0;
		for( i = rear; i < length ; i++ )
		{
			tempData[j++] = data[i] ;
		}
		for( i = 0; i < rear ; i++ )
		{
			tempData[j++] = data[i] ;
		}
		rear = 0 ;
		front = length ;
		delete[] data ;
		data = tempData ;
	}

	data[front] = item ;
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
	rear = (rear + 1) % queueMaxSize ;
	length-- ;
	return item ;
}


Queue::~Queue()
{
    if(data) delete[] data;
    data = 0;
}

class ArrayList
{
	int * list;
	int length ;
	int listMaxSize ;
	int listInitSize ;
public:
	ArrayList() ;
	~ArrayList() ;
	int searchItem(int item) ;
    void insertItem(int item) ;
	void removeItem(int item) ;
	void removeItemAt(int item);
	int getItem(int position) ;
	int getLength();
	bool empty();
	void printList();
} ;


ArrayList::ArrayList()
{
	listInitSize = 2 ;
	listMaxSize = listInitSize ;
	list = new int[listMaxSize] ;
	length = 0 ;
}

void ArrayList::insertItem(int newitem)
{
	int * tempList ;
	if (length == listMaxSize)
	{
		listMaxSize = 2 * listMaxSize ;
		tempList = new int[listMaxSize] ;
		int i;
        for( i = 0; i < length ; i++ )
        {
            tempList[i] = list[i] ;
        }
        delete[] list ;
        list = tempList ;
	};

	list[length] = newitem ;
	length++ ;
}

int ArrayList::searchItem(int item)
{
	int i = 0;
	for (i = 0; i < length; i++)
	{
		if( list[i] == item ) return i;
	}
	return NULL_VALUE;
}

void ArrayList::removeItemAt(int position)
{
	if ( position < 0 || position >= length ) return ;
	list[position] = list[length-1] ;
	length-- ;
}


void ArrayList::removeItem(int item)
{
	int position;
	position = searchItem(item) ;
	if ( position == NULL_VALUE ) return ;
	removeItemAt(position) ;
}


int ArrayList::getItem(int position)
{
	if(position < 0 || position >= length) return NULL_VALUE ;
	return list[position] ;
}

int ArrayList::getLength()
{
	return length ;
}

bool ArrayList::empty()
{
    if(length==0)return true;
    else return false;
}

void ArrayList::printList()
{
    int i;
    for(i=0;i<length;i++)
        printf("%d ", list[i]);
    printf("Current size: %d, current length: %d\n", listMaxSize, length);
}

ArrayList::~ArrayList()
{
    if(list) delete [] list;
    list = 0 ;
}

class Graph
{
    int nVertices, nEdges ;
	bool directed ;
	ArrayList  * adjList ;
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
	adjList = 0 ;
	dist = 0;
	color = 0;
	parent = 0;
	directed = dir ;
}

void Graph::setnVertices(int n)
{
	this->nVertices = n ;
	if(adjList!=0) delete[] adjList ;

	adjList = new ArrayList[nVertices] ;
	dist = new int[nVertices];
	color = new int[nVertices];
	parent = new int[nVertices];
}

bool Graph::addEdge(int u, int v)
{
    if(u<0 || v<0 || u>=nVertices || v>=nVertices || isEdge(u, v)) return false;
    this->nEdges++ ;
	adjList[u].insertItem(v) ;
	if(!directed) adjList[v].insertItem(u) ;
	return true;
}

void Graph::removeEdge(int u, int v)
{
    if(u<0 || v<0 || u>=nVertices || v>=nVertices) return;

    this->nEdges-- ;
	adjList[u].removeItem(v) ;
	if(!directed) adjList[v].removeItem(u) ;
}

bool Graph::isEdge(int u, int v)
{
    if(u<0 || u>=nVertices || v<0 || v>=nVertices) return false;

    if(adjList[u].searchItem(v) != NULL_VALUE)
        return true;

    return false;
}

int Graph::getInDegree(int u)
{
    int degree = 0;

    if(u<0 || u>=nVertices) return NULL_VALUE;

    for(int i = 0; i < nVertices; i++){
        if(adjList[i].searchItem(u) != NULL_VALUE)
            degree++;
    }

    return degree;
}

int Graph::getOutDegree(int u)
{
    if(u<0 || u>=nVertices) return NULL_VALUE;

    return adjList[u].getLength();
}

bool Graph::hasCommonAdjacent(int u, int v)
{
    if(u<0 || u>=nVertices || v<0 || v>=nVertices) return false;

    for(int i = 0; i < adjList[u].getLength(); i++){

        if(isEdge(v, adjList[u].getItem(i)))
            return true;
    }

    if(directed){
        for(int i = 0; i < nVertices; i++){
            if(isEdge(i, u) && isEdge(i, v))
                return true;
        }
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

        for(int i = 0; i < adjList[u].getLength(); i++){

            int v = adjList[u].getItem(i);

            if(color[v] == WHITE){

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
    for(int i = 0; i < adjList[u].getLength(); i++){
        v = adjList[u].getItem(i);
        if(color[v] == WHITE){
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
        for(int j=0; j<adjList[i].getLength();j++)
        {
            printf(" %d", adjList[i].getItem(j));
        }
        printf("\n");
    }
}

Graph::~Graph()
{
    if(adjList) delete [] adjList;
    if(dist) delete [] dist;
    if(color) delete [] color;
    adjList = 0 ;
    dist = 0 ;
    color = 0 ;
}

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
            out.open("adjacency_list_bfs_runtime.txt");

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
