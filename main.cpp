#include "raylib.h"
#include <iostream>
#include <vector>

// Starting position
char board[24] = {2, 0, 0, 0, 0, -5, 0, -3, 0, 0, 0, 5, 
                 -5, 0, 0, 0, 3, 0, 5, 0, 0, 0, 0, -2};
Rectangle dice_rects[6] = {
    (Rectangle){2, 42, 12, 12},
    (Rectangle){18, 42, 12, 12},
    (Rectangle){2, 58, 12, 12},
    (Rectangle){18, 58, 12, 12},
    (Rectangle){2, 74, 12, 12},
    (Rectangle){18, 74, 12, 12},
};

class Node {

    private:
        Node* parent;
        std::vector<Node> children;
        float eval;

    public:
        Node() { }
        Node(Node* _parent) {
            parent = _parent;
        }
        
        int get_children_size() {
            return children.size();
        }
        Node get_child(int index) {
            return children.at(index);
        }
        void make_child() {
            children.resize(children.size() + 1); // new child always at end
            children[children.size()-1].parent = this;
        }
        void add_child(Node child) {
            children.push_back(child);
        }
        void remove_child(int index) {
            children.erase(children.begin() + index);
        }

        void set_parent(Node* _parent) {
            parent = _parent;
        }
        Node* get_parent() {
            return parent;
        }

        void set_eval(float _eval) {
            eval = _eval;
        }
        float get_eval() {
            return eval;
        }


};

void roll_dice(int* d1, int* d2) {
    do {
        *d1 = rand();
    } while(*d1 > 5);
    do {
        *d2 = rand();
    } while(*d2 > 5);
}

void write_binary(void* c, int size) {
    void* cho_gath = malloc(size);
    std::cout << std::endl;
    free(cho_gath);
}

void draw_pieces(Texture2D sheet);

int main()
{
    srand(time(NULL));
    
    InitWindow(512, 512, "Backgammon");

    SetTargetFPS(60);
    
    Texture2D spritesheet = LoadTexture("assets/spritesheet.png");
    
    char dice = (char)0;
    
    
    int d1 = rand() % 6, d2 = rand() % 6;
    
    while(!WindowShouldClose()) {
        
        if(IsMouseButtonPressed(0)) {
            int m_x = GetMouseX(), m_y = GetMouseY();
            if(m_x > 200 && m_x < 248 && m_y > 28 && m_y < 76) {
                // d1 = rand() % 6, d2 = rand() % 6; // roll dice
                roll_dice(&d1, &d2);
            }
            else if(m_x > 268 && m_x < 316 && m_y > 28 && m_y < 76) {
                // d1 = rand() % 6, d2 = rand() % 6; // roll dice
                roll_dice(&d1, &d2);
            }
        }
        if(IsKeyPressed(KEY_SPACE)) {
            // d1 = rand() % 6, d2 = rand() % 6; // roll dice
            roll_dice(&d1, &d2);
        }
        
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            // background
            DrawTexturePro(spritesheet, (Rectangle){48, 0, 128, 128}, (Rectangle){0, 0, 512, 512}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
            
            DrawTexturePro(spritesheet, dice_rects[d1], (Rectangle){200, 28, 48, 48}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
            DrawTexturePro(spritesheet, dice_rects[d2], (Rectangle){268, 28, 48, 48}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
            
            // draw pieces
            draw_pieces(spritesheet);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

void draw_pieces(Texture2D sheet) {
    for(int i = 1; i <= 24; i++) {
        if(board[i-1] != 0) {
            switch(i) {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                    for(int j = 0; j < abs(board[i-1]); j++) {
                        if((int)board[i-1] > 0) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)(288 + abs(i - 6) * 32), (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                        else {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)(288 + abs(i - 6) * 32), (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                    }
                    break;
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                    for(int j = 0; j < abs(board[i-1]); j++) {
                        if((int)board[i-1] > 0) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)(32 + abs(i - 12) * 32), (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                        else {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)(32 + abs(i - 12) * 32), (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                    }
                    break;
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                    for(int j = 0; j < abs(board[i-1]); j++) {
                        if((int)board[i-1] > 0) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)(32 + (i - 13) * 32), (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                        else {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)(32 + (i - 13) * 32), (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                    }
                    break;
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                    for(int j = 0; j < abs(board[i-1]); j++) {
                        if((int)board[i-1] > 0) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)(288 + (i - 19) * 32), (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                        else {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)(288 + (i - 19) * 32), (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                    }
                    break;
            }
        }
    }
}
