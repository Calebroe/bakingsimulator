#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_RECIPES 5
#define NUM_INGREDIENTS 9
#define MAX_BAKERS 10

/* Semaphore declarations */
sem_t pantry_sem, fridge_sem, mixer_sem, bowl_sem, spoon_sem, oven_sem;

/* Shared memory declarations */
int *pantry;
int *fridge;
int mixer = 2;
int bowl = 3;
int spoon = 5;
int oven = 1;

/*check for baker ramsied flag per user*/
Boolean ramsiedBaker = false; //flag check for ramsied baker option;

/* Recipe data */
const char *recipe_names[NUM_RECIPES] = {"Cookies", "Pancakes", "Pizza Dough", "Soft Pretzels", "Cinnamon Rolls"};

/* Baker data */
struct Baker {
    int id;
    int currentRecipe;
};

struct Baker *bakers;

/* Function prototypes */
void *baker_thread(void *arg);
void acquire_ingredients(int recipeID, int baker_id);//function that has a switch case to print ingredients in pantry and a check for if it needs access to fridge ingredients
void use_kitchen_resources(); //function for using the kitchen resources
void use_oven_resource(); //function for using the oven resource
void release_kitchen_resources();//function to release kitchen resources
void release_oven_resources(); //function to release oven resource
void bake_recipe(int recipe_id); //function to bake 
void getStartingRecipe(struct baker *bakerPtr); //used to get a random starting recipe

* Signal handler for cleanup */
void cleanup_handler(int sig);

int main() {
    int i, j, num_ingredients;

    /* Initialize semaphores */
    sem_init(&pantry_sem, 0, 1);
    sem_init(&fridge_sem, 0, 2);
    sem_init(&mixer_sem, 0, mixer);
    sem_init(&bowl_sem, 0, bowl);
    sem_init(&spoon_sem, 0, spoon);
    sem_init(&oven_sem, 0, oven);

    /* Prompt user for number of bakers */
    int num_bakers;
    printf("Enter number of bakers (max %d): ", NUM_BAKERS);
    scanf("%d", &num_bakers);

    char isRamsied; //temp char  for getting user input
    printf("Do you want bakers to be randomized? (Y/N): ");
    scanf(" %c", &isRamsied);

    if (isRamsied == 'Y') {
        ramsiedBaker = true;
        printf("Bakers now have a chance to be ramsied");    
    } else {
        printf("Bakers will not have a chance to be ramsied");    
    }

    //pantry = (int *)malloc(num_ingredients * sizeof(int));
    //fridge = (int *)malloc(num_ingredients * sizeof(int));
    //for (i = 0; i < num_ingredients; i++) {
    //    pantry[i] = 2; // start with 2 of each ingredient
    //    fridge[i] = 3; // start with 3 of each ingredient
    }

    /* Initialize recipe data */
    //recipe_ingredients = (int **)malloc(NUM_RECIPES * sizeof(int *));
    //for (i = 0; i < NUM_RECIPES; i++) {
    //    recipe_ingredients[i] = (int *)malloc(num_ingredients * sizeof(int));
    //}

    void *baker_thread(void *arg) { //we need to heavily modify this function so that we start with a currentrecipeID, then during the while loop, we increment whatever the value is modulo 5 until we hit the starting recipeID again and quit.
    struct Baker *baker = (struct Baker *)arg;
    int i, recipe_id;
    recipe_id = rand() % NUM_RECIPES;
    baker
    while (1) {
        /* Choose a random recipe to bake */
        //recipe_id = rand() % NUM_RECIPES;

        /* Check if the baker knows the recipe */
        if (baker->recipes[recipe_id] == 0) {
            /* If the baker does not know the recipe, learn it */
            printf("Baker %d is learning how to make %s.\n", baker->id, recipe_names[recipe_id]);
            baker->recipes[recipe_id] = 1;
        } else {
            /* If the baker already knows the recipe, attempt to bake it */
            printf("Baker %d is attempting to make %s.\n", baker->id, recipe_names[recipe_id]);

            /* Acquire necessary ingredients */
            acquire_ingredients(recipe_ingredients[recipe_id], baker->id);

            /* Use kitchen resources */
            use_kitchen_resources();

            /* Bake the recipe */
            bake_recipe(recipe_id);

            /* Release kitchen resources */
            release_kitchen_resources();

            printf("Baker %d has made %s.\n", baker->id, recipe_names[recipe_id]);
        }

        /* Sleep for a random amount of time between 1 and 5 seconds */
        sleep(rand() % 5 + 1);
    }
}

