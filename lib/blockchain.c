#include "lib/blockchain.h"
#include "lib/dataio.h"
#include "lib/error.h"
#include "lib/vote.h"

#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file blockchain.c
 * @brief File to create and handle blockchain structures.
 */

/**
 * @brief print the content of a Block struct in a file
 * 
 * @param filename name of the file
 * @param b object to print
 */
bool write_block(const char* filename, const Block* b){
    FILE* f = fopen(filename, "w");
    if(!f){
        fprintf(stderr, "write_block : failed opening\n");
        return false;
    }
    if(!b){
        fprintf(stderr, "write_block : null argument\n");
        fclose(f);
        return false;
    }

    char* keystr = key_to_str(b->author);
    char* l_str = list_protected_to_str(b->votes);
    char* chash = hash_to_str(b->hash);
    char* cphash = hash_to_str(b->previous_hash);
    if(!keystr || !l_str || !cphash || !chash){
        fprintf(stderr, "str conversion failed\n");
        if(keystr) free(keystr);
        if(l_str) free(l_str);
        if(chash) free(chash);
        if(cphash) free(cphash);
        fclose(f);
        return false;
    }

    fprintf(f,"%s\n", keystr); // author
    fprintf(f,"%s\n", chash); // hash
    fprintf(f, "%s\n", cphash); // previous_hash
    fprintf(f, "%d\n", b->nonce); // nonce
    fprintf(f, "%s\n", l_str); // votes

    free(l_str);
    free(keystr);
    free(chash);
    free(cphash);

    fclose(f);

    return true;
}

/**
 * @brief read a file and save it's content in a Block object
 * 
 * @param filename 
 * @return Block* 
 */
Block* read_block(const char* filename){
    FILE* F = fopen(filename, "r");
    if(!F){
        fprintf(stderr, "read_block : failed opening\n");
        return NULL;
    }
    Block* BLOCK = malloc(sizeof(Block));
    if(!BLOCK){
        MALLOC_ERROR("could not read");
        fclose(F);
        return NULL;
    }
    
    { // basic line by line part
        const size_t buff_size = 250;
        char buff[buff_size];

        // author
        if(!fgets(buff, buff_size, F)){
            FILE_ERROR("couldn't read next line of file (author)");
            free(BLOCK);
            fclose(F);
            return NULL;
        }
        BLOCK->author = str_to_key(buff);
        if(!BLOCK->author){
            free(BLOCK);
            fclose(F);
            return NULL;
        }

        // hash
        if(!fgets(buff, buff_size, F)){
            FILE_ERROR("couldn't read next line of file (hash)");
            free(BLOCK->author);
            free(BLOCK);
            fclose(F);
            return NULL;
        }
        if(!str_to_hash(buff, &BLOCK->hash)){
            free(BLOCK->author);
            free(BLOCK);
            fclose(F);
            return NULL;
        }

        // previous_hash
        if(!fgets(buff, buff_size, F)){
            FILE_ERROR("couldn't read next line of file (previous_hash)");
            free(BLOCK->author);
            if(BLOCK->hash) free(BLOCK->hash);
            free(BLOCK);
            fclose(F);
            return NULL;
        }
        if(!str_to_hash(buff, &BLOCK->previous_hash)){
            free(BLOCK->author);
            if(BLOCK->hash) free(BLOCK->hash);
            free(BLOCK);
            fclose(F);
            return NULL;
        }

        // nonce
        if(!fgets(buff, buff_size, F)){
            FILE_ERROR("couldn't read next line of file (nonce)");
            free(BLOCK->author);
            if(BLOCK->hash) free(BLOCK->hash);
            if(BLOCK->previous_hash) free(BLOCK->previous_hash);
            free(BLOCK);
            fclose(F);
            return NULL;
        }
        if(1 != sscanf(buff, "%d", &BLOCK->nonce)){
            free(BLOCK->author);
            if(BLOCK->hash) free(BLOCK->hash);
            if(BLOCK->previous_hash) free(BLOCK->previous_hash);
            free(BLOCK);
            fclose(F);
            return NULL;
        }
    }

    { // reading votes
        const size_t start = ftell (F);
        fseek (F, 0, SEEK_END);
        const size_t end = ftell (F);
        fseek (F, start, SEEK_SET);
        const size_t length = end - start;
        char* buffer = malloc(sizeof(char)*(length+1));
        if(!buffer){
            free(BLOCK->author);
            if(BLOCK->hash) free(BLOCK->hash);
            if(BLOCK->previous_hash) free(BLOCK->previous_hash);
            free(BLOCK);
            fclose(F);
            MALLOC_ERROR("couldn't allocate sufficient amount of memory!")
            return NULL;
        }
        fread(buffer, 1, length, F);
        buffer[length] = '\0';
        fclose (F);

        BLOCK->votes = str_to_list_protected(buffer);
        free(buffer);
        if(!BLOCK->votes){
            free(BLOCK->author);
            if(BLOCK->hash) free(BLOCK->hash);
            if(BLOCK->previous_hash) free(BLOCK->previous_hash);
            free(BLOCK);
            return NULL;
        }
    }

    return BLOCK;
}

