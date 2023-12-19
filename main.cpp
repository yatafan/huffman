#include <iostream>
#include <fstream>
#include <bitset>
#include <set>
#include <queue>
#include <vector>
#include <cmath>

#include "include/Node.h"

#define BYTE_SIZE 8

//#define DEBUG

struct words_pair {
    string key;
    string value;
};

struct compress_data {
    float compression_ratio;
    float compression_with_dict;
    float shannon_entropy;
    float b_entropy;
};

//Логарифм с произвольным основанием
double logbase(const double a, const double base)
{
    return log(a) / log(base);
}

//Функция читает файл и возвращает вектор байт
vector<char> read_file(const char *filename);

//Функиця превращает вектор байт в строку битов
string to_binary_string(const vector<char> &bytes);

//Функция разбивает строку на слова по длине входного слова
vector<string> get_words(string &str, unsigned int word_size);

//Функция возвращает вектор уникальных слов
vector<string> get_unique_words(const vector<string> &words);

//Функиця считает количество повторов каждого уникального слова в строке
vector<unsigned int> get_repeats(const vector<string> &words, const vector<string> &unique_words);

//Функция строит бинарное дерево и возвращает указатель на его корень
Node *build_tree(const vector<string> &words, const vector<unsigned int> &repeats);

//Функция выводит таблицу частотности
void show_frequency_table(const vector<string> &words, const vector<unsigned int> &repeats);

//Функция инициализирует словарь
void init_dictionary(const Node *root, const string& value, vector<words_pair> &dict);

//Функция выводит словарь
void show_dict(const vector<words_pair> &dict);

//Преобразовывает строку из 8 битов в байт
char str_to_byte(const string &byte_str);

//Сжатие со словарем
string encode(const vector<string> &words, const vector<words_pair> &dict);

//Записывает строку битов в файл
void save_in_file(string &bits, const char *filename);

struct comp
{
    bool operator()(const Node *l, const Node *r) const {
        return l->repeats > r->repeats;
    }
};

//Функция выполняет сжатие по Хаффману
string compress(const char *filename, unsigned int word_size, compress_data &data);

//Функция считает энтропию Шеннона
float shannon_entropy(const vector<unsigned int> &repeats);

//Функция считает B экнтропию
float b_entropy(const vector<unsigned int> &repeats);

int main(int argc, char **argv){
    unsigned int word_size; //Длина входного слова

#ifdef DEBUG
    word_size = 8;
#else
    word_size = stoi(argv[argc - 1]);
    //cout << "ну ты нефор" << endl;
#endif
    compress_data data;
    string result = compress("input.txt", word_size, data);
    save_in_file(result, "output.txt");

    vector<compress_data> graphics_data;
    for(int i = 1; i <= word_size; i++) {
        compress("input.txt", i, data);
        graphics_data.push_back(data);
    }

    ofstream graphics_file("data.txt");

    for(auto i : graphics_data) {
        graphics_file << i.compression_ratio << ' ';
    }
    graphics_file << endl;

    for(auto i : graphics_data) {
        graphics_file << i.compression_with_dict << ' ';
    }
    graphics_file << endl;

    for(auto i : graphics_data) {
        graphics_file << i.shannon_entropy << ' ';
    }
    graphics_file << endl;

    for(auto i : graphics_data) {
        graphics_file << i.b_entropy << ' ';
    }
    graphics_file << endl;

    graphics_file.close();

    return 0;
}

vector<char> read_file(const char *filename){
    ifstream input_file(filename);

    vector<char> bytes;

    char buf;
    while(!input_file.eof()){
        buf = input_file.get();
        bytes.push_back(buf);
    }
    bytes.pop_back();

    input_file.close();
    return bytes;
}

string to_binary_string(const vector<char> &bytes){
    string bin_str;
    bitset<BYTE_SIZE> buf;

    for(const auto i : bytes){
        buf = i;
        bin_str += buf.to_string();
    }

    return bin_str;
}

vector<string> get_words(string &str, const unsigned int word_size){
    vector<string> words;
    const unsigned int zeros = word_size - (str.size() % word_size);

    for(unsigned int i = 0; i < zeros; i++){
        str += '0';
    }

    for(int i = 0; i < str.size(); i += word_size){
        words.push_back(str.substr(i, word_size));
    }

    return words;
}

vector<string> get_unique_words(const vector<string> &words){
    set<string> unique_words;
    for(auto i : words){
        unique_words.insert(i);
    }

    vector<string> unique_words_vector;
    for(auto i : unique_words){
        unique_words_vector.push_back(i);
    }

    return unique_words_vector;
}

vector<unsigned int> get_repeats(const vector<string> &words, const vector<string> &unique_words){
    vector<unsigned int> repeats;

    for(int i = 0; i < unique_words.size(); i++){
        repeats.push_back(0);
    }

    for(int i = 0; i < unique_words.size(); i++){
        for(auto j : words){
            if(unique_words[i] == j){
                repeats[i]++;
            }
        }
    }

    return repeats;
}

