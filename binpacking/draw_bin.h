#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
 
#include <iostream>
#include <list>
#include <stdbool.h>

#include "BPDecoder.h"

using namespace std;

bool inicializar();
bool start_allegro(unsigned bin_x, unsigned bin_y);

void clear_display();

void draw_box(Box box);
void draw_boxes(list < Box > &packedBoxes, unsigned bin_number);

void draw_space(Space space);
void draw_spaces(list < Space > &spaces, unsigned bin_number);
 
int draw_bin(list < Box > &packedBoxes, unsigned bin_x, unsigned bin_y);

void display_space(list < Space > &spaces, list< Space >::iterator start, list< Space >::iterator end);
void display(list < Box > &packedBoxes, list< Box >::iterator start, list< Box >::iterator end);
 
int draw_spaces(list < Space > &spaces, unsigned bin_x, unsigned bin_y);