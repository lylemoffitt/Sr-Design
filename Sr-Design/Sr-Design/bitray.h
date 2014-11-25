//
//  bitray.h
//  Sr-Design
//
//  Created by Lyle Moffitt on 11/18/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef __Sr_Design__bitray__
#define __Sr_Design__bitray__

#include <iostream>
#include <valarray>
#include <utility>

using namespace std;

template <typename arith_t=uintmax_t>
struct bitray : valarray<bool>
{
    static_assert(is_arithmetic<arith_t>::value,
                  "Template parameter <kind> must be an arithmetic type.");
    
    typedef arith_t number;
    
    bitray(number val=0):
    valarray<bool>( sizeof(number)*CHAR_BIT )
    {    
        uintmax_t pos=1;
        for(auto ii = begin(*this); ii!=end(*this); ++ii){
            (*ii) = (val & pos);    pos<<=1;
        }
    }
    
    bitray &
    operator=(number _n){
        uintmax_t pos=1;
        for(auto ii = begin(*this); ii!=end(*this); ++ii){
            (*ii) = (_n & pos);    pos<<=1;
        }
        return *this;
    }
    
    operator number()
    {   number _n;
        uintmax_t pos=1;
        for(auto ii = begin(*this); ii!=end(*this); ++ii){
            _n += (pos & (*ii));    pos<<=1;
        } 
        return _n;
    }
    
};




#endif /* defined(__Sr_Design__bitray__) */
