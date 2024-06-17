#include "raylib.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <chrono>
#include <map>

typedef enum Mode { 
    player_move = 0, computer_thinking = 1, computer_move = 2, title_screen = 3, paused = 4, first_dice_throw = 5
} Mode;

const Color bg_color = (Color){78, 40, 46, 255};

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

typedef struct Vector2i {
    int x;
    int y;
} Vector2i;

class Move {
    public:
        unsigned short data;
        // bits 0-3: starting position
        // bits 4-7: ending position
        // bit 8: whether a piece has been taken
        // bit 9: what player is making the move, 0 for black 1 for white
        
        void write_move(char start, char end, bool taken, bool player) {
            for(int i = 0; i < 4; i++) {
                data += start & 1;
                data <<= 1;
                start >>= 1;
            }
            for(int i = 0; i < 4; i++) {
                data += start & 1;
                data <<= 1;
                start >>= 1;
            }
            data += (taken) ? 1 : 0;
            data <<= 1;
            data += (player) ? 1 : 0;
        }
        void read_move(char* start, char* end, bool* taken, bool* player) {
            unsigned short tmp = data;
            *player = (tmp & 1) ? true : false;
            tmp >>= 1;
            *taken = (tmp & 1) ? true : false;
            tmp >>= 1;
            for(int i = 0; i < 4; i++) {
                *end += tmp & 1;
                *end <<= 1;
                tmp >>= 1;
            }
            for(int i = 0; i < 4; i++) {
                *start += tmp & 1;
                *start <<= 1;
                tmp >>= 1;
            }
        }
};

/*class Node {

    private:
        Node* parent;
        std::vector<Node*> children;
        std::vector<Move> moves;
        float eval;
        unsigned char dice_roll;
        unsigned char depth;

    public:
        Node() { }
        Node(Node* _parent) {
            parent = _parent;
        }
        
        int get_children_size() {
            return children.size();
        }
        Node* get_child(int index) {
            return children[index];
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

    AI has been scrapped for now, finish by the end of this year maybe.

};*/

int sign(char c) {
    if(c < 0) {
        return -1;
    }
    return 1;
}

bool check_move(char start, char end, bool player, int d) {
    if(start == (char)26 || start == (char)27 || end == (char)0 || end == (char)25) {
        return false; // cant start at end
    }
    if(start != (char)((player) ? 0 : 25) && board[((player) ? 0 : 25)] != 0) {
        return false;
    }
    if((int)end == ((player) ? 26 : 27)) {
        if((char)start - ((player) ? 0 : 23) == d + 1) {
            return true; // ngl trve....
        }
        else {
            // TODO: add bear off logic
            return false;
        }
    }
    if((int)end - (int)start != (d + 1) * ((player) ? 1 : -1)) {
        return false;
    }
    return true;
}

bool execute_move(char start, char end, bool player, int* d, int d_size) {
    if(board[(int)start] == 0) { return false; }
    
    // normal variation
    if((board[(int)end] == 0) || (sign(board[(int)end]) == ((player) ? 1 : -1))) {
        for(int i = 0; i < d_size; i++) {
            if(check_move(start, end, player, *(d + i)) && *(d + i) != -1) {
                board[(int)end] += (player) ? 1 : -1;
                board[(int)start] -= (player) ? 1 : -1;
                *(d + i) = -1;
                goto label3;
            }
        }
        return false;
        label3: ;
    }
    // there is an element at the end
    else if(sign(board[(int)end]) != ((player) ? 1 : -1)) {
        if(abs(board[(int)end]) > 1) {
            return false;
        }

        for(int i = 0; i < d_size; i++) {
            if(check_move(start, end, player, *(d + i)) && *(d + i) != -1) {
                if(sign(board[(int)end]) == -1) { // send to bar
                    board[25]--;
                }
                else { // 1 send to bar
                    board[0]++;
                }
                board[(int)end] = (player) ? 1 : -1;
                board[(int)start] -= (player) ? 1 : -1;
                *(d + i) = -1;
                goto label2;
            }
        }
        return false;
        label2: ;
    }
    return true;
}

void undo_move() {
    // TODO
}

void roll_dice(int* d, int* d_size) {
    std::random_device rd;     // Only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0, 5); // Guaranteed unbiased

    int d1 = uni(rng);
    int d2 = uni(rng);
    if(d1 == d2) {
        for(int i = 0; i < 4; i++) {
            *(d + i) = d1;
        }
        *d_size = 4;
    }
    else {
        *d = d1;
        *(d + 1) = d2;
        *(d + 2) = -1;
        *(d + 3) = -1;
        *d_size = 2;
    }
}

bool get_binary_digit(void* _ptr, int _pos) { // _pos in bits
    char* ptr = (char*)_ptr;
    char temp = ptr[_pos/8];
    return (temp >> (_pos % 8)) & 1;
}

