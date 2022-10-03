Ошибки Тимура

1. Использование dtor сразу после ctor

в main.cpp 

    StackCtor (&stk1, 10);
    StackDtor (&stk1);

Ошибки не было 

2. Использование pop после push большее кол-во раз.
    
в main.cpp

    for (int i = 0; i < 10; ++i)
        StackPush(&stk1, i);

    for (int i = 0; i < 100; ++i)
        printf("%d\n", StackPop(&stk1));
    
Ошибка выводил некоректные значения

Причина:
    не было верификатора, на проверку size < 0
    err2.jpg


3. Обработка, если Stack = nullptr.

в main.cpp

    Stack stk1 = {};
    stk1 - nullptr;

    StackCtor(&stk1, 10);

Случай обработался корректно, ошибки не было.

4. Обработка, если Stack = nullptr.

в main.cpp

    Stack stk1 = {};

    StackCtor(&stk1, -100);

Падал, ничего не выводил,

Причина отрицательное выделение в памяти, не было проверки на память

    ASSERT(!isBadPtr(stk));
    Elem* data = (Elem*) calloc(capacity ON_CANARY_PROT(+2), sizeof(Elem));

    ASSERT(!isBadPtr(stk));

5. Изменение поле структуры capacity = -1.

в main.cpp 

    StackCtor (&stk1, 10);
    
    stk1.capacity = -1;

    for (int i = 0; i < 10; ++i)
        StackPush(&stk1, i);

в логах выводилась ошибка: size < 0;

Причина: 
    
    if (flags | STACK_SIZE_NEG_ERROR)
        return "Stack size is negative\n";

6. Изменение значения data[i].

в main.cpp 

    StackCtor (&stk1, 10);

    for (int i = 0; i < 10; ++i)
        StackPush(&stk1, i);

    stk1.data[5] = -13213;

    for (int i = 0; i < 10; ++i)
        StackPush(&stk1, i);

Случай обработался корректно, ошибки не было.


