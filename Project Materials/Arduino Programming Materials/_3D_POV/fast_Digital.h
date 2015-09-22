/**
 * \file fast_Digital.h
 *
 * This is a port-manipulation library for doing things similar to
 * what you'd find in \c <wiring_digital.c> , e.g. \c digitalWrite() ,
 * \c digitalRead() and \c pinMode() .
 *
 * The primary benefit of using this library is that it uses templates
 * to compile-time optimize the operations.
 *
 * Included as well are some extra convenience structures and functions.
 *
 * \date Feb 1, 2015
 * \author Lyle Moffitt
 * \copyright MIT License
 */

#ifndef FAST_DIGITAL_H_
#define FAST_DIGITAL_H_

#include <stddef.h>
#include <avr/io.h>

#define RM( _STR_ ) "" // _STR_


#ifdef CONSTEXPR_ARDUINO_PINS
#  include <Arduino.h>
#endif


template<bool B, class T = void>
struct enable_if {};
template<class T>
struct enable_if<true, T> { typedef T type; };

template<bool B, class T, class F>
struct type_if { typedef T type; };
template<class T, class F>
struct type_if<false, T, F> { typedef F type; };


template<unsigned bits,class t0, class ... tN>
struct least_t {
	typedef typename type_if
			< (bits <= (8*sizeof(t0)))
			, t0
			, typename least_t<bits,tN...>::type
			>::type		type;
};

template<unsigned bits,class t0>
struct least_t <bits,t0>{
	typedef typename enable_if
			< (bits <= (8*sizeof(t0)))
			, t0
			>::type		type;
};

namespace fast_binary_helpers
{

	constexpr inline static
	uint8_t to_binary(char ch){ return ( ch=='0' ? 0u : 1u ); }

	template<typename val_t>
	constexpr inline static
	val_t mk_binary(val_t total)
	{
		return  total;//((total<<1)|to_binary<head>());
	}
	template<typename val_t,typename chr, typename...chrs>
	constexpr inline static
	val_t mk_binary(val_t total, chr head, chrs...tail)
	{
		return mk_binary((total<<1)|to_binary(head),tail...);
	}

};//namespace fast_binary_helpers


/**
 * Binary string-literal operator.
 * Creates an easy to use representation of a value as a literal binary
 * string, whilst optimizing for smallest size necessary to store that value.
 * Value is computed at compile time.
 */
template<char...chrs>
constexpr inline
typename least_t<sizeof...(chrs),uint8_t,uint16_t,uint32_t,uint64_t>::type
operator"" _b()
{
	using type = typename least_t<sizeof...(chrs),uint8_t,uint16_t,uint32_t,uint64_t>::type;
	return fast_binary_helpers::mk_binary<type>(0u,chrs...);
}
/**
 * Binary string-literal, strictly padded (or truncated) to 8bits.
 */
template<char...chrs>
constexpr inline
uint8_t operator"" _b8()
{
	return fast_binary_helpers::mk_binary<uint8_t>(0u,chrs...);
}
/**
 * Binary string-literal, strictly padded (or truncated) to 16bits.
 */
template<char...chrs>
constexpr inline
uint16_t operator"" _b16()
{
	return fast_binary_helpers::mk_binary<uint16_t>(0u,chrs...);
}
//Show that bits will have smallest possible packing
static_assert( sizeof(uint8_t )==sizeof(0_b),"");
static_assert( sizeof(uint8_t )==sizeof(10101101_b),"");
static_assert( sizeof(uint16_t)==sizeof(101011101_b),"");
static_assert( sizeof(uint16_t)==sizeof(1111000011110000_b),"");
static_assert( sizeof(uint32_t)==sizeof(11110000111100001_b),"");
static_assert( sizeof(uint32_t)==sizeof(00010010001101000101011001111000_b),"");
//Show that the bits directly represent the value
static_assert( 0x05==0101_b,"");
static_assert( 0x65==01100101_b,"");
static_assert( 0x56==001010110_b,"");
static_assert( 0x12345678==00010010001101000101011001111000_b,"");


