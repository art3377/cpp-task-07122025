#include <iostream>
#include <string>

// Assume journal number = 7 (can be changed)
// 2^((7+3)%10 + 2) = 2^(10%10 + 2) = 2^(0+2) = 2^2 = 4
// Next prime number after 4 = 5
const unsigned int TABLE_SIZE = 5;

// Structure for chain node (chaining method for collisions)
struct HashNode {
    unsigned int key;           // key (unsigned int)
    char* value;                // pointer to string value
    HashNode* next;             // next node in chain
};

// djb2 hash function for string
unsigned long djb2_hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }
    return hash;
}

// Hash function for pair (unsigned int key, char* str)
unsigned int get_hash(unsigned int key, const char* str) {
    unsigned long str_hash = djb2_hash(str);
    unsigned long combined = (unsigned long)key ^ str_hash;  // XOR for combination
    return combined % TABLE_SIZE;
}

// Hash table class
class HashTable {
private:
    HashNode** table;  // array of pointers to chains

public:
    HashTable() {
        table = new HashNode*[TABLE_SIZE]();
        for (int i = 0; i < TABLE_SIZE; ++i) {
            table[i] = nullptr;
        }
    }

    ~HashTable() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            HashNode* current = table[i];
            while (current) {
                HashNode* next = current->next;
                delete[] current->value;  // free string
                delete current;
                current = next;
            }
        }
        delete[] table;
    }

    // Insert/Update
    void insert(unsigned int key, const char* value_str) {
        unsigned int index = get_hash(key, value_str);
        HashNode* new_node = new HashNode{key, nullptr, nullptr};
        
        // Copy string
        size_t len = strlen(value_str) + 1;
        new_node->value = new char[len];
        strcpy(new_node->value, value_str);
        
        // Check if key already exists
        HashNode* prev = nullptr;
        HashNode* current = table[index];
        while (current) {
            if (current->key == key) {
                // Update value
                delete[] current->value;
                current->value = new_node->value;
                delete new_node;
                return;
            }
            prev = current;
            current = current->next;
        }
        
        // Insert at beginning of chain
        if (prev) {
            new_node->next = prev->next;
            prev->next = new_node;
        } else {
            new_node->next = table[index];
            table[index] = new_node;
        }
    }

    // Search
    char* find(unsigned int key, const char* search_str) {
        unsigned int index = get_hash(key, search_str);
        HashNode* current = table[index];
        while (current) {
            if (current->key == key) {
                return current->value;
            }
            current = current->next;
        }
        return nullptr;
    }

    // Remove
    bool remove(unsigned int key, const char* search_str) {
        unsigned int index = get_hash(key, search_str);
        HashNode* prev = nullptr;
        HashNode* current = table[index];
        while (current) {
            if (current->key == key) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    table[index] = current->next;
                }
                delete[] current->value;
                delete current;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    // Print contents for demonstration
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
    
    std::cout << "=== Hash Table Demonstration ===" << std::endl;
    std::cout << "Table size: " << TABLE_SIZE << std::endl << std::endl;
    
    // Test 1: Insert
    std::cout << "1. Inserting elements:" << std::endl;
    ht.insert(123, "value1");
    ht.insert(456, "value2");
    ht.insert(789, "value3");  // May collide in same bucket
    ht.print();
    
    // Test 2: Search
    std::cout << "\n2. Search:" << std::endl;
    char* found1 = ht.find(123, "value1");
    std::cout << "Key 123: " << (found1 ? found1 : "not found") << std::endl;
    
    char* found2 = ht.find(456, "value2");
    std::cout << "Key 456: " << (found2 ? found2 : "not found") << std::endl;
    
    char* not_found = ht.find(999, "test");
    std::cout << "Key 999: " << (not_found ? not_found : "not found") << std::endl;
    
    // Test 3: Update
    std::cout << "\n3. Updating value:" << std::endl;
    ht.insert(123, "new_value1");  // Update existing key
    ht.print();
    
    // Test 4: Remove
    std::cout << "\n4. Removing key 456:" << std::endl;
    bool removed = ht.remove(456, "value2");
    std::cout << "Removed: " << (removed ? "yes" : "no") << std::endl;
    ht.print();
    
    return 0;
}