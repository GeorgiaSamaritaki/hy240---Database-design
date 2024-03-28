/********************************************************//**
 * @file   troy.h                                      	    *
 * @author Nikolaos Batsaras <batsaras@csd.uoc.gr>  	    *
 *                                                    	    *
 * @brief Header file for the needs of cs-240b project 2017 *
 ***********************************************************/

#ifndef TROY_H_
#define TROY_H_


struct soldier {
    int sid;
    int gid;
    struct soldier *next;
};

struct TREE_soldier {
    int sid;
    struct TREE_soldier *rc;
    struct TREE_soldier *lc;
    struct TREE_soldier *p;
};

struct general {
    int gid;
    int combats_no;
    int soldiers_no;
    struct TREE_soldier *soldiers_R;
    struct TREE_soldier *soldiers_S;
    struct general *next;
};

struct c_soldier {
    int sid;
    int gid;
    int alive;
    int left_cnt;
    struct c_soldier *rc;
    struct c_soldier *lc;
};

struct combat {
    int soldier_cnt;
    struct c_soldier *combat_s;
};


/* global, the maximum number of soldiers */
unsigned int max_soldiers_g;

/* global, the registration hashtable */
struct soldier **registration_hashtable;

/* global, the generals list*/
struct general *generals_list;

/* global, the combat record */
struct combat my_combat;

#endif /* TROY_H_ */

