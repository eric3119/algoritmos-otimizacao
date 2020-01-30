#include "draw_bin.h"

const int LARGURA_TELA = 600;
const int ALTURA_TELA = 600;

int factor_x, factor_y;

ALLEGRO_DISPLAY *janela = NULL;

bool inicializar();

void draw_box(Box box){
    unsigned R, G, B, box_size;
    box_size = pow(box.X - box.x, 2) + pow(box.Y - box.y, 2);
        
    R = ((box_size * 3) % 100) + 100;
    G = ((box_size * 5) % 100) + 100;
    B = ((box_size * 7) % 100) + 100;
    unsigned x1 = box.x * factor_x;
    unsigned x2 = box.X * factor_x;
    unsigned y1 = ALTURA_TELA - (box.y * factor_y);
    unsigned y2 = ALTURA_TELA - (box.Y * factor_y);
    //std::cout << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
    // Retângulo preenchido: x1, y1, x2, y2, cor
    al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(R, G, B));
    al_flip_display();
    al_rest(1.0);
}

void display(std::list < Box > &packedBoxes, std::list< Box >::iterator start, std::list< Box >::iterator end){
    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    for (std::list< Box >::iterator it=start; it != end; ++it){
        draw_box(*it);
    }
}

bool compare_bin_number(const Box &b1, const Box &b2){
	return b1.bin_number < b2.bin_number;
}
bool compare_space_number(const Space &s1, const Space &s2){
	return s1.bin_number < s2.bin_number;
}

int draw_bin(std::list < Box > &packedBoxes, unsigned bin_x, unsigned bin_y){
    if (!inicializar()){
        return -1;
    }

    factor_x = LARGURA_TELA / bin_x;
    factor_y = ALTURA_TELA / bin_y;

    packedBoxes.sort(compare_bin_number);

    std::list< Box >::iterator it=packedBoxes.end();
    unsigned number_of_bins = (*it).bin_number;

    for (std::list< Box >::iterator it=packedBoxes.begin(); it != packedBoxes.end(); ++it){
        std::cout << (*it).x << " " << (*it).y << " " << (*it).X << " " << (*it).Y << " " << (*it).bin_number << " " << std::endl;
    }

    std::list< Box >::iterator start = packedBoxes.begin();
    for (std::list< Box >::iterator it=packedBoxes.begin(); it != packedBoxes.end(); ++it){

        if((*start).bin_number != (*it).bin_number){
            display(packedBoxes, start, it);
            al_rest(1.0);
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_flip_display();
            al_rest(0.5);
            start = it;
        }
    }

    display(packedBoxes, start, packedBoxes.end());
    al_flip_display();
    al_rest(2.0);

    while(true){}
 
 
    al_destroy_display(janela);
    
    return 0;
}

void draw_space(Space space){
    unsigned R, G, B, space_size, count;

    space_size = pow(space.X - space.x, 2) + pow(space.Y - space.y, 2);
        
    R = ((space_size * 3) % 100) + 100;
    G = ((space_size * 5) % 100) + 100;
    B = ((space_size * 7) % 100) + 100;
    unsigned x1 = space.x * factor_x;
    unsigned x2 = space.X * factor_x;
    unsigned y1 = ALTURA_TELA - (space.y * factor_y);
    unsigned y2 = ALTURA_TELA - (space.Y * factor_y);
    //std::cout << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
    // Retângulo: x1, y1, x2, y2, cor, borda
    al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgb(R, G, B));
    al_flip_display();
    al_rest(2.0);
}

void display_space(std::list < Space > &spaces, std::list< Space >::iterator start, std::list< Space >::iterator end){
    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    for (std::list< Space >::iterator it=start; it != end; ++it){
        draw_space(*it);
    }
}

int draw_space(std::list < Space > &spaces, unsigned bin_x, unsigned bin_y){
    if (!inicializar()){
        return -1;
    }

    spaces.sort(compare_space_number);

    factor_x = LARGURA_TELA / bin_x;
    factor_y = ALTURA_TELA / bin_y;

    std::list< Space >::iterator it=spaces.end();
    unsigned number_of_bins = (*it).bin_number;

    for (std::list< Space >::iterator it=spaces.begin(); it != spaces.end(); ++it){
        std::cout << (*it).x << " " << (*it).y << " " << (*it).X << " " << (*it).Y << " " << (*it).bin_number << " " << std::endl;
    }

    std::list< Space >::iterator start = spaces.begin();
    for (std::list< Space >::iterator it=spaces.begin(); it != spaces.end(); ++it){

        if((*start).bin_number != (*it).bin_number){
            display_space(spaces, start, it);
            al_rest(1.0);
            al_clear_to_color(al_map_rgb(0, 0, 0));
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
 
bool inicializar(){
    if (!al_init()){
        std::cout << "Falha ao inicializar a biblioteca Allegro.\n";
        return false;
    }
    
    if (!al_init_primitives_addon()){
        std::cout << "Falha ao inicializar add-on de primitivas.\n";
        return false;
    }
    
    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela){
        std::cout << "Falha ao criar janela.\n";
        return false;
    }
    
    al_set_window_title(janela, "Bin Packing");
    
    return true;
}