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

boolean ramsiedBaker = false; //boolean to check if bakers can be ramsied
int percentChanceRamsied = 0; 

/* Recipe data */
const char *recipe_names[NUM_RECIPES] = {"Cookies", "Pancakes", "Pizza Dough", "Soft Pretzels", "Cinnamon Rolls"};

/* Baker data */
struct Baker {
    int id;
    int firstRecipeId;
    int recipes[NUM_RECIPES];
};

struct Baker *bakers;

/* Function prototypes */
void *baker_thread(void *arg);
void aquireIngredients(int recipeID, int bakerId);//function that has a switch case to print ingredients in pantry and a check for if it needs access to fridge ingredients
void getKitchenResources(int bakerId); //function for using the kitchen resources
void useOven(int bakerId); //function for using the oven resource
void releaseKitchenResources(int bakerId);//function to release kitchen resources
void release_oven_resources(int bakerId); //function to release oven resource
void bake_recipe(int recipeID); //function to bake 
void getStartingRecipe(struct baker *bakerPtr); //used to get a random starting recipe

/* Signal handler for cleanup */
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
        printf("Enter the percent chance that a baker will be ramsied: ");
        scanf("%d", &percentChanceRamsied);
        printf("Bakers now have a %d to be ramsied", percentChanceRamsied);    
    } else {
        printf("Bakers will not have a chance to be ramsied");    
    }

    // DO WE NEED singal() handler here?
    // DO WE NEED threadStatus and joinStatus here?

    pthread_t threads[num_bakers];
    int inputs[num_bakers];

    /* Initialize bakers */
    for(int i = 0; i < num_bakers; i++) {
        // this sets the id of the baker to the index of the array and is passed into the baker_thread function
        inputs[i] = i;
        int ret = pthread_create(&threads[i], NULL, baker_thread, &inputs[i]);
        if(ret != 0) {
            fprintf(stderr, "Error creating thread %d: %s\n", i, ret);
            return 1;
    }
    printf("Baker %d is ready to bake.\n", i);
    }

    /* Wait for threads to finish */
    void *result;
    ret = pthread_join(threads[i], &result);
    if(ret != 0) {
        fprintf(stderr, "Error joining thread %d: %s\n", i, ret);
        return 1;
    }
    printf("Baker %d is done baking all their recipies.\n", i);
    return 0
}
    
void *baker_thread(void *arg) { //we need to heavily modify this function so that we start with a currentrecipeID, then during the while loop, we increment whatever the value is modulo 5 until we hit the starting recipeID again and quit.
    
    struct Baker *baker = (struct Baker *)arg;
    // this takes the of the argument passed in and sets it to the baker id
    baker->id = *(int *)arg;

    // this sets the current recipe to a random number between 0 and NUM_RECIPES
    int recipe_id = rand() % NUM_RECIPES;
    int baker->firstRecipeId = recipe_id;
    int currentRecipe = recipe_id;

    while (1) {            
        printf("Baker %d is attempting to make %s.\n", baker->id, recipe_names[currentRecipe]);

        /* Acquire necessary ingredients */
        aquireIngredients(recipe_ingredients[baker->currentRecipe], baker->id);
        
        /* Use kitchen resources */
        getKitchenResources(baker->id);

        /* Check if baker has been ramsied */
        if(ramsiedBaker == true) {
            int random_num = rand() % 100; 
            if (random_num < percentChanceRamsied && ramsiedBaker == true) {
                printf("Baker %d has been ramsied and will not be able to bake %s.\n", baker->id, recipe_names[currentRecipe]);
                printf("Baker %d has return ingredients to fridge and pantry.\n", baker->id);
                releaseKitchenResources(baker->id);
                // if the baker has been ramsied, go back to beginning of loop
                continue;             
            }
        }

        /* Release kitchen resources */
        releaseKitchenResources(baker->id);

        /* Bake the recipe */
        useOven(baker->id, currentRecipe);

        // finished currentRecipe
        printf("Baker %d has made %s.\n", baker->id, recipe_names[currentRecipe]);

        /* Sleep for a random amount of time between 1 and 5 seconds */
        sleep(rand() % 5 + 1);

        /* Increment currentRecipe */
        currentRecipe = (currentRecipe + 1) % NUM_RECIPES;

        // if the baker has gone through all the recipes, break out of the loop
        if(currentRecipe == baker->firstRecipeId) {
            break;
        }
    }
}

