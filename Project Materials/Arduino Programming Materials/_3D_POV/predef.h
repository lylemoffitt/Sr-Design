/**
 * \file   transmission.h
 *
 * 
 * 
 * \date  		Apr 16, 2015
 * \author		Lyle
 * \copyright	MIT Public License
 */

#ifndef PREDEF_H_
#define PREDEF_H_

union wing;
namespace SRAM
{
struct addr_t;
}

struct block_ref_t;

struct block_spec_t;

struct animate_spec_t;

struct image_spec_t;

// Received by the master control node
struct external_spec_t;

// Received by the slave display nodes
struct internal_spec_t;

// Stored by the slave display nodes
struct storage_spec_t;

struct transmission;

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}


#endif /* PREDEF_H_ */
