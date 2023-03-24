#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

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

/* Recipe data */
const char *recipe_names[NUM_RECIPES] = {"Cookies", "Pancakes", "Pizza Dough", "Soft Pretzels", "Cinnamon Rolls"};
int **recipe_ingredients;

/* Baker data */
struct Baker {
    int id;
    int *recipes;

};

struct Baker *bakers;

/* Function prototypes */
void *baker_thread(void *arg);
void acquire_ingredients(int *ingredients);
void use_kitchen_resources();
void release_kitchen_resources();
void bake_recipe(int recipe_id);

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

    /* Initialize shared memory */
    pantry = (int *)malloc(num_ingredients * sizeof(int));
    fridge = (int *)malloc(num_ingredients * sizeof(int));
    for (i = 0; i < num_ingredients; i++) {
        pantry[i] = 2; // start with 2 of each ingredient
        fridge[i] = 3; // start with 3 of each ingredient
    }

    /* Initialize recipe data */
    recipe_ingredients = (int **)malloc(NUM_RECIPES * sizeof(int *));
    for (i = 0; i < NUM_RECIPES; i++) {
        recipe_ingredients[i] = (int *)malloc(num_ingredients * sizeof(int));
    }

    recipe_ingredients[0][0] = 1;
    recipe_ingredients[0][1] = 2;
    recipe_ingredients[0][2] = 3;
    recipe_ingredients[0][3] = -1;

    recipe_ingredients[1][0] = 1;
    recipe_ingredients[1][1] = 2;
    recipe_ingredients[1][2] = 4;
    recipe_ingredients[1][3] = 5;
    recipe_ingredients[1][4] = 1;
    recipe_ingredients[1][5] = 2;
    recipe_ingredients[1][6] = 3;

    recipe_ingredients[2][0] = 3;
    recipe_ingredients[2][1] = 2;
    recipe_ingredients[2][2] = 5;
    recipe_

    void *baker_thread(void *arg) {
    struct Baker *baker = (struct Baker *)arg;
    int i, recipe_id;

    while (1) {
        /* Choose a random recipe to bake */
        recipe_id = rand() % NUM_RECIPES;

        /* Check if the baker knows the recipe */
        if (baker->recipes[recipe_id] == 0) {
            /* If the baker does not know the recipe, learn it */
            printf("Baker %d is learning how to make %s.\n", baker->id, recipe_names[recipe_id]);
            baker->recipes[recipe_id] = 1;
        } else {
            /* If the baker already knows the recipe, attempt to bake it */
            printf("Baker %d is attempting to make %s.\n", baker->id, recipe_names[recipe_id]);

            /* Acquire necessary ingredients */
            acquire_ingredients(recipe_ingredients[recipe_id]);

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

void acquire_ingredients(int *ingredients) {
    int i;

    /* Acquire semaphore for pantry */
    sem_wait(&pantry_sem);

    /* Check pantry for each ingredient */
    for (i = 0; i < NUM_INGREDIENTS && ingredients[i] != -1; i++) {
        while (pantry[ingredients[i]] == 0) {
            /* Ingredient is not available, release pantry semaphore and wait for it to be restocked */
            sem_post(&pantry_sem);
            printf("Acquiring more %s from the store.\n", ingredient_names[ingredients[i]]);
            sleep(5); // simulate time to go to the store and restock pantry
            sem_wait(&pantry_sem);
        }
        /* Ingredient is available, decrement pantry count */
        pantry[ingredients[i]]--;
    }

    swtich case
    if recipeID == 1 print "grabbed flour, salt, sugar from pantry"

    /* Release semaphore for pantry */
    sem_post(&pantry_sem);

    /* Acquire semaphore for fridge */
    sem_wait(&fridge_sem);

    /* Check fridge for each ingredient */
    for (i = 0; i < NUM_INGREDIENTS && ingredients[i] != -1; i++) {
        while (fridge[ingredients[i]] == 0) {
            /* Ingredient is not available, release fridge semaphore and wait for it to be restocked */
            sem_post(&fridge_sem);
            printf("Acquiring more %s from the store.\n", ingredient_names[ingredients[i]]);
            sleep(5); // simulate time to go to the store and restock fridge
            sem_wait(&fridge_sem);
        }
        /* Ingredient is available, decrement fridge count */
        fridge[ingredients[i]]--;
    }

    /* Release semaphore for fridge */
    sem_post(&fridge_sem);
}

void use_kitchen_resources() {
    /* Acquire semaphores for kitchen resources */
    
    sem_wait(&mixer_sem);
    sem_wait(&bowl_sem);
    sem_wait(&spoon_sem);
    sem_wait(&oven_sem);
}

void release_kitchen_resources() {
    /* Release semaphores for kitchen resources */
    sem_post(&oven_sem);
    sem_post(&spoon_sem);
    sem_post(&bowl_sem);
    sem_post(&mixer_sem);
}

void bake_recipe(int recipe_id) {
    printf("Baker %lu is baking %s...\n", pthread_self(), recipe_names[recipe_id]);

    // Get the required ingredients for the recipe
    int ingredients[NUM_INGREDIENTS];
    memcpy(ingredients, recipe_ingredients[recipe_id], NUM_INGREDIENTS * sizeof(int));

    // Acquire the required ingredients
    acquire_ingredients(ingredients);

    // Use the kitchen resources
    use_kitchen_resources();

    
    // Release the kitchen resources
    release_kitchen_resources();

    use_kitchen_oven();
    release_kitchen_oven();
    // Release the acquired ingredients
    //release_ingredients(ingredients);

    printf("Baker %lu finished baking %s!\n", pthread_self(), recipe_names[recipe_id]);
}
}