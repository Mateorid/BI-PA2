#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <string>
#include <queue>

using namespace std;
#endif /* __PROGTEST__ */

struct binTree {
    uint16_t data{};
    binTree *left = nullptr;
    binTree *right = nullptr;
};

void deleteTree(binTree *leaf) { //Almost perfectly deletes the binary tree
    if (leaf->right != nullptr) {
        deleteTree(leaf->right);
        return;
    }
    if (leaf->left != nullptr) {
        deleteTree(leaf->left);
        return;
    }
    delete leaf;
}

bool makeQueue(const char *inFileName, queue<uint16_t> &bits) { //Makes a binary queue from the input file
    ifstream inputFile(inFileName, ios::binary | ios::in);
    char c;

    while (inputFile.get(c).good()) {
        if (!inputFile.is_open()) {
            inputFile.close();
            return false;
        }
        for (int i = 7; i >= 0; i--) {
            if (((c >> i) & 1)) // 1
                bits.push(1);
            else if (!((c >> i) & 1)) // 0
                bits.push(0);
            else {
                inputFile.close();
                return false;
            }
        }
    }
    inputFile.close();
    return true;
}

bool buildTree(queue<uint16_t> &bits, binTree *strom) { //Builds the Huffman tree
    if (bits.empty())
        return false;

    if (bits.front() == 0) {
        strom->left = new binTree();
        strom->right = new binTree();
        bits.pop();
        buildTree(bits, strom->left);
        buildTree(bits, strom->right);
    } else if (bits.front() == 1) {
        bits.pop();
        for (int i = 7; i >= 0; i--) {
            strom->data <<= 1;
            strom->data |= bits.front();
            bits.pop();
        }
    } else
        return false;
    return true;
}

bool toDecimal(queue<uint16_t> &qq, uint32_t &decimal) { //Converts binary queue sequence to decimal uint32_t
    decimal = 0;
    if (qq.size() < 12) {
        return false;
    }
    for (int i = 11; i >= 0; i--) {
        decimal += (1 << i) * qq.front();
        qq.pop();
    }
    return true;
}

bool findChar(char &c, binTree *root, queue<uint16_t> &bits) { //Finds char from binary tree
    if ((root->right == nullptr) && (root->left == nullptr)) {
        c = root->data;
        return true;
    }

    if (bits.empty())
        return false;
    if (bits.front() == 1) {
        bits.pop();
        return findChar(c, root->right, bits);
    } else if (bits.front() == 0) {
        bits.pop();
        return findChar(c, root->left, bits);
    } else
        return false;
}

bool getChunk(queue<uint16_t> &bits, uint32_t &chunk, bool &lastChunk) { //Gets the number of bits to scan
    if (bits.empty())
        return false;
    int current = bits.front();
    bits.pop();

    if (current == 1) {
        chunk = 4096;
        lastChunk = false;
    } else if (current == 0) {
        if (!toDecimal(bits, chunk))
            return false;
        lastChunk = true;
    } else
        return false;
    return true;
}

bool decompressFile(const char *inFileName, const char *outFileName) {
    queue<uint16_t> bits;
    binTree strom;
    uint32_t chunks = 0;
    char c;

    //making queue & binary tree
    if (!makeQueue(inFileName, bits))
        return false;
    if (!buildTree(bits, &strom))
        return false;

    //outputting to file
    ofstream output(outFileName, ios::binary | ios::out | ios::trunc);
    if (!output.is_open()) {
        deleteTree(&strom);
        return false;
    }

    bool lastChunk = false;
    while (output.good()) {
        if (!output.good()) { //Error
            deleteTree(&strom);
            output.close();
            return false;
        }

        if (!getChunk(bits, chunks, lastChunk)) { //Error
            deleteTree(&strom);
            output.close();
            return false;
        }

        for (uint32_t i = 0; i < chunks; ++i) {
            if (!findChar(c, &strom, bits)) { //Error
                deleteTree(&strom);
                output.close();
                return false;
            }
            output.put(c);
        }

        if (lastChunk)
            break;
    }

    if (!output.good()) { //Error
        deleteTree(&strom);
        output.close();
        return false;
    }

    deleteTree(&strom);
    output.close();
    return true;
}

bool compressFile(const char *inFileName, const char *outFileName) {
    // keep this dummy implementation (no bonus) or implement the compression (bonus)
    return false;
}

#ifndef __PROGTEST__

bool identicalFiles(const char *fileName1, const char *fileName2) {
    return false;
}

int main() {
    return 0;
}

#endif /* __PROGTEST__ */
