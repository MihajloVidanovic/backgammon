#include "raylib.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <set>

typedef enum Mode { 
    player_move = 0, computer_thinking = 1, computer_move = 2, title_screen = 3, paused = 4, first_dice_throw = 5, winner_w = 6, winner_b = 7
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
        // unsigned short data;
        // bits 0-4: starting position
        // bits 5-8: ending position
        // bit 9: whether a piece has been taken
        // bit 10: what player is making the move, 0 for black 1 for white
        
        char start, end, dice;
        bool taken, player;
        
        // constructors
        Move() { }
        Move(char _start, char _end, bool _taken, bool _player, char _dice) {
            start = _start;
            end = _end;
            taken = _taken;
            player = _player;
            dice = _dice;
        }
        void write_move(char _start, char _end, bool _taken, bool _player, char _dice) {
            start = _start;
            end = _end;
            taken = _taken;
            player = _player;
            dice = _dice;
        }
        void read_move(char* _start, char* _end, bool* _taken, bool* _player, char* _dice) {
            *_start = start;
            *_end = end;
            *_taken = taken;
            *_player = player;
            *_dice = dice;
        }
        bool operator==(Move move1) {
            char start1, start2, end1, end2, dice1, dice2;
            bool taken1, taken2, player1, player2;
            this->read_move(&start1, &end1, &taken1, &player1, &dice1);
            move1.read_move(&start2, &end2, &taken2, &player2, &dice2);
            if(start1 == start2 && end1 == end2 && taken1 == taken2 && player1 == player2 && dice1 == dice2) {
                return true;
            }
            return false;
        }
        /*void write_move(char start, char end, bool taken, bool player) {
            for(int i = 0; i < 5; i++) {
                data += start & 1;
                data <<= 1;
                start >>= 1;
            }
            for(int i = 0; i < 5; i++) {
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
            for(int i = 0; i < 5; i++) {
                *end += tmp & 1;
                *end <<= 1;
                tmp >>= 1;
            }
            for(int i = 0; i < 5; i++) {
                *start += tmp & 1;
                *start <<= 1;
                tmp >>= 1;
            }
        }
        unsigned short* data_ptr() {
            return &data;
        }
        bool operator==(Move move1) {
            char start1, start2, end1, end2;
            bool taken1, taken2, player1, player2;
            this->read_move(&start1, &end1, &taken1, &player1);
            move1.read_move(&start2, &end2, &taken2, &player2);
            if(start1 == start2 && end1 == end2 && taken1 == taken2 && player1 == player2) {
                return true;
            }
            return false;
        }*/
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
    else if(c > 0) {
        return 1;
    }
    return 2;
}

bool check_move(char start, char end, bool player, std::vector<Move>& available_moves, int dice) {
    Move move1(start, end, board[(int)end] == ((player) ? -1 : 1), player, dice);
    for(int i = 0; i < (int)available_moves.size(); i++) {
        if(available_moves[i] == move1) {
            return true;
        }
    }
    return false;
}

