#ifndef NODE_H
#define NODE_H

#include <string>
using namespace std;

class Node{
public:
    string word;
    unsigned int repeats;
    Node *left, *right;

    Node(const string &_word, const unsigned int _repeats);
    Node(Node *_left, Node *_right);
    Node(const string &_word, const unsigned int _repeats, Node *_left, Node *_right);
};

#endif //NODE_H
