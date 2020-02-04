#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
 
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <stdbool.h>

#include "BPDecoder.h"

using namespace std;

bool inicializar();
bool start_allegro(unsigned bin_x, unsigned bin_y);
void draw_wait(float time);

void clear_display();

void draw_box(Box box);
void draw_boxes(list < Box > &packedBoxes, unsigned bin_number);

void draw_space(Space space);
void draw_spaces(list < Space > &spaces, unsigned bin_number);
 
void draw_bin(list < Box > &packed_boxes, list < Space > &empty_spaces, unsigned number_of_bins);

void display_space(list < Space > &spaces, list< Space >::iterator start, list< Space >::iterator end);
void display(list < Box > &packedBoxes, list< Box >::iterator start, list< Box >::iterator end);
 
int draw_spaces(list < Space > &spaces, unsigned bin_x, unsigned bin_y);