#include <iostream>
#include<cstdio>
#include<cstdlib>
#include <cmath>
#include <sstream>

using namespace std;

#define GRIDSIZE 4
#define UP      0
#define DOWN    2
#define LEFT    3
#define RIGHT   1
#define MAX_SHOTS 3


string to_string(int x) {
    std::string out_string;
    std::stringstream ss;
    ss << x;
    return ss.str();
}


class Position {

    int x, y;

public:

    Position (int x, int y) {
        this->x = x;
        this->y = y;
    }

    Position() {
        x=0;
        y=0;
    }

    void moveRight() {
        if(x<3)
            x++;
    }

    void moveLeft() {
        if(x>0)
            x--;
    }

    void moveUp() {
        if(y<3)
            y++;
    }

    void moveDown() {
        if(y>0)
            y--;
    }

    bool isAdjacent(Position p) {
        if((x==p.getX()+1 || x==p.getX()-1) && y==p.getY()){
                return true;
            }
        else if((y==p.getY()+1 || y==p.getY()-1) && x==p.getX()){
            return true;
        }
        return false;
    }

    bool isSamePoint(Position p) {
        if(x==p.getX() && y==p.getY()){
                return true;
            }
        return false;
    }

    void changeX(){
        if(x==3) x--;
        else x++;
        return;
    }

    int getX() {
        return x;
    }

    int getY() {
        return y;
    }

};


class Wumpus {

    bool killed;
    Position p;

public:

    Wumpus(int x, int y) {
        p = Position(x, y);
        killed = false;
    }

    Wumpus() {
        p=Position(rand()%4,rand()%4);
        killed=false;
    }

    void kill() {
        killed = true;
    }

    Position getPosition() {
        return p;
    }

    bool isKilled(){
        return killed;
    }

    bool isSamePoint(Position pos){
        return p.isSamePoint(pos);
    }

    void changeX(){
        return p.changeX();
    }

};


class Player {

    int direction;
    int total_shots;
    bool killed;
    Position p;

public:

    Player() {
        direction=0;
        killed=false;
        total_shots=MAX_SHOTS;

    }

    void turnLeft() {
        if(!direction)
            direction=3;
        else
            direction--;
    }

    void turnRight() {
        if(direction==3)
            direction=0;
        else
            direction++;
    }

    void moveForward() {
        if(direction==UP) p.moveUp();
        if(direction==DOWN) p.moveDown();
        if(direction==RIGHT) p.moveRight();
        if(direction==LEFT) p.moveLeft();
    }

    bool isAdjacent(Position pos) {
        return p.isAdjacent(pos);
    }

    bool isSamePoint(Position pos) {
        return p.isSamePoint(pos);
    }

    void kill() {
        killed = true;
    }

    string getPositionInfo() {
        return "Player is now at " + to_string(p.getX()) + ", " + to_string(p.getY());
    }

    string getDirectionInfo() {
        string s;
        if (direction == UP) s = "up";
        if (direction == DOWN) s = "down";
        if (direction == LEFT) s = "left";
        if (direction == RIGHT) s = "right";
        return "Player is moving at direction: " + s;
    }

    void changeShot(){
    total_shots--;
    }

    int getShotInfo(){
        return total_shots;
    }

    bool isWumpusKilled(Position pos){
        if(direction==UP){
            if(pos.getX()==p.getX() && pos.getY()>p.getY())
                return true;
        }
        if(direction==DOWN){
            if(pos.getX()==p.getX() && pos.getY()>p.getY())
                return true;
        }
        if(direction==RIGHT){
            if(pos.getX()>p.getX() && pos.getY()==p.getY())
                return true;
        }
        if(direction==LEFT){
            if(pos.getX()<p.getX() && pos.getY()==p.getY())
                return true;
        }
        return false;
    }

    Position getPosition(){
        return p;
    }
};

class WumpusWorld {

private:

    Player player;
    Wumpus wumpus;
    Position pit_position;
    Position gold_position;
    bool ended;

public:

