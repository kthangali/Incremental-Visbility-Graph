#include "State.h"

////////////////////////////////////////////////
///////////////// StateXY //////////////////////

size_t StateXY::customHash() const {
    size_t res = 17;
    res = res * 31 + hash<int>()( c[0] );
    res = res * 31 + hash<int>()( c[1] );
    return res;
}

bool StateXY::operator==(StateXY const& other) const {
    return c[0] == other.c[0] && c[1] == other.c[1];
}


string StateXY::getStr() const {
    return "(" + std::to_string(c[0]) + "," + std::to_string(c[1]) + ")";
}

StateXY StateXY::operator+(StateXY const& other) const {
    return StateXY(c[0]+other.c[0], c[1]+other.c[1]);
}

bool StateXY::operator<(StateXY const& other) const 
{
    if(c[0] == other.c[0]){
        return c[1] < other.c[1];}
    else{
        return c[0] < other.c[0];
    };
}


////////////////////////////////////////////////
/////////////// StateXYTime ////////////////////

size_t StateXYTime::customHash() const {
    size_t res = 17;
    res = res * 31 + hash<int>()( c[0] );
    res = res * 31 + hash<int>()( c[1] );
    res = res * 31 + hash<int>()( c[2] );
    return res;
}

bool StateXYTime::operator==(StateXYTime const& other) const {
    return c[0] == other.c[0] && c[1] == other.c[1] && c[2] == other.c[2];
}

string StateXYTime::getStr() const {
    return "(" + std::to_string(c[0]) + "," + std::to_string(c[1]) + ","
                + std::to_string(c[2]) + ")";
}

StateXYTime StateXYTime::operator+(StateXYTime const& other) const {
    return StateXYTime(c[0]+other.c[0], c[1]+other.c[1], c[2]+other.c[2]);
}


/////////////////////////////////////////////////////
///////////////// StateXYTheta //////////////////////

size_t StateXYTheta::customHash() const {
    size_t res = 17;
    res = res * 31 + hash<int>()( c[0] );
    res = res * 31 + hash<int>()( c[1] );
    res = res * 31 + hash<int>()( c[2] );
    return res;
}

bool StateXYTheta::operator==(StateXYTheta const& other) const {
    return c[0] == other.c[0] && c[1] == other.c[1] && c[2] == other.c[2];
}

string StateXYTheta::getStr() const {
    return "(" + std::to_string(c[0]) + "," + std::to_string(c[1]) + ","
                + std::to_string(c[2]) + ")";
}

StateXYTheta StateXYTheta::operator+(StateXYTheta const& other) const {
    return StateXYTheta(c[0]+other.c[0], c[1]+other.c[1], (c[2]+other.c[2]+4)%4);
}