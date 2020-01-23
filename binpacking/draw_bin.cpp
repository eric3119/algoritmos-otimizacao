#include "draw_bin.h"

const int LARGURA_TELA = 640;
const int ALTURA_TELA = 480;
 
ALLEGRO_DISPLAY *janela = NULL;

bool inicializar();

void display(){
    al_clear_to_color(al_map_rgb(0, 0, 0));
 
    // Retângulo preenchido: x1, y1, x2, y2, cor
    al_draw_filled_rectangle(10.0, 10.0, 50.0, 50.0, al_map_rgb(0, 255, 255));
}
 
int draw_bin(std::list < Box > &packedBoxes){
    if (!inicializar()){
        return -1;
    }
  
    while(true){
        display();
        al_flip_display();
    }
 
 
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