/**
 * @brief convert sha256 hash to string representation
 * 
 * @param hash 
 * @return char* 
 */
char* hash_to_str(const uint8* hash){
    if(hash == NULL){
        
        char* ret = malloc(sizeof(char) * 8);
        if(!ret){
            MALLOC_ERROR("hash to str failed");
            return NULL;
        }
        memcpy(ret, "NULLHSH\0", 8);
        return ret;
    }
    const size_t maxsize = 2*BLOCK_HASH_SIZE +1;
    char* ret = malloc(sizeof(char)*maxsize);    
    if(!ret){
        MALLOC_ERROR("hash to str failed");
        return NULL;
    }    
    ret[maxsize-1] = '\0';
    size_t i;
    for(i=0; i<BLOCK_HASH_SIZE; i++){
        char buf[4];
        sprintf(buf, "%02x", hash[i]); 
        ret[i*2] = buf[0];
        ret[i*2+1] = buf[1];
    }
    return ret;
}

/**
 * @brief convert string sha256 hash representation to sha256 hash
 * 
 * @param str 
 * @param uint8* return value (can be NULL)
 * @return true : data read correctly
 * @return false : reading error 
 */
bool str_to_hash(const char* str, uint8** RET){
    if(strncmp(str, "NULLHSH", 7) == 0){
        *RET = NULL;
        return true;
    }
    uint8* ret = malloc(sizeof(uint8)*BLOCK_HASH_SIZE);
    if(!ret){
        MALLOC_ERROR("str to hash failed");
        return false;
    }
    size_t i;
    for(i=0; i<BLOCK_HASH_SIZE; i++){
        char buff[3];
        buff[2] = '\0';
        if(!str[i*2] || !str[i*2 +1]){
            free(ret);
            return false;
        }
        memcpy(buff, str + i*2, 2);
        unsigned int V;
        if(1 != sscanf(buff, "%02x", &V)){
            fprintf(stderr, "sscanf failed\n");
            free(ret);
            return false;
        }
        ret[i] = (uint8)V;
    }
    *RET = ret;
    return true;
}

/**
 * @brief convert block head to string and leave extra space for nonce to be appended
 * 
 * @param b 
 * @return char* 
 */
char* block_head_to_str(const Block* b){
    char* kstr = key_to_str(b->author);
    char* lstr = list_protected_to_str(b->votes);
    if(!kstr || !lstr){
        if(kstr) free(kstr);
        if(lstr) free(lstr);
        return NULL;
    }
    size_t len = strlen(kstr) + BLOCK_HASH_SIZE*2 + strlen(lstr) + 10 + 1;
    char* ret = malloc(sizeof(char)*len);
    if(!ret){
        MALLOC_ERROR("could not convert");
        free(kstr);
        free(lstr);
        return NULL;
    }
    ret[0] = '\0';
    strcat(ret, kstr);
    free(kstr);
    char* hshs = hash_to_str(b->previous_hash);
    strcat(ret, hshs);
    free(hshs);
    strcat(ret, lstr);
    free(lstr);
    
    return ret;
}

/**
 * @brief append nonce to the block head string. 
 * If nonce is already appended, the function will 
 * delete the old nonce and append the new one.
 * 
 * @param headstr 
 * @param b 
 */