Node *build_tree(const vector<string> &words, const vector<unsigned int> &repeats){
    priority_queue<Node*, vector<Node*>, comp> tree;

    for(int i = 0; i < words.size(); i++){
        auto node = new Node(words[i], repeats[i]);
        tree.push(node);
    }

    while (tree.size() != 1)
    {
        Node *left = tree.top(); tree.pop();
        Node *right = tree.top();	tree.pop();

        const int sum = left->repeats + right->repeats;
        auto node = new Node("", sum, left, right);
        tree.push(node);
    }

    Node* root = tree.top();

    return root;
}

void show_frequency_table(const vector<string> &words, const vector<unsigned int> &repeats) {
    for(int i = 0; i < words.size(); i++) {
        cout << words[i] << '\t' << repeats[i] << endl;
    }
}

auto init_dictionary(const Node* root, const string& value, vector<words_pair>& dict) -> void {
    if(root->left == nullptr && root->right == nullptr) {
        dict.push_back({root->word, value});
    }

    if(root->right != nullptr) {
        init_dictionary(root->right, value + '1', dict);
    }
    if(root->left != nullptr) {
        init_dictionary(root->left, value + '0', dict);
    }
}

bool compare_nodes(const Node &first, const Node &second) {
    return first.repeats > second.repeats;
}

void show_dict(const vector<words_pair> &dict) {
    for(auto [key, value] : dict) {
        cout << key << ' ' << value << endl;
    }
}

char str_to_byte(const string &byte_str) {
    bitset<BYTE_SIZE> byte = 0;

    for(int i = 0; i < byte_str.size(); i++) {
        if(byte_str[i] == '1') {
            byte[BYTE_SIZE - i - 1] = 1;
        }
    }

    return byte.to_ulong();
}

string encode(const vector<string> &words, const vector<words_pair> &dict) {
    string result;
    for(auto word : words) {
        for(auto [key, value] : dict) {
            if(key == word) {
                result += value;
                break;
            }
        }
    }

    return result;
}

void save_in_file(string &bits, const char *filename) {
    const unsigned int zeros = BYTE_SIZE - (bits.size() % BYTE_SIZE);
    for(int i = 0; i < zeros; i++) {
        bits += '0';
    }

    ofstream output_file(filename, ios::binary);

    for(int i = 0; i < bits.size(); i += BYTE_SIZE) {
        string byte_str = bits.substr(i, BYTE_SIZE);
        char buf = str_to_byte(byte_str);
        output_file.write(&buf, 1);
    }

    output_file.close();
}

string compress(const char *filename, const unsigned int word_size, compress_data &data) {
    const vector<char> bytes = read_file(filename);
    string bits = to_binary_string(bytes);
    const vector<string> words = get_words(bits, word_size);
    vector<string> unique_words = get_unique_words(words);
    vector<unsigned int> repeats = get_repeats(words, unique_words); //Количество повторов уникальных слов в строке

    //Сортировка по возрастанию частотности
    for (int i = 0; i < repeats.size() - 1; i++) {
        for (int j = 0; j < repeats.size() - i - 1; j++) {
            if (repeats[j] > repeats[j + 1]) {
                const unsigned int temp_repeats = repeats[j];
                repeats[j] = repeats[j + 1];
                repeats[j + 1] = temp_repeats;

                const string temp_word = unique_words[j];
                unique_words[j] = unique_words[j + 1];
                unique_words[j + 1] = temp_word;
            }
        }
    }

    const Node *root = build_tree(unique_words, repeats);

    vector<words_pair> dictionary;
    init_dictionary(root, "", dictionary);

    string result = encode(words, dictionary);

    string result_with_dict;

    const bitset<BYTE_SIZE> byte = word_size;
    result_with_dict += byte.to_string();
    for(const auto [key, value] : dictionary) {
        result_with_dict += key;
        result_with_dict += value;
    }
    result_with_dict += result;

    data.compression_ratio = static_cast<float>(bytes.size()) / (static_cast<float>(result.size() / BYTE_SIZE));
    data.compression_with_dict = static_cast<float>(bytes.size()) / (static_cast<float>(result_with_dict.size() / BYTE_SIZE));
    data.shannon_entropy = shannon_entropy(repeats);
    data.b_entropy = b_entropy(repeats);

    return result;
}

float shannon_entropy(const vector<unsigned int> &repeats) {
    float entropy = 0;
    int sum = 0;

    for(const auto i : repeats) {
        sum += i;
    }

    for(const auto pi : repeats) {
        const float freq = static_cast<float>(pi) / static_cast<float>(sum);
        entropy += freq * logbase(freq, 2);
    }

    return -1 * entropy;
}

float b_entropy(const vector<unsigned int> &repeats) {
    float b = 0;
    int sum = 0;

    for(const auto i : repeats) {
        sum += i;
    }

    for(int i = 0; i < repeats.size(); i++) {
        float coefficient = 0;
        for(int j = 0; j < repeats.size(); j++) {
            coefficient += (1.0 - abs(((static_cast<float>(repeats[i]) - static_cast<float>(repeats[j])) / sum) * (static_cast<float>(repeats[j]) / sum)));
        }
        b += ((static_cast<float>(repeats[i]) / sum) * logbase(coefficient, 2));
    }

    return (b/sum);
}