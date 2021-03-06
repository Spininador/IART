#include "Board.h"
#include <iostream>
#include <cstdlib>
Board::Board() {
	board =
	{
		{EMPTY        ,EMPTY        ,EMPTY        ,EMPTY        ,BLACK | SRG    ,BLACK | CPT    ,BLACK | LIT    ,BLACK | GEN    },
		{EMPTY        ,EMPTY        ,EMPTY        ,EMPTY        ,BLACK | SLD    ,BLACK | SRG    ,BLACK | CPT    ,BLACK | LIT    },
		{EMPTY        ,EMPTY        ,EMPTY        ,EMPTY        ,BLACK | SLD    ,BLACK | SLD    ,BLACK | SRG    ,BLACK | CPT    },
		{EMPTY        ,EMPTY        ,EMPTY        ,EMPTY        ,BLACK | SLD    ,BLACK | SLD    ,BLACK | SLD    ,BLACK | SRG    },
		{WHITE | SRG    ,WHITE | SLD    ,WHITE | SLD    ,WHITE | SLD    ,EMPTY        ,EMPTY        ,EMPTY        ,EMPTY        },
		{WHITE | CPT    ,WHITE | SRG    ,WHITE | SLD    ,WHITE | SLD    ,EMPTY        ,EMPTY        ,EMPTY        ,EMPTY        },
		{WHITE | LIT    ,WHITE | CPT    ,WHITE | SRG    ,WHITE | SLD    ,EMPTY        ,EMPTY        ,EMPTY        ,EMPTY        },
		{WHITE | GEN    ,WHITE | LIT    ,WHITE | CPT    ,WHITE | SRG    ,EMPTY        ,EMPTY        ,EMPTY        ,EMPTY        }
	};

}


std::list<Move> Board::getAllMoves(Team team) {
	//List of all moves for the team
	std::list<Move> ret;
	//Team for bitwise cmp
	Piece pteam;
	switch (team)
	{
	case Black:
		pteam = BLACK;
		break;
	case White:
		pteam = WHITE;
		break;
	default:
		break;
	}
	//Get moves for all pieces in the board
	for (MCoord i = 0; i < this->board.size(); i++) {
		for (MCoord j = 0; j < this->board.size(); j++) {
			if (pteam & board[i][j]) {
				std::list<Move> moves = getPieceMoves(j, i);
				ret.splice(ret.end(),moves);
			}
		}
	}
	return ret;
}



std::list<Move> Board::getPieceMoves(MCoord x, MCoord y) {
	std::list<Move> ret;
	//Get piece at coords
	Piece p = board[y][x];
	//Checks if piece is at the trench
	bool atTrench = x == y;
	//Checks if piece is in it's territory(not used if in trench)
	bool atFriendlyTerritory = (p & BLACK && x > y) || (p & WHITE && x < y);
	//Length the piece can move
	MLength maxLength = Move::getMaxLength(p);
	for (MAngle angles = 0; angles < 8; angles++) {
	    //Run through all lengths.Note that when an allied piece is found or an enemy, no further lengths are tested because the piece can't move behind(except when at trench)
		for (MLength length = 1; length <= maxLength; length++) {
            //If move is legal
			if (Move::legalAngle(angles, p)) {
				//Get destiny coords
				MCoord ydest = y + Move::getAngleY(angles)*length;
				MCoord xdest = x + Move::getAngleX(angles)*length;
				//Check if outside boundaries
				if (ydest < 0 || xdest < 0 || ydest > 7 || xdest > 7) {
					continue;
				}
				//Get destiny
				Piece p2 = board[ydest][xdest];
				//If destiny is empty
				if (p2 & EMPTY) {
					Move move = Move(x, y, xdest, ydest);
					ret.push_back(move);
					continue;
				}
				//If destiny is friend
				else if (p & BLACK && p2 & BLACK || p & WHITE && p2 & WHITE) {
					break;
				}
				//If destiny is enemy
				else{
                    if(atTrench){
                        //If both target and origin are trench
                        if (xdest == ydest) {
                            break;
                        }
                        else if ((p & BLACK && xdest < ydest) || (p & WHITE && xdest > ydest)) {
                            Move move = Move(x, y, xdest, ydest);
                            ret.push_back(move);
                            //Continue pois pode comer mais pe�as na mesma linha caso esteja na trincheira
                            continue;
                        }
                        //If target is in enemy field and current in trench
                        else if ((p & BLACK && xdest < ydest) || (p & WHITE && xdest > ydest)) {
                            Move move = Move(x, y, xdest, ydest);
                            ret.push_back(move);
                            //Continue pois pode comer mais pe�as na mesma linha caso esteja na trincheira
                            continue;
                        }
                        //If target is in ally field and current in trench
                        else if ((p & BLACK && xdest > ydest) || (p & WHITE && xdest < ydest)) {
                            break;
                        }
                    }else{
                        if (xdest == ydest) {
                            if (atFriendlyTerritory) {
                                break;
                            }
                            else {
                                Move move = Move(x, y, xdest, ydest);
                                ret.push_back(move);
                                break;
                            }
                        }else
                        if (xdest != ydest) {
                            Move move = Move(x, y, xdest, ydest);
                            ret.push_back(move);
                            break;
                        }
                    }
				}
			}
		}
	}
	return ret;
}

