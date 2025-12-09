#include <iostream>
#include <cstring>
#include <cmath>
#include <cstdio>  // for snprintf (cross-platform sprintf replacement)

// Journal number sequence starts from 1: 1,2,3,4,5,...
// TABLE_SIZE uses formula: next prime after 2^((journal+3)%10 + 2)
// For journal=1: 2^((1+3)%10 +2) = 2^(4+2) = 2^6 = 64, next prime=67
const unsigned int TABLE_SIZE = 67;

// HashNode structure for chaining collision resolution
struct HashNode {
    unsigned int key;      // unsigned int key
    char* value;           // pointer to string value
    HashNode* next;        // next node in chain
    
    HashNode(unsigned int k, const char* v) : key(k), next(nullptr) {
        value = new char[strlen(v) + 1];
        strcpy(value, v);
    }
    
    ~HashNode() {
        delete[] value;
    }
};

class HashTable {
private:
    HashNode** table;  // array of bucket pointers
    
public:
    HashTable() {
        table = new HashNode*[TABLE_SIZE];
        for (unsigned int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }
    
    ~HashTable() {
        for (unsigned int i = 0; i < TABLE_SIZE; i++) {
            HashNode* current = table[i];
            while (current != nullptr) {
                HashNode* next = current->next;
                delete current;
                current = next;
            }
        }
        delete[] table;
    }
    
    // djb2 hash function for strings (recommended from presentation)
    unsigned int djb2_hash(const char* str) {
        unsigned int hash = 5381;
        int c;
        while ((c = *str++)) {
            hash = ((hash << 5) + hash) + c;  // hash * 33 + c
        }
        return hash;
    }
    
    // Convert unsigned int key to string and hash it
    unsigned int get_hash_index(unsigned int key) {
        char key_str[32];
        snprintf(key_str, sizeof(key_str), "u%u", key);  // cross-platform safe sprintf
        unsigned int hash = djb2_hash(key_str);
        return hash % TABLE_SIZE;
    }
    
    // Insert or update key-value pair
    bool insert(unsigned int key, const char* value) {
        unsigned int index = get_hash_index(key);
        HashNode* current = table[index];
        
        // Check if key exists (update value)
        while (current != nullptr) {
            if (current->key == key) {
                delete[] current->value;
                current->value = new char[strlen(value) + 1];
                strcpy(current->value, value);
                return true;
            }
            current = current->next;
        }
        
        // Insert new node at head of chain
        HashNode* new_node = new HashNode(key, value);
        new_node->next = table[index];
        table[index] = new_node;
        return true;
    }
    
    // Find value by key
    const char* find(unsigned int key) {
        unsigned int index = get_hash_index(key);
        HashNode* current = table[index];
        
        while (current != nullptr) {
            if (current->key == key) {
                return current->value;
            }
            current = current->next;
        }
        return "";
    }
    
    // Remove key-value pair
    bool remove(unsigned int key) {
        unsigned int index = get_hash_index(key);
        HashNode* current = table[index];
        HashNode* prev = nullptr;
        
        while (current != nullptr) {
            if (current->key == key) {
                if (prev == nullptr) {
                    table[index] = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }
    
    // Print table contents (all buckets and chains)
    void print() {
        std::cout << "\nHash table contents (" << TABLE_SIZE << " buckets):" << std::endl;
        for (unsigned int i = 0; i < TABLE_SIZE; i++) {
            std::cout << "Bucket " << i << ": ";
            HashNode* current = table[i];
            while (current != nullptr) {
                std::cout << "(" << current->key << ", \"" << current->value << "\") ";
                current = current->next;
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    HashTable ht;
    
    // Test keys: next primes after 2^((journal+3)%10 +2) for journal=1,2,3,4,5
    // journal=1: 2^(6)=64 ? 67
    // journal=2: 2^(7)=128 ? 131  
    // journal=3: 2^(8)=256 ? 257
    // journal=4: 2^(9)=512 ? 521
    // journal=5: 2^(10)=1024 ? 1031
    unsigned int keys[] = {67, 131, 257, 521, 1031};
    const char* values[] = {"Artemova", "banana", "cherry", "date", "elderberry"};
    
    std::cout << "1. Insert test:" << std::endl;
    for (int i = 0; i < 5; i++) {
        ht.insert(keys[i], values[i]);
        std::cout << "Inserted " << keys[i] << " -> \"" << values[i] << "\"" << std::endl;
    }
    ht.print();
    
    std::cout << "\n2. Find test:" << std::endl;
    std::cout << "Key 257: \"" << ht.find(257) << "\"" << std::endl;
    std::cout << "Key 999: \"" << ht.find(999) << "\"" << std::endl;  // not found
    
    std::cout << "\n3. Update test:" << std::endl;
    ht.insert(131, "blueberry");  // update existing key
    std::cout << "Updated 131 -> \"blueberry\"" << std::endl;
    std::cout << "Key 131: \"" << ht.find(131) << "\"" << std::endl;
    ht.print();
    
    std::cout << "\n4. Delete test:" << std::endl;
    bool removed = ht.remove(131);
    std::cout << "Removed 131: " << (removed ? "yes" : "no") << std::endl;
    ht.print();
    
    return 0;
}