void block_nonce_to_str(char* headstr, const Block* b){
    const size_t l = strlen(headstr);
    char buf[12];
    sprintf(buf, "|%08x|", b->nonce);
    if(headstr[l-1] == '|'){
        // delete the old nonce first
        size_t beg = l-2;
        while(headstr[beg] != '|') beg--;
        headstr[beg] = '\0';
    }
    strcat(headstr, buf);
}

/**
 * @brief convert block to string
 * 
 * @param b 
 * @return char* 
 */
char* block_to_str(const Block* b){
    char* ret = block_head_to_str(b);
    block_nonce_to_str(ret, b);
    return ret;
}

/**
 * @brief hashes string s using sha256
 * 
 * @param s 
 * @return uint8* 
 */
uint8* hash_string(const char* s){
    // copy_hash is needed here because SHA256 is not mallocd
    // this can cause problems down the line when we free or 
    // when we realloc the memory
    return copy_hash(SHA256((const unsigned char*)s, strlen(s), 0));
}

/**
 * @brief checks if the number of zeros in hash (as per the hexadecimal representation) is at least d
 * 
 * @param hash 
 * @param d 
 * @return true 
 * @return false 
 */
bool check_zeros(const uint8* hash, size_t d){
    if(!hash) return false;
    size_t n = d/2;
    n = n < BLOCK_HASH_SIZE ? n : BLOCK_HASH_SIZE;
    for(size_t i=0; i<n; i++)
        if(hash[i] != 0) return false;
    if(d%2 == 1 && hash[n] & 0b11110000) return false;
    return true;
}

/**
 * @brief compute proof of work of block B with d zeros in hexadecimal representation
 * 
 * @param B 
 * @param d 
 */
void compute_proof_of_work(Block *B, int d){
    char* bstr = block_to_str(B);
    while(!check_zeros(B->hash, d)){
        B->nonce++;
        // modify the nonce in the string.
        // makes the program faster than reconverting 
        // the whole block each time...
        block_nonce_to_str(bstr, B); 
        uint8* hsh = hash_string(bstr);
        if(B->hash) free(B->hash);
        B->hash = hsh;
    }
    free(bstr);
}

/**
 * @brief print sha256 hash
 * 
 * @param hash 
 */
void print_sha256_hash(const uint8* hash){
    size_t i;
    for(i = 0; i < BLOCK_HASH_SIZE; i++)
        printf("%02x", hash[i]);
    putchar('\n');
}

/**
 * @brief checks if A and B are the same hash
 * 
 * @param A 
 * @param B 
 * @return true 
 * @return false 
 */
bool compare_hash(const uint8* A, const uint8* B){
    if(!A && !B) return true;
    if(!A || !B) return false;
    bool same = true;
    for(int i = 0; (i < BLOCK_HASH_SIZE) && same; i++)
        same = same && A[i] == B[i];
    return same;
}

/**
 * @brief copy hash
 * 
 * @param hash 
 * @return uint8* 
 */
uint8* copy_hash(const uint8* hash){
    uint8* ret = malloc(sizeof(uint8)*(BLOCK_HASH_SIZE+1));
    if(!ret){
        MALLOC_ERROR("failed ret allocation");
        return NULL;
    }
    memcpy(ret, hash, BLOCK_HASH_SIZE);
    ret[BLOCK_HASH_SIZE] = '\0';
    return ret;
}

/**
 * @brief verifies if the block is valid with d zeros in hexadecimal representation
 * 
 * @param d 
 * @return true 
 * @return false 
 */
bool verify_block(const Block* B, int d){
    char* str = block_to_str(B);
    uint8* hsh = hash_string(str);
    free(str);
    const bool ret = check_zeros(hsh, d) && compare_hash(B->hash, hsh);
    free(hsh);
    return ret;
}

/**
 * @brief init a block object. Takes ownership of all auth, votes and prev_hash
 * 
 * @param auth 
 * @param votes 
 * @param prev_hash 
 * @return Block* 
 */
Block* init_block_raw(Key* auth, CellProtected* votes, uint8* prev_hash){
    Block* b = malloc(sizeof(Block));
    if(!b){
        MALLOC_ERROR("failed b allocation");
        return NULL;
    }
    b->author = auth;
    b->votes = votes;
    b->hash = NULL;
    b->previous_hash = prev_hash;
    b->nonce = 0;
    return b;
}

