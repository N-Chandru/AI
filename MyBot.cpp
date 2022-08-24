/*
* @file botTemplate.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-02-04
* Template for users to create their own bots
*/

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include<bits/stdc++.h>
using namespace std;
using namespace Desdemona;

#define DOUBLE_MAX 1e18
#define DOUBLE_MIN -1e18
#define K_PLY 6 // 6 levels

clock_t start, finish; //used to tackle the 2 sec limit for each move

class MyBot : public OthelloPlayer
{
public:
    /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
    MyBot(Turn turn);

    /**
         * Play something 
         */
    virtual Move play(const OthelloBoard &board);

private:
        int stableDiscsFromCorner(vector<int> board, vector<int> corners,
        int color);
    double get_beta_value(const OthelloBoard &board, int level, double alpha, double beta);
    double get_alpha_value(const OthelloBoard &board, int level, double alpha, double beta);
    double heuristic_function(const OthelloBoard &board, Turn turn);
    double heuristic(const OthelloBoard &board, Turn turn);
    double playerPotentialMobility(vector<int> board, int color);
};

MyBot::MyBot(Turn turn)
    : OthelloPlayer(turn)
{
}
double MyBot::heuristic_function(const OthelloBoard &board, Turn turn)
{

      Turn my_colour = turn;
    Turn opp_colour = other(turn);
    Turn board_colour;
    int color=(BLACK == my_colour) ? 1 : ((RED == my_colour) ? -1 : 0);
    //index 1 = colour of max player ;index = 0 means colour of min player; index = -1 means empty position at board
    int my_front_tiles = 0, opp_front_tiles = 0, x, y, index; 
    int parameter_value[] = {0, 0}; // at position 0 for min player the parameter is stored ; at position 1 the max_plapyer paramter is stored
    double parity = 0.0, corner_occupancy = 0.0, corner_closeness = 0.0, mobility = 0.0, f = 0.0, stability = 0.0;
    // all the direction next to a certain co-ordinate in the board
    int direction_x[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int direction_y[] = {0, 1, 1, 1, 0, -1, -1, -1};
    //weights associated with each position on the board
   
    vector<int> oboard;
    //only static stability is taken into account not the potential stability.
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            board_colour = board.get(i, j);
            //index = (board_colour == my_colour) ? 1 : ((board_colour == opp_colour) ? 0 : -1);
            oboard.push_back((BLACK == board_colour) ? 1 : ((RED == board_colour) ? -1 : 0));
            
        }


    //parity feature
    int blacks=board.getBlackCount();
    int reds=board.getRedCount();
    int remaining= 64-blacks-reds;
    parity=(remaining%2==0)? -1:1;
    int discsOnBoard=blacks+reds;
    
    // square weights
     vector<int> weights = {
         200, -100, 100,  50,  50, 100, -100,  200,
        -100, -200, -50, -50, -50, -50, -200, -100,
         100,  -50, 100,   0,   0, 100,  -50,  100,
          50,  -50,   0,   0,   0,   0,  -50,   50,
          50,  -50,   0,   0,   0,   0,  -50,   50,
         100,  -50, 100,   0,   0, 100,  -50,  100,
        -100, -200, -50, -50, -50, -50, -200, -100,
         200, -100, 100,  50,  50, 100, -100,  200,
    };
    
      if (oboard[0] != 0) {
        weights[1]  = 0;
        weights[2]  = 0;
        weights[3]  = 0;
        weights[8]  = 0;
        weights[9]  = 0;
        weights[10] = 0;
        weights[11] = 0;
        weights[16] = 0;
        weights[17] = 0;
        weights[18] = 0;
        weights[24] = 0;
        weights[25] = 0;
    }

    if (oboard[7] != 0) {
        weights[4]  = 0;
        weights[5]  = 0;
        weights[6]  = 0;
        weights[12] = 0;
        weights[13] = 0;
        weights[14] = 0;
        weights[15] = 0;
        weights[21] = 0;
        weights[22] = 0;
        weights[23] = 0;
        weights[30] = 0;
        weights[31] = 0;
    }

    if (oboard[56] != 0) {
        weights[32] = 0;
        weights[33] = 0;
        weights[40] = 0;
        weights[41] = 0;
        weights[42] = 0;
        weights[48] = 0;
        weights[49] = 0;
        weights[50] = 0;
        weights[51] = 0;
        weights[57] = 0;
        weights[58] = 0;
        weights[59] = 0;
    }

    if (oboard[63] != 0) {
        weights[38] = 0;
        weights[39] = 0;
        weights[45] = 0;
        weights[46] = 0;
        weights[47] = 0;
        weights[52] = 0;
        weights[53] = 0;
        weights[54] = 0;
        weights[55] = 0;
        weights[60] = 0;
        weights[61] = 0;
        weights[62] = 0;
    }
    double squareweights=0.0;
    if (color == 1) {
        squareweights=std::inner_product(oboard.begin(),
                oboard.end(), weights.begin(), 0);
    }
    else {
        squareweights=-1*std::inner_product(oboard.begin(),
                oboard.end(), weights.begin(), 0);
    }
    
    //corners
    std::vector<int> corners = {0, 7, 56, 63};
    int blackCorners = 0;
    int whiteCorners = 0;

    for (int corner : corners) {
        if (oboard[corner] == 1) {
            blackCorners++;
        }
        else if (oboard[corner] == -1) {
            whiteCorners++;
        }
    }
    double corner;
    if (color == 1) {
        corner= 100 * (blackCorners - whiteCorners)
            / (blackCorners + whiteCorners + 1);
    }
    else {
        corner= 100 * (whiteCorners - blackCorners)
            / (blackCorners + whiteCorners + 1);
    }
    
    // disk difference
    double diskdifference=0.0;
    if (color == 1) {
        diskdifference= 100 * (blacks - reds) / (blacks + reds);
    }
    else {
        diskdifference=100 *  (reds-blacks) / (blacks + reds);
    }
    
    //utility
    int util = std::accumulate(oboard.begin(),
            oboard.end(), 0);

    if (color == 1) {
        return util; 
    }
    else {
        return -1*util;
    }
    
     //corner-occupancy feature

    //co-ordinate of the corners
    int corners_x[] = {0, 0, 7, 7};
    int corners_y[] = {0, 7, 0, 7};

    parameter_value[0] = parameter_value[1] = 0;
    for (int i = 0; i < 4; i++)
    {
        board_colour = board.get(corners_x[i], corners_y[i]);
        index = (board_colour == my_colour) ? 1 : ((board_colour == opp_colour) ? 0 : -1);
        if (index == 0 || index == 1)
            parameter_value[index]++;
    }
    //percent corner taken by the players
    corner_occupancy = 25 * (parameter_value[1] - parameter_value[0]);


    //corner close-ness feature

    parameter_value[0] = parameter_value[1] = 0;
    //co-ordinate of the positions closer to the corners
    int close_to_corners_x[] = {0, 1, 1, 0, 1, 1, 6, 7, 6, 7, 6, 6};
    int close_to_corners_y[] = {1, 0, 1, 6, 7, 6, 0, 1, 1, 6, 7, 6};

    for (int i = 0; i < 4; i++)
    {
        int j = i * 3;
        board_colour = board.get(corners_x[i], corners_y[i]);
        index = (board_colour == my_colour) ? 1 : ((board_colour == opp_colour) ? 0 : -1);
        if (index != 0 && index != 1)
        {
            for (; j < i * 3 + 3; j++)
            {
                board_colour = board.get(close_to_corners_x[j], close_to_corners_y[j]);
                index = (board_colour == my_colour) ? 1 : ((board_colour == opp_colour) ? 0 : -1);
                if (index == 0 || index == 1)
                    parameter_value[index]++;
            }
        }
    }
    stability = -12.5 * (parameter_value[1] - parameter_value[0]);
    parameter_value[0] = parameter_value[1] = 0;
    
    
    //stability
    
    vector<int> cc={0,7,56,63};
    int mystables=stableDiscsFromCorner(oboard,cc,color);
    int oppstables=stableDiscsFromCorner(oboard,cc,-color);
    int stability1=mystables-oppstables;
    
    
    
    if (discsOnBoard > 58) {
    return 100*diskdifference
            + 100*parity
            + 350*stability +100*stability1 + (850.724 * corner_occupancy) ;
            }    
            
            
    //potential mobility
    int myPotentialMobility = playerPotentialMobility(oboard, color);
    int opponentPotentialMobility = playerPotentialMobility(oboard, -color);
    double potentialMobility=0.0;
     potentialMobility= 100 * (myPotentialMobility - opponentPotentialMobility)
        / (myPotentialMobility + opponentPotentialMobility + 1);
     
     
     //mobility
        list<Move> my_moves = board.getValidMoves(turn);
    list<Move> opp_moves = board.getValidMoves(other(turn));
    parameter_value[1] = my_moves.size();
    parameter_value[0] = opp_moves.size();
    if (parameter_value[1] + parameter_value[0] != 0)
    {
        //percentage mobility
        mobility = 100 * (parameter_value[1] - parameter_value[0]) / (parameter_value[1] + parameter_value[0]);
    }


        if (discsOnBoard <= 20) {
        // Opening game
        return 5*mobility
            + 5*potentialMobility
            + 20*squareweights
            + 350*stability +100*stability1 +(850.724 * corner_occupancy);
    }
    else if (discsOnBoard <= 58) {
        // Midgame
        return 10*diskdifference
            + 2*mobility
            + 2*potentialMobility
            + 10*squareweights
            + 100*parity
            + 350*stability +100*stability1 + (850.724 * corner_occupancy);
            }




}
int MyBot::stableDiscsFromCorner(vector<int> board, vector<int> corners,
        int color) {
 vector<int> sd;
 for(int corner: corners){
    bool down, right;
    if (corner == 0) {
        down = true;
        right = true;
    }
    else if (corner == 7) {
        down = true;
        right = false;
    }
    else if (corner == 56) {
        down = false;
        right = true;
    }
    else {
        down = false;
        right = false;
    }

    int horizIncr = 1, horizStop = 7, vertIncr = 8, vertStop = 56;
    if (!right) {
        horizIncr *= -1;
        horizStop *= -1;
    }
    if (!down) {
        vertIncr *= -1;
        vertStop *= -1;
    }

    // Iterate horizontally
    for (int i = corner; i != corner + horizIncr + horizStop; i += horizIncr) {
        // If there is a disc of our color on this square
        if (board[i] == color) {
            // Iterate vertically
            for (int j = i; j != i + vertStop; j+= vertIncr) {
                // If there is a disc of our color on this square,
                // and it is not in the set of stable discs
                if (board[j] == color
                        && find(sd.begin(),sd.end(),j) == sd.end()) {
                    // Insert it to the set
                     sd.push_back(j);
                }
                // If we come across a disc of the opposite color, break
                else {
                    break;
                }
            }
        }
        // Otherwise, break
        else {
            break;
        }
    }
  }
  return sd.size();
}