/**
 * A simple struct representing a byte as a list of indexes.
 * Compile-time calculated value.
 */
template< typename value_t = byte>
struct bit_list{
	typedef value_t value_type;

	/// The value constructed from the initializer list
	value_type value;

	/// Cast to \c value_type
	inline constexpr operator value_type() const{
		return value;
	}
	inline constexpr value_t operator()(void) const{return value;}

	/// The size of the value in bits
	size_t inline static constexpr bit_size(){
		return (sizeof(value_type)*8);
	}
	/// Default construct with \c NULL value
	constexpr bit_list() : value(0x0){
	}
	/**
	 * Construct from a list of bit positions.
	 * \param b0...bN	An initializer list of bit positions to set as true.
	 * Ex: { 7,3,1 } == B10001010
	 * \note Index values must be positive and less than \c bit_size()
	 * \note The number of indexes must be less than \c bit_size()
	 */
	template<typename t0, typename ... tN>
	constexpr bit_list( t0 b0, tN ... bN ) : value( (1<<((value_type(b0))%bit_size())) | bit_list(bN...).value ){
		static_assert( (sizeof...(bN) + 1) < bit_size(),"Too many members in initializer array.");
	}
};
using _B = bit_list<byte>;
using _W = bit_list<uint16_t>;

static_assert(bit_list<>({7,3,1})==((1<<1)|(1<<3)|(1<<7)),RM("Bit order error."));
static_assert(bit_list<uint16_t>({15,14,7,3,1})==uint16_t((1<<15)|(1<<14)|(1<<7)|(1<<3)|(1<<1)),RM("Byte order error."));

/**
 * An 8-bit union with a plethora of different breakdowns.
 */
union Bit_Mask {
	/// The whole value of the byte
	byte value;

	/// The individual bits
	struct {
		byte b0 : 1; ///< B00000001
		byte b1 : 1; ///< B00000010
		byte b2 : 1; ///< B00000100
		byte b3 : 1; ///< B00001000
		byte b4 : 1; ///< B00010000
		byte b5 : 1; ///< B00100000
		byte b6 : 1; ///< B01000000
		byte b7 : 1; ///< B10000000
	};

	/// The upper and lower half
	struct {
		byte lower_nibble : 4; ///< 0x0f
		byte upper_nibble : 4; ///< 0xf0
	};

	/// Default construct with NULL value;
	constexpr Bit_Mask() : value(0x0){
	}

	/**
	 * Value-construct from a byte value
	 */
	template<typename value_type = byte>
	constexpr Bit_Mask( value_type v) : value(v){
	}
//	template<typename value_type = byte>
//	constexpr Bit_Mask(const value_type & v) : value(static_cast<byte>(v)){
//	}


	/**
	 * Construct from an list of bits positions:
	 * 		ex. 0b01000101 == { 6,2,1 }
	 * @param bits The indexes of the bits that will be true
	 */
	constexpr Bit_Mask( bit_list<byte> bits ) : value(bits.value){
	}

	/**
	 * Cast to byte from \c (*this).value
	 */
	constexpr inline operator byte(){
		return value;
	}
	inline
	byte operator = (byte b){
		return value = b;
	}

//	template<typename value_type = byte>
//	constexpr inline operator value_type (){
//		return static_cast<value_type>( value );
//	}

}; // union Bit_Mask