Team Board::getGameEnded() {
    int totalBlack = 72;
    int totalWhite = 72;



    for(int i = 0;i < board.size();i++){
        for(int j = 0;j < board.size();j++){
            if(board[i][j] & BLACK){
                totalWhite -= getPieceValue(board[i][j]);
            }else if(board[i][j] & WHITE){
                totalBlack -= getPieceValue(board[i][j]);
            }
        }
    }
    if(totalBlack >= SCORE_TO_WIN){
        return Black;
    }else if(totalWhite >= SCORE_TO_WIN){
        return White;
    }else return None;
}

Board Board::movePiece(Move move) {
	Board b;
	b.board = this->board;
	b.board[move.getDY()][move.getDX()] = b.board[move.getSY()][move.getSX()];
	MCoord deltax,deltay;
	if((move.getDX() - move.getSX()) != 0)
        deltax = (move.getDX() - move.getSX())/(abs(move.getDX() - move.getSX()));
    else deltax = 0;
    if((move.getDY() - move.getSY()) != 0)
        deltay = (move.getDY() - move.getSY())/(abs(move.getDY() - move.getSY()));
    else deltay = 0;
	int sourcex = move.getSX();
	int sourcey = move.getSY();
	while(!(sourcex == move.getDX() && sourcey == move.getDY())){
        b.board[sourcey][sourcex] = EMPTY;
        sourcex += deltax;
        sourcey += deltay;
	}
	return b;
}

int Board::getTeamScore(Team t){
    int max = 72;
    Piece comp;
    if(t == White){
        comp = BLACK;
    }else comp = WHITE;

    for(int i = 0;i < board.size();i++){
        for(int j = 0;j < board.size();j++){
            if(board[i][j] & comp){
                max -= this->getPieceValue(board[i][j]);
            }
        }
    }
    return max;
}

SHeur Board::calculateScore() {
	SHeur ret = 0;
	//third and second heuristic
	int nwhitestrench = 0;
    int nblackstrench = 0;
    int nwhitesenemy = 0;
    int nblacksenemy = 0;
    for (int i = 0; i < board.size(); i++) {
		for (int j = 0; j < board.size(); j++) {
            if (board[i][j] & WHITE) {
                //Primeira heuristica (maximizar pe�as consumidas)
				ret += getPieceValue(board[i][j]);
				if(i == j){
                    nwhitestrench++;
				}else if(i < j){
                    nwhitesenemy++;
				}
			}
			else if (board[i][j] & BLACK) {
			    //Primeira heuristica (maximizar pe�as consumidas)
				ret -= getPieceValue(board[i][j]);
				if(i == j){
                    nblackstrench++;
				}else if(i > j){
                    nblacksenemy++;
				}
			}
		}
	}
	//Segunda heuristica ( maximizar pe�as na trench)
    ret += nwhitestrench - nblackstrench;
    //Terceira heuristica ( tentar equivaler numero de pe�as inimigas na trench e aliadas no territorio inimigo)
    ret += (nwhitesenemy >= nblackstrench)*nblackstrench - (nblacksenemy >= nwhitestrench)*nwhitestrench;
	return ret;
}

////////////////////////////// MOVE ///////////////////////////////

Move::Move(MCoord sx, MCoord sy, MCoord dx, MCoord dy) {
	this->startX = sx;
	this->startY = sy;
	this->destX = dx;
	this->destY = dy;
}

MCoord Move::getSX() {
	return this->startX;
}

MCoord Move::getSY() {
	return this->startY;
}

MCoord Move::getDX() {
	return this->destX;
}

MCoord Move::getDY() {
	return this->destY;
}

Piece Board::getPiece(MCoord y, MCoord x) {
	return this->board[y][x];
}

int Board::getPieceValue(Piece p) {
	Piece paux = p;
	for (int i = 1; i < 6; paux >>= 1,i++) {
		if (paux & 1) {
			return 2*i;
		}
	}
	return -1;
}

MCoord Move::getAngleX(MAngle angle) {
	if (angle == 0 || angle == 1 || angle == 7) {
		return 1;
	}
	if (angle == 2 || angle == 6) {
		return 0;
	}
	else return -1;
}

MCoord Move::getAngleY(MAngle angle) {
	if (angle == 1 || angle == 2 || angle == 3) {
		return 1;
	}
	if (angle == 0 || angle == 4) {
		return 0;
	}
	else return -1;
}

bool Move::legalAngle(MAngle angle, Piece p) {
	if (p & SLD) {
		if (angle % 2 == 0)
			return true;
		else return false;
	}
	else
		if (p & SRG) {
			if (p & BLACK) {
				if (angle == 7 || angle == 1 || angle == 5)
					return false;
				else return true;
			}
			else if (p & WHITE) {
				if (angle == 3 || angle == 1 || angle == 5)
					return false;
				else return true;
			}
		}
		else
			if (p & CPT) {
				if (angle == 1 || angle == 5)
					return false;
				else return true;
			}
			else
				if (p & LIT) {
					if (p & BLACK) {
						if (angle == 7)
							return false;
						else return true;
					}
					else if (p & WHITE) {
						if (angle == 3) {
							return false;
						}
						else return true;
					}
				}
				else return true;
}

MLength Move::getMaxLength(Piece p) {
	Piece paux = p;
	for (int i = 1; i < 6; paux >>= 1, i++) {
		if (paux & 1) {
			return i;
		}
	}
	return 0;
}
