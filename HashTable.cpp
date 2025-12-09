#include <iostream>
#include <cstring>
#include <cmath>
#include <cstdio>   // для snprintf

const unsigned int TABLE_SIZE = 67;

struct HashNode {
    unsigned int key;
    char* value;
    HashNode* next;
};

unsigned int djb2_hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % TABLE_SIZE;
}

unsigned int generate_key(int journal_number) {
    int exp = ((journal_number + 3) % 10) + 2;
    unsigned int power = 1u << exp;
    unsigned int candidate = power + 1;
    while (true) {
        bool is_prime = true;
        for (unsigned int i = 2; i * i <= candidate; ++i) {
            if (candidate % i == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) return candidate;
        candidate += (candidate % 2 == 0) ? 1 : 2;
    }
}

class HashTable {
private:
    HashNode* table[TABLE_SIZE];

public:
    HashTable() {
        for (int i = 0; i < TABLE_SIZE; ++i)
            table[i] = nullptr;
    }

    ~HashTable() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            HashNode* current = table[i];
            while (current) {
                HashNode* next = current->next;
                delete[] current->value;
                delete current;
                current = next;
            }
        }
    }

    bool insert(unsigned int key, const char* value_str) {
        char key_str[32];
        std::snprintf(key_str, sizeof(key_str), "u%u", key);  // замена sprintf_s на snprintf
        unsigned int index = djb2_hash(key_str);
        HashNode* current = table[index];

        while (current) {
            if (current->key == key) {
                delete[] current->value;
                current->value = new char[strlen(value_str) + 1];
                strcpy(current->value, value_str);
                return true;
            }
            current = current->next;
        }

        HashNode* new_node = new HashNode;
        new_node->key = key;
        new_node->value = new char[strlen(value_str) + 1];
        strcpy(new_node->value, value_str);
        new_node->next = table[index];
        table[index] = new_node;
        return true;
    }

    char* find(unsigned int key) {
        char key_str[32];
        std::snprintf(key_str, sizeof(key_str), "u%u", key);
        unsigned int index = djb2_hash(key_str);
        HashNode* current = table[index];
        while (current) {
            if (current->key == key)
                return current->value;
            current = current->next;
        }
        return nullptr;
    }

    bool remove(unsigned int key) {
        char key_str[32];
        std::snprintf(key_str, sizeof(key_str), "u%u", key);
        unsigned int index = djb2_hash(key_str);
        HashNode* current = table[index];
        HashNode* prev = nullptr;

        while (current) {
            if (current->key == key) {
                if (prev)
                    prev->next = current->next;
                else
                    table[index] = current->next;

                delete[] current->value;
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    void print() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            std::cout << "Bucket " << i << ": ";
            HashNode* current = table[i];
            while (current) {
                std::cout << "(" << current->key << ", \"" << current->value << "\") ";
                current = current->next;
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    HashTable ht;

    unsigned int keys[] = {67, 131, 257, 521, 1031};
    const char* values[] = {"Artemova", "data131", "data257", "data521", "data1031"};

    std::cout << "1. Insert test data:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        ht.insert(keys[i], values[i]);
        std::cout << "Inserted: key=" << keys[i] << ", value=\"" << values[i] << "\"" << std::endl;
    }
    ht.print();

    std::cout << "\n2. Search tests:" << std::endl;
    std::cout << "Find 257: \"" << (ht.find(257) ? ht.find(257) : "not found") << "\"" << std::endl;
    std::cout << "Find 999: \"" << (ht.find(999) ? ht.find(999) : "not found") << "\"" << std::endl;

    std::cout << "\n3. Update test:" << std::endl;
    ht.insert(257, "updated_data257");
    std::cout << "Updated 257: \"" << ht.find(257) << "\"" << std::endl;

    std::cout << "\n4. Delete test:" << std::endl;
    bool removed = ht.remove(131);
    std::cout << "Removed 131: " << (removed ? "yes" : "no") << std::endl;
    ht.print();

    return 0;
}
