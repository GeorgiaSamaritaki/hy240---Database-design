/************************************************************//**
 * @file   main.c                                      		*
 * @author Nikolaos Batsaras <batsaras@csd.uoc.gr>  		*
 *                                                    		*
 * @brief Main function for the needs of cs-240b project 2017   *
 ***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "troy.h"

#define BUFFER_SIZE 1024  /**< Maximum length of a line in input file */

/* Uncomment the following line to enable debugging prints 
 * or comment to disable it */
#define DEBUG

#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */


/*global number of size of hashtable*/
unsigned int m;
int count;

/**
 * @brief Optional function to initialize data structures that 
 * need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
 int initialize() {
    if(max_soldiers_g < 0) return 0;
    m = max_soldiers_g / 7;
    m = findNextPrime(m);
    printf("SIze of array m is %d",m);
    registration_hashtable = (struct soldier **)malloc(m * sizeof(struct soldier)); 
    int i;
	for(i=0;i<m;i++){
        registration_hashtable[i] = NULL;
    }
    return 1;
}

int findNextPrime(int n){
    while (1){
		int i;
        for ( i = 2; i <= n/2; i++){
            if (n % i == 0)    break;
        }
        if(i == n/2+1) return n;
        n++;
    }
}
int h(int key){
  return key % m;
}

struct general* findGen(int gid){		
	struct general* g= generals_list;
	while(g !=NULL){
        if(g->gid == gid) break;
		g = g->next;
	}
	return g;
}

/**
 * @brief Register soldier
 *
 * @param sid The soldier's id
 * @param gid The general's id who orders the soldier
 *
 * @return 1 on success
 *         0 on failure
 */
 int register_soldier(int sid, int gid) {
    struct soldier *temp = (struct soldier *) malloc(sizeof(struct soldier));
	if(temp == NULL) return 0;
	temp->sid = sid;
	temp->gid = gid;
    
    int pos = h(sid);
    if(registration_hashtable[pos]==NULL){
        registration_hashtable[pos] = temp;
        temp->next = NULL;
    }else{
        temp->next = registration_hashtable[pos];
        registration_hashtable[pos] = temp;
    }
    return 1;
}



/**
 * @brief General or King joins the battlefield
 *
 * @param gid The general's id
 *
 * @return 1 on success
 *         0 on failure
 */
 int add_general(int gid) {
    struct general * temp = (struct general *)malloc(sizeof(struct general));
	if(temp==NULL) return 0;
	temp->soldiers_R = NULL; 
	temp->gid = gid;
    temp->soldiers_no = 0 ;
	temp->combats_no = 0;
	temp->next= generals_list;
	generals_list =  temp; 	
    temp->soldiers_S=(struct TREE_soldier *)malloc(sizeof(struct TREE_soldier)); 
    temp->soldiers_S->sid = -1;
    print_generalsList();
    return 1;
}

/**
 * @brief Distribute soldiers to the camps
 * 
 * @return 1 on success
 *         0 on failure
 */
 struct TREE_soldier* insertBST(struct TREE_soldier* root , struct TREE_soldier* node){
    if(root == NULL) return node;

    struct TREE_soldier * p = root;
    struct TREE_soldier * parent = NULL;
     
    while (p->sid != -1) { 
        parent = p; 
        if (node->sid < p->sid) p = p->lc; 
        else p = p->rc; 
    }   
    if (node->sid < parent->sid) parent->lc = node; 
    else parent->rc = node; 
    node->p = parent;
    return root; 

 }

 struct TREE_soldier * soldierToTree(int sid , struct TREE_soldier * sentinel){
    struct TREE_soldier * new = (struct TREE_soldier *)malloc(sizeof(struct TREE_soldier));
	new-> sid = sid;
    new->rc = sentinel;
    new->lc = sentinel;
    new->p = NULL;
	return new;
 } 


 int distribute() {
    int i;
    struct soldier * list;
    struct TREE_soldier *insert;
    struct general* general;

    for(i=0;i<m;i++){
        list = registration_hashtable[i];
        while(list != NULL){
            general = findGen(list->gid);
            insert = soldierToTree(list->sid, general->soldiers_S);
            if(insert == NULL) return 0;
            general->soldiers_no++;
            general->soldiers_R = insertBST(general->soldiers_R,insert);
            list = list->next;
        }
    }

    print_generals();
    return 1;
}



