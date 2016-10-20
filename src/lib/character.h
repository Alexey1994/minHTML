#ifndef CHARACTER_H_INCLUDED
#define CHARACTER_H_INCLUDED


#include "../extends.h"


typedef char Character;


Boolean is_number(Character c);
Boolean is_hex_number(Character c);
Boolean is_letter(Character c);
Boolean is_space(Character c);


#endif // CHARACTER_H_INCLUDED
