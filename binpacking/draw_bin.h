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
void finalize_allegro();
bool start_allegro(unsigned bin_x, unsigned bin_y);

void clear_display();

void draw_box(Box box, unsigned offset_x, unsigned offset_y);
void draw_boxes(list < Box > &packedBoxes, unsigned bin_number);
 
bool draw_solution(list < Box >& packed_boxes, vector< list<Space> >& Bins);
bool draw_bin(list < Box > &packed_boxes, list < Space > &empty_spaces, unsigned bin_number);