/**
 * @brief General resigns from battlefield
 *
 * @param gid1 The id of general that resigns
 * @param gid2 The id of the general that will  
 * 
 * @return 1 on success
 *         0 on failure
 */
 struct TREE_soldier* ArrayToTree(struct TREE_soldier** array,int start,int end,struct TREE_soldier* parent,struct TREE_soldier* sentinel){  
    if(start>end) return sentinel;
    int mid = (start+end)/2;  
    struct TREE_soldier* insert = array[mid];    
    insert->p = parent;
    insert->lc = ArrayToTree(array,start,mid-1,insert,sentinel);  
    insert->rc = ArrayToTree(array, mid+1,end,insert,sentinel);  
    return insert;  
 } 

 int countNodes(struct TREE_soldier *root){
	int count = 0;
    if (root->lc->sid != -1 ) count += countNodes(root->lc);
    if (root->rc->sid != -1 ) count += countNodes(root->rc);
    return count + 1;
 }

 int fill_array(struct TREE_soldier * root, struct TREE_soldier** array , int pos) {     
    if (root->lc->sid != -1) pos = fill_array(root->lc, array, pos); 
    array[pos++] = root;     
    if (root->rc->sid != -1) pos = fill_array(root->rc, array, pos);        
    return pos;  
 } 

 void ChangeRegistration(int gid1, int gid2){
	int i,count;
    struct soldier * tmp;
    for(i=0;i<m;i++){
        tmp = registration_hashtable[i];
        while(tmp != NULL){
			if(tmp->gid == gid1){
				tmp->gid = gid2;
			}
			tmp = tmp->next;
		}
    }
 }

 int general_resign(int gid1, int gid2) {
    struct general * g1 = generals_list;
    struct 
	general * g2 = NULL;
    while( g1 !=NULL){
        if(g1->gid == gid1){
            if(g2 != NULL) g2->next = g1->next;
            else generals_list = g1->next;
            break;
        }
        g2 = g1;
        g1 = g1->next;
    }
    g2 = findGen(gid2);
   

    ChangeRegistration(gid1,gid2);
    if(g1->soldiers_R == NULL){
        free(g1);
        return 1;
    }
    if(g2->soldiers_R == NULL){
        g2->soldiers_R = g1->soldiers_R;
        free(g2->soldiers_S);
        g2->soldiers_S = g1->soldiers_S;
        free(g1);
        return 1;
    }

    int n1 = countNodes(g1->soldiers_R);
    int n2 = countNodes(g2->soldiers_R);

    struct TREE_soldier **A1 = (struct TREE_soldier **)malloc(n1 * sizeof(struct TREE_soldier));
    struct TREE_soldier **A2 = (struct TREE_soldier **)malloc(n2 * sizeof(struct TREE_soldier));

    fill_array(g1->soldiers_R,A1,0);
    fill_array(g2->soldiers_R,A2,0);

    struct TREE_soldier **sorted = (struct TREE_soldier **)malloc((n2+n1) * sizeof(struct TREE_soldier));
    

    int i=0,j=0,index=0;  
    while(i != n1 && j!=n2){         
        if(A1[i]->sid < A2[j]->sid) {
            A1[i]->lc = g2->soldiers_S;
            A1[i]->rc = g2->soldiers_S;
            sorted[index++] = A1[i++];
        }
        else if(A2[j]->sid < A1[i]->sid )  sorted[index++] = A2[j++];         
        
    } 
    while(i<n1)  sorted[index++] = A1[i++];  
    while(j<n2)  sorted[index++] = A2[j++]; 

    g2->soldiers_R = ArrayToTree(sorted, 0,(n1+n2-1), NULL ,g2->soldiers_S);
    free(g1->soldiers_S);

    if(g1!=NULL) free(g1);

    print_generals();
    return 1;
}

/**
 * @brief Prepare for battle
 * 
 * @param gid1 The id of the first general
 * @param gid2 The id of the second general
 * 
 * @return 1 on success
 *         0 on failure
 */

