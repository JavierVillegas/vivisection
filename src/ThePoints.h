//
//  ThePoints.h
//  DitheringExplorations
//
//  Created by Javier Villegas on 3/29/12.
//  Copyright (c) 2012 UCSB. All rights reserved.
//

#ifndef DitheringExplorations_ThePoints_h
#define DitheringExplorations_ThePoints_h
#include "ofMain.h"

class ThePoints {
    
public:
    
    ofVec2f Pos;
    float Error;
    ThePoints(float,float,float);
    ThePoints();
    void set(float,float,float);
    void normalize();
    float getLength();

    friend bool operator>(const ThePoints& Ell1, const ThePoints& Ell2);
    friend bool operator<(const ThePoints& Ell1, const ThePoints& Ell2);
    friend ThePoints operator-(const ThePoints& Ell1, const ThePoints& Ell2);
    friend ThePoints operator*(const float& K, const ThePoints& Ell1);
    
};






#endif
