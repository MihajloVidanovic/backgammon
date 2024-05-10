#include "raylib.h"
#include <iostream>
#include <random>
#include <vector>

// Starting position
char board[28] = {0, 2, 0, 0, 0, 0, -5, 0, -3, 0, 0, 0, 5, 
                 -5, 0, 0, 0, 3, 0, 5, 0, 0, 0, 0, -2, 0, 0, 0};
                 // board[26] = white's end, board[27] = black's end
Rectangle dice_rects[6] = {
    (Rectangle){2, 42, 12, 12},
    (Rectangle){18, 42, 12, 12},
    (Rectangle){2, 58, 12, 12},
    (Rectangle){18, 58, 12, 12},
    (Rectangle){2, 74, 12, 12},
    (Rectangle){18, 74, 12, 12},
};

typedef struct Move {

} Move;

class Node {

    private:
        Node* parent;
        std::vector<Node> children;
        float eval;
        Move move;

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
    std::random_device rd;     // Only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0, 5); // Guaranteed unbiased

    *d1 = uni(rng);
    *d2 = uni(rng);
}

bool get_binary_digit(void* _ptr, int _pos) { // _pos in bits
    char* ptr = (char*)_ptr;
    char temp = ptr[_pos/8];
    return (temp >> (_pos % 8)) & 1;
}

int get_binary_number(void* _ptr, int _pos, int _size) { // _size and _pos in bits
    // work in progress
}

void write_binary(void* cho_gath, int size) {
    char* ptr = (char*)cho_gath;
    char temp;
    bool temp1[8];
    for(int i = 0; i < size; i++) {
        temp = ptr[size - i - 1];
        for(int j = 0; j < 8; j++) {
            temp1[j] = ((temp >> j) & 1);
        }
        for(int j = 0; j < 8; j++) {
            std::cout << temp1[7-j];
        }
    }
    std::cout << std::endl;
}

void draw_pieces(Texture2D sheet);

int main()
{
    srand(time(NULL));
    
    InitWindow(640, 512, "Backgammon");

    SetTargetFPS(60);
    
    Texture2D spritesheet = LoadTexture("assets/spritesheet.png");
    
    char dice = (char)0;
    int test = 5000;
    write_binary((void*)&test, sizeof(test));
    char selected = (char)0;
    
    int d1 = rand() % 6, d2 = rand() % 6;
    
    printf("%d\n", sizeof(Rectangle));
    
    while(!WindowShouldClose()) {
        
        if(IsMouseButtonPressed(0)) {
            int m_x = GetMouseX(), m_y = GetMouseY();
            if(m_x > 36 && m_x < 80 && m_y > 28 && m_y < 76) {
                // d1 = rand() % 6, d2 = rand() % 6; // roll dice
                roll_dice(&d1, &d2);
            }
            else if(m_x > 100 && m_x < 148 && m_y > 28 && m_y < 76) {
                // d1 = rand() % 6, d2 = rand() % 6; // roll dice
                roll_dice(&d1, &d2);
            }
            if(32 < m_x && m_x < 220 && 116 < m_y && m_y < 464) {
                if(m_y < 288) {
                    // 13-18
                    selected = (m_x - 32) / 32 + 13; // :thumbs_up:
                }
                else {
                    // 12-7
                }
            }
        }
        if(IsKeyPressed(KEY_SPACE)) {
            // d1 = rand() % 6, d2 = rand() % 6; // roll dice
            roll_dice(&d1, &d2);
        }
        
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            // background
            DrawTexturePro(spritesheet, (Rectangle){48, 0, 160, 128}, (Rectangle){0, 0, 640, 512}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
            
            DrawTexturePro(spritesheet, dice_rects[d1], (Rectangle){36, 28, 48, 48}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
            DrawTexturePro(spritesheet, dice_rects[d2], (Rectangle){100, 28, 48, 48}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
            
            // draw pieces
            draw_pieces(spritesheet);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

void draw_pieces(Texture2D sheet) {
    for(int i = 0; i <= 27; i++) {
        if(board[i] != 0) {
            switch(i) {
                case 0:
                    if(abs((int)board[i]) > 5) {
                        for(int j = 0; j < 5; j++) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)236, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                        for(int j = 0; j < abs(board[i]) - 5; j++) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)268, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                    }
                    else {
                        for(int j = 0; j < abs(board[i]); j++) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)236, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                    } // normal variation
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                    for(int j = 0; j < abs(board[i]); j++) {
                        if((int)board[i] > 0) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)(312 + abs(i - 6) * 32), (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                        else {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)(312 + abs(i - 6) * 32), (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                    }
                    break;
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                    for(int j = 0; j < abs(board[i]); j++) {
                        if((int)board[i] > 0) {
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
                    for(int j = 0; j < abs(board[i]); j++) {
                        if((int)board[i] > 0) {
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
                    for(int j = 0; j < abs(board[i]); j++) {
                        if((int)board[i] > 0) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)(312 + (i - 19) * 32), (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                        else {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)(312 + (i - 19) * 32), (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                    }
                    break;
                case 25:
                    if(abs((int)board[i]) > 5) {
                        for(int j = 0; j < 5; j++) {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)236, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                        for(int j = 0; j < abs(board[i]) - 5; j++) {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)268, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                    }
                    else {
                        for(int j = 0; j < abs(board[i]); j++) {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)236, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                    } // normal variation
                    break;
                case 26:
                    // 516 116
                    if(abs((int)board[i]) > 10) {
                        for(int j = 0; j < 5; j++) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)516, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)548, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                        for(int j = 0; j < abs(board[i]) - 10; j++) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)580, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                    }
                    else if(abs((int)board[i]) > 5) {
                        for(int j = 0; j < 5; j++) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)516, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                        for(int j = 0; j < abs(board[i]) - 5; j++) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)548, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                    } 
                    else {
                        for(int j = 0; j < abs(board[i]); j++) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)516, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                    } // normal variation
                    break;
                case 27:
                    // 516 116
                    if(abs((int)board[i]) > 10) {
                        for(int j = 0; j < 5; j++) {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)516, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)548, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                        for(int j = 0; j < abs(board[i]) - 10; j++) {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)580, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                    }
                    else if(abs((int)board[i]) > 5) {
                        for(int j = 0; j < 5; j++) {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)516, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                        for(int j = 0; j < abs(board[i]) - 5; j++) {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)548, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                    } 
                    else {
                        for(int j = 0; j < abs(board[i]); j++) {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)516, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                        }
                    } // normal variation
                    break;
            }
        }
    }
}