/**
 * @brief init block object, copies all parameters.
 * 
 * @param auth 
 * @param votes 
 * @param prev_hash 
 * @return Block* 
 */
Block* init_block(const Key* auth, const CellProtected* votes, const uint8* prev_hash){
    return init_block_raw(copy_key(auth), copy_protected_list(votes), copy_hash(prev_hash));
}

/**
 * @brief frees block (ce n'est pas delete_block)
 * 
 * @param b 
 */
void free_block(Block* b){
    if(!b)return;
    free(b->author);
    free(b->hash);
    free(b->previous_hash);
    free_list_protected(b->votes);
    free(b);
}

/**
 * @brief cette fonction ne free pas le champ author ni le contenu des votes
 * 
 * @param b block to be deleted
 */
void delete_block(Block* b){
    free(b->hash);
    free(b->previous_hash);
    while(b->votes){
        CellProtected* temp = b->votes;
        b->votes = b->votes->next;
        // temp->data // on ne free pas le contenu comme l'enonce le demande
        free(temp);
    }
    free(b);
}

/**
 * @brief Create a node object. Takes ownership of Block b!
 * 
 * @param b 
 * @return CellTree* 
 */
CellTree* create_node(Block* b){
    CellTree* t = malloc(sizeof(CellTree));
    t->block = b;
    t->height = 0;
    t->father = NULL;
    t->firstChild = NULL;
    t->nextBro = NULL;
    return t;
}

bool update_height(CellTree* father, CellTree* child){
    size_t newh = 1 + child->height;
    if(newh > father->height){
        father->height = newh;
        return true;
    }else{
        return false;
    }
}

void add_child(CellTree* father, CellTree* child){
    if(child->nextBro != NULL){
        fprintf(stderr, "invalid child passed as parameter!");
        return;
    }
    child->nextBro = father->firstChild;
    father->firstChild = child;
    child->father = father;
    // udpate all ascendants
    while(father && update_height(father, child)){
        child = father;
        father = father->father;
    }
}

void print_tree_r(CellTree* tr, int indent){
    if(!tr) return;

    for(int i=0; i<indent-1; i++)
        printf(" |  ");
    if(indent > 0) printf(" |- ");
    
    printf("%zd ", tr->height);
    char* bstr = block_to_str(tr->block);
    uint8* bhash = hash_string(bstr);
    free(bstr);
    print_sha256_hash(bhash);
    free(bhash);
    
    print_tree_r(tr->firstChild, indent +1);
    print_tree_r(tr->nextBro, indent);
}
void print_tree(CellTree* tr){
    print_tree_r(tr, 0);
}

/**
 * @brief fonction de l'enonce
 * 
 * @param node 
 */
void delete_node(CellTree* node){
    delete_block(node->block);
    free(node);
}

/**
 * @brief free everything the node contains
 * 
 * @param node 
 */
void free_node(CellTree* node){
    free_block(node->block);
    free(node);
}

/**
 * @brief fonction de l'enonce
 * 
 * @param node 
 */
void delete_tree(CellTree* node){
    CellTree* nextb = node->nextBro;
    CellTree* child = node->firstChild;
    delete_node(node);
    delete_tree(nextb);
    delete_tree(child);
}

/**
 * @brief returns the child with max height of cell
 * 
 * @param cell 
 * @return CellTree* 
 */
CellTree* highest_child(const CellTree* cell){
    CellTree* cur = cell->firstChild;
    CellTree* mx = cur;

    while(cur){
        if(cur->height > mx->height) mx = cur;
        cur = cur->nextBro;
    }

    return mx;
}

/**
 * @brief 
 * 
 * @param tree 
 * @return CellTree* 
 */
CellTree* last_node(CellTree* tree){
    if(!tree) return NULL;

    while(tree->firstChild){
        tree = highest_child(tree);
    }

    return tree;
}

/**
 * @brief Get all the trusted declarations from the longest
 * chain.
 * 
 * @param tree 
 * @return CellProtected* 
 */