void aquireIngredients(int recipeID, int bakerID) {
    /* Acquire semaphore for pantry */
    sem_wait(&pantry_sem);

    switch(recipeID) {
        case 0: //Cookies
            printf("Baker %d grabbed Flour and Sugar from pantry.\n", bakerID);
            break;
        case 1: //Pancakes
            printf("Baker %d grabbed Flour, Sugar, Baking Soda, and Salt from pantry.\n", bakerID);
            break;
        case 2: //Pizza Dough
            printf("Baker %d grabbed Sugar, Salt, and Yeast from pantry.\n", bakerID);
            break;
        case 3: //Pretzels
            printf("Baker %d grabbed Flour, Sugar, Baking Soda, Salt, and Yeast from pantry.\n", bakerID);
            break;
        case 4: //Cinnamon Rolls
            printf("Baker %d grabbed Flour, Sugar, Salt, and Cinnamon from pantry.\n", bakerID);
            break;
        default:
            printf("Invalid recipe! Baker %d is going back to double check cookbook...\n", bakerID);
            break;
    }

    sem_post(&pantry_sem);

    if(recipeID == 2) {// if the recipe is dough, we dont need to acquire anything from the fridge
        printf("Baker %d has acquired all ingredients needed!\n", baker->id);
        break; //break out of function and continue to acquiring kitchen resources
    }

    sem_wait(&fridge_sem);

    switch(recipeID) {
        case 0: //Cookies
            printf("Grabbed Milk and Butter from fridge.\n");
            break;
        case 1: //Pancakes
            printf("Grabbed Milk, Butter, and Eggs from fridge.\n");
            break;
        case 2: //dough, do nothing
            break;
        case 3: //Pretzels
            printf("Grabbed Eggs from fridge.\n");
            break;
        case 4: //Cinnamon Rolls
            printf("Grabbed Butter and Eggs from fridge.\n");
            break;
        default: //probably dont need a check here since we are catching in top switch case, we should just break out of function entirly with an error code if required.
            printf("Invalid recipe ID.\n");
            break;
    }
    // release semaphore for fridge
    sem_post(&fridge_sem);
    printf("Baker %d has acquired all ingredients needed!\n", baker->id);
}

void getKitchenResources(int bakerId) {
    /* Acquire semaphores for kitchen resources */
    sem_wait(&mixer_sem);
    sem_wait(&bowl_sem);
    sem_wait(&spoon_sem);
    printf("Baker %d has acquired a mixer, bowl, and spoon to mix ingredients\n", bakerId);
}

void releaseKitchenResources(int bakerId) {
    /* Release semaphores for kitchen resources */
    sem_post(&spoon_sem);
    sem_post(&bowl_sem);
    sem_post(&mixer_sem);
    printf("Baker %d has returned the mixer, bowl, and spoon.\n", bakerId);
}

void useOven(int bakerID, int recipeID) {
    /* Acquire semaphore for oven resource */
    sem_wait(&oven_sem);
    printf("Baker %d is baking %s...\n", bakerID, recipe_names[recipeID]);
    sleep(5); //mimic baking in oven
    sem_post(&oven_sem);
    printf("Baker %d finished baking %s!\n", bakerID, recipe_names[recipeID]);
}

//function to exit program gracefully
void cleanup_handler(int sig) {
    /* Destroy semaphores */    
    sem_destroy(&pantry_sem);
    sem_destroy(&fridge_sem);
    sem_destroy(&mixer_sem);
    sem_destroy(&bowl_sem);
    sem_destroy(&spoon_sem);
    sem_destroy(&oven_sem);

    free(recipe_ingredients);
    free(bakers);
    printf("Terminating program. Goodbye.")
    /* Exit program */
    exit(0);
}