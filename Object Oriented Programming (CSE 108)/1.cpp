#include<iostream>
#include<cstring>
using namespace std;
#define distance 100

class Unit
{
    int range,health,damage,position,step,cost,return_coin;
public:
    Unit(int r,int h,int d,int c=0,int rc=0,int s=0)
    {
        range=r;
        health=h;
        damage=d;
        cost=c;
        return_coin=rc;
        step=s;
        position=0;
    }

    void healthChange(int change)
    {
        health-=change;
    }

    int posChange()
    {
        if(position+step>100)
            return 0;
        position+=step;
        return 1;
    }

    int getHealth(){return health;}

    int getPosition(){return position;}

    int getDamage(){return damage;}

    int getRange(){return range;}

    int getCost(){return cost;}

    int ReturnCoin(){return return_coin;}
};

class Archer:public Unit
{
public:
    Archer(int r,int h,int d,int c,int rc,int s):Unit(r,h,d,c,rc,s){}
};

class AdvancedArcher:public Unit
{
public:
    AdvancedArcher(int r,int h,int d,int c,int rc,int s):Unit(r,h,d,c,rc,s){}
};

class Bowman:public Unit
{
public:
    Bowman(int r,int h,int d,int c,int rc):Unit(r,h,d,c,rc){}
};

class AdvancedBowman:public Unit
{
public:
    AdvancedBowman(int r,int h,int d,int c,int rc):Unit(r,h,d,c,rc){}
};

class EnemyTower:public Unit
{
public:
    EnemyTower(int r,int h,int d):Unit(r,h,d){}
};

int main()
{
    Unit *w,*e=new EnemyTower(200,300,40);
    int choice,coin=1600,round=1;
    const char *name,*moveType,*posType,*arrowType;

    while(1)
    {
        cout<<"Remaining coin: "<<coin<<endl;
        cout<<"1.Archer <cost 150>"<<endl;
        cout<<"2.AdvancedArcher <cost 600>"<<endl;
        cout<<"3.Bowman <cost 100>"<<endl;
        cout<<"4.AdvancedBowman <cost 200>"<<endl;

        cin>>choice;

        switch(choice)
        {
            case 1:
                w =  new Archer(50,100,25,150,0,25);

                name="Archer";
                moveType="Running. ";
                posType="Positioned";
                arrowType="Advanced Arrow";

                break;

            case 2:
                w = new AdvancedArcher(50,120,50,600,0,30);

                name="AdvancedArcher";
                moveType="Riding horse. ";
                posType="Positioned";
                arrowType="Improved Arrow";

                break;

            case 3:
                w =  new Bowman(110,60,10,100,40);

                name="Bowman";
                moveType="";
                posType="Stationary Positioned";
                arrowType="Basic Arrow";

                    break;

            case 4:
                w = new AdvancedBowman(130,85,15,200,60);

                name="AdvancedBowman";
                moveType="";
                posType="Stationary Positioned";
                arrowType="Canon";

                break;

            default:
                continue;
        }

        if(coin-w->getCost()<0){
            cout<<"not enough coins!!"<<endl<<endl;
             continue;
        }

        coin -= w->getCost();

        while(w->getHealth()>0){

                    cout<<"Round: "<<round<<endl;

                    cout<<name<<". Health: "<<w->getHealth()<<". ";

                    if(w->posChange()){
                        cout<<moveType;
                    }

                    cout<<posType<<" at "<<w->getPosition()<<". ";

                    if( distance - w->getPosition() <= w->getRange() ){
                        cout<<name<<" is shooting "<<arrowType<<".";
                        e->healthChange(w->getDamage());
                    }

                    cout<<endl;

                    cout<<"EnemyTower Health: "<<e->getHealth()<<". ";

                    if(e->getHealth()<=0){
                        cout<<" EnemyTower has fallen!! You have won!!"<<endl;
                        return 0;
                    }

                    cout<<"Enemy is shooting Fire arrow."<<endl<<endl;
                    w->healthChange(e->getDamage());

                    round++;
                }

                cout<<"Round: "<<round<<endl;
                cout<<name<<" Health: "<<w->getHealth()<<". Unit Died!!"<<endl<<endl;

                coin+=w->ReturnCoin();
                cout<<name<<" has returned "<<w->ReturnCoin()<<" coins."<<endl<<endl;

                if(coin<100){
                    cout<<"Not enough coin to deploy another unit! You have lost the war!!";
                    return 0;
                }

    }
}
