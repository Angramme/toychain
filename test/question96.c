#include <stdio.h>
#include <stdlib.h>

#include <lib/blockchain.h>


int main(){
    create_dir_structure();

    // 1
    const size_t nV = 1000, nC = 5;
    generate_random_data(nV, nC, BLOCK_STORAGE_DIR);
    delete_dir_files(BLOCKCHAIN_DIR);
    
    {
        // 2
        CellProtected* decl = read_protected(BLOCK_STORAGE_DIR "declarations.txt");
        CellKey* citoyens = read_public_keys(BLOCK_STORAGE_DIR "keys.txt");
        size_t i;
        CellProtected* cur = decl;
        
        // 3
        CellTree* blocktree = NULL; 
        const int d = 1;
        const size_t interval = 10;
        i=0;
        while(cur){
            submit_vote(cur->data);
            i++;
            if(i!=0 && i%interval==0){
                create_block(&blocktree, cur->data->pKey, d);
                Block* B = read_block(PENDING_BLOCK_FILE);
                if(!B){
                    printf("error reading block! \n");
                    free_tree(blocktree);
                    free_list_protected(decl);
                    free_list_keys(citoyens);
                    return 1;
                }
                char* name = hash_to_str(B->hash);
                add_block(d, name);
                free(name);
                free_block(B);
            }
            cur = cur->next;
        }

        // 3.5
        printf("arbre original: \n");
        print_tree(blocktree);

        free_tree(blocktree);
        free_list_protected(decl);
        free_list_keys(citoyens);
    }
    {
        // 4 
        CellTree* tr = read_tree();
        printf("\n\n arbre chargé: \n");
        print_tree(tr);

        CellKey* candidates = read_public_keys(BLOCK_STORAGE_DIR "candidates.txt");
        CellKey* voters = read_public_keys(BLOCK_STORAGE_DIR "keys.txt");

        Key* winner = compute_winner_BT(tr, candidates, voters, nC, nV);
        char* kstr = key_to_str(winner);
        printf("WINNER: %s \n", kstr);
        free(kstr);
        free(winner);

        free_list_keys(candidates);
        free_list_keys(voters);

        free_tree(tr);
    }
}