double MyBot::playerPotentialMobility(vector<int> board, int color) {
    std::vector<int> boardInterior = {18, 19, 20, 21,
                                      26, 27, 28, 29,
                                      34, 35, 36, 37,
                                      42, 43, 44, 45};

    int here = 0, up = 0, down = 0, left = 0, right = 0,
        upperLeft = 0, upperRight = 0, lowerLeft = 0, lowerRight = 0;
    int potentialMobility = 0;

    for (int square : boardInterior) {
        here = board[square];
        up = board[square-8];
        down = board[square+8];
        left = board[square-1];
        right = board[square+1];
        upperLeft = board[square-9];
        upperRight = board[square-7];
        lowerLeft = board[square+7];
        lowerRight = board[square+9];

        if (here == -color && up == 0)
            potentialMobility++;
        if (here == -color && down == 0)
            potentialMobility++;
        if (here == -color && right == 0)
            potentialMobility++;
        if (here == -color && right == 0)
            potentialMobility++;
        if (here == -color && upperLeft == 0)
            potentialMobility++;
        if (here == -color && upperRight == 0)
            potentialMobility++;
        if (here == -color && lowerLeft == 0)
            potentialMobility++;
        if (here == -color && lowerRight == 0)
            potentialMobility++;
    }

    std::vector<int> topRow = {10, 11, 12, 13};
    for (int square : topRow) {
        here = board[square];
        left = board[square-1];
        right = board[square+1];
        if (here == -color && left == 0)
            potentialMobility++;
        if (here == -color && right == 0)
            potentialMobility++;
    }

    std::vector<int> bottomRow = {50, 51, 52, 53};
    for (int square : bottomRow) {
        here = board[square];
        left = board[square-1];
        right = board[square+1];
        if (here == -color && left == 0)
            potentialMobility++;
        if (here == -color && right == 0)
            potentialMobility++;
    }

    std::vector<int> leftColumn = {17, 25, 33, 41};
    for (int square : leftColumn) {
        here = board[square];
        up = board[square-8];
        down = board[square+8];
        if (here == -color && up == 0)
            potentialMobility++;
        if (here == -color && down == 0)
            potentialMobility++;
    }

    std::vector<int> rightColumn = {22, 30, 38, 46};
    for (int square : leftColumn) {
        here = board[square];
        up = board[square-8];
        down = board[square+8];
        if (here == -color && up == 0)
            potentialMobility++;
        if (here == -color && down == 0)
            potentialMobility++;
    }

    return potentialMobility;
}
Move MyBot::play(const OthelloBoard &board)
{
    start = clock();
    int level = 0;
    list<Move> moves = board.getValidMoves(turn);
    list<Move>::iterator it = moves.begin();
    double max_value = DOUBLE_MIN;
    double beta_value;
    Move next_move = *it;
    for (; it != moves.end(); it++)
    {   
        //in test board the moves are made and based on the board position 
        //the state of the board is calculated and if the state is better than the the best found till then
        //the move is selected as the best move
        OthelloBoard test_board = board;
        test_board.makeMove(turn, *it);
        beta_value = get_beta_value(test_board, level + 1, max_value, DOUBLE_MAX);
        //comparision between  the value obatained and the max value
        if (beta_value > max_value)
        {
            max_value = beta_value;
            next_move = *it;
        }
    }
    return next_move;
}
double MyBot::get_beta_value(const OthelloBoard &board, int level, double alpha, double beta)
{   
    // to avoid the time limit threshold
    finish = clock();
    if (((double)(finish - start) / CLOCKS_PER_SEC) > 1.97)
    {
        return DOUBLE_MIN;
    }

    Turn otherTurn = other(turn);
    double beta_value = DOUBLE_MAX;
    double alpha_value;
    list<Move> moves = board.getValidMoves(otherTurn);
    list<Move>::iterator it;
    for (it = moves.begin(); it != moves.end(); it++)
    {
        OthelloBoard test_board = board;
        test_board.makeMove(otherTurn, *it);
        alpha_value = get_alpha_value(test_board, level + 1, alpha, beta);
        if (alpha_value < beta_value)
        {
            beta_value = alpha_value;
            if (beta_value < beta)
            {
                beta = beta_value;
            }
            //alpha pruning 
            if (alpha >= beta)
            {
                return beta_value;
            }
        }
    }
    return beta_value;
}

