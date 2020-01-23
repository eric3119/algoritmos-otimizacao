#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
 
#include <iostream>
#include <list>
#include <stdbool.h>

#include "BPDecoder.h"

bool inicializar();

void display();
 
int draw_bin(std::list < Box > &packedBoxes);