CellProtected* get_trusted_declarations(const CellTree* tree){
    CellTree* hc = highest_child(tree);

    CellProtected* ret = NULL;

    while(hc){
        ret = fuse_protected_lists(copy_protected_list(hc->block->votes), ret);
        hc = hc->father;
    }

    return ret;
}

void submit_vote(Protected* p){
    FILE* F = fopen(PENDING_VOTES_FILE, "a");
    if(!F){
        FILE_ERROR("cannot open " PENDING_VOTES_FILE " !");
        return;
    }
    char* str = protected_to_str(p);
    if(!str){
        fprintf(stderr, "cannot convert protected to string!");
        fclose(F);
        return;
    }
    fprintf(F, "%s\n", str);
    free(str);
    fclose(F);
}

void create_block(CellTree* tree, Key* author, int d){
    CellProtected* plist = read_protected(PENDING_VOTES_FILE);

    CellTree* last = last_node(tree);

    Block* B = init_block_raw(
        author, 
        plist, 
        copy_hash(last->block->hash));

    compute_proof_of_work(B, d);
    write_block(PENDING_BLOCK_FILE, B);
    while(plist){
        free(plist->data);
        plist = plist->next;
    }
    delete_block(B);
    remove(PENDING_VOTES_FILE);
}

void add_block(int d, const char* name){
    Block* b = read_block(PENDING_BLOCK_FILE);
    verify_block(b, d);
    remove(PENDING_BLOCK_FILE);

    const size_t len = strlen(BLOCKCHAIN_DIR) + strlen(name);
    char* fullname = malloc(sizeof(char)*(len+1));
    if(!fullname){
        MALLOC_ERROR("couldn't add block!");
        return;
    }
    fullname[0] = '\0';
    strcat(fullname, BLOCKCHAIN_DIR);
    strcat(fullname, name);

    write_block(fullname, b);
    free(fullname);
}


CellTree* read_tree(){

    //////////////////// ETAPE 1 : lire tout les blocs

    DIR* D = opendir(BLOCKCHAIN_DIR);
    if(!D){
        FILE_ERROR("cannot open blockchain directory!");
        fprintf(stderr, " message : %s \n", strerror(errno));
        return NULL;
    }

    size_t i=0, j=0;
    size_t STAB = 0;
    CellTree** TAB;

    struct dirent* dir;
    while((dir = readdir(D))){
        if(dir->d_type == DT_DIR || strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            continue;
        STAB++;
    }

    closedir(D);
    D = opendir(BLOCKCHAIN_DIR);
    if(!D){
        FILE_ERROR("cannot open blockchain directory!");
        fprintf(stderr, " message : %s \n", strerror(errno));
        return NULL;
    }

    TAB = malloc(sizeof(CellTree*)*STAB);
    for(i=0; i<STAB; i++) TAB[i] = NULL;

    char fpath[500];
    i=0;
    while((dir = readdir(D))){
        if(dir->d_type == DT_DIR || strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            continue;
        strcpy(fpath, BLOCKCHAIN_DIR);
        strcat(fpath, dir->d_name);

        Block* B = read_block(fpath);
        TAB[i] = create_node(B);
        if(!TAB[i]){
            closedir(D);
            fprintf(stderr, "couldnt' read block %s \n", fpath);
            i = 0;
            while(TAB[i]) free_node(TAB[i++]);
            free(TAB);
            return NULL;
        }
    }

    closedir(D);

    ////////////// ETAPE 2 : constituer l'arbre

    for(i = 0; i<STAB; i++){
        for(j = 0; j<STAB; j++){
            if(i==j || !compare_hash(TAB[j]->block->previous_hash, TAB[i]->block->hash))
                continue; // not a child, skip...
            add_child(TAB[i], TAB[j]);
        }
    }

    CellTree* root;
    for(i=0; i<STAB; i++)
        if(TAB[i]->father == NULL){
            root = TAB[i];
            break;
        }
    free(TAB);
    return root;
}

Key* compute_winner_BT(CellTree* tree, CellKey* candidates, CellKey* voters, size_t sizeC, size_t sizeV){
    CellProtected* decl = get_trusted_declarations(tree);
    remove_fraudulent_declarations(&decl);
    Key* winner = compute_winner(decl, candidates, voters, sizeC, sizeV);
    free_list_protected(decl);
    return winner;
}







