#include "raylib.h"
#include <iostream>
#include <vector>

// Starting position
char board[24] = {2, 0, 0, 0, 0, -5, 0, -3, 0, 0, 0, 5, 
                            -5, 0, 0, 0, 3, 0, 5, 0, 0, 0, 0, -2};

// starts at 32 116

class Node {

    private: // radi paradigme, mada bih ja licno ovo ostavio public
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

void draw_pieces(Texture2D sheet) {
    for(int i = 1; i <= 24; i++) {
        if(board[i-1] != 0) {
            switch(i-1) {
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                    // 288 486 y mora biti oduzet
                    
                    break;
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                    // 32 486 y mora biti oduzet
                    
                    break;
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                    // 32, 116
                    for(int j = 0; j < board[i-1]; j++) {
                        if(board[i-1] > 0) {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)(32 + (i - 13) * 32), 116 + j * 32, 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                        else {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)(32 + (i - 13) * 32), 116 + j * 32, 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                    }
                    break;
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                    // 288, 116
                    
                    break;
            }
        }
    }
}

int main()
{
    InitWindow(512, 512, "Backgammon");

    SetTargetFPS(60);
    
    Texture2D spritesheet = LoadTexture("assets/spritesheet.png");
    
    while(!WindowShouldClose()) {
        
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            // background
            DrawTexturePro(spritesheet, (Rectangle){48, 0, 128, 128}, (Rectangle){0, 0, 512, 512}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
            
            // draw pieces
            draw_pieces(spritesheet);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
