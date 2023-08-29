# CIS-452---Project-2
 The "Baker Simulator" is a multi-threaded program written in C that simulates a group of bakers working together in a kitchen environment to bake various recipes. The program employs pthreads for concurrent execution and utilizes semaphores for resource management. It provides an engaging simulation of bakers learning, gathering ingredients, using kitchen resources, baking, and interacting with potential challenges such as being "ramsied."

Key Features:

    Multi-Threaded Simulation:
    The program uses pthreads to simulate multiple bakers working concurrently. Each baker is represented as a separate thread, allowing them to perform independent tasks simultaneously.

    Resource Management:
    The simulation effectively manages kitchen resources such as the pantry, fridge, mixer, bowl, spoon, and oven using semaphores. This ensures that bakers acquire and release resources as needed to complete their tasks.

    Dynamic Recipes:
    Bakers are equipped with a set of recipes to learn and bake. They learn recipes they are not familiar with and proceed to bake recipes they know, highlighting the learning and execution stages of the baking process.

    Random Ramsied Baker Event:
    The simulation introduces a chance for a baker to be "ramsied." If enabled, a baker has a user-defined percentage chance of encountering a disruption in their baking process, forcing them to restart the current recipe.

    Graceful Exit Handling:
    The program employs a signal handler to ensure a graceful exit. Upon receiving a termination signal (SIGINT), it releases resources, cleans up semaphores, and prints a farewell message before exiting.

Usage Instructions:

    Upon launching the program, the user is prompted to enter the number of bakers participating in the simulation.

    The user can choose whether bakers have a chance to be ramsied. If enabled, the user specifies the percentage chance for a baker to be ramsied.

    The simulation then proceeds to initialize bakers, each represented as a separate thread, and simulates their activities in the kitchen.

    Bakers learn recipes they do not know, gather ingredients, use kitchen resources, bake, and handle ramsied events if applicable.

    Once a baker has gone through all available recipes, the simulation ends, and the program gracefully exits, releasing resources and displaying a farewell message.

Technologies Used:

    C Programming Language
    Pthreads (POSIX Threads) for Multi-Threading
    Semaphores for Resource Management

Conclusion:

The "Baker Simulator" offers a unique insight into the challenges of managing resources and coordinating concurrent activities in a kitchen environment. Through multi-threading and semaphore usage, the simulation provides a hands-on experience of a group of bakers working together to bake a variety of recipes, all while handling potential disruptions. This project demonstrates proficiency in multi-threaded programming, resource management, and signal handling.
