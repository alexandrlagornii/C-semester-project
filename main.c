#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

void KMP(const char *pattern, const char *text); // Поиск используя КМП
unsigned int *create_table(const char *pattern, const unsigned int pattern_length); // Создание таблицы для КМП

void linear_search_string(const char *pattern, const char *text); // Линейный поиск

char *get_string(const char *prompt, unsigned int capacity, const char case_type); // Функция для получения строки
char *get_string_from_file(const char *prompt, unsigned int capacity, const char case_type); // Функция для получения строки из файла

int main(void)
{
    // Создаем патерн и текст
    char *pattern = NULL;
    char *text = NULL;

    // Меню ввода строки
    printf("1. To get the text from keyboard.\n");
    printf("2. To get the text from a file.\n");
    printf("3. To exit.\n");
    int choice;
    do
    {
        printf("Your choice: ");
        scanf("%d", &choice);
    }
    while (choice < 1 || choice > 3);

    // Очищаем поток ввода после сканф
    fgetc(stdin);

    // Вводим строку в зависимости от выбора
    switch (choice)
    {
        case 1:
            // Вводим текст с клавиатуры
            text = get_string("Enter the text: ", 128, 'l');
            if (!text)
            {
                printf("ERROR\n");
                return 1;
            }
            break;

        case 2:
            // Считываем текст с файла
            text = get_string_from_file("", 128, 'l');
            if (!text)
            {
                printf("ERROR\n");
                return 1;
            }
            break;

        case 3:
            // Выходим
            printf("Goodbye\n");
            return 0;
    }

    // Вводим патерн
    pattern = get_string("Enter the pattern: ", 8, 'l');
    if (!pattern)
    {
        printf("ERROR\n");

        free(text);
        text = NULL;

        return 1;
    }

    // Выводим текст и паттерн (для тестов) (слишком большие текста выводятся долго)
    // printf("Text is: %s\n", text);
    // printf("Pattern is: %s\n", pattern);

    // Создаем переменную замера времени исполнения функции
    clock_t time_kmp, time_linear;
    double time_taken_kmp, time_taken_linear;

    // Исследуем текст с помощью КМП
    time_kmp = clock();
    KMP(pattern, text);
    time_kmp = clock() - time_kmp;
    time_taken_kmp = ((double) time_kmp) / CLOCKS_PER_SEC;

    // Исследуем текст с помощью линейного поиска
    time_linear = clock();
    linear_search_string(pattern, text);
    time_linear = clock() - time_linear;
    time_taken_linear = ((double) time_linear) / CLOCKS_PER_SEC;

    // Выводим результаты скорости
    printf("KMP speed: %f seconds\n", time_taken_kmp);
    printf("Linear search speed: %f seconds\n", time_taken_linear);

    // Освобождаем всю память
    free(pattern);
    pattern = NULL;

    free(text);
    text = NULL;

    // Выходим
    printf("Goodbye\n");
    return 0;
}

void KMP(const char *pattern, const char *text)
{
    // КМП поиск патернов

    // Определеяем длину патерна, текста
    unsigned int pattern_length = strlen(pattern);
    unsigned int text_length = strlen(text);

    // Создаем таблицу совпадений суффиксов и префиксов
    unsigned int *table = create_table(pattern, pattern_length);
    if (!table)
    {
        printf("ERROR\n");
        return;
    }

    //Тестирование
    /*
    printf("Pattern is: %s\n", pattern);
    printf("Text is %s\n", text);
    printf("Patterns table is: ");
    for (unsigned int k = 0; k < pattern_length; k++)
        printf("%u  ", table[k]);
    printf("\n");
    */

    // Создаем индексы и счетчик найденных патернов
    unsigned int pattern_counter = 0;
    unsigned int pattern_index = 0, text_index = 0;

    // Ищем патерны
    while (text_index < text_length)
    {
        if (text[text_index] == pattern[pattern_index]) // Если буквы совпадают
        {
            // Передвигаем указатель на текст и патерн
            text_index++;
            pattern_index++;

            // Если нашли патерн
            if (pattern_index == pattern_length)
            {
                pattern_counter++; // Увеличиваем счетчик патернов
                //printf("Match found at %d\n", text_index - pattern_length);

                pattern_index = table[pattern_index - 1]; // Двигаем указатель патерна назад в зависимоти от значения в таблице
            }
        }
        else if (pattern_index != 0) // Если буквы не совпадают, но и указатель не на нулевом элеменете таблицы
        {
            pattern_index = table[pattern_index - 1]; // Двигаем указатель патерна назад в зависимости от значения в таблице
        }
        else // Все остальное
        {
            text_index++; // Двигаем указатель текста вперед
        }
    }

    printf("KMP) Patterns found: %u\n", pattern_counter);

    free(table); // Освобождаем память таблицы

    return;
}

