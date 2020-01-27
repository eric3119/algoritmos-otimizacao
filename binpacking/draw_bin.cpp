#include "draw_bin.h"

const int LARGURA_TELA = 800;
const int ALTURA_TELA = 800;

int factor_x, factor_y;

ALLEGRO_DISPLAY *janela = NULL;

bool inicializar();

void display(std::list < Box > &packedBoxes, std::list< Box >::iterator start, std::list< Box >::iterator end){
    al_clear_to_color(al_map_rgb(0, 0, 0));

    unsigned R, G, B, box_size, count;
    
    for (std::list< Box >::iterator it=start; it != end; ++it){
        box_size = pow((*it).X - (*it).x, 2) + pow((*it).Y - (*it).y, 2);
        
        R = ((box_size * 3) % 100) + 100;
        G = ((box_size * 5) % 100) + 100;
        B = ((box_size * 7) % 100) + 100;
        // Retângulo preenchido: x1, y1, x2, y2, cor
        al_draw_filled_rectangle((*it).x * factor_x, (*it).y * factor_y, (*it).X * factor_x, (*it).Y * factor_y, al_map_rgb(R, G, B));
        al_flip_display();
        al_rest(1.0);
    }
}

bool compare(const Box &b1, const Box &b2){
	return b1.bin_number < b2.bin_number;
}

int draw_bin(std::list < Box > &packedBoxes, unsigned bin_x, unsigned bin_y){
    if (!inicializar()){
        return -1;
    }

    factor_x = LARGURA_TELA / bin_x;
    factor_y = ALTURA_TELA / bin_y;

    packedBoxes.sort(compare);

    std::list< Box >::iterator it=packedBoxes.end();
    unsigned number_of_bins = (*it).bin_number;

    for (std::list< Box >::iterator it=packedBoxes.begin(); it != packedBoxes.end(); ++it){
        std::cout << (*it).x << " " << (*it).y << " " << (*it).X << " " << (*it).Y << " " << (*it).bin_number << " " << std::endl;
    }

    std::list< Box >::iterator start = packedBoxes.begin();
    for (std::list< Box >::iterator it=packedBoxes.begin(); it != packedBoxes.end(); ++it){

        if((*start).bin_number != (*it).bin_number){
            display(packedBoxes, start, it);
            al_flip_display();
            al_rest(2.0);
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