int get_binary_number(void* _ptr, int _pos, int _size, bool _signed) { // _size and _pos in bits
    if(_signed) {
        long long intv = 0;
        for(int i = 0; i < _size; i++) { // TODO: make this faster
            intv <<= 1;
            intv += get_binary_digit(_ptr, _pos + _size - i - 1);
        }
        return intv;
    }
    else {
        unsigned long long intv = 0;
        for(int i = 0; i < _size; i++) { // TODO: make this faster
            intv <<= 1;
            intv += get_binary_digit(_ptr, _pos + _size - i - 1);
        }
        return intv;
    }
}

void write_binary(void* _ptr, int size) {
    char* ptr = (char*)_ptr;
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

/*

// initial call
minimax(Move m, 3, -127, 127, true)

// im gonna need static evaluation too

NewStruct minimax(Move m, char depth, double alpha, double beta, bool maximizingPlayer)
{
    if(depth == 0 || end_check()) {
        return static_eval();
    }

    if(maximizingPlayer) {
        char maxEval = -infinity
        for each child of position {
            eval = minimax(child, depth - 1, alpha, beta, false)
            maxEval = max(maxEval, eval)
            alpha = max(alpha, eval)
            if beta <= alpha {
                break;
            }
        }
        return maxEval
    }
    else {
        minEval = +infinity
        for each child of position {
            eval = minimax(child, depth - 1, alpha, beta, false)
            minEval = min(minEval, eval)
            beta = min(beta, eval)
            if beta <= alpha {
                break;
            }
        }
        return minEval
    }
}

*/

/*std::map<Move, char> get_possible_moves(int d1, int d2) {
    for(int i = 0; i < 26; i++) {
        if(sign(board[i]) == -1) {
            
        }
        else {
            
        }
    }
}*/

void draw_pieces(Texture2D sheet, char selected);

int main()
{
    srand(time(NULL));
    
    InitWindow(640, 512, "Backgammon");

    SetTargetFPS(60);
    
    Texture2D spritesheet = LoadTexture("assets/spritesheet.png");
    
    char selected = (char)0, p_selected = (char)-1;
    
    bool player_side = false;
    bool dice_rolled = false;
    
    std::string black = "Black to play", white = "White to play"; // 624
    
    int dice[4];
    int d_size;
    roll_dice(&dice[0], &d_size);
    int dd1 = dice[0], dd2 = dice[1];
    
    Mode m = player_move;
    
    while(!WindowShouldClose()) {
        if(m == player_move) {
            if(IsKeyPressed(KEY_SPACE) && !dice_rolled) {
                // d1 = rand() % 6, d2 = rand() % 6; // roll dice
                roll_dice(&dice[0], &d_size);
                    dd1 = dice[0]; dd2 = dice[1];
                dice_rolled = true;
            }
            if(IsMouseButtonPressed(0)) {
                int m_x = GetMouseX(), m_y = GetMouseY();
                if(m_x > 36 && m_x < 80 && m_y > 28 && m_y < 76 && !dice_rolled) {
                    // d1 = rand() % 6, d2 = rand() % 6; // roll dice
                    roll_dice(&dice[0], &d_size);
                    dd1 = dice[0]; dd2 = dice[1];
                    dice_rolled = true;
                }
                else if(m_x > 100 && m_x < 148 && m_y > 28 && m_y < 76 && !dice_rolled) {
                    // d1 = rand() % 6, d2 = rand() % 6; // roll dice
                    roll_dice(&dice[0], &d_size);
                    dd1 = dice[0]; dd2 = dice[1];
                    dice_rolled = true;
                }
                
                if(!dice_rolled) { goto label1; }
                
                // move selection
                if(p_selected != (char)-1) {
                    p_selected = selected;
                }
                // board selection
                if(32 < m_x && m_x < 220 && 116 < m_y && m_y < 464) {
                    if(m_y < 288) {
                        // 13-18
                        if((board[(m_x - 32) / 32 + 13] * ((player_side) ? 1 : -1) >= 0) || p_selected != (char)-1) {
                            selected = (m_x - 32) / 32 + 13; // :thumbs_up:
                        }
                    }
                    else {
                        // 12-7
                        if((board[12 - (m_x - 32) / 32] * ((player_side) ? 1 : -1) >= 0) || p_selected != (char)-1) {
                            selected = 12 - (m_x - 32) / 32;
                        }
                    }
                }
                else if(312 < m_x && m_x < 504 && 116 < m_y && m_y < 464) {
                    if(m_y < 288) {
                        if((board[19 + (m_x - 312) / 32] * ((player_side) ? 1 : -1) >= 0) || p_selected != (char)-1) {
                            selected = 19 + (m_x - 312) / 32;
                        }
                    }
                    else {
                        if((board[6 - (m_x - 312) / 32] * ((player_side) ? 1 : -1) >= 0) || p_selected != (char)-1) {
                            selected = 6 - (m_x - 312) / 32;
                        }
                    }
                }
                // middle of the board
                else if(236 <= m_x && 116 <= m_y && m_x <= 299 && m_y <= 275) {
                    // 0
                    selected = (char)0;
                }
                else if(236 <= m_x && 308 <= m_y && m_x <= 299 && m_y <= 467) {
                    // 25
                    selected = (char)25;
                }
                // the homes
                // else if()
                
                if(p_selected == selected && p_selected != (char)-1) { // clicked out of bounds
                    selected = (char)-1;
                }
                else if(p_selected != (char)-1 && p_selected != selected) { // move
                    if(execute_move(p_selected, selected, player_side, &dice[0], d_size)) {
                        for(int i = 0; i < 4; i++) {
                            if(dice[i] != -1) {
                                goto label4;
                            }
                        }
                        player_side = !player_side;
                        dice_rolled = false;
                        label4: ;
                    }
                    selected = (char)-1;
                }
                p_selected = selected;
            }
            label1:
            
            BeginDrawing();
            
                ClearBackground(RAYWHITE);
                
                // background
                DrawTexturePro(spritesheet, (Rectangle){48, 0, 160, 128}, (Rectangle){0, 0, 640, 512}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                
                // dice
                DrawTexturePro(spritesheet, dice_rects[dd1], (Rectangle){36, 28, 48, 48}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                DrawTexturePro(spritesheet, dice_rects[dd2], (Rectangle){100, 28, 48, 48}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                
                // arrow
                if(!dice_rolled) {
                    DrawTexturePro(spritesheet, (Rectangle){12, 86, 12, 10}, (Rectangle){168, 32, 48, 40}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); // 360, 32
                }
                
                // to move
                if(player_side) {
                    DrawText(white.c_str(), 600 - MeasureText(white.c_str(), 32), 35, 32, (Color){249, 245, 235, 255});
                }
                else {
                    DrawText(black.c_str(), 600 - MeasureText(black.c_str(), 32), 35, 32, (Color){75, 61, 107, 255});
                }
                
                // draw pieces
                draw_pieces(spritesheet, selected);
            
            EndDrawing();
        }
        else if(m == first_dice_throw) {
            
            BeginDrawing();
                
                ClearBackground(bg_color);
                
            EndDrawing();
        }
    }
    
    CloseWindow();
    return 0;
}

void draw_pieces(Texture2D sheet, char selected) {
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
                    for(int j = 0; j < std::min(abs(board[i]), 5); j++) {
                        if((int)board[i] > 0) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)(312 + abs(i - 6) * 32), (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                        else {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)(312 + abs(i - 6) * 32), (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                    }
                    if((int)selected == i && abs(board[i]) > 0) {
                        DrawRectangleLinesEx((Rectangle){(float)(312 + abs(6 - i) * 32), (float)(436 - (std::min(abs(board[i]), 5) - 1) * 32), 32, 32}, 4.0, (Color){255, 255, 0, 200});
                    }
                    break;
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                    for(int j = 0; j < std::min(abs(board[i]), 5); j++) {
                        if((int)board[i] > 0) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)(32 + abs(i - 12) * 32), (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                        else {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)(32 + abs(i - 12) * 32), (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                    }
                    if((int)selected == i && abs(board[i]) > 0) {
                        DrawRectangleLinesEx((Rectangle){(float)(32 + abs(12 - i) * 32), (float)(436 - (std::min(abs(board[i]), 5) - 1) * 32), 32, 32}, 4.0, (Color){255, 255, 0, 200});
                    }
                    break;
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                    for(int j = 0; j < std::min(abs(board[i]), 5); j++) {
                        if((int)board[i] > 0) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)(32 + (i - 13) * 32), (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                        else {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)(32 + (i - 13) * 32), (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                    }
                    if((int)selected == i && abs(board[i]) > 0) {
                        DrawRectangleLinesEx((Rectangle){(float)(32 + abs(i - 13) * 32), (float)(116 + (std::min(abs(board[i]), 5) - 1) * 32), 32, 32}, 4.0, (Color){255, 255, 0, 200});
                    }
                    break;
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                    for(int j = 0; j < std::min(abs(board[i]), 5); j++) {
                        if((int)board[i] > 0) {
                            DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)(312 + (i - 19) * 32), (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                        else {
                            DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)(312 + (i - 19) * 32), (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE); }
                    }
                    if((int)selected == i && abs(board[i]) > 0) {
                        DrawRectangleLinesEx((Rectangle){(float)(312 + abs(i - 19) * 32), (float)(116 + (std::min(abs(board[i]), 5) - 1) * 32), 32, 32}, 4.0, (Color){255, 255, 0, 200});
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