    WumpusWorld() {
        gold_position=Position(rand()%4,rand()%4);
        pit_position=Position(rand()%4,rand()%4);

        if(player.isSamePoint(wumpus.getPosition())){
            wumpus.changeX();
        }
        if(player.isSamePoint(gold_position)){
            gold_position.changeX();
        }
        if(player.isSamePoint(pit_position)){
            pit_position.changeX();
        }

        if(wumpus.isSamePoint(gold_position)){
            gold_position.changeX();
        }
        if(wumpus.isSamePoint(pit_position)){
            pit_position.changeX();
        }
        if(gold_position.isSamePoint(pit_position)){
            pit_position.changeX();
        }

        ended=false;
    }

    WumpusWorld(int wumpus_x, int wumpus_y) {
        wumpus=Wumpus(wumpus_x,wumpus_y);
        ended=false;
    }

    WumpusWorld(int wumpus_x, int wumpus_y, int gold_x, int gold_y) {
        wumpus=Wumpus(wumpus_x,wumpus_y);
        gold_position=Position(gold_x,gold_y);
        ended=false;
    }

    WumpusWorld(int wumpus_x, int wumpus_y, int gold_x, int gold_y, int pit_x, int pit_y) {
        wumpus=Wumpus(wumpus_x,wumpus_y);
        gold_position=Position(gold_x,gold_y);
        pit_position=Position(pit_x,pit_y);

        ended=false;
    }

    void moveForward() {
        player.moveForward();
        return showGameState();
    }

    void turnLeft() {
        player.turnLeft();
        return showGameState();
    }

    void turnRight() {
        player.turnRight();
        return showGameState();
    }

    void shoot() {
        if(wumpus.isKilled()){
            return showGameState();
        }

        if(!player.getShotInfo()){
            return showGameState();
        }

        player.changeShot();

        if(player.isWumpusKilled(wumpus.getPosition())){
            cout<<"Wumpus is killed!"<<endl;
            wumpus.kill();
        }

        return showGameState();
    }


    void showGameState() {
        cout << player.getPositionInfo() << endl;
        cout << player.getDirectionInfo() << endl;

        if (!wumpus.isKilled() && player.isAdjacent(wumpus.getPosition())) {
            cout << "stench!" << endl;
        }

        if(player.isAdjacent(pit_position)){
            cout<<"breeze!"<<endl;
        }

        if(!player.getShotInfo()){
            cout<<"no shot remaining!"<<endl;
        }

        if (!wumpus.isKilled() && player.isSamePoint(wumpus.getPosition())) {
            cout << "Player is killed!" << endl;
            player.kill();
            cout << "Game over!" << endl;
            ended = true;
        }

        if (player.isSamePoint(pit_position)) {
            cout << "Player is killed!" << endl;
            player.kill();
            cout << "Game over!" << endl;
            ended = true;
        }

        if (player.isSamePoint(gold_position)) {
            cout << "Got the gold!" << endl;
            cout << "Game ended, you won!" << endl;
            ended = true;
        }

    }

    bool isOver() {
        return ended;
    }

};


int main()
{
    int c, wumpus_x, wumpus_y, gold_x, gold_y, pit_x, pit_y;
    FILE *fp;

    fp=fopen("wumpus.txt","r");
    fscanf(fp,"%d%d%d%d%d%d",&wumpus_x,&wumpus_y,&gold_x,&gold_y,&pit_x,&pit_y);

    WumpusWorld w(wumpus_x, wumpus_y, gold_x, gold_y, pit_x, pit_y);
    w.showGameState();
    while (!w.isOver()) {
        cout << "1: move forward" << endl;
        cout << "2: Turn left" << endl;
        cout << "3: Turn right" << endl;
        cout << "4: Shoot" << endl;
        cin >> c;
        if (c == 1) {
            w.moveForward();
        } else if (c == 2) {
            w.turnLeft();
        } else if (c == 3) {
            w.turnRight();
        } else if (c == 4) {
            w.shoot();
        }
    }
    return 0;
}
