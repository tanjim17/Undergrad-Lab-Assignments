#include<iostream>
#include<cstdlib>
using namespace std;

class stack{
    int *stck;
    int tos;
    int Size;
public:
    stack();
    stack(int n);
    stack(const stack &stk);

    void push(int n);
    void push(int a[],int n);
    void push(stack Stack,int n);

    int pop();
    int top(){return stck[tos-1];};
    int size(){return tos;};

    void resize();
    void resize(int n);

    float similarity(stack* tempStack);

    ~stack();

};

stack::stack()
{
    stck=new int[10];
    tos=0;
    Size=10;
    cout<<"default constructor called"<<endl;
}

stack::stack(int n)
{
    stck=new int[n];
    tos=0;
    Size=n;
    cout<<"parameterized constructor called"<<endl;
}

stack::stack(const stack &stk)
{
    int i;

    tos=stk.tos;
    Size=stk.Size;
    stck=new int[Size];

    for(i=0;i<tos;i++){
        stck[i]=stk.stck[i];
    }

    cout<<"copy constructor called"<<endl;
}

void stack::push(int n)
{
    resize(1);
    stck[tos]=n;
    tos++;
}

void stack::push(int a[],int n)
{
    resize(n);

    for(int i=0;i<n;i++){
        stck[tos]=a[i];
        tos++;
    }
}

void stack::push(stack Stack,int n)
{
    resize(n);

    for(int i=0;i<n;i++){
        stck[tos]=Stack.pop();
        tos++;
    }
}

int stack::pop()
{
    if(tos==0){
        cout<<"stack is empty";
        exit(0);
    }
    tos--;

    if(Size-tos>10)
        resize();

    return stck[tos];
}

void stack::resize(int n)
{
    int required=tos+n;

    if(required>Size){
        Size=required+10;
        stck=(int*)realloc(stck,sizeof(int)*Size);
    }
}

void stack::resize()
{
    stck=(int*)realloc(stck,sizeof(int)*(Size-=10));
}

float stack::similarity(stack* tempStack)
{
    int count=0,temp=tos;
    float ave_size=(size()+tempStack->size())/2.0;

    while(tos && tempStack->size()){
        if(pop()==tempStack->pop()) count++;
    }

    tos=temp;
    return count/ave_size;
}

stack::~stack()
{
    delete [] stck;
    cout<<"destructing"<<endl;
}


int main()
{
    stack mainStack,Stack;
    stack* tempStack;
    int n,i,j;

    while(1){
        cout<<"1: push an element   5:top"<<endl;
        cout<<"2: push an array     6:size"<<endl;
        cout<<"3: push a stack      7:similarity"<<endl;
        cout<<"4: pop               8:exit"<<endl;

        cin>>i;

        if(i==1){
            cout<<"push the element:"<<endl;
            cin>>n;
            mainStack.push(n);
        }

        if(i==2){
            cout<<"provide the size and elements of the array:"<<endl;

            cin>>n;
            int a[n];
            for(j=0;j<n;j++){
                cin>>a[j];
            }

            mainStack.push(a,n);
        }

        if(i==3){
            cout<<"provide the size and elements of the stack:"<<endl;

            cin>>n;
            int a[n];
            for(j=0;j<n;j++){
                cin>>a[j];
            }

            tempStack=&Stack;

            tempStack->push(a,n);

            mainStack.push(Stack,n);

            delete tempStack;
        }
        if(i==4){
            cout<<mainStack.pop()<<" has been popped."<<endl;
        }
        if(i==5){
            cout<<"the top element of the stack: "<<mainStack.top()<<endl;
        }
        if(i==6){
            cout<<"the total number of elements right now: "<<mainStack.size()<<endl;
        }
        if(i==7){
            cout<<"provide the size and elements of the stack:"<<endl;

            cin>>n;
            int a[n];
            for(j=0;j<n;j++){
                cin>>a[j];
            }

            tempStack=new stack(n);

            tempStack->push(a,n);

            cout<<"Similarity score: "<<mainStack.similarity(tempStack)<<endl;

            delete tempStack;
        }

        if(i==8){
            while(1){
            cout<<mainStack.pop()<<" ";
            }
        }
    }
}