static_assert(sizeof(Bit_Mask)==1,RM("Size of Bit_Mask incorrect: should be 1 byte."));
static_assert(Bit_Mask({0,4,7}).value == ((1<<0)|(1<<4)|(1<<7)),RM("Initializer bit-order mismatch."));
static_assert(Bit_Mask((1<<0)).b0,RM("Struct bit-field packing error: bit[0] mismatch."));
static_assert(Bit_Mask((1<<1)).b1,RM("Struct bit-field packing error: bit[1] mismatch."));
static_assert(Bit_Mask((1<<2)).b2,RM("Struct bit-field packing error: bit[2] mismatch."));
static_assert(Bit_Mask((1<<3)).b3,RM("Struct bit-field packing error: bit[3] mismatch."));
static_assert(Bit_Mask((1<<4)).b4,RM("Struct bit-field packing error: bit[4] mismatch."));
static_assert(Bit_Mask((1<<5)).b5,RM("Struct bit-field packing error: bit[5] mismatch."));
static_assert(Bit_Mask((1<<6)).b6,RM("Struct bit-field packing error: bit[6] mismatch."));
static_assert(Bit_Mask((1<<7)).b7,RM("Struct bit-field packing error: bit[7] mismatch."));
static_assert(Bit_Mask(0x30).upper_nibble==0x3,RM("Struct bit-field packing error: nibble mismatch."));
static_assert(Bit_Mask(0x07).lower_nibble==0x7,RM("Struct bit-field packing error: nibble mismatch."));


#ifdef CONSTEXPR_ARDUINO_PINS
#define ARDUINO_MAIN
#define __PGMSPACE_H_
#undef PROGMEM
#define PROGMEM constexpr
#undef Pins_Arduino_h
#include <pins_arduino.h>
#undef __PGMSPACE_H_
#undef PROGMEM

constexpr uint16_t inline pin_to_port(uint8_t pin){
	return *(digital_pin_to_port_PGM + pin);
}
constexpr uint16_t inline pin_to_bit_mask(uint8_t pin){
	return *(digital_pin_to_bit_mask_PGM + pin );
}
constexpr uint16_t inline pin_to_timer(uint8_t pin){
	return *(digital_pin_to_timer_PGM + pin );
}
constexpr uint16_t inline pin_to_output_register(uint8_t pin){
	return *(port_to_output_PGM + *(digital_pin_to_port_PGM + pin) );
}
constexpr uint16_t inline pin_to_input_register(uint8_t pin){
	return *(port_to_input_PGM + *(digital_pin_to_port_PGM + pin) );
}
constexpr uint16_t inline pin_to_mode_register(uint8_t pin){
	return *(port_to_mode_PGM + *(digital_pin_to_port_PGM + pin) );
}
template<uint16_t PORT_, uint16_t PIN_, uint16_t DDR_>
constexpr uint8_t inline mask_to_pin(uint16_t _mask,uint16_t indx=0){
	return NUM_DIGITAL_PINS<=indx ? 0 :
		( *(digital_pin_to_bit_mask_PGM + indx) != _mask
		|| pin_to_output_register(indx) != DDR_
		|| pin_to_input_register(indx) != PORT_
		|| pin_to_mode_register(indx) != PIN_
		) ? mask_to_pin<PORT_,PIN_,DDR_>(_mask,indx++) : indx;
}
#endif

/// Get the address of the PORTx which ends with specified letter
uint16_t constexpr inline letter_to_PORT_addr(char ltr){
	return
#ifdef PORTA
			ltr=='A' ? (uint16_t) & PORTA :
#endif
#ifdef PORTB
			ltr=='B' ? (uint16_t) & PORTB :
#endif
#ifdef PORTC
			ltr=='C' ? (uint16_t) & PORTC :
#endif
#ifdef PORTD
			ltr=='D' ? (uint16_t) & PORTD :
#endif
#ifdef PORTE
			ltr=='E' ? (uint16_t) & PORTE :
#endif
#ifdef PORTF
			ltr=='F' ? (uint16_t) & PORTF :
#endif
	0;
}
/// Get the address of the PINx which ends with specified letter
uint16_t constexpr inline letter_to_PIN_addr(char ltr){
	return
#ifdef PINA
			ltr=='A' ? (uint16_t) & PINA :
#endif
#ifdef PINB
			ltr=='B' ? (uint16_t) & PINB :
#endif
#ifdef PINC
			ltr=='C' ? (uint16_t) & PINC :
#endif
#ifdef PIND
			ltr=='D' ? (uint16_t) & PIND :
#endif
#ifdef PINE
			ltr=='E' ? (uint16_t) & PINE :
#endif
#ifdef PINF
			ltr=='F' ? (uint16_t) & PINF :
#endif
	0;
}
/// Get the address of the DDRx which ends with specified letter
uint16_t constexpr inline letter_to_DDR_addr(char ltr){
	return
#ifdef DDRA
			ltr=='A' ? (uint16_t) & DDRA :
#endif
#ifdef DDRB
			ltr=='B' ? (uint16_t) & DDRB :
#endif
#ifdef DDRC
			ltr=='C' ? (uint16_t) & DDRC :
#endif
#ifdef DDRD
			ltr=='D' ? (uint16_t) & DDRD :
#endif
#ifdef DDRE
			ltr=='E' ? (uint16_t) & DDRE :
#endif
#ifdef DDRF
			ltr=='F' ? (uint16_t) & DDRF :
#endif
	0;
}