struct TREE_soldier* inorder_successor(struct TREE_soldier* node){     
    if (node->rc->sid != -1) {         
        struct TREE_soldier* tmp = node->rc;         
        while(tmp->lc->sid!=-1){             
            tmp= tmp->lc;         
        }         
        
		return tmp;     
    }     
    struct TREE_soldier* parent = node->p;     
    while(parent != NULL && node == parent->rc){
        node = parent;         
        parent= parent->p;     
    }    
    return parent; 
} 
 
struct TREE_soldier* inorder_predeccesor(struct TREE_soldier* node){     
    if (node->lc->sid != -1) {         
        struct TREE_soldier* tmp = node->lc;         
        while(tmp->rc->sid!=-1){             
            tmp= tmp->rc;         
        }         
        return tmp;     
    }     
    struct TREE_soldier* parent = node->p;     
    while(parent != NULL && node == parent->lc){
        node = parent;         
        parent= parent->p;     
    }    
    return parent; 
} 

struct TREE_soldier* findMinSid(struct TREE_soldier* node){
    while (node->lc->sid != -1){
        node = node->lc;
    }
    return node;
}
struct TREE_soldier* findMaxSid(struct TREE_soldier* node){
    while (node->rc->sid != -1){
        node = node->rc;
    }
    return node;
}

struct c_soldier* TreeToCombat(struct TREE_soldier* node,struct general* g){
    struct c_soldier* new = (struct c_soldier*)malloc(sizeof(struct c_soldier));
    new->alive = 1;
    new->left_cnt = 0;
    new->rc = NULL;
    new->lc = NULL;
    new->sid = node->sid;
    new ->gid = g->gid;
    return new;
}

 void insertCombat(struct c_soldier* node){
    if(node==NULL) return;
    my_combat.soldier_cnt++;

    if(my_combat.combat_s == NULL) {
        my_combat.combat_s =node;
        return;
    }
    struct c_soldier * p = my_combat.combat_s;
    struct c_soldier * prev = p;
    while (p != NULL ) { 
        prev = p;
        if (node->sid < p->sid) {
            p->left_cnt++;
            p = p->lc;
        }else{ 
            p = p->rc;
        }
    }   
    if (node->sid < prev->sid){
        prev->lc = node; 
    }else{ 
        prev->rc = node; 
    }
    return; 
}


int prepare_battle(int gid1, int gid2) {
    struct general * g1 = findGen(gid1);
    struct general * g2 = findGen(gid2);
    struct TREE_soldier* s1, *s2,*e1,*e2;
    if(g1!= NULL) {
        g1->combats_no++;
        s1 = findMinSid(g1->soldiers_R);
        e1 = findMaxSid(g1->soldiers_R);
    }
    if(g2!= NULL) {
        g2->combats_no++;
        s2 = findMaxSid(g2->soldiers_R);
        e2 = findMinSid(g2->soldiers_R);
    }
    my_combat.soldier_cnt = 0;
    my_combat.combat_s = NULL;

    while(!(g1==NULL && g2==NULL)){
        if(g1!=NULL){
            insertCombat(TreeToCombat(s1,g1));
            if(s1==e1) g1 =NULL;
            else s1 = inorder_successor(s1);
        }
        if(g2!=NULL){
            insertCombat(TreeToCombat(s2,g2));
            if(s2==e2) g2 =NULL;
            else s2 = inorder_predeccesor(s2);
        }
        
    }
    printCombat();

    return 1;
}

/**
 * @brief Battle
 * 
 * @param god_favor
 * @param bit_stream A sequence of bits
 * @param bitsno The number of bits in the sequence
 *
 * @return 1 on success
 *         0 on failure
 */
 
void postorderKill(struct c_soldier* node,int i){
    if(node == NULL || count++>i) return;
    postorderKill(node->rc,i);
    postorderKill(node->lc,i);
    node->alive = 0;
}
 
int battle(int god_favor, char *bit_stream, int bitsno) {
    if(god_favor){
        struct c_soldier * p = my_combat.combat_s;
        int i = 0;
        p->alive = 0;
        while(p!=NULL && i<bitsno){
            p->alive = 0;
            if(bit_stream[i] == '1') p=p->rc;
            else if(bit_stream[i] == '0') p=p->lc;
            i++;
            if(i+1 == bitsno) i = 0;
        }
    }else{
        count = 0;
        postorderKill(my_combat.combat_s,my_combat.soldier_cnt * 0.4);
        count = 0;
    }
    printCombatAlive();
    return 1;
}


