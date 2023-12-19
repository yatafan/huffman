#include "../include/Node.h"

Node::Node(const string &_word, const unsigned int _repeats){
    word = _word;
    repeats = _repeats;
    left = nullptr;
    right = nullptr;
}

Node::Node(Node *_left, Node *_right){
    word = "";
    repeats = 0;
    left = _left;
    right = _right;
}

Node::Node(const string &_word, const unsigned int _repeats, Node *_left, Node *_right){
    word = _word;
    repeats = _repeats;
    left = _left;
    right = _right;
}