void acquire_ingredients(int recipeID, int baker_id) {
    /* Acquire semaphore for pantry */
    sem_wait(&pantry_sem);

    switch(recipeID) {
        case 0: //Cookies
            printf("Baker %d grabbed Flour and Sugar from pantry.\n", baker->id);
            break;
        case 1: //Pancakes
            printf("Baker %d grabbed Flour, Sugar, Baking Soda, and Salt from pantry.\n", baker->id);
            break;
        case 2: //Pizza Dough
            printf("Baker %d grabbed Sugar, Salt, and Yeast from pantry.\n", baker->id);
            break;
        case 3: //Pretzels
            printf("Baker %d grabbed Flour, Sugar, Baking Soda, Salt, and Yeast from pantry.\n", baker->id);
            break;
        case 4: //Cinnamon Rolls
            printf("Baker %d grabbed Flour, Sugar, Salt, and Cinnamon from pantry.\n", baker->id);
            break;
        default:
            printf("Invalid recipe! Baker %d is going back to double check cookbook...\n", baker->id);
            break;
    }

    /* Release semaphore for pantry */
    sem_post(&pantry_sem);
    if()
    /* Acquire semaphore for fridge */
    sem_wait(&fridge_sem);

    switch(recipeID) {
        case 1:
            printf("Grabbed eggs, milk from fridge.\n");
            break;
        case 2:
            printf("Grabbed eggs from fridge.\n");
            break;
        case 3:
            printf("Grabbed butter from fridge.\n");
            break;
        default:
            printf("Invalid recipe ID.\n");
            break;
    }

    /* Release semaphore for fridge */
    sem_post(&fridge_sem);
}

void use_kitchen_resources() {
    /* Acquire semaphores for kitchen resources */
    sem_wait(&mixer_sem);
    sem_wait(&bowl_sem);
    sem_wait(&spoon_sem);
    //sem_wait(&oven_sem); putting in own acquire function
    printf("Baker %lu has acquired a mixer, bowl, and spoon to mix ingredients\n", pthread_self());
}

void release_kitchen_resources() {
    /* Release semaphores for kitchen resources */
    //sem_post(&oven_sem);
    sem_post(&spoon_sem);
    sem_post(&bowl_sem);
    sem_post(&mixer_sem);
    printf("Baker %lu has returned a mixer, bowl, and spoon to the kitchen\n", pthread_self());
}

void use_oven_resource() {
    /* Acquire semaphore for oven resource */
    sem_wait(&oven_sem);
    printf("Baker %lu is baking %s...\n", pthread_self(), recipe_names[recipe_id]);
}

void bake_recipe(int recipe_id) {
    printf("Baker %lu has begun preparing %s...\n", pthread_self(), recipe_names[recipe_id]);

    // Get the required ingredients for the recipe
    //int ingredients[NUM_INGREDIENTS];
    //memcpy(ingredients, recipe_ingredients[recipe_id], NUM_INGREDIENTS * sizeof(int));

    // Acquire the required ingredients
    acquire_ingredients(recipe_id, baker -> id);

    // Use the kitchen resources
    use_kitchen_resources();

    // Release the kitchen resources
    release_kitchen_resources();

    use_oven_resource();
    release_oven_resource();
    // Release the acquired ingredients
    //release_ingredients(ingredients); //this will be for ramsie check, will have to have a function that just says baker returned ingredients to fridge and pantry.

    printf("Baker %lu finished baking %s!\n", pthread_self(), recipe_names[recipe_id]);
}
}