#define REG( _PORT_ , _TYPE_ ) MMIO< (uint16_t)&_PORT_ , _TYPE_ >


/**
 * Memory-Mapped Input/Output
 * \param	<addr>   The address of the port to be manipulated.
 * \param	<val_t>  The type of value that can be read from and written to the port. [default \c uint8_t]
 */
template< uint16_t addr, class val_t=uint8_t>
struct MMIO{
	/// The type of value that can be read from and written to the port. [default \c uint8_t]
	typedef val_t value_type;

	constexpr inline static uint16_t address(){ return addr; }

	/**
	 * Read the Memory-Mapped register
	 * @return The value of the register cast to \c value_type
	 */
	value_type inline static read(){
		return (* (volatile value_type *)(addr) );
	}
	/**
	 * Write to the Memory-Mapped register
	 * @param value	The value to write to the register.
	 * @return The value that was written.
	 */
	void inline static write(value_type value){
		(* (volatile value_type *)(addr) ) = value;
	}

	// Simplifies defining all the operators necessary for MMIO
	#define define_operator( _op_ )                                 \
	template< typename data_t = value_type >                        \
	value_type inline operator _op_ (data_t val){                   \
	    return ((* (volatile data_t *)(addr) ) _op_ val);           \
	}                                                               \

	/* ************************************************************************* */
	// "Fancy" operators

	/// Cast operator; performs the same operation as \c read()
	template<typename data_t = value_type>
	inline operator data_t (){
		return (*( volatile data_t *)(addr)) ;
	}
	template<typename data_t = value_type>
	inline operator data_t *(){
		return ( volatile data_t *)(addr) ;
	}

	/// Pointer to member access operator; useful for accessing bit-field members.
	inline volatile value_type * operator->() {
		return ((volatile value_type *)(addr)) ;
	}

	/// Assignment operator; performs the same operation as \c write()
	template< typename data_t = value_type >
	value_type inline operator =  (data_t val){
		return (* (( value_type *)(addr)) ) = *((value_type*)(&val));
	}

//	define_operator( = )

	/* ************************************************************************* */
	// Bitwise operators

	/// Bitwise NOT operator
	value_type inline operator~() {
		return ~static_cast<unsigned>(* (value_type *)(addr) );
	}

	/// Bitwise AND operator.
	define_operator( & )

	/// Bitwise OR operator.
	define_operator( | )

	/// Bitwise XOR operator.
	define_operator( ^ )

	/// Bitwise shift-left operator.
	define_operator( << )

	/// Bitwise shift-right operator.
	define_operator( >> )

	/// Bitwise AND-EQUALS operator.
	define_operator( &= )

	/// Bitwise OR-EQUALS operator.
	define_operator( |= )

	/// Bitwise XOR-EQUALS operator.
	define_operator( ^= )

	/// Bitwise SHIFT-LEFT-EQUALS operator.
	define_operator( <<= )

	/// Bitwise SHIFT-RIGHT-EQUALS operator.
	define_operator( >>= )

	/* ************************************************************************* */
	// Arithmetic operators

	/// Arithmetic ADD operator.
	define_operator( + )

	/// Arithmetic MINUS operator.
	define_operator( - )

	/// Arithmetic MULTIPLY operator.
	define_operator( * )

	/// Arithmetic DIVIDE operator.
	define_operator( / )

