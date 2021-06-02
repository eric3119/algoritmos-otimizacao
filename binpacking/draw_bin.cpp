#include "draw_bin.h"

const int LARGURA_TELA = 700;
const int ALTURA_TELA = 700;

int factor_x, factor_y;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;

bool compare_bin_number(const Box &b1, const Box &b2){
	return b1.bin_number < b2.bin_number;
}

bool compare_space_number(const Space &s1, const Space &s2){
	return s1.bin_number < s2.bin_number;
}

void draw_box(Box box, unsigned offset_x, unsigned offset_y){
    unsigned R, G, B, box_size, x1, x2, y1, y2;

    box_size = pow(box.X - box.x, 2) + pow(box.Y - box.y, 2);

    R = ((box_size * 3) % 50) + 100;
    G = ((box_size * 5) % 50) + 100;
    B = ((box_size * 7) % 50) + 100;

    x1 = offset_x + box.x * factor_x;
    x2 = offset_x + box.X * factor_x;
    y1 = offset_y - box.y * factor_y;
    y2 = offset_y - box.Y * factor_y;

    al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(R, G, B));
    al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(0,0,0), 1.0);
    al_flip_display();
    al_rest(0.5);
}

void draw_boxes(list < Box > &packedBoxes, unsigned bin_number){
    for (list< Box >::iterator it=packedBoxes.begin(); it != packedBoxes.end(); ++it){
        if((*it).bin_number == bin_number)
            draw_box(*it, 10 * it->bin_number, ALTURA_TELA / 2 + 10 * it->bin_number);
    }
}

bool start_allegro(unsigned bin_x, unsigned bin_y){
    if (!inicializar()){
        return false;
    }else{
        factor_x = LARGURA_TELA / (2 * bin_x);
        factor_y = ALTURA_TELA / (2 * bin_y);

        return true;
    }
}

bool draw_solution(list < Box >& packed_boxes, vector< list<Space> >& Bins) {
    bool ret = true;
    for (int i = 0; i < Bins.size(); ++i) {
        ret = draw_bin(packed_boxes, Bins[i], i + 1);
        if (!ret) break;
    }
    finalize_allegro();
    return ret;
}

bool draw_bin(list < Box > &packed_boxes, list < Space > &empty_spaces, unsigned bin_number){

    bool exit = false;
    bool continue_draw = false;

    clear_display();
    draw_boxes(packed_boxes, bin_number);

    while(!exit){
        while(!al_is_event_queue_empty(event_queue)){
            ALLEGRO_EVENT evento;
            al_wait_for_event(event_queue, &evento);

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                switch (evento.keyboard.keycode)
                {
                    case ALLEGRO_KEY_SPACE:
                        continue_draw = true;
                        exit = true;
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        exit = true;
                        break;
                }
            }
            else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                exit = true;
            }
        }
    }

    return continue_draw;
}

void clear_display(){
    al_clear_to_color(al_map_rgb(255,255,255));
}

void finalize_allegro(){
    al_destroy_display(janela);
}
 
bool inicializar(){
    if (!al_init()){
        cout << "Falha ao inicializar a biblioteca Allegro.\n";
        return false;
    }
    
    if (!al_init_primitives_addon()){
        cout << "Falha ao inicializar add-on de primitivas.\n";
        return false;
    }
    
    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela){
        cout << "Falha ao criar janela.\n";
        return false;
    }
    
    al_set_window_title(janela, "Bin Packing");
    
    if (!al_install_keyboard()){
        cout << "Falha ao inicializar o keyboard.\n";
        return false;
    }

    event_queue = al_create_event_queue();
    
    if (!event_queue){
        cout << "Falha ao criar fila de eventos.\n";
        al_destroy_display(janela);
        return false;
    }

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(janela));
    
    return true;
}