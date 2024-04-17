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

int main()
{
    InitWindow(512, 512, "Backgammon");

    SetTargetFPS(60);
    
    Texture2D spritesheet = LoadTexture("assets/spritesheet.png");
    
    while(!WindowShouldClose()) {
        
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            DrawTexturePro(spritesheet, (Rectangle){48, 0, 128, 128}, (Rectangle){0, 0, 512, 512}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