	/// Arithmetic MODULUS operator.
	define_operator( % )

	/// Arithmetic ADD-EQUALS operator.
	define_operator( += )

	/// Arithmetic MINUS-EQUALS operator.
	define_operator( -= )

	/// Arithmetic MULTIPLY-EQUALS operator.
	define_operator( *= )

	/// Arithmetic DIVIDE-EQUALS operator.
	define_operator( /= )

	/// Arithmetic MODULUS-EQUALS operator.
	define_operator( %= )


#undef define_operator
};// struct MMIO




/**
 * Macro to easily specify PORTx, PINx, and DDRx by letter.
 * \param _X_	The letter of the port, ex: A, B, C, etc..
 * \sa PIN_base, PORT_base, PIN, PORT
 */
#define P( _X_ ) (uint16_t)&PORT##_X_, (uint16_t)&PIN##_X_, (uint16_t)&DDR##_X_

/**
 * A PORT manipulator using:
 *  - a port-letter { A, B, C, etc...} <or>
 *  - a triplet of port-addresses { & PORTx, & PINx, & DDRx }
 */
template<uint16_t ...>
struct PIN_base;

#ifdef CONSTEXPR_ARDUINO_PINS
template<uint16_t _pin>
struct PIN_base<_pin>
{
	static_assert(pin_to_output_register(_pin)!=NOT_A_PORT,RM("Invalid pin number: NOT_A_PORT"));
	static_assert(pin_to_input_register(_pin)!=NOT_A_PORT,RM("Invalid pin number: NOT_A_PORT"));
	static_assert(pin_to_mode_register(_pin)!=NOT_A_PORT,RM("Invalid pin number: NOT_A_PORT"));
	typedef  MMIO<pin_to_output_register(_pin)> PORTX;
	typedef  MMIO<pin_to_input_register(_pin)>  PINX ;
	typedef  MMIO<pin_to_mode_register(_pin)>   DDRX ;
	constexpr const static uint8_t MASK = pin_to_bit_mask(_pin);
	constexpr const static uint8_t INDEX = _pin;
};
#endif //CONSTEXPR_ARDUINO_PINS

/**
 * Base the \c PIN definition off of a specified letter
 * \param _letter	The address of DDRx
 * \param bit_place	The bit position of the pin within the PORTx
 */
template<uint16_t _letter, uint16_t bit_place>
struct PIN_base<_letter, bit_place>
{
	static_assert(letter_to_PORT_addr(_letter)!=0,	"Invalid pin letter: NOT_A_PORT");
	static_assert(letter_to_PIN_addr(_letter)!=0,	"Invalid pin letter: NOT_A_PORT");
	static_assert(letter_to_DDR_addr(_letter)!=0,	"Invalid pin letter: NOT_A_PORT");
	typedef  MMIO<letter_to_PORT_addr(_letter)> PORTX;
	typedef  MMIO<letter_to_PIN_addr(_letter)>  PINX ;
	typedef  MMIO<letter_to_DDR_addr(_letter)>  DDRX ;
	constexpr const static uint8_t 	MASK 	= (0x01<<bit_place);
	constexpr const static uint8_t 	INDEX 	= bit_place;
	constexpr const static char 	LETTER 	= _letter;
};

/**
 * Base the \c PIN definition off of specified addresses
 * \param PORT_addr	The address of PORTx
 * \param PIN_addr	The address of PINx
 * \param DDR_addr	The address of DDRx
 * \param bit_place	The bit position of the pin within the PORTx
 */
template<uint16_t PORT_addr, uint16_t PIN_addr, uint16_t DDR_addr, uint16_t bit_place>
struct PIN_base<PORT_addr, PIN_addr, DDR_addr, bit_place>
{
	typedef   MMIO<PORT_addr>  PORTX;
	typedef   MMIO<PIN_addr>   PINX ;
	typedef   MMIO<DDR_addr>   DDRX ;
	constexpr const static uint8_t 	MASK 	= (0x01<<bit_place);
	constexpr const static uint8_t 	INDEX 	= bit_place;
};

