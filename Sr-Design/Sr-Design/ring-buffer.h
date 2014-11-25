//
//  ring-buffer.h
//  Sr-Design
//
//  Created by Lyle Moffitt on 11/17/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef __Sr_Design__ring_buffer__
#define __Sr_Design__ring_buffer__

#include <iostream>
#include <cstdlib>
#include <cstdint>

/* ========================================================================== *\
 *      Compile-time constant configurations for ring buffers                 *
\* ========================================================================== */

///PI
#define PI 3.141592653589793

namespace ring_conf 
{
    ///A list of the radial distance of each LED
    constexpr const static 
    double radial_dist[] = 
    {   0.50, 0.51, 0.52, 0.53, 0.54, 0.55, 0.56, 0.57, //LSB
        0.58, 0.59, 0.60, 0.61, 0.62, 0.63, 0.64, 0.65  //MSB
    };
    
    ///Time components to complete one pulse.
    /*  
          (write) ________ (write)    (write) ________ (write)
             ''/    (on)    \''          ''/    (on)    \''   
     _______ '/              \'  _______ '/              \'  _
      (off)                       (off)                       
                   Pulse[n]                     pulse[n+1]                   
     */
    namespace pulse
    {  
        ///Time required to write the pulse data
        constexpr const static
        double      t_write     = 0.0000001;
        
        ///Duration that LED is active
        constexpr const static
        double      t_on        = 0.0000001;
        
        ///Duration that LED is inactive
        constexpr const static
        double      t_off       = 0.0000001;
        
        ///Total pulse duration
        constexpr const static
        double      t_period    = t_off + t_write + t_on + t_write;
    }
    
    ///Minimum time that LED must be on to reach activation threshold
    constexpr const static
    double persist_time = 0.01;

    ///Number of interpolations between primary period pulses
    constexpr const static
    uint interpolate_lvl = 0;
    
    ///rotational frequency
    constexpr const static
    double rot_freq = 30.1; 
    
}
using namespace ring_conf;



/* ========================================================================== *\
|*      Data structure definitions ring buffers                               *|
\* ========================================================================== */

///A number of bits to hold the state of each LED in a wing
typedef uint16_t pulse_data;

///A structure to hold all the LED states in a cycle
struct ring_buffer
{   
    ///total size of buffer
    constexpr const static
    size_t size     = (  2.0 * PI  )/(  rot_freq * pulse::t_period  );
    
    ///raw buffer of pulses, indexed by degree [0-size).
    pulse_data data[size];
};

///An array of rings containing the full display image
struct display_buffer
{   /** The number of levels of interpolation.
     
     0  :   The primary image
     1-n:   The interpolation layers; each is situated evenly spaced between 
            the indexes of the primary mage
     */
    constexpr const static
    size_t          depth = interpolate_lvl;
    
    ///The display data array
    ring_buffer     data[depth];
    
    static std::string __S;
};



/* ========================================================================== *\
|*      Data structure definitions ring buffers                               *|
\* ========================================================================== */

///A struct to handle iterating through a ring_buffer
struct ring_reader
{
    size_t              position;
    
    ring_buffer     &   buffer;
    
    ring_reader(ring_buffer & buf):buffer(buf),position(0)
    {}
    
    inline
    pulse_data               operator++(int){
        auto ret = buffer.data[position];
        (++position)%=buffer.size ;
        return ret;
    }
    
    inline
    pulse_data               operator++(){
        return  buffer.data[(++position)%=buffer.size];
    }
    
    inline      
    pulse_data               operator()(void){
        return buffer.data[position];
    }
    
};



#endif /* defined(__Sr_Design__ring_buffer__) */
