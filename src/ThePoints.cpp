//
//  ThePoints.cpp
//  DitheringExplorations
//
//  Created by Javier Villegas on 3/29/12.
//  Copyright (c) 2012 UCSB. All rights reserved.
//

#include <iostream>

# include "ThePoints.h"

ThePoints::ThePoints(){
    
    Pos.set(0.0,0.0);
    Error = 0.0;
}

ThePoints::ThePoints(float ex,float ey,float eError){
    
    Pos.x = ex;
    Pos.y = ey;
    Error = eError;
}

void ThePoints::set(float ex,float ey,float eError){
    Pos.x = ex;
    Pos.y = ey;
    Error = eError;
}


bool operator> (const ThePoints &Ellip1, const ThePoints &Ellip2)
{
    return (Ellip1.Error) > (Ellip2.Error);	
}
bool operator< (const ThePoints &Ellip1, const ThePoints &Ellip2)
{
    return (Ellip1.Error) < (Ellip1.Error);	
}

ThePoints operator-(const ThePoints &Ell1, const ThePoints &Ell2){
    
    ThePoints OutElli;
    float angledif;
    OutElli.Pos = Ell1.Pos -Ell2.Pos;
    OutElli.Error = 0;
    return OutElli;
}

ThePoints operator*(const float& K, const ThePoints& Ell1){
    
    ThePoints OutElli;
    
    OutElli.Pos = K*Ell1.Pos;
    OutElli.Error = Ell1.Error;
    return OutElli;
     
}