unsigned int *create_table(const char *pattern, const unsigned int pattern_length)
{
    // Создаем таблицу суффиксов и префиксов

    // Проверяем если патерн был вообще введен
    if (!pattern_length)
        return NULL;

    // Выделяем память
    unsigned int *table = (unsigned int*) malloc(pattern_length * sizeof(unsigned int));
    if (!table)
        return NULL;

    // Переменные указывающие на элементы
    unsigned int i = 0;
    unsigned int j = 1;
    table[i] = 0; // Первый элемент всегда нуль

    while (j < pattern_length)
    {
        if (pattern[i] == pattern[j]) // Если нашли одинаковый суффикс и префикс
        {
            table[j] = i + 1; // Присваиваем значение в таблице на которое указывает j на i + 1

            // Двигаем указатели
            i++;
            j++;
        }
        else
        {
            if (i == 0) // Если дошли до начала и не нашли совпадающего суффика то присваиваем нуль
            {
                table[j] = 0;
                j++;
            }
            else // Отходим на один назад в поиске подходящего суффикса
            {
                i = table[i - 1];
            }
        }
    }

    return table; // Возвращаем адрес
}

void linear_search_string(const char *pattern, const char *text)
{
    //  Линейный поиск

    // Проверяем если паттерн был введен
    unsigned int pattern_length = strlen(pattern);
    if (!pattern_length)
    {
        printf("ERROR\n");
        return;
    }
    unsigned int text_length = strlen(text);
    unsigned int pattern_counter = 0;

    // Проходимся по всему тексту - патерн
    for (unsigned int i = 0, n = text_length - pattern_length; i <= n; i++)
    {
        unsigned int j;

        // Проверяем данный участок текста
        for (j = 0; j < pattern_length; j++)
            if (text[i + j] != pattern[j])
                break; // Выходим если не совпали символы

        // Проверяем если все символы совпали на данном участке
        if (j == pattern_length)
        {
            //printf("Match found at %u\n", i);
            pattern_counter++;
        }
    }

    printf("Linear Search) Patterns found: %u\n", pattern_counter);

    return;
}

char *get_string(const char *prompt, unsigned int capacity, const char case_type)
{
    // Ввод строки

    // Выводим подсказу
    fputs(prompt, stdout);

    // Выделяем память на изначальное значение
    char *buffer = (char*) malloc(capacity);
    if (!buffer)
        return NULL;

    // Создаем переменные - индекс и символ хранения
    unsigned int index = 0;
    char c;

    do
    {
        // Считываем символ, записываем его в память, увеличиваем индекс + делаем все символы нижним регистром если требуется
        c = fgetc(stdin);
        buffer[index++] = (case_type == 'l') ? tolower(c) : c;

        // Увеличиваем объем память в два раза если требуется
        if (index == capacity)
        {
            capacity *= 2; // Увеличиваем объем памяти

            buffer = (char*) realloc(buffer, capacity); // Перевыделяем
            if (!buffer)
                return NULL;
        }
    }
    while(c != '\n' && c != EOF); // Делаем это все пока не дойдем до конца строки

    buffer[index - 1] = '\0'; // Ставим вместо \n(переход на новую строку) \0(конец строки)

    // Создаем новый указатель с точным количество памяти
    char *str = (char*) malloc(strlen(buffer) + 1); // Выделяем память
    if (!str)
        return NULL;

    strcpy(str, buffer); // Копируем содержимое буфера
    str[strlen(buffer)] = '\0'; // Ставим в конец \0(конец строки)

    free(buffer); // Освобождаем буфер

    return str; // Возвращаем адрес
}

char *get_string_from_file(const char *prompt, unsigned int capacity, const char case_type)
{
    // Считывание строки из файла

    // Выводим подсказу
    fputs(prompt, stdout);

    // Вводим имя файла
    char *file_name = get_string("File name: ", 8, 'n');
    if (!file_name)
        return NULL;

    // Открываем файл
    FILE *our_file = fopen(file_name, "r");
    if (!our_file)
    {
        free(file_name);
        return NULL;
    }

    // Выделяем память на изначальное значение
    char *buffer = (char*) malloc(capacity);
    if (!buffer)
    {
        free(file_name);
        return NULL;
    }

    // Создаем переменные - индекс и символ хранения
    unsigned int index = 0;
    char c;

    do
    {
        // Считываем символ, записываем его в память, увеличиваем индекс + делаем все символы нижним регистром если требуется
        c = fgetc(our_file);
        buffer[index++] = (case_type == 'l') ? tolower(c) : c;

        // Увеличиваем объем памяти в два раза если требуется
        if (index == capacity)
        {
            capacity *= 2; // Увеличиваем объем памяти

            buffer = (char*) realloc(buffer, capacity); // Перевыделяем
            if (!buffer)
            {
                free(file_name);
                return NULL;
            }
        }
    }
    while(c != EOF); // Делаем это все пока не дойдем до конца файла

    fclose(our_file); // Закрываем наш файл
    free(file_name); // Освобождаем память под имя

    buffer[index - 1] = '\0'; // Ставим вместо \n(переход на новую строку) \0(конец строки)

    // Создаем новый указатель с точным количество памяти
    char *str = (char*) malloc(strlen(buffer) + 1); // Выделяем память
    if (!str)
        return NULL;

    strcpy(str, buffer); // Копируем содержимое буфера
    str[strlen(buffer)] = '\0'; // Ставим в конец \0(конец строки)

    free(buffer); // Освобождаем буфер

    return str; // Возвращаем адрес
}