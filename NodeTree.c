#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dnode {
    void *key;
    size_t keySize;
    void *value;
    size_t valueSize;
    struct dnode *next;
    struct dnode *prev;
} DNode;

typedef struct {
    DNode *head;
    DNode *tail;
    size_t numelems;
} DList;


DList *list_create() {
    DList *list = (DList *)malloc(sizeof(DList));
    if (!list) return NULL; // Check for malloc failure

    list->head = NULL;
    list->tail = NULL;
    list->numelems = 0;
    return list;
}


void list_insert(DList *list, void *key, size_t keySize, void *value, size_t valueSize) {
    if (!list || !key || !value) return;

    DNode *newNode = (DNode *)malloc(sizeof(DNode));
    if (!newNode) return; // Check for malloc failure

    newNode->key = malloc(keySize);
    newNode->value = malloc(valueSize);
    if (!newNode->key || !newNode->value) { // Check for malloc failure
        free(newNode->key); // Free any allocated memory
        free(newNode->value);
        free(newNode);
        return;
    }

    memcpy(newNode->key, key, keySize);
    memcpy(newNode->value, value, valueSize);
    newNode->keySize = keySize;
    newNode->valueSize = valueSize;
    newNode->next = NULL;
    newNode->prev = list->tail;

    if (list->tail) {
        list->tail->next = newNode;
    } else {
        list->head = newNode;
    }
    list->tail = newNode;
    list->numelems++;
}


void list_free(DList *list) {
    if (!list) return;

    DNode *current = list->head;
    while (current != NULL) {
        DNode *next = current->next;
        free(current->key);   // Free the memory allocated for the key
        free(current->value); // Free the memory allocated for the value
        free(current);        // Free the node itself
        current = next;       // Move to the next node
    }

    free(list); // Finally, free the list structure
}

void list_free(DList *list) {
    if (!list) return;

    DNode *current = list->head;
    while (current != NULL) {
        DNode *next = current->next;
        free(current->key);   // Free the memory allocated for the key
        free(current->value); // Free the memory allocated for the value
        free(current);        // Free the node itself
        current = next;       // Move to the next node
    }

    free(list); // Finally, free the list structure
}

void input_key_value(DList *list) {
    if (!list) return;

    printf("Insert key: ");
    void *key = read_data(256); // Temporarily read up to 256 characters

    size_t keySize;
    printf("Key data size: ");
    scanf("%zu", &keySize);

    printf("Insert value: ");
    void *value = read_data(256); // Temporarily read up to 256 characters

    size_t valueSize;
    printf("Value data size: ");
    scanf("%zu", &valueSize);

    // Resize key and value based on actual sizes
    key = realloc(key, keySize);
    value = realloc(value, valueSize);

    list_insert(list, key, keySize, value, valueSize);

    // The list now owns the key and value, so don't free them here
}

void print_key(const void *key, size_t keySize) {
    if (keySize == sizeof(char)) {
        printf("%c", *(char *)key);
    } else if (keySize == sizeof(int)) {
        printf("%d", *(int *)key);
    } else if (keySize == sizeof(float)) {
        printf("%f", *(float *)key);
    } else if (keySize == sizeof(double)) {
        printf("%lf", *(double *)key);
    } else {
        printf("Unknown key type or size\n");
    }
}

void print_value(const void *value, size_t valueSize) {
    if (valueSize == sizeof(char)) {
        printf("%c", *(char *)value);
    } else if (valueSize == sizeof(int)) {
        printf("%d", *(int *)value);
    } else if (valueSize == sizeof(float)) {
        printf("%f", *(float *)value);
    } else if (valueSize == sizeof(double)) {
        printf("%lf", *(double *)value);
    } else {
        printf("Unknown value type or size\n");
    }
}

void saveDListToFile(DList *list) {
    char filename[100];

    // Prompt for the filename
    printf("Enter the filename to save: ");
    scanf("%99s", filename); // Limit input to 99 chars to prevent buffer overflow

    // Open the file in binary write mode
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Write the number of elements first
    fwrite(&(list->numelems), sizeof(size_t), 1, file);

    // Iterate over the list and write each node's key and value
    DNode *current = list->head;
    while (current != NULL) {
        // Write key size and key
        fwrite(&(current->keySize), sizeof(size_t), 1, file);
        fwrite(current->key, 1, current->keySize, file);

        // Write value size and value
        fwrite(&(current->valueSize), sizeof(size_t), 1, file);
        fwrite(current->value, 1, current->valueSize, file);

        // Move to the next node
        current = current->next;
    }

    // Close the file
    fclose(file);

    printf("File saved successfully.\n");
}

DList* restoreDListFromFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    DList *list = list_create();
    if (list == NULL) {
        fclose(file);
        return NULL;
    }

    size_t numElems;
    fread(&numElems, sizeof(size_t), 1, file);

    for (size_t i = 0; i < numElems; i++) {
        size_t keySize, valueSize;
        void *key, *value;

        // Read key size and allocate memory for key
        fread(&keySize, sizeof(size_t), 1, file);
        key = malloc(keySize);
        fread(key, 1, keySize, file);

        // Read value size and allocate memory for value
        fread(&valueSize, sizeof(size_t), 1, file);
        value = malloc(valueSize);
        fread(value, 1, valueSize, file);

        // Insert the key-value pair into the list
        list_insert(list, key, keySize, value, valueSize);

        // Free the temporary key and value memory
        free(key);
        free(value);
    }

    fclose(file);

    return list;
}

int main() {
    DList *list = list_create();
    int choice;

    while(1) {
        // Displaying the menu
        printf("\nMenu:\n");
        printf("1) Print Keys\n");
        printf("2) Print Values\n");
        printf("3) Insert KV Pair\n");
        printf("4) Save\n");
        printf("5) Restore\n");
        printf("6) Exit\n");
        printf("Enter your choice: ");

        // Reading the user input
        scanf("%d", &choice);

        // Handling the user choice
        switch(choice) {
            case 1:
                printf("Print Keys selected.\n");
                DNode *current = list->head;
                while (current != NULL) {
                    print_key(current->key, current->keySize);
                    printf("\n");
                    current = current->next;
                }
                break;
            case 2:
                printf("Print Values selected.\n");
                DNode *current = list->head;
                while (current != NULL) {
                    print_value(current->value, current->valueSize);
                    printf("\n");
                    current = current->next;
                }
                break;
            case 3:
                printf("Insert KV Pair selected.\n");
                input_key_value(list);
                break;
            case 4:
                printf("Save selected.\n");
                saveDListToFile(list);
                break;
            case 5:
                printf("Restore selected.\n");
                char filename[100];
                // Prompt for the filename
                printf("Enter the filename to restore: ");
                scanf("%99s", filename); // Limit input to 99 chars to prevent buffer overflow
                restoreDListFromFile(filename);
                break;
            case 6:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice, please try again.\n");
                // The loop will continue
        }
    }

    return 0;
}