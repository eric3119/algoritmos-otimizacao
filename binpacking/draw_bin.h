#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
 
#include <iostream>
#include <list>
#include <stdbool.h>

#include "BPDecoder.h"

bool inicializar();

void display(std::list < Box > &packedBoxes, std::list< Box >::iterator start, std::list< Box >::iterator end);
 
int draw_bin(std::list < Box > &packedBoxes, unsigned bin_x, unsigned bin_y);

void display_space(std::list < Space > &spaces, std::list< Space >::iterator start, std::list< Space >::iterator end);
 
int draw_space(std::list < Space > &spaces, unsigned bin_x, unsigned bin_y);