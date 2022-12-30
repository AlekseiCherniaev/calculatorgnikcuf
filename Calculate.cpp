#include "Calculate.h"
#include "Vector.h"
#include <cmath>

char* reverseString(const char* string, char length){
    auto temp = (char*)malloc((length + 1)* sizeof(char));
    if(!temp)
        return nullptr;

    temp[length] = '\0';
    for (int i = 0; i < length; ++i) {
        temp[i] = string[length - 1 - i];
    }
    return temp;
}

float calculate(const char* eq,char start,char end,const float* vars){
    // проверка строки
    if(end <= start)
        return std::nanf("");

    // создание массивов для распределения цифр и операций
    Vector<float> numbers;
    Vector<char> plusIndex;
    Vector<char> multIndex;
    Vector<char> powIndex;

    auto temp = (char*)malloc(MAX_NUMBER_LENGTH*sizeof(char));
    if(!temp)
        return std::nanf("");
    char tmpc = 0;
    char* reversed;

    // разбор строки справа налево
    for (char i = end - 1; i >= start; --i) {
        // проверка на char
        if((eq[i] > 47 && eq[i] < 58) || eq[i] == 46)
        {
            if(tmpc >= MAX_NUMBER_LENGTH)
                return std::nanf("");
            temp[tmpc++] = eq[i];
        }
            // если char, то добавляем tmp в массив
            // сумма
        else if(eq[i] == '+')
        {
            if(tmpc > 0)
            {
                reversed = reverseString(temp,tmpc);
                if(!reversed)
                    return std::nanf("");
                plusIndex.push(numbers.push(strtof(reversed,nullptr)));
                free(reversed);
                tmpc = 0;
            }
                // учитывая неправильный ввод
            else if(i == end - 1)
            {
                return std::nanf("");
            }
                // те случаи ввода типа a++--+b (равно а+b)
            else if(plusIndex.size() == 0 || (plusIndex.size() > 0 && numbers.size() != *plusIndex.at(
                    plusIndex.size() - 1))){
                plusIndex.push(numbers.size());
            }
        }
            // разность
        else if(eq[i] == '-')
        {
            if(tmpc > 0)
            {
                reversed = reverseString(temp,tmpc);
                if(!reversed)
                    return std::nanf("");
                plusIndex.push(numbers.push(-strtof(reversed,nullptr)));
                free(reversed);
                tmpc = 0;
            }
                // учитывая неправильный ввод
            else if(i == end - 1)
            {
                return std::nanf("");
            }
            else if(plusIndex.size() == 0 || (plusIndex.size() > 0 && numbers.size() != *plusIndex.at(
                    plusIndex.size() - 1))){
                *numbers.at(numbers.size() - 1)*= -1;
                plusIndex.push(numbers.size());
            }else{
                *numbers.at(numbers.size() - 1)*= -1;
            }
        }
            // умножение
        else if(eq[i] == '*'){
            if(tmpc > 0)
            {
                reversed=reverseString(temp,tmpc);
                if(!reversed)
                    return std::nanf("");
                multIndex.push(numbers.push(strtof(reversed,nullptr)));
                free(reversed);
                tmpc = 0;
            }else if(i == end - 1 || i == start)
            {
                return std::nanf("");
            }
                // этот случай предназначен для a * (-b).
            else if(plusIndex.size() > 0 && *plusIndex.at(plusIndex.size() - 1) == numbers.size()){
                plusIndex.pop();
                multIndex.push(numbers.size());
            }
            else{
                multIndex.push(numbers.size());
            }
        }
            // деление
        else if(eq[i] == '/'){
            if(tmpc > 0)
            {
                reversed=reverseString(temp,tmpc);
                if(!reversed)
                    return std::nanf("");
                multIndex.push(numbers.push((float)1 / strtof(reversed,nullptr)));
                free(reversed);
                tmpc = 0;
            }else if(i == end - 1 || i == start)
            {
                return std::nanf("");
            }
            else if(plusIndex.size() > 0 && *plusIndex.at(plusIndex.size() - 1) == numbers.size()){
                plusIndex.pop();
                (*numbers.at(numbers.size() - 1)) = 1 / (*numbers.at(numbers.size() - 1));
                multIndex.push(numbers.size());
            }else{
                (*numbers.at(numbers.size() - 1)) = 1 / (*numbers.at(numbers.size() - 1));
                multIndex.push(numbers.size());
            }
        }
            // степень
        else if(eq[i] == '^') {
            if (tmpc > 0) {
                reversed = reverseString(temp, tmpc);
                if (!reversed)
                    return std::nanf("");
                multIndex.push(numbers.push(strtof(reversed, nullptr)));
                powIndex.push(numbers.size());
                free(reversed);
                tmpc = 0;
            } else if (i == end - 1 || i == start) {
                return std::nanf("");
            } else if (plusIndex.size() > 0 && *plusIndex.at(plusIndex.size() - 1) == numbers.size()) {
                plusIndex.pop();
                multIndex.push(numbers.size());
                powIndex.push(numbers.size());
            } else {
                multIndex.push(numbers.size());
                powIndex.push(numbers.size());
            }
        }
            //скобки
            //если нашли ')' то ищем '('
        else if(eq[i] == ')'){
            //ищем '(':
            char numClosingBrackets = 0;
            char foundMatching = 0;
            for(char j = i - 1; j >= start; --j){
                if(eq[j] == ')')
                    ++numClosingBrackets;
                else if(eq[j] == '(' && numClosingBrackets > 0)
                    --numClosingBrackets;
                else if(eq[j] == '(' && numClosingBrackets == 0)
                {
                    // '(' найдено
                    if(!foundMatching) {
                        numbers.push(calculate(eq, j + 1, i,vars));
                        i = j; // пропустить часть между () при парсинге
                        foundMatching = 1;
                    }
                }
            }
            if(!foundMatching)
                return std::nanf("");
        }
            // тригонометрия и логарифм
        else{
            //считаем в радианах
            if(i > 2 && eq[i] == 'n' && eq[i - 1] == 'i' && eq[i - 2] == 's' && eq[i - 3] == 'a'){
                if(numbers.size())
                    *numbers.at(numbers.size() - 1) = asin(*numbers.at(numbers.size() - 1));
                i -= 3;
                if(plusIndex.size() > 0 && *plusIndex.at(plusIndex.size() - 1) == numbers.size()) {
                    plusIndex.pop();
                }
            }
            else if(i > 2 && eq[i] == 's' && eq[i - 1] == 'o' && eq[i - 2] == 'c' && eq[i - 3] == 'a'){
                if(numbers.size())
                    *numbers.at(numbers.size() - 1) = acos(*numbers.at(numbers.size() - 1));
                i -= 3;
                if(plusIndex.size() > 0 && *plusIndex.at(plusIndex.size() - 1) == numbers.size()) {
                    plusIndex.pop();
                }
            }
            else if(i > 2 && eq[i] == 'n' && eq[i - 1] == 'a' && eq[i - 2] == 't' && eq[i - 3] == 'a'){
                if(numbers.size())
                    *numbers.at(numbers.size() - 1) = atan(*numbers.at(numbers.size() - 1));
                i -= 3;
                if(plusIndex.size() > 0 && *plusIndex.at(plusIndex.size() - 1) == numbers.size()) {
                    plusIndex.pop();
                }
            }
            else if(i > 1 && eq[i] == 'n' && eq[i - 1] == 'i' && eq[i - 2] == 's'){
                if(numbers.size())
                    *numbers.at(numbers.size() - 1) = sin(*numbers.at(numbers.size() - 1));
                i -= 2;
                if(plusIndex.size() > 0 && *plusIndex.at(plusIndex.size() - 1) == numbers.size()) {
                    plusIndex.pop();
                }
            }
            else if(i > 1 && eq[i] == 's' && eq[i - 1] == 'o' && eq[i - 2] == 'c'){
                if(numbers.size())
                    *numbers.at(numbers.size() - 1) = cos(*numbers.at(numbers.size() - 1));
                i -= 2;
                if(plusIndex.size() > 0 && *plusIndex.at(plusIndex.size() - 1) == numbers.size()) {
                    plusIndex.pop();
                }
            }
            else if(i > 1 && eq[i] == 'n' && eq[i - 1] == 'a' && eq[i - 2] == 't'){
                if(numbers.size())
                    *numbers.at(numbers.size() - 1) = tan(*numbers.at(numbers.size() - 1));
                i -= 2;
                if(plusIndex.size() > 0 && *plusIndex.at(plusIndex.size() - 1) == numbers.size()) {
                    plusIndex.pop();
                }
            }
            else if(i > 1 && eq[i] == 'g' && eq[i - 1] == 'o' && eq[i - 2] == 'l'){
                if(numbers.size())
                    *numbers.at(numbers.size() - 1) = log(*numbers.at(numbers.size() - 1));
                i -= 2;
                if(plusIndex.size() > 0 && *plusIndex.at(plusIndex.size() - 1) == numbers.size()) {
                    plusIndex.pop();
                }
            }

                // константы pi, e
            else if(i > 0 && eq[i] == 'i' && eq[i - 1] == 'p'){
                numbers.push(M_PI);
                i -= 1;
            }
            else if(eq[i] == 'e'){
                numbers.push(M_E);
            }
            else if(i > 1 && eq[i] == 's' && eq[i - 1] == 'n' && eq[i - 2] == 'a'){
                if(vars)
                    numbers.push(vars[0]);
                else
                    numbers.push(std::nanf(""));
                i -= 2;
            }
            else
                return std::nanf("");
        }
    }

    // заносим в массив чисел в последний раз
    if(tmpc > 0)
    {
        reversed = reverseString(temp,tmpc);
        if(!reversed)
            return std::nanf("");
        numbers.push(strtof(reversed,nullptr));
        free(reversed);
        tmpc = 0;
    }

    if(numbers.size() == 0)
        return std::nanf("");

    // степень a^b
    if(powIndex.size() > 0) {
        for (char i = powIndex.size()-1; i >= 0; --i){
            if(*powIndex.at(i) >= numbers.size())
                return std::nanf("");
            (*numbers.at(*powIndex.at(i) - 1)) = pow((*numbers.at(*powIndex.at(i))),(*numbers.at(*powIndex.at(i)-1)));
            (*numbers.at(*powIndex.at(i))) = 1;
        }
    }
    // скобки сделаны, теперь умножение
    //  a * b * c = a * f
    if(multIndex.size() > 0) {
        for (char i = multIndex.size() - 1; i >= 0  ; --i){
            if(*multIndex.at(i)>= numbers.size())
                return std::nanf("");
            (*numbers.at(*multIndex.at(i) - 1)) *= (*numbers.at(*multIndex.at(i)));
        }
    }
    //мы суммируем первое число и все числа, которые находятся справа от символа +.
    //эти числа имеют индекс *plusIndex.at()-1
    float result = *numbers.at(0);
    for (char i = 0; i < plusIndex.size(); ++i){
        result += *numbers.at(*plusIndex.at(i));
    }
    free(temp);

    return result;
}