/**
 * @brief Cease fire
 * 
 * @return 1 on success
 *         0 on failure
 */
void deleteFromRegistration(int sid){
    int index = h(sid);
    struct soldier* p = registration_hashtable[index],*prev = NULL;
    
    while(p!=NULL){
        if(p->sid = sid){
            if(prev!=NULL) prev->next = p->next;
            else registration_hashtable[index] = p->next;
            return;
        }
        prev=p;
        p=p->next;
    }
    return;
}

void deleteFromGeneral(struct TREE_soldier* soldier, struct general* general){
    if(soldier->sid == -1) return;
    struct TREE_soldier* tmp = soldier;
    general ->soldiers_no --;
    if(soldier->lc == general->soldiers_S && soldier->rc == general->soldiers_S){
        if(soldier->p->rc == soldier)
            soldier->p->rc = general->soldiers_S;
        else soldier->p->lc = general->soldiers_S;

    }else if(soldier->lc == general->soldiers_S) {
        if(soldier->p->rc == soldier){
            soldier->p->rc = soldier->rc;
            soldier->rc->p = soldier->p;
        }else{
            soldier->p->lc = soldier->rc;
            soldier->rc->p = soldier->p;
        }
    }else if(soldier->rc == general->soldiers_S){
        if(soldier->p->rc == soldier){
            soldier->p->rc = soldier->lc;
            soldier->lc->p = soldier->p;
        }else{
            soldier->p->lc = soldier->lc;
            soldier->lc->p = soldier->p;
        }
    }else{
        tmp = inorder_successor(soldier);
        soldier->sid= tmp->sid;
        deleteFromGeneral(tmp,general);
        return;
    }
    free(tmp);
}

struct TREE_soldier* LookUp(int sid, struct general* general){
    if(general == NULL) return NULL;
    struct TREE_soldier* p = general->soldiers_R;
    while(p->sid != -1){
        if(p->sid < sid){
            p=p->rc;
        }else if(p->sid > sid){
            p = p->lc;
        }else if (p->sid == sid){
            return p;
        }
    }
    return NULL;
}



int deleteFromCombat(struct c_soldier* soldier, struct c_soldier* prev){
    if(soldier== NULL) return;
    struct c_soldier* tmp = soldier;

    if(soldier->lc == NULL && soldier->rc == NULL){
        if(prev == NULL){ my_combat.combat_s = NULL; 
        }else{
            if(prev->rc == soldier) prev->rc = NULL;
            else prev->lc = NULL;
        }
    }else if(soldier->lc == NULL) {
        if(prev == NULL){ my_combat.combat_s = soldier->rc; 
        }else{
            if(prev->rc == soldier) prev->rc = soldier->rc;
            else prev->lc = soldier->rc;
        }
    }else if(soldier->rc == NULL){
        if(prev == NULL){ my_combat.combat_s = soldier->lc; 
        }else{
            if(prev->rc == soldier) prev->rc = soldier->lc;
            else prev->lc = soldier->lc;
        }
    }else{
        prev = tmp;
        tmp = tmp->rc;
        while(tmp->lc != NULL){
            prev = tmp;
            tmp = tmp->lc;
        }
        soldier->sid= tmp->sid;
        soldier->alive = tmp->alive;
        deleteFromCombat(tmp,prev);
        if(tmp->alive == 0 ) return 1;
        else return 0;
    }
    free(tmp);
    return 0;
}

void clear(struct c_soldier* node,struct c_soldier* prev){
    if(node == NULL) return;
    clear(node->lc,node);    
    clear(node->rc,node);
    
    if(node->alive) return;
    deleteFromRegistration(node->sid);
    struct general* g = findGen(node->gid);
    struct TREE_soldier* soldier = LookUp(node->sid,g);
    deleteFromGeneral(soldier , g);
    
    while(deleteFromCombat(node,prev));
}

int cease_fire() {
    clear(my_combat.combat_s,NULL);
    print_generals();
    print_registration_hashtable();
    printCombatAlive();
    return 1;
}



/**
 *Print functions
 *
 */