double MyBot::get_alpha_value(const OthelloBoard &board, int level, double alpha, double beta)
{
     // to avoid the time limit threshold
    finish = clock();
    if (((double)(finish - start) / CLOCKS_PER_SEC) > 1.97)
    {
        return DOUBLE_MAX;
    }

    if (level == K_PLY)
        return heuristic_function(board, turn);

    double alpha_value = DOUBLE_MIN;
    double beta_value;
    list<Move> moves = board.getValidMoves(turn);
    list<Move>::iterator it;
    for (it = moves.begin(); it != moves.end(); it++)
    {
        OthelloBoard test_board = board;
        test_board.makeMove(turn, *it);
        beta_value = get_beta_value(test_board, level + 1, alpha, beta);

        if (beta_value > alpha_value)
        {
            alpha_value = beta_value;
            if (alpha_value > alpha)
            {
                alpha = alpha_value;
            }
            //beta pruning
            if (alpha >= beta)
            {
                return alpha_value;
            }
        }
    }
    return alpha_value;
}


// The following lines are _very_ important to create a bot module for Desdemona

extern "C"
{
    OthelloPlayer *createBot(Turn turn)
    {
        return new MyBot(turn);
    }

    void destroyBot(OthelloPlayer *bot)
    {
        delete bot;
    }
}
