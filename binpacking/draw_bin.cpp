#include "draw_bin.h"

const int LARGURA_TELA = 600;
const int ALTURA_TELA = 600;

int factor_x, factor_y;

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;

bool compare_bin_number(const Box &b1, const Box &b2){
	return b1.bin_number < b2.bin_number;
}
bool compare_space_number(const Space &s1, const Space &s2){
	return s1.bin_number < s2.bin_number;
}

bool inicializar();

void draw_box(Box box){
    unsigned R, G, B, box_size;
    box_size = pow(box.X - box.x, 2) + pow(box.Y - box.y, 2);
        
    R = ((box_size * 3) % 50) + 100;
    G = ((box_size * 5) % 50) + 100;
    B = ((box_size * 7) % 50) + 100;
    unsigned x1 = box.x * factor_x;
    unsigned x2 = box.X * factor_x;
    unsigned y1 = ALTURA_TELA - (box.y * factor_y);
    unsigned y2 = ALTURA_TELA - (box.Y * factor_y);
    // cout << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
    // Retângulo preenchido: x1, y1, x2, y2, cor
    al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(R, G, B));
    al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(0,0,0), 2.0);
    al_flip_display();
    al_rest(0.5);
}

void draw_boxes(list < Box > &packedBoxes, unsigned bin_number){

    for (list< Box >::iterator it=packedBoxes.begin(); it != packedBoxes.end(); ++it){

        if((*it).bin_number == bin_number){
            draw_box(*it);
        }
    }

    // display(packedBoxes, start, packedBoxes.end());
    // al_flip_display();
}

bool start_allegro(unsigned bin_x, unsigned bin_y){
    if (!inicializar()){
        return false;
    }else{
        factor_x = LARGURA_TELA / bin_x;
        factor_y = ALTURA_TELA / bin_y;

        return true;
    }
}

bool draw_bin(list < Box > &packed_boxes, list < Space > &empty_spaces, unsigned bin_number){

    bool exit = false;
    bool continue_draw = false;

    clear_display();
    draw_boxes(packed_boxes, bin_number);
    //draw_spaces(empty_spaces, bin_number);

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

void draw_space(Space space){
    unsigned R, G, B, space_size, count;

    space_size = pow(space.X - space.x, 2) + pow(space.Y - space.y, 2);
        
    R = 50;
    G = 50;
    B = 50;
    unsigned x1 = space.x * factor_x;
    unsigned x2 = space.X * factor_x;
    unsigned y1 = ALTURA_TELA - (space.y * factor_y);
    unsigned y2 = ALTURA_TELA - (space.Y * factor_y);
    //cout << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
    // Retângulo: x1, y1, x2, y2, cor, borda
    al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(R, G, B), 5.0);
    al_flip_display();
    al_rest(0.5);
}
void draw_spaces(list < Space > &spaces, unsigned bin_number){
    spaces.sort(compare_space_number);

    for (list< Space >::iterator it=spaces.begin(); it != spaces.end(); ++it){

        if((*it).bin_number == bin_number){
            draw_space(*it);
        }
    }

    // display(spaces, start, spaces.end());
    // al_flip_display();
}
void draw_wait(float time){
    al_rest(time);
}
void clear_display(){
    al_clear_to_color(al_map_rgb(255,255,255));
}

void display_space(list < Space > &spaces, list< Space >::iterator start, list< Space >::iterator end){
    al_clear_to_color(al_map_rgb(255,255,255));
    
    for (list< Space >::iterator it=start; it != end; ++it){
        draw_space(*it);
    }
}
void display(list < Box > &packedBoxes, list< Box >::iterator start, list< Box >::iterator end){
    al_clear_to_color(al_map_rgb(255,255,255));
    
    for (list< Box >::iterator it=start; it != end; ++it){
        draw_box(*it);
    }
}

int draw_spaces(list < Space > &spaces, unsigned bin_x, unsigned bin_y){
    if (!inicializar()){
        return -1;
    }

    spaces.sort(compare_space_number);

    factor_x = LARGURA_TELA / bin_x;
    factor_y = ALTURA_TELA / bin_y;

    list< Space >::iterator it=spaces.end();
    unsigned number_of_bins = (*it).bin_number;

    for (list< Space >::iterator it=spaces.begin(); it != spaces.end(); ++it){
        cout << (*it).x << " " << (*it).y << " " << (*it).X << " " << (*it).Y << " " << (*it).bin_number << " " << endl;
    }

    list< Space >::iterator start = spaces.begin();
    for (list< Space >::iterator it=spaces.begin(); it != spaces.end(); ++it){

        if((*start).bin_number != (*it).bin_number){
            display_space(spaces, start, it);
            al_rest(1.0);
            al_clear_to_color(al_map_rgb(255,255,255));
            al_flip_display();
            al_rest(0.5);
            start = it;
        }
    }

    display_space(spaces, start, spaces.end());
    al_flip_display();
    al_rest(5.0);

    al_destroy_display(janela);
    
    return 0;
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