/**
 * A PIN manipulator using:
 *  - a port-letter { A, B, C, etc...} <or>
 *  - a triplet of port-addresses { & PORTx, & PINx, & DDRx }
 */
template<uint16_t ... args>
struct PIN : PIN_base<args...>
{
	typedef typename PIN_base<args...>::PORTX PORTX	;
	typedef typename PIN_base<args...>::PINX  PINX	;
	typedef typename PIN_base<args...>::DDRX  DDRX	;
	using  PIN_base<args...>::MASK ;
	using  PIN_base<args...>::INDEX ;

	/// Read the value of the pin
	bool inline static read(){
		return (PINX:: read() & MASK);
	}
	/// Write a value to the pin
	void inline static write(bool val){
		switch(val){
		case true:  PORTX::write(PORTX::read() |  MASK); break;
		case false: PORTX::write(PORTX::read() & ~MASK); break;
		}
	}
	/**
	 * Toggle the value of the pin.
	 * Equivalent to: \code PIN = value \endcode \em OR \code PIN = !PIN \endcode
	 */
	void inline static toggle(){
		PINX::write( MASK );
	}

	/// Cast to byte via \c read()
	inline operator bool(){
		return (PINX::read() & MASK);
	}
	/// Assignment via \c write()
	void inline operator = (bool val){
		switch(val){
			case true:  PORTX::write(PORTX::read() |  MASK); break;
			case false: PORTX::write(PORTX::read() & ~MASK); break;
		}
	}
	template<class bool_expr_t>
	void inline operator = (bool_expr_t val){
		return (*this) = static_cast<bool>(val);
	}

	/**
	 * Set the mode of selected pin.
	 * \sa \c pinMode() in \c <Arduino.h>
	 * @param _mode	The mode to set; one of: \c INPUT, \c INPUT_PULLUP, or \c OUTPUT
	 */
	void inline static mode(byte _mode){
		switch(_mode){
		case INPUT:{
			uint8_t oldSREG (SREG); 	cli();
			PORTX::write(	PORTX::read()	& ~MASK	);
			DDRX::write(	DDRX::read()	& ~MASK	);
			SREG = oldSREG;
			return;
		}
		case INPUT_PULLUP:{
			uint8_t oldSREG (SREG); 	cli();
			DDRX::write(	DDRX::read()	& ~MASK	);
			PORTX::write(	PORTX::read()	|  MASK	);
			SREG = oldSREG;
			return;
		}
		case OUTPUT:{
			uint8_t oldSREG (SREG); 	cli();
			DDRX::write(	DDRX::read()	|  MASK	);
			SREG = oldSREG;
			return;
		}
		}
	}

};// struct PIN


/**
 * A PORT manipulator using:
 *  - a port-letter { A, B, C, etc...} <or>
 *  - a triplet of port-addresses { & PORTx, & PINx, & DDRx }
 */
template<uint16_t ... args>
struct PORT_base;

/**
 * Base the \c PORT definition off of specified addresses
 * \param PORT_addr	The address of PORTx
 * \param PIN_addr	The address of PINx
 * \param DDR_addr	The address of DDRx
 */
template<uint16_t _letter>
struct PORT_base <_letter>
{
	static_assert(letter_to_PORT_addr(_letter)!=0,RM("Invalid pin letter: NOT_A_PORT"));
	static_assert(letter_to_PIN_addr(_letter)!=0,RM("Invalid pin letter: NOT_A_PORT"));
	static_assert(letter_to_DDR_addr(_letter)!=0,RM("Invalid pin letter: NOT_A_PORT"));
	typedef  MMIO<letter_to_PORT_addr(_letter)> PORTX;
	typedef  MMIO<letter_to_PIN_addr(_letter)>  PINX ;
	typedef  MMIO<letter_to_DDR_addr(_letter)>  DDRX ;
	constexpr const static char LETTER = _letter;
};

/**
 * Base the \c PORT definition off of specified addresses
 * \param PORT_addr	The address of PORTx
 * \param PIN_addr	The address of PINx
 * \param DDR_addr	The address of DDRx
 */