void inorderTree(struct TREE_soldier* node){
    if(node == NULL) return;
    if(node->sid == -1) return;
    inorderTree(node->lc); 
    if(count%10 == 0) printf("\n\t   ") ;
    count++;
    printf("<%d>,",node->sid);
    inorderTree(node->rc);
}


int print_generals_soldiers(int gid) {
    count = 0;
    inorderTree(findGen(gid)->soldiers_R);
    count =0;
    printf("\n");
    return 1;
}

int print_generals(){
	struct general* glist= generals_list;
	if(glist == NULL) return 0;
    printf("\n\tGENERALS:");
	while( glist != NULL){
        printf("\n\t<%d>: ",glist->gid);
        count = 0;
        if(glist->soldiers_R !=NULL) inorderTree(glist->soldiers_R);
        printf("\n");
        glist = glist->next;
    }
    printf("\n");
    count = 0;
    return 1;
}


int print_generalsList(){
	struct general* glist= generals_list;
	if(glist == NULL) return 0;
	printf("\n\tGENERALS:");
	int count = 1 ;
	while( glist != NULL){
	    printf("<%d> ",glist->gid);
		if((count % 10) == 0) printf("\n\t\t ");	
		glist = glist->next;	
		count++;
    }
	printf("\n");	
}

int print_registration_hashtable() {
    if(registration_hashtable == NULL) return 0;
    int i;
    count = 0;
    struct soldier * list;
    printf("\tRegistration Hashtable:\n");
    for(i=0;i<m;i++){
        list = registration_hashtable[i];
        printf("\t %d : ",i);
        while(list != NULL){
            printf("<%d:%d> ",list->sid,list->gid);
            if(count % 10) printf("\n\t    ");
            list= list->next;
        }
        printf("\n");
    }
    count = 0;

    return 1;
}

/*functions that print combat list nodes*/

void inorderC(struct c_soldier* node,int printalive){
    if(node == NULL) return ;
    inorderC(node->lc,printalive); 
    if(count%10 == 0) printf("\n\t   ") ;
    count++;
    if(printalive) printf("<%d:%d>,",node->sid,node->alive);
    else printf("<%d>,", node->sid);
    inorderC(node->rc,printalive);
}

int printCombat(){

	if(my_combat.combat_s == NULL) return 0;
    printf("\n\tCombat soldiers:");
    count = 0;
    inorderC(my_combat.combat_s,0);
    printf("\n");
    count = 0;
    return 1;
}
int printCombatAlive(){
	if(my_combat.combat_s == NULL) return 0;
    printf("\n\tCombat soldiers:");
    count = 0;
    inorderC(my_combat.combat_s,1);
    printf("\n");
    count = 0;
    return 1;
}

/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */
int free_world() {
    return 1;
}


/**
 * @brief The main function
 *
 * @param argc Number of arguments
 * @param argv Argument vector
 *
 * @return 0 on success
 *         1 on failure
 */
