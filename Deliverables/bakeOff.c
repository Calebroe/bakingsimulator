#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include <unistd.h> //had to include to get sleep to work

#define NUM_RECIPES 5
#define NUM_INGREDIENTS 9
#define MAX_BAKERS 10

/* Semaphore declarations */
sem_t pantrySem, fridgeSem, mixerSem, bowlSem, spoonSem, ovenSem;

int ramsiedBaker = 0; //boolean int to check if bakers can be ramsied
int percentChanceRamsied = 0; 

/* Recipe data */
const char *recipeNames[] = {"Cookies", "Pancakes", "Pizza Dough", "Soft Pretzels", "Cinnamon Rolls"};

/* Baker data */
struct Baker {
    int id;
    int firstRecipeId;
    int recipes[NUM_RECIPES];
};

struct Baker bakers[MAX_BAKERS];

/* Function prototypes */
void *bakerThread(void *arg);
void aquireIngredients(int recipeID, int bakerID);//function that has a switch case to print ingredients in pantry and a check for if it needs access to fridge ingredients
void getKitchenResources(int bakerID); //function for using the kitchen resources
void useOven(int recipeID, int bakerID); //function for using the oven resource
void releaseKitchenResources();//function to release kitchen resources

/* Signal handler for cleanup */
void exitHandler (int sigNum);

int main() {

    /* Initialize semaphores */
    sem_init(&pantrySem, 0, 1);
    sem_init(&fridgeSem, 0, 2);
    sem_init(&mixerSem, 0, 2);
    sem_init(&bowlSem, 0, 3);
    sem_init(&spoonSem, 0, 5);
    sem_init(&ovenSem, 0, 1);

    signal(SIGINT, exitHandler);
    /* Prompt user for number of bakers */
    int numBakers;
    printf("Enter number of bakers (max %d): ", MAX_BAKERS);
    scanf("%d", &numBakers);

    char isRamsied; //temp char  for getting user input
    printf("Do you want bakers to be randomized? (Y/N): ");
    scanf(" %c", &isRamsied);

    if (isRamsied == 'Y') {
        ramsiedBaker = 1;
        printf("Enter the percent chance that a baker will be ramsied as an integer: ");
        scanf("%d", &percentChanceRamsied);
        printf("Bakers now have a %d percent chance to be ramsied\n", percentChanceRamsied);    
    } else {
        printf("Bakers will not have a chance to be ramsied\n");    
    }

    // DO WE NEED singal() handler here?
    // DO WE NEED threadStatus and joinStatus here?

    pthread_t threads[numBakers];
    int inputs[numBakers];

    /* Initialize bakers */
    for(int i = 0; i < numBakers; i++) {
        // this sets the id of the baker to the index of the array and is passed into the baker_thread function
        inputs[i] = i;
        int ret = pthread_create(&threads[i], NULL, bakerThread, &inputs[i]);
        if(ret != 0) {
            fprintf(stderr, "Error creating thread %d: %d\n", i, ret);
            return 1;
        }
        printf("Baker %d is ready to bake.\n", i);
    }
    for(int i = 0; i < numBakers; i++){
        /* Wait for threads to finish */
        void *result;
        int ret = pthread_join(threads[i], &result);
        if(ret != 0) {
            fprintf(stderr, "Error joining thread %d: %d\n", i, ret);
            return 1;
        }
        printf("Baker %d is done baking.\n", i);
    }
    return 0;
}
    
void *bakerThread(void *arg) { 
    int bakerID = *(int *)arg;
    struct Baker *baker = &bakers[bakerID];
    // this takes the of the argument passed in and sets it to the baker id
    baker->id = bakerID;

    // this sets the current recipe to a random number between 0 and NUM_RECIPES
    int recipe_id = rand() % NUM_RECIPES;
    baker->firstRecipeId = recipe_id;
    int currentRecipe = recipe_id;

    while (1) {            

        // if the baker has not learned the recipe, learn it
        if (baker->recipes[currentRecipe] == 0) {
            printf("Baker %d is learning how to make %s.\n", baker->id, recipeNames[currentRecipe]);
            baker->recipes[currentRecipe] = 1;
        } 
        // if the baker has learned the recipe, attempt to bake it
        else {

            /* If the baker already knows the recipe, attempt to bake it */
            printf("Baker %d is attempting to make %s.\n", baker->id, recipeNames[currentRecipe]);

            /* Acquire necessary ingredients */
            aquireIngredients(currentRecipe, baker->id);
            
            /* Use kitchen resources */
            getKitchenResources(baker->id);

            /* Check if baker has been ramsied */
            if(ramsiedBaker == 1) {
                srand(time(NULL)); //seed random number generator
                int random_num = rand() % 100; 
                if (random_num < percentChanceRamsied && ramsiedBaker == 1) {
                    printf("Baker %d has been ramsied and will not be able to bake %s.\n", baker->id, recipeNames[currentRecipe]);
                    releaseKitchenResources();
                    printf("Baker %d has released the mixer, bowl, and spoon.\n", baker->id);
                    // if the baker has been ramsied, go back to beginning of loop
                    continue;             
                }
            }

            /* Bake the recipe */
            useOven(currentRecipe, baker->id);

            // finished currentRecipe
            printf("Baker %d has made %s.\n", baker->id, recipeNames[currentRecipe]);

            /* Release kitchen resources */
            releaseKitchenResources();
            printf("Baker %d has returned the mixer, bowl, and spoon.\n", baker->id);

            /* Increment currentRecipe */
            currentRecipe = (currentRecipe + 1) % NUM_RECIPES;

            // if the baker has gone through all the recipes, break out of the loop
            if(currentRecipe == baker->firstRecipeId) {
                break;
            }
        }
    }
    sleep(5);
    return NULL;
}

void aquireIngredients(int recipeID, int bakerID) {
    /* Acquire semaphore for pantry */
    sem_wait(&pantrySem);

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

    sem_post(&pantrySem);

    // if the recipe is dough, we dont need to acquire anything from the fridge
    if(recipeID == 2) {
        printf("Baker %d has acquired all ingredients needed!\n", bakerID);
        return; //break out of function and continue to acquiring kitchen resources
    }

    sem_wait(&fridgeSem);

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
    sem_post(&fridgeSem);

    printf("Baker %d has acquired all ingredients needed!\n", bakerID);
}

void getKitchenResources(int bakerId) {
    /* Acquire semaphores for kitchen resources */
    sem_wait(&mixerSem);
    sem_wait(&bowlSem);
    sem_wait(&spoonSem);
    printf("Baker %d has acquired a mixer, bowl, and spoon to mix ingredients\n", bakerId);
}

void releaseKitchenResources() {
    /* Release semaphores for kitchen resources */
    sem_post(&spoonSem);
    sem_post(&bowlSem);
    sem_post(&mixerSem);
}

void useOven(int recipeID, int bakerID) {
    /* Acquire semaphore for oven resource */
    sem_wait(&ovenSem);
    printf("Baker %d is baking %s...\n", bakerID, recipeNames[recipeID]);
    //sleep(5); //mimic baking in oven
    sem_post(&ovenSem);
    printf("Baker %d finished baking %s!\n", bakerID, recipeNames[recipeID]);
}

//function to exit program gracefully
void exitHandler (int sigNum) {
    /* Destroy semaphores */    
    sem_destroy(&pantrySem);
    sem_destroy(&fridgeSem);
    sem_destroy(&mixerSem);
    sem_destroy(&bowlSem);
    sem_destroy(&spoonSem);
    sem_destroy(&ovenSem);

    //free(bakers);
    printf("terminating program. Goodbye.");
    /* Exit program */
    exit(0);
}