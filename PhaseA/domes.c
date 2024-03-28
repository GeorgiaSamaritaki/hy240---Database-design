
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
#endif /* DEBUG*/



	/**
	* @brief Optional function to initialize data structures that 
	* need initialization
	*
	* @return 1 on success
	*         0 on failure
	*/
	int initialize() {
		register_soldier(-1,-1);
		registration_sentinel = registration_list;
		add_general(-1);
		general_sentinel = generals_list;
    	return 1;
    }
	
	     
	/**
	 * @brief Print generals
	 *
	 * @return 1 on success
	 *         0 on failure
	 */
	int print_generalList() {
		printf("\n\tGENERALS:");
		struct general* glist= generals_list;

		if(glist == NULL) return 0;
		
		
		while( glist->next != NULL){
			printf("\n\t<%d>:",glist->gid);
			struct DDL_soldier *slist = glist->soldiers_head;
			int count = 1 ;
			while(slist!=NULL){
				printf(" <%d>",slist->sid);
				if((count % 10) == 0) printf("\n\t   ");
				slist = slist->next;
				count++;
			}
			glist = glist->next;	
		}
		printf("\n");
		return 1;
	}
	
	int print_generals(){
		printf("\n\tGENERALS:");
		struct general* glist= generals_list;
		if(glist == NULL) return 0;

		int count = 1 ;
		while( glist != general_sentinel){
			printf("<%d> ",glist->gid);
			if((count % 10) == 0) printf("\n\t\t ");	
			glist = glist->next;	
			count++;
		}
		printf("\n");
			
	}
        
	/**
	 * @brief Print registration list
	 *
	 * @return 1 on success
	 *         0 on failure
	 */
	 int print_registration_list() {

		printf("\n\tRegistration list= ");
		struct soldier * temp= registration_list;
		
		if(temp == NULL) return 0;

		int count = 1; 
		while( temp->next != NULL){
			printf("<%d:%d>, ",temp->sid,temp->gid);
			if((count % 10) == 0) printf("\n\t    ");
			temp = temp->next;
			count++;
		}
		printf("\n");
		
		return 1;
	}

	/**
	 * @brief Print combat list created in P event
	 *
	 * @return 1 on success
	 *         0 on failure
	 */
	 int printCsoldiers(){
		struct c_soldier * list =  my_combat.combat_s;
		
		if(list==NULL) return 0;

		printf("\n\tCombat soldiers: ");
		int count = 1;
		while(list!=NULL){
			printf("<%d> ",list->sid);	
			if((count % 10) == 0) printf("\n\t    ");
			list = list->next;
			count++;
		}

		printf("\n");
		return 1;
	 }

	 int printCsoldiersStatus(){
		struct c_soldier * list =  my_combat.combat_s;
		if(list==NULL) return 0;
		printf("\n\tCombat soldiers: ");

		int count = 1;
		while(list!=NULL){
			printf("<%d:%d> ",list->sid,list->alive);
			if((count % 10) == 0) printf("\n\t    ");
			list = list->next;
			count++;
		}
		printf("\n");
		return 1;
	 }

	/**Registers a soldier at the start of registration_list
	 *
	 * @brief Register soldier
	 *
	 * @param sid The soldier's id
     * @param gid The general's id who orders the soldier
 	 * @return 1 on success
	 *         0 on failure
	 */

	int register_soldier (int sid, int gid) {
		struct soldier *temp = (struct soldier *) malloc(sizeof(struct soldier));
		if(temp == NULL) return 0;
		
		temp->sid = sid;
		temp->gid = gid;
		temp->next= registration_list;
		registration_list = temp;
		return 1;
	}

	/**
	 * @brief General or King joins the battlefield
	 *
     * @param gid The general's id
	 * @return 1 on success
	 *         0 on failure
	 */
	 int add_general(int gid) {
	 	struct general * temp = (struct general *)malloc(sizeof(struct general));
		if(temp==NULL) return 0;
		
		temp->soldiers_head=NULL; 
		temp->soldiers_tail=NULL; 
		temp->gid = gid;
		temp->combats_no = 0;
		temp->next= generals_list;
		generals_list =  temp; 	
		return 1;
	}
	

	/*Converts a soldier struct to a DDL_soldier*/
	struct DDL_soldier * SoldierToDDL(struct soldier * old){
		struct DDL_soldier * new=  (struct DDL_soldier *)malloc(sizeof(struct DDL_soldier));
		new->sid=old->sid;
		return new;
	}

	/*Converts a DDLsoldier struct to a combatSoldier*/
	struct c_soldier * DDltoCombatSoldier(struct DDL_soldier * old,int gid){
		struct c_soldier * new = (struct c_soldier *)malloc(sizeof(struct c_soldier));
		new-> sid = old->sid;
		new -> gid = gid;
		new->alive = 1;
		return new;
	}

	/*removes a node from a double linked list with O(1)*/
	 void removeDDLnode(struct DDL_soldier* node, struct general * gen){
		
		/*if its the first node*/
		if(node == gen->soldiers_head && node == gen->soldiers_tail){
			gen->soldiers_head = NULL;
			gen->soldiers_tail = NULL;
		}else if(node == gen->soldiers_head) {
			gen->soldiers_head = node->next;
			node->next->prev = NULL;
		}else if(node == gen->soldiers_tail){
			gen->soldiers_tail = node->prev;
			node->prev->next = NULL;
		}else{
			node->next->prev = node	-> prev;
		}

		node->next = NULL;
		node->prev = NULL;
		free(node);			
	}	

	void Reset_Combat(){
		if(my_combat.combat_s == NULL){
			my_combat.combat_s  = (struct c_soldier *)malloc(sizeof(struct c_soldier));
			return;
		}
		struct c_soldier * temp = my_combat.combat_s;
		struct c_soldier * next = my_combat.combat_s;
		while(temp!=NULL){
			next = temp->next;
			free(next);		
			temp = next;	
		}
		
		my_combat.soldier_cnt = 0;

	}

	void InsertSoldier(struct DDL_soldier * newAdd, struct general* gen){
		
		if(gen->soldiers_head == NULL){
			gen->soldiers_head = newAdd;
			gen->soldiers_tail = newAdd;
			return;
		}
	
		struct DDL_soldier* first = gen->soldiers_head;
		struct DDL_soldier* last = gen->soldiers_tail;
		
	
		/*soldier has to be placed first*/
		if(first->sid > newAdd->sid){
			newAdd->next = first;
			first->prev = newAdd;
			gen->soldiers_head = newAdd;
		
		/*soldier has to be placed last*/
		}else if(last->sid < newAdd->sid){
			last->next = newAdd;
			newAdd->prev= last;
			gen->soldiers_tail = newAdd;
	
		/*soldier is to be placed somewhere inbetween*/
		}else{
			while(first->sid < newAdd->sid){
				first = first->next;
			}
			newAdd->prev = first->prev;
			first->prev ->next = newAdd;
			newAdd->next = first;
			first->prev = newAdd;
		}
	
	}

	struct general* findGen(int gid){

		general_sentinel ->gid = gid;
		
		struct general* g= generals_list;
		while( g->gid != gid){
				g = g->next;
		}
			
		if(g==general_sentinel)	g == NULL;

		general_sentinel ->gid = -1;
		return g;
	}
	/**
	 * Changes the gid field of soldiers with gid1 to gid2
	 *
	 **/
	void ChangeRegistration(int gid1, int gid2){
		struct soldier * tmp =  registration_list;

		while(tmp!= registration_sentinel){
			if(tmp->gid == gid1){
				tmp->gid = gid2;
			}
			tmp = tmp->next;
		}
	}
	
	/**
	 * @bried Removes registry with given sid from registration list
	 * 
	 * @param sid The sid we want to delete
	 * @return 1 on success
	 *         0 on failure
	 * */

	int removeRegistry(int sid){
		struct soldier * tmp =  registration_list;
		struct soldier * prev = NULL;

		while(tmp!= registration_sentinel){
			if(tmp->sid = sid){
				if(prev != NULL ) prev->next = tmp -> next;
				else registration_list = tmp->next;
				tmp->next = NULL;
				free(tmp);
				return 1;
			}
			prev = tmp;
			tmp = tmp->next;
		}
		return 0;
	}

	/**
	 * @brief Distribute soldiers to the camps
     * 
	 * @return 1 on success
	 *         0 on failure
	 */

	int distribute() {
		struct soldier * list= registration_list;
		if (list == NULL) return 0;
	
		/* Find general - Generate Node - InsertNode*/
		while( list != registration_sentinel){ 
			struct general* gen = findGen(list -> gid);
			struct DDL_soldier * newInsert =  SoldierToDDL(list);
			if (newInsert == NULL ) return 0; /*malloc didnt succeed*/

			InsertSoldier(newInsert , gen);		
			list = list->next;
		}
		
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
	
	int general_resign(int gid1, int gid2) {
		struct general* gid_1 = findGen(gid1); 
		struct general* gid_2 = findGen(gid2);

		if(gid_2->soldiers_head==NULL){
			gid_2->soldiers_head = gid_1->soldiers_head;
			gid_2->soldiers_tail = gid_1->soldiers_tail;
			gid_1->soldiers_head = NULL;
			gid_1->soldiers_tail = NULL;
		}

		struct DDL_soldier* list= gid_1->soldiers_head;
		struct DDL_soldier* list2= gid_2->soldiers_head;

		struct DDL_soldier* nextInsert = NULL;

		/* Move each node of list to list2 traversing both list and list2 once ~> O(list.length + list2.length)*/
		
		while(list != NULL){
			/*Next pointer will change because node will change list*/
			nextInsert = list->next;  

			/*find the position in list2 */
			while(list2->sid < list->sid && list2->next !=NULL){
				list2 = list2->next;
			}

			/*case1 : Node has to be placed at the end of list2*/
			if(list2->sid < list->sid && list2->next == NULL){
				list2->next = list;
				list->prev = list2;
				gid_2->soldiers_tail = gid_1 ->soldiers_tail;
				break; 
			/*case2 : Node has to be placed in the begging of list2*/
			}else if(list2->prev == NULL){
				list->next = list2;
				list->prev = NULL;
				gid_2->soldiers_head = list;
				
			/*case3: Node has to be placed somewhere in the middle*/
			}else{
				list->next = list2;
				list->prev = list2->prev;
				list2->prev->next =list;
				list2->prev = list;
			}
			/*keep pointer of list2 at the last insert*/
			list2 = list;
			list = nextInsert;
		}

		ChangeRegistration(gid_1->gid, gid_2 ->gid);
		/* Delete gid_1 node */
		struct general* g = generals_list;
		struct general* p = g;
		while(g!=NULL){
			if(g->gid == gid_1->gid) break;
			p=g;
			g = g->next;
		}
		if(p==g){
			generals_list = generals_list->next;
		} else {
			p->next = g->next;
		}
		gid_1->next =NULL;
		free(gid_1);
		return 1;
	}	
	
	/**
	 * @brief Prepare for battle
     * 
	 * @param gid1 The id of the first general
     * @param gid2 The id of the second general
     * @param gid3 The id of the third general
      * 
	 * @return 1 on success
	 *         0 on failure
	 */
	int prepare_battle(int gid1, int gid2, int gid3) {

		struct general* gid_1 = findGen(gid1); 
		struct general* gid_2 = findGen(gid2);
		struct general* gid_3 = findGen(gid3);

		gid_1->combats_no+=1;
		gid_2->combats_no+=1;
		gid_3->combats_no+=1;

		/*pointers beggining of lists ~ start1*/
		struct DDL_soldier * s1 = gid_1 ->soldiers_head;
		struct DDL_soldier * s2 = gid_2 ->soldiers_head;
		struct DDL_soldier * s3 = gid_3 ->soldiers_head;

		/*pointers starting at the end of each list ~ end1*/	
		struct DDL_soldier * e1 = gid_1 ->soldiers_tail;
		struct DDL_soldier * e2 = gid_2 ->soldiers_tail;
		struct DDL_soldier * e3 = gid_3 ->soldiers_tail;

		if(gid_1 == NULL || gid_2 == NULL || gid_3 == NULL)  return 0;
		
		/*first node of combat_s*/
		Reset_Combat();
		
		struct c_soldier * cur = my_combat.combat_s; /*first place will not be used and will be deleted after loop*/
		
		while(!(s1 == NULL && s2 == NULL && s3 == NULL)){
			
			/*begining elemnts */			
			if(s1!=NULL){
				cur->next = DDltoCombatSoldier(s1,gid_1->gid);
				my_combat.soldier_cnt += 1;
				cur = cur->next;
				s1 = s1->next;
			}
			if(s2!=NULL){
				cur->next = DDltoCombatSoldier(s2,gid_2->gid);
				my_combat.soldier_cnt += 1;
				cur = cur->next;
				s2 = s2->next;
			}
			if(s3!=NULL){
				cur->next = DDltoCombatSoldier(s3,gid_3->gid);
				my_combat.soldier_cnt += 1;
				cur = cur->next;
				s3 = s3->next;
			}
			
			/*ends of lists*/	
			if(s1!=NULL){
				cur->next = DDltoCombatSoldier(e1,gid_1->gid);
				my_combat.soldier_cnt += 1;
				cur = cur->next;
				e1 = e1->prev;
			}
			if(s2!=NULL){
				cur->next = DDltoCombatSoldier(e2,gid_2->gid);
				my_combat.soldier_cnt += 1;
				cur = cur->next;
				e2 = e2->prev;
			}
			if(s3!=NULL){
				cur->next = DDltoCombatSoldier(e3,gid_3->gid);
				my_combat.soldier_cnt += 1;
				cur = cur->next;
				e3 = e3->prev;
			}	
			
			/* Checks */
			if(s1 != NULL){
				if(s1== e1){
					cur->next = DDltoCombatSoldier(s1,gid_1->gid);
					my_combat.soldier_cnt += 1;
					cur = cur->next;
					s1=NULL;
				}else if(s1->prev == e1){
					s1=NULL;
				} 
			}
			if(s2!=NULL){
				if(s2 == e2){
					cur->next = DDltoCombatSoldier(s2,gid_2->gid);
					my_combat.soldier_cnt += 1;
					cur = cur->next;
					s2=NULL;
				}else if(s2->prev == e2){
					s2=NULL;
				}
			}
			if(s3 != NULL){
				if(s3 == e3){
					cur->next = DDltoCombatSoldier(s3,gid_3->gid);
					my_combat.soldier_cnt += 1;
					cur = cur->next;
					s3=NULL;
				}else if(s3->prev == e3){
					s3=NULL;
				}
			}
		}

		cur = my_combat.combat_s->next;
		my_combat.combat_s->next = NULL;
		free( my_combat.combat_s);
		my_combat.combat_s = cur;
		return 1;
	}
        
	/**
	 * @brief Battle
     * 
     * @param god_favor 
     *
	 * @return 1 on success
	 *         0 on failure
	 */
	int battle(int god_favor) {
		int i,count = 0 ;
		struct c_soldier * soldier = my_combat.combat_s;
		switch(god_favor){

			case 0: {/* 40% of the army is lost ~ first 40% of soldiers in list die */
				count =(int) (my_combat.soldier_cnt * 40)/100;	
				for(i = 0; i<count ; i++){
					soldier ->alive = 0 ;
					soldier = soldier->next;
				}
				break;
			}
			case 1:{ /* one every ten dies */
				while(soldier!=NULL){
					if(i % 10 == 0){
						soldier->alive = 0;
					}
					i++;
					soldier = soldier->next;
				}
				break;
			}
			default:
			{
				return 0;
			}
		}
		return 1;
	}
	

	/**
	 * @brief Cease fire
     * 
	 * @return 1 on success
	 *         0 on failure
	 */
	int cease_fire() {
		if(generals_list == NULL) return 0;
		/* take three first soldiers gid to find the three generals that took part in battle*/
		struct c_soldier * rlist = my_combat.combat_s;
		if(rlist == NULL) return 0;
				
		int gid1 = rlist->gid;
		if(rlist->next!=NULL)rlist = rlist->next;
		int gid2 = rlist->gid;
		if(rlist->next!=NULL)rlist = rlist->next;
		int gid3 = rlist->gid;
		
		struct general* gid_1 = findGen(gid1);
		struct general* gid_2 = findGen(gid2);
		struct general* gid_3 = findGen(gid3);

		/*6 pointers to heads and tails of each general*/
		struct DDL_soldier * s1 = gid_1 ->soldiers_head;
		struct DDL_soldier * s2 = gid_2 ->soldiers_head;
		struct DDL_soldier * s3 = gid_3 ->soldiers_head;

		struct DDL_soldier * e1 = gid_1 ->soldiers_tail;
		struct DDL_soldier * e2 = gid_2 ->soldiers_tail;
		struct DDL_soldier * e3 = gid_3 ->soldiers_tail;
		
		if(gid_1 == gid_2){
			s2 = (struct DDL_soldier *)malloc(sizeof(struct DDL_soldier));
			s2->sid = -1;
			e2 = s2;
			s3 = s2;
			e3 = s2;
		}else if(gid_1 == gid_3){
			s3 = (struct DDL_soldier *)malloc(sizeof(struct DDL_soldier));
			s3->sid = -1;
			e3 = s3;
		}
		
		/*reset pointer to start*/
		rlist = my_combat.combat_s;

		struct c_soldier * rlistNext = rlist; /*is needed because nodes will be lost */
		struct DDL_soldier *toDelete;
		struct general* gid;

		while(rlist!=NULL){
			rlistNext = rlist->next;
			/*find in which of 6 pointers is the soldier of combat_s
				save the general that he was assigned to and the pointer to the soldier in the general's list*/
			if (rlist->sid == s1->sid){
				toDelete = s1;
				if(s1->next != NULL) s1 = s1->next;
				gid = gid_1;	
			}else if(rlist->sid == e1->sid){
				toDelete = e1;
				if(e1->prev != NULL) e1 = e1->prev;
				gid = gid_1;
			}else if(rlist->sid == s2->sid){
				toDelete = s2;
				if(s2->next != NULL) s2 = s2->next;
				gid = gid_2;
			}else if(rlist->sid == e2->sid){
				toDelete = e2;
				if(e2->prev != NULL) e2 = e2->prev;
				gid = gid_2;		
			}else if(rlist->sid == s3->sid){
				toDelete = s3;
				if(s3->next != NULL) s3 = s3->next;
				gid = gid_3;
			}else if(rlist->sid == e3->sid){
				toDelete = e3;
				if(e3->prev != NULL) e3 = e3->prev;
				gid = gid_3;
			}
			/*remove node from gid list */
			if( rlist->alive == 0 ){
				removeDDLnode(toDelete , gid);
				
				/*remove node from registration list*/
				removeRegistry(rlist->sid);
			}
			/*remove node from combat list */
			rlist->next = NULL;
			free(rlist);


			/*continue with next soldier*/
			rlist = rlistNext;
		}
		if(s3->sid == -1) free(s3);

		/*reset combat list*/
		my_combat.combat_s = NULL;
		my_combat.soldier_cnt = 0;
		return 1;
	}

	
	/**
	 * @brief Trojan Horse
	 *
	 * @return 1 on success
	 *         0 on failure
	 */
	int trojan_horse() {
		struct general * array[5];
		struct general * g = generals_list;
		int i = 0;

		struct general * min = g;
		int minposition;
		
		for(i=0 ; i <5 ; i++){
			array[i]= g;

			if(g->combats_no < min->combats_no ) {
				min = g; 
				minposition = i;
			}
			
			if(g->next == NULL){
				printf("\nGeneral= ");
				int temp = i;
				for(i=0 ; i <temp ; i++){
					printf(" <%d:%d>, ", array[i]->gid,array[i]->combats_no);
				}
				printf("\n");
				return 1;
			}
			g=g->next;
		}

		g = generals_list;
		while ( g != NULL){
			if( g->combats_no > min->combats_no ){
				array[minposition] = g;
				min = g;
				for(i=0 ; i <5 ; i++){
					if(array[i]->combats_no < min->combats_no ) {
						min = array[i]; 
						minposition = i;
					}
				}
			}
		
			g = g->next;
		}

		printf("\nGeneral= ");
		for(i=0 ; i <5 ; i++){
			printf(" <%d:%d>, ", array[i]->gid,array[i]->combats_no);
		}
		printf("\n");
		
		return 1;
	}
   

	int free_world() {
		return 1;
	}


/**
 * 
 * @brief The main function
 *
 * @param argc Number of arguments
 * @param argv Argument vector
 *
 * @return 0 on success
 *         1 on failure
 */

 int main(int argc, char** argv){
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

	initialize();

	/* Read input file buff-by-buff and handle the events */
	while ( fgets(buff, BUFFER_SIZE, fin) ) {

		DPRINT("\n>>> Event: %s", buff);

		switch(buff[0]) {

		/* Comment */
		case '#':
			break;

		/* Register soldier
		 * R <sid> <gid> */
		case 'R':{
			int sid, gid;
			sscanf(buff, "%c %d %d", &event, &sid, &gid);
			DPRINT("%c %d %d\n", event, sid, gid);

			if ( register_soldier(sid, gid) ) {
				DPRINT("R %d %d succeeded\n", sid, gid);
				
			} else {
				fprintf(stderr, "R %d %d failed\n", sid, gid);
			}
			print_registration_list();
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
			
			print_generals();

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
			print_generalList();
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
			print_generalList();
			/*print_generals_backwards();*/
			break;
		}

		/* Prepare for battle
		 * P <gid1> <gid2> <gid3> */
		case 'P':
		{
			int gid1, gid2, gid3;
			sscanf(buff, "%c %d %d %d", &event, &gid1, &gid2, &gid3);
			DPRINT("%c %d %d %d\n", event, gid1, gid2, gid3);
			
			if ( prepare_battle(gid1, gid2, gid3) ) {
				DPRINT("%c %d %d %d succeeded\n", event, gid1, gid2, gid3);
			} else {
				fprintf(stderr, "%c %d %d %d failed\n", event, gid1, gid2, gid3);
			}
			printCsoldiers();
			break;
		}

		/* Battle
		 * B <god_favor> */
		case 'B':
		{
			int god_favor;
			sscanf(buff, "%c %d\n", &event, &god_favor);
			DPRINT("%c %d\n", event, god_favor);

			if ( battle(god_favor) ) {
				DPRINT("%c %d succeeded\n", event, god_favor);
			} else {
				fprintf(stderr, "%c %d failed\n", event, god_favor);
			}
			printCsoldiersStatus();
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
			print_generalList();
			break;
		}

		/* Trojan Horse
		 * T */
		case 'T':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if ( trojan_horse() ) {
				DPRINT("%c succeeded\n", event);
			} else {
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}

		/* Print generals
		 * X */
		case 'X':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if ( print_generalList() ) {
				DPRINT("%c succeeded\n", event);
			} else {
				fprintf(stderr, "%c failed\n", event);
			}

			break;
		}

		/* Print registration list
		 * Y */
		case 'Y':
		{
			sscanf(buff, "%c", &event);
			DPRINT("%c\n", event);

			if ( print_registration_list() ) {
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