template<uint16_t PORT_addr, uint16_t PIN_addr, uint16_t DDR_addr>
struct PORT_base <PORT_addr, PIN_addr, DDR_addr>
{
	typedef   MMIO<PORT_addr>  PORTX;
	typedef   MMIO<PIN_addr>   PINX ;
	typedef   MMIO<DDR_addr>   DDRX ;
};

/**
 * A PORT manipulator using:
 *  - a port-letter { A, B, C, etc...} <or>
 *  - a triplet of port-addresses { & PORTx, & PINx, & DDRx }
 */
template<uint16_t ... args>
struct PORT : PORT_base<args...>
{
	typedef typename PORT_base<args...>::PORTX PORTX;
	typedef typename PORT_base<args...>::PINX  PINX;
	typedef typename PORT_base<args...>::DDRX  DDRX;

	/// Read the value of the port
	Bit_Mask inline static read(){
		return Bit_Mask(PINX:: read());
	}
	/// Write a value to the port
	void inline static write(Bit_Mask val){
		 PORTX::write(val.value);
	}
	/// Write only the true bits of the mask, i.e. \code PORT |= value \endcode
	void inline static set(Bit_Mask val){
		PORTX::write(PORTX::read() |  val.value);
	}
	/// Clear only the true bits of the mask, i.e. \code PORT &= ~value \endcode
	void inline static clr(Bit_Mask val){
		PORTX::write(PORTX::read() & ~val.value);
	}
	/// Toggle only the true bits of the mask, i.e. \code PIN = value \endcode
	void inline static toggle(Bit_Mask val){
		PINX::write( val.value );
	}

	/// Cast to byte by reading from PIN
	inline operator byte(){
		return PINX::read();
	}
	/// Assignment via writing to PORT
	Bit_Mask inline operator = (Bit_Mask val){
		return PORTX::write(val.value);
	}

	/**
	 * Set the mode(s) of selected pin(s) the PORT.
	 * \sa \c pinMode() in \c <Arduino.h>
	 * @param _mode	The mode to set; one of: \c INPUT, \c INPUT_PULLUP, or \c OUTPUT
	 * @param val	Which pins (based on the true bits) to set with that mode.
	 */
	void inline static mode(byte _mode,Bit_Mask val){
		switch(_mode){
		case INPUT:{
			uint8_t oldSREG (SREG); 	cli();
			PORTX::write(	PORTX::read()	& ~val.value	);
			DDRX::write(	DDRX::read()	& ~val.value	);
			SREG = oldSREG;
			return;
		}
		case INPUT_PULLUP:{
			uint8_t oldSREG (SREG); 	cli();
			DDRX::write(	DDRX::read()	& ~val.value	);
			PORTX::write(	PORTX::read()	|  val.value	);
			SREG = oldSREG;
			return;
		}
		case OUTPUT:{
			uint8_t oldSREG (SREG); 	cli();
			DDRX::write(	DDRX::read()	|  val.value	);
			SREG = oldSREG;
			return;
		}
		}
	}

};// struct PORT


/* ************************************************************************* *\
 * Digital Read, Write, & Mode Functions
 * ************************************************************************* *
 *
 * These functions are template-optimized versions of the same from <Arduino.h>
 *
\* ************************************************************************* */

/**
 * Turn off a specific PWM.
 * \param timer	The name of the timer used to control the PWM.
 * \sa Function \c turnOffPWM() in \c <Arduino.h>
 */
template<uint8_t timer>
void turnOffPWM(){}

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

//Inlined empty function call if \c NOT_ON_TIMER amounts to a NOP
template<> void inline turnOffPWM<0>(){}


#if defined(TCCR1A) && defined(COM1A1)
template<> void inline turnOffPWM<TIMER1A>(){	cbi(TCCR1A, COM1A1);	}
#endif
#if defined(TCCR1A) && defined(COM1B1)
template<> void inline turnOffPWM<TIMER1B>(){	cbi(TCCR1A, COM1B1);	}
#endif
#if defined(TCCR1A) && defined(COM1C1)
template<> void inline turnOffPWM<TIMER1C>(){	cbi(TCCR1A, COM1C1);	}
#endif