bool execute_move(char start, char end, bool player, int* d, int d_size, std::vector<Move>& available_moves) {
    if(board[(int)start] == 0) { return false; }
    
    // normal variation
    if((board[(int)end] == 0) || (sign(board[(int)end]) == ((player) ? 1 : -1))) {
        for(int i = 0; i < d_size; i++) {
            if(check_move(start, end, player, available_moves, (*(d + i))) && *(d + i) != -1) {
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
            if(check_move(start, end, player, available_moves, *(d + i))) {
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
    std::uniform_int_distribution<int> uni(1, 6); // Guaranteed unbiased

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

bool check_for_bearoff(bool player) {
    int sum = board[((player) ? 26 : 27)];
    for(int i = ((player) ? 19 : 1); i <= ((player) ? 24 : 6); i++) {
        if(sign(board[i]) == ((player) ? 1 : -1)) {
            sum += board[i];
        }
    }
    if(abs(sum) == 15) {
        return true;
    }
    return false;
}

char check_for_win() {
    if(board[26] == 15) {
        return '1';
    }
    else if(board[27] == -15) {
        return '2';
    }
    return '0';
}

std::vector<Move> get_possible_moves(int* d, int d_size, bool player) {
    std::vector<Move> return_vector;
    Move move1;
    for(int i = 0; i < 26; i++) {
        if(sign(board[i]) == ((player) ? 1 : -1) && board[i] != 0) {
            for(int j = 0; j < 4; j++) {

                // general board
                if(*(d + j) == -1 || !(i - ((player) ? -1 : 1) * (*(d + j)) > 0 && i - ((player) ? -1 : 1) * (*(d + j)) < 25)) {
                    // do nothing
                }
                else if(board[((player) ? 0 : 25)] != 0 && i != ((player) ? 0 : 25)) {
                    // do nothing, again
                }
                else if(board[i - ((player) ? -1 : 1) * (*(d + j))] == 0 || sign(board[i - ((player) ? -1 : 1) * (*(d + j))]) == ((player) ? 1 : -1) || abs(board[i - ((player) ? -1 : 1) * (*(d + j))]) == 1) {
                    move1.write_move((char)i, (char)(i - ((player) ? -1 : 1) * (*(d + j))), (((sign(board[i - ((player) ? -1 : 1) * (*(d + j))])) != ((player) ? 1 : -1) && abs(board[i - ((player) ? -1 : 1) * (*(d + j))]) == 1) ? true : false), player, *(d + j));
                    return_vector.push_back(move1);
                }
            }
        }
    }
    
    // home row
    for(int i = 0; i < 4; i++) { 
        if(sign(board[((player) ? -1 : 1) * (*(d + i)) + ((player) ? 25 : 0)]) ==  ((player) ? 1 : -1) && check_for_bearoff(player) && *(d + i) > 0) { 
            move1.write_move(((player) ? -1 : 1) * (*(d + i)) + ((player) ? 25 : 0), ((player) ? 26 : 27), false, player, *(d + i));
            return_vector.push_back(move1);
        }
    }
    return return_vector;
}

void draw_pieces(Texture2D sheet, char selected);

int main()
{
    srand(time(NULL));
    
    InitWindow(640, 512, "Backgammon");

    SetTargetFPS(60);
    
    Texture2D spritesheet = LoadTexture("assets/spritesheet.png");
    
    char selected = (char)-1, p_selected = (char)-1;
    
    bool player_side = false;
    bool dice_rolled = false;
    
    std::string black = "Black to play", white = "White to play"; // 624
    
    int dice[4];
    int d_size;
    roll_dice(&dice[0], &d_size);
    int dd1 = dice[0] - 1, dd2 = dice[1] - 1;
    
    char start, end, m_dice;
    bool taken, player;
    
    Mode m = player_move;
    
    std::vector<Move> available_moves = get_possible_moves(&dice[0], d_size, player_side);
    for(int i = 0; i < available_moves.size(); i++) {
        available_moves[i].read_move(&start, &end, &taken, &player, &m_dice);
        std::cout << (int)start << ' ' << (int)end << ' ' << taken << ' ' << player << ' ' << (int)m_dice << std::endl;
    }
    std::cout << std::endl;
    
    while(!WindowShouldClose()) {
        if(m == player_move) {
            if(IsKeyPressed(KEY_SPACE) && !dice_rolled) {
                // d1 = rand() % 6, d2 = rand() % 6; // roll dice
                roll_dice(&dice[0], &d_size);
                dd1 = dice[0] - 1; dd2 = dice[1] - 1;
                dice_rolled = true;
                available_moves = get_possible_moves(&dice[0], d_size, player_side);
                for(int i = 0; i < available_moves.size(); i++) {
                    available_moves[i].read_move(&start, &end, &taken, &player, &m_dice);
                    std::cout << (int)start << ' ' << (int)end << ' ' << taken << ' ' << player << ' ' << (int)m_dice << std::endl;
                }
                std::cout << std::endl;
            }
            if(IsMouseButtonPressed(0)) {
                int m_x = GetMouseX(), m_y = GetMouseY();
                if(m_x > 36 && m_x < 80 && m_y > 28 && m_y < 76 && !dice_rolled) {
                    // d1 = rand() % 6, d2 = rand() % 6; // roll dice
                    roll_dice(&dice[0], &d_size);
                    dd1 = dice[0] - 1; dd2 = dice[1] - 1;
                    dice_rolled = true;
                    available_moves = get_possible_moves(&dice[0], d_size, player_side);
                    for(int i = 0; i < available_moves.size(); i++) {
                        available_moves[i].read_move(&start, &end, &taken, &player, &m_dice);
                        std::cout << (int)start << ' ' << (int)end << ' ' << taken << ' ' << player << ' ' << (int)m_dice << std::endl;
                    }
                    std::cout << std::endl;
                }
                else if(m_x > 100 && m_x < 148 && m_y > 28 && m_y < 76 && !dice_rolled) {
                    // d1 = rand() % 6, d2 = rand() % 6; // roll dice
                    roll_dice(&dice[0], &d_size);
                    dd1 = dice[0] - 1; dd2 = dice[1] - 1;
                    dice_rolled = true;
                    available_moves = get_possible_moves(&dice[0], d_size, player_side);
                    for(int i = 0; i < available_moves.size(); i++) {
                        available_moves[i].read_move(&start, &end, &taken, &player, &m_dice);
                        std::cout << (int)start << ' ' << (int)end << ' ' << taken << ' ' << player << ' ' << (int)m_dice << std::endl;
                    }
                    std::cout << std::endl;
                }
                
                if(!dice_rolled) { goto label1; }
                
                // move selection
                if(available_moves.size() == 0) {
                    player_side = !player_side;
                    dice_rolled = false;
                }
                if(p_selected != (char)-1) {
                    p_selected = selected;
                }
                // board selection
                if(32 < m_x && m_x < 220 && 116 < m_y && m_y < 464) {
                    if(m_y < 288) {
                        // 13-18
                        if((board[(m_x - 32) / 32 + 13] * ((player_side) ? 1 : -1) >= 0) || p_selected != (char)-1) {
                            selected = (m_x - 32) / 32 + 13;
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
                else if(516 <= m_x && 116 <= m_y && m_x <= 608 && m_y <= 272) {
                    selected = (char)26;
                }
                else if(516 <= m_x && 308 <= m_y && m_x <= 608 && m_y <= 464) {
                    selected = (char)27;
                }
                
                if(p_selected == selected && p_selected != (char)-1) { // clicked out of bounds
                    selected = (char)-1;
                }
                else if(p_selected != (char)-1 && p_selected != selected) { // move
                    if(execute_move(p_selected, selected, player_side, &dice[0], d_size, available_moves)) {
                        available_moves = get_possible_moves(&dice[0], d_size, player_side);
                        for(int i = 0; i < available_moves.size(); i++) {
                            available_moves[i].read_move(&start, &end, &taken, &player, &m_dice);
                            std::cout << (int)start << ' ' << (int)end << ' ' << taken << ' ' << player << ' ' << (int)m_dice << std::endl;
                        }
                        for(int i = 0; i < 4; i++) {
                            std::cout << *(dice + i) << ' ';
                        }
                        std::cout << std::endl;
                        if(available_moves.size() == 0) {
                            player_side = !player_side;
                            dice_rolled = false;
                            goto label4;
                        }
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
            
            if(check_for_win() == '1') {
                m = winner_w;
            }
            else if(check_for_win() == '2') {
                m = winner_b;
            }
            
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
    
    UnloadTexture(spritesheet);
    
    CloseWindow();
    return 0;
}

void draw_pieces(Texture2D sheet, char selected) {
    for(int i = 0; i <= 27; i++) {
        if(board[i] != 0) {
            switch(i) {
                case 0:
                    for(int j = 0; j < std::min(5, abs(board[i])); j++) {
                        DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)236, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                    }
                    for(int j = 0; j < std::min(5, abs(board[i]) - 5); j++) {
                        DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)268, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                    }
                    if((int)selected == i && abs(board[i]) > 0) {
                        DrawRectangleLinesEx((Rectangle){(float)((abs(board[i]) > 5) ? 268 : 236), (float)(116 + ((abs(board[i]) - 1) % 5) * 32), 32, 32}, 4.0, (Color){255, 255, 0, 200});
                    }
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
                    for(int j = 0; j < std::min(5, abs(board[i])); j++) {
                        DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)236, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                    }
                    for(int j = 0; j < std::min(5, abs(board[i]) - 5); j++) {
                        DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)268, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                    }
                    if((int)selected == i && abs(board[i]) > 0) {
                        DrawRectangleLinesEx((Rectangle){(float)((abs(board[i]) > 5) ? 268 : 236), (float)(436 - ((abs(board[i]) - 1) % 5) * 32), 32, 32}, 4.0, (Color){255, 255, 0, 200});
                    }
                    break;
                case 26:
                    // 516 116
                    for(int j = 0; j < std::min(5, abs(board[i])); j++) {
                        DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)516, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                    }
                    for(int j = 0; j < std::min(5, abs(board[i]) - 5); j++) {
                        DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)548, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                    }
                    for(int j = 0; j < std::min(5, abs(board[i]) - 10); j++) {
                        DrawTexturePro(sheet, (Rectangle){40, 104, 8, 8}, (Rectangle){(float)580, (float)(116 + j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                    }
                    break;
                case 27:
                    // 516 116
                    for(int j = 0; j < std::min(5, abs(board[i])); j++) {
                        DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)516, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                    }
                    for(int j = 0; j < std::min(5, abs(board[i]) - 5); j++) {
                        DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)548, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                    }
                    for(int j = 0; j < std::min(5, abs(board[i]) - 10); j++) {
                        DrawTexturePro(sheet, (Rectangle){32, 104, 8, 8}, (Rectangle){(float)580, (float)(436 - j * 32), 32, 32}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
                    }
                    break;
            }
        }
    }
}