int main(int argc, char** argv)
{
    FILE *fin = NULL;
    char buff[BUFFER_SIZE], event;

    /* Check command buff arguments */
    if ( argc != 2 ) {
        fprintf(stderr, "Usage: %s <input_file> \n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Open input file */
    if (( fin = fopen(argv[1], "r") ) == NULL ) {
        fprintf(stderr, "\n Could not open file: %s\n", argv[1]);
        perror("Opening test file\n");
        return EXIT_FAILURE;
    }

    /* Read input file buff-by-buff and handle the events */
    while ( fgets(buff, BUFFER_SIZE, fin) ) {

        DPRINT("\n>>> Event: %s", buff);

        switch(buff[0]) {

            /* Comment */
            case '#':
                break;

                /* Maximum soldiers */
            case '0':
                {
                    sscanf(buff, "%c %d", &event, &max_soldiers_g);
                    DPRINT("%c %d\n", event, max_soldiers_g);

                    initialize();

                    break;
                }

                /* Register soldier
                 * R <sid> <gid> */
            case 'R':
                {
                    int sid, gid;

                    sscanf(buff, "%c %d %d", &event, &sid, &gid);
                    DPRINT("%c %d %d\n", event, sid, gid);

                    if ( register_soldier(sid, gid) ) {
                        DPRINT("R %d %d succeeded\n", sid, gid);
                    } else {
                        fprintf(stderr, "R %d %d failed\n", sid, gid);
                    }

                    break;
                }

                /* General or king joins the battlefield
                 * G <gid> */
            case 'G':
                {
                    int gid;

                    sscanf(buff, "%c %d", &event, &gid);
                    DPRINT("%c %d\n", event, gid);

                    if ( add_general(gid) ) {
                        DPRINT("%c %d succeeded\n", event, gid);
                    } else {
                        fprintf(stderr, "%c %d failed\n", event, gid);
                    }

                    break;
                }

                /* Distribute soldier
                 * D */
            case 'D':
                {
                    sscanf(buff, "%c", &event);
                    DPRINT("%c\n", event);

                    if ( distribute() ) {
                        DPRINT("%c succeeded\n", event);
                    } else {
                        fprintf(stderr, "%c failed\n", event);
                    }

                    break;
                }

                /* General resigns from battle
                 * M <gid1> <gid2> */
            case 'M':
                {
                    int gid1, gid2;

                    sscanf(buff, "%c %d %d", &event, &gid1, &gid2);
                    DPRINT("%c %d %d\n", event, gid1, gid2);

                    if ( general_resign(gid1, gid2) ) {
                        DPRINT("%c %d %d succeeded\n", event, gid1, gid2);
                    } else {
                        fprintf(stderr, "%c %d %d failed\n", event, gid1, gid2);
                    }

                    break;
                }

                /* Prepare for battle
                 * P <gid1> <gid2> */
            case 'P':
                {
                    int gid1, gid2;
                    sscanf(buff, "%c %d %d", &event, &gid1, &gid2);
                    DPRINT("%c %d %d\n", event, gid1, gid2);

                    if ( prepare_battle(gid1, gid2) ) {
                        DPRINT("%c %d %d succeeded\n", event, gid1, gid2);
                    } else {
                        fprintf(stderr, "%c %d %d failed\n", event, gid1, gid2);
                    }

                    break;
                }

                /* Battle
                 * B <god_favor> <bit_stream> */
            case 'B':
                {
                    int god_favor;
                    char bit_stream[9];

                    sscanf(buff, "%c %d %s\n", &event, &god_favor, bit_stream);
                    DPRINT("%c %d %s\n", event, god_favor, bit_stream);

                    if ( battle(god_favor, bit_stream, 8) ) {
                        DPRINT("%c %d %s succeeded\n", event, god_favor, bit_stream);
                    } else {
                        fprintf(stderr, "%c %d %s failed\n", event, god_favor, bit_stream);
                    }

                    break;
                }

                /* Cease fire
                 * U */
            case 'U':
                {
                    sscanf(buff, "%c", &event);
                    DPRINT("%c\n", event);

                    if ( cease_fire() ) {
                        DPRINT("%c succeeded\n", event);
                    } else {
                        fprintf(stderr, "%c failed\n", event);
                    }

                    break;
                }

                /* Print general's soldiers
                 * W <gid> */
            case 'W':{
                    int gid;

                    sscanf(buff, "%c %d", &event, &gid);
                    DPRINT("%c %d\n", event, gid);
                    printf("\tSoldier Tree");
                    if ( print_generals_soldiers(gid) ) {
                        DPRINT("%c %d succeeded\n", event, gid);
                    } else {
                        fprintf(stderr, "%c %d failed\n", event, gid);
                    }

                    break;
                }

                /* Print generals
                 * X */
            case 'X':
                {
                    sscanf(buff, "%c", &event);
                    DPRINT("%c\n", event);

                    if ( print_generals() ) {
                        DPRINT("%c succeeded\n", event);
                    } else {
                        fprintf(stderr, "%c failed\n", event);
                    }

                    break;
                }

                /* Print registration hashtable
                 * Y */
            case 'Y':
                {
                    sscanf(buff, "%c", &event);
                    DPRINT("%c\n", event);

                    if ( print_registration_hashtable() ) {
                        DPRINT("%c succeeded\n", event);
                    } else {
                        fprintf(stderr, "%c failed\n", event);
                    }

                    break;
                }

                /* Empty line */
            case '\n':
                break;

                /* Ignore everything else */
            default:
                DPRINT("Ignoring buff: %s \n", buff);

                break;
        }
    }
    free_world();
    return (EXIT_SUCCESS);
}