#if defined(TCCR2) && defined(COM21)
template<> void inline turnOffPWM<TIMER2>(){	cbi(TCCR2, COM21);	}
#endif

#if defined(TCCR0A) && defined(COM0A1)
template<> void inline turnOffPWM<TIMER0A>(){	cbi(TCCR0A, COM0A1);	}
#endif
#if defined(TCCR0A) && defined(COM0B1)
template<> void inline turnOffPWM<TIMER0B>(){	cbi(TCCR0A, COM0B1);	}
#endif

#if defined(TCCR2A) && defined(COM2A1)
template<> void inline turnOffPWM<TIMER2A>(){	cbi(TCCR2A, COM2A1);	}
#endif
#if defined(TCCR2A) && defined(COM2B1)
template<> void inline turnOffPWM<TIMER2B>(){	cbi(TCCR2A, COM2B1); 	}
#endif

#if defined(TCCR3A) && defined(COM3A1)
template<> void inline turnOffPWM<TIMER3A>(){	cbi(TCCR3A, COM3A1);	}
#endif
#if defined(TCCR3A) && defined(COM3B1)
template<> void inline turnOffPWM<TIMER3B>(){	cbi(TCCR3A, COM3B1);	}
#endif
#if defined(TCCR3A) && defined(COM3C1)
template<> void inline turnOffPWM<TIMER3C>(){	cbi(TCCR3A, COM3C1);	}
#endif

#if defined(TCCR4A) && defined(COM4A1)
template<> void inline turnOffPWM<TIMER4A>(){	cbi(TCCR4A, COM4A1);	}
#endif
#if defined(TCCR4A) && defined(COM4B1)
template<> void inline turnOffPWM<TIMER4B>(){	cbi(TCCR4A, COM4B1);	}
#endif
#if defined(TCCR4A) && defined(COM4C1)
template<> void inline turnOffPWM<TIMER4C>(){	cbi(TCCR4A, COM4C1); 	}
#endif
#if defined(TCCR4C) && defined(COM4D1)
template<> void turnOffPWM<TIMER4D>(){	cbi(TCCR4C, COM4D1);	}
#endif

#if defined(TCCR5A)
template<> void inline turnOffPWM<TIMER5A>(){	cbi(TCCR5A, COM5A1);	}
template<> void inline turnOffPWM<TIMER5B>(){	cbi(TCCR5A, COM5B1);	}
template<> void inline turnOffPWM<TIMER5C>(){	cbi(TCCR5A, COM5C1);	}
#endif


#ifdef CONSTEXPR_ARDUINO_PINS
/**
 * Modify the value of a specific pin.
 * \param _pin 	The number of the pin to be modified.
 * \param val	The value to write to the pin.
 * \sa Function \c digitalWrite() in \c <Arduino.h>
 */
template<uint8_t _pin>
void inline static digitalWrite(bool val){
	turnOffPWM<pin_to_timer(_pin)>();//no function call if NOT_ON_TIMER
	PIN<_pin>::write(val);
}

/**
 * Read the value of a specific pin.
 * \param _pin 	The number of the pin to be read.
 * \return		The value of the pin; true == HIGH , false == LOW
 * \sa Function \c digitalRead() in \c <Arduino.h>
 */
template<uint8_t _pin>
bool inline static digitalRead(){
	turnOffPWM<pin_to_timer(_pin)>();//no function call if NOT_ON_TIMER
	return PIN<_pin>::read();
}

/**
 * Set the mode of a specific pin.
 * \param _pin 	The number of the pin to be set.
 * \sa Function \c pinMode() in \c <Arduino.h>
 */
template<uint8_t _pin>
void inline static pinMode(uint8_t _mode){
	PIN<_pin>::mode(_mode);
}
#endif // CONSTEXPR_ARDUINO_PINS



#endif /* FAST_DIGITAL_H_ */
