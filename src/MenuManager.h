#ifndef MENU_MANAGER_H
#define MENU_MANAGER_H

#include "Encoder.h"

#define MENU_COUNT_MAX 10

class MenuManager
{
private:
    void (*functionList[MENU_COUNT_MAX])();
    int size = 0;
    int currentIndex = 0;

public:
    void add(void (*func)())
    {
        if (size < MENU_COUNT_MAX)
        {
            functionList[size++] = func;
        }
        else
        {
            printf("Function list is full. Cannot add more functions.\n");
        }
    }

    void run(int index)
    {
        if (index >= 0 && index < size)
        {
            functionList[index]();
        }
        else
        {
            printf("Invalid function index.\n");
        }
    }

    void navigate(Encoder_State_t state)
    {
        if (!state.changed)
            return;
        if (size == 0)
            return;

        if (state.navigating)
        {
            currentIndex += state.delta;
            if (currentIndex < 0)
                currentIndex = 0;
            else if (currentIndex > (size - 1))
                currentIndex = (size - 1);
        }
        functionList[currentIndex]();
    };
};

#endif
