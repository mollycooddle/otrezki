#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <limits>
#include <chrono>
#include <fstream>
#include "avl_tree.h"
#include "otrezki.h"

using namespace std;
using namespace std::chrono;

// Функция для измерения времени работы алгоритма
template<typename Func>
double measureTime(Func func) {
    auto start = high_resolution_clock::now();   //Засекаем начальное время
    func();                             //Выполнение функции переданной
    auto end = high_resolution_clock::now();     // Засекаем конечное время
    
    // duraction_cast - преобразовывает временной интервал в дабл
    return duration_cast<duration<double>>(end - start).count();    //Вычисляем разницу времени
}

// Вспомогательная функция для обработки событий в эффективном алгоритме
// Содержит основную логику работы с AVL-деревом для поиска пересечений
bool processEvents(const SetSection& set, const vector<Event>& events, section& s1, section& s2) {
    AVLTree<double, int> active_segments;   // Дерево для хранения активных отрезков
    double current_x = 0;                   // Координата заметающей прямой

    // Обработка уже отсортированных событий(проход слева направо)
    for (const auto& event : events) {
        current_x = event.p.x;              // Обновляем текущую x-координату
        int seg_id = event.segment_index;   // Получаем индекс обрабатываемого отрезка
        const section& current_seg = set.getSection(seg_id);    // Получаем сам отрезок

        if (event.is_left) {
            // Левый конец - вставка отрезка
            double y_key = set.get_y_at_x(current_seg, current_x);
            active_segments.insert(y_key, seg_id);

            // Поиск соседей и проверка пересечений
            try {
                // Поиск предшественника
                double pred_key;
                int pred_id = -1;
                if (active_segments.predecessor(y_key, pred_key, pred_id)) {
                    if (set.intersection(set.getSection(pred_id), current_seg)) {
                        s1 = set.getSection(pred_id);
                        s2 = current_seg;
                        return true;
                    }
                }

                // Поиск преемника
                double succ_key;
                int succ_id = -1;
                if (active_segments.successor(y_key, succ_key, succ_id)) {
                    if (set.intersection(set.getSection(succ_id), current_seg)) {
                        s1 = set.getSection(succ_id);
                        s2 = current_seg;
                        return true;
                    }
                }
            }
            catch (...) {
                // Игнорируем ошибки
            }

        }
        else {
            // Удаление отрезка
            double y_key = set.get_y_at_x(current_seg, current_x);

            // Поиск соседей перед удалением
            try {
                double pred_key, succ_key;
                int pred_id = -1, succ_id = -1;

                bool has_pred = active_segments.predecessor(y_key, pred_key, pred_id);
                bool has_succ = active_segments.successor(y_key, succ_key, succ_id);

                // Проверка пересечения между соседями
                if (has_pred && has_succ &&
                    set.intersection(set.getSection(pred_id), set.getSection(succ_id))) {
                    s1 = set.getSection(pred_id);
                    s2 = set.getSection(succ_id);
                    return true;
                }
            }
            catch (...) {
                // Игнорируем ошибки
            }

            // Удаляем отрезок из дерева
            try {
                active_segments.erase(y_key);
            }
            catch (...) {
                // Игнорируем ошибки удаления
            }
        }
    }

    return false;
}

// Функция для подготовки событий (вынесена для переиспользования)
// Создает и сортирует массив событий для алгоритма заметающей прямой
void prepareEvents(const SetSection& set, vector<Event>& events) {
    events.clear();
    for (int i = 0; i < set.size(); ++i) {
        section seg = set.getSection(i);
        point left = seg.begin;
        point right = seg.end;
        if (right < left) {
            std::swap(left, right);
        }
        events.push_back({ left, i, true });
        events.push_back({ right, i, false });
    }
    std::sort(events.begin(), events.end());
}

// Функция для измерения времени обоих алгоритмов
void measureBothAlgorithms(SetSection& set, double& time_naive, double& time_effective) {
    section s1, s2;

    // Измерение времени наивного алгоритма
    time_naive = measureTime([&]() {
        set.intersectionNaive(s1, s2);
        });

    // Подготовка событий для эффективного алгоритма
    vector<Event> events;
    prepareEvents(set, events);

    // Измерение времени эффективного алгоритма
    time_effective = measureTime([&]() {
        processEvents(set, events, s1, s2);
        });
}

// Тест 3.1: первый способ задания отрезков, n = 1,...,10000 с шагом 100
void test3_1() {
    cout << "Тест 3.1: n = 1...10000, шаг 100" << endl;
    ofstream file("test3_1.csv");
    file << "n,T1,T2" << endl;

    for (int n = 1; n <= 10000; n += 100) {
        SetSection set;
        set.generate_controlled_sections(n, n / 2); // k = n/2

        double time_naive, time_effective;
        measureBothAlgorithms(set, time_naive, time_effective);

        file << n << "," << time_naive << "," << time_effective << endl;
        cout << "n=" << n << " T1=" << time_naive << " T2=" << time_effective << endl;
    }
    file.close();
    cout << "Результаты сохранены в test3_1.csv" << endl;
}

// Тест 3.2: первый способ задания отрезков, n = 1000, k = 1...1000 с шагом 10
void test3_2() {
    cout << "Тест 3.2: n=1000, k=1...1000, шаг 10" << endl;
    ofstream file("test3_2.csv");
    file << "k,T1,T2" << endl;

    int n = 1000;
    for (int k = 1; k <= 1000; k += 10) {
        SetSection set;
        set.generate_controlled_sections(n, k);

        double time_naive, time_effective;
        measureBothAlgorithms(set, time_naive, time_effective);

        file << k << "," << time_naive << "," << time_effective << endl;
        cout << "k=" << k << " T1=" << time_naive << " T2=" << time_effective << endl;
    }
    file.close();
    cout << "Результаты сохранены в test3_2.csv" << endl;
}

// Тест 3.3: второй способ задания отрезков, r = 0.001, n = 1...10000 с шагом 100
void test3_3() {
    cout << "Тест 3.3: r=0.001, n=1...10000, шаг 100" << endl;
    ofstream file("test3_3.csv");
    file << "n,T1,T2" << endl;

    double r = 0.001;
    for (int n = 1; n <= 10000; n += 100) {
        SetSection set;
        set.generate_controlled_fixed_length_sections(n, n / 2, r);

        double time_naive, time_effective;
        measureBothAlgorithms(set, time_naive, time_effective);

        file << n << "," << time_naive << "," << time_effective << endl;
        cout << "n=" << n << " T1=" << time_naive << " T2=" << time_effective << endl;
    }
    file.close();
    cout << "Результаты сохранены в test3_3.csv" << endl;
}

// Тест 3.4: второй способ задания отрезков, r = 0.0001...0.01 с шагом 0.0001, n = 10000
void test3_4() {
    cout << "Тест 3.4: n=10000, r=0.0001...0.01, шаг 0.0001" << endl;
    ofstream file("test3_4.csv");
    file << "r,T1,T2" << endl;

    int n = 10000;
    int k = 5000;
    for (double r = 0.0001; r <= 0.01; r += 0.0001) {
        SetSection set;
        set.generate_controlled_fixed_length_sections(n, k, r);

        double time_naive, time_effective;
        measureBothAlgorithms(set, time_naive, time_effective);

        file << r << "," << time_naive << "," << time_effective << endl;
        cout << "r=" << r << " T1=" << time_naive << " T2=" << time_effective << endl;
    }
    file.close();
    cout << "Результаты сохранены в test3_4.csv" << endl;
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(0)); // Инициализация генератора случайных чисел

    int choice;

    cout << "Выберите режим работы:" << endl;
    cout << "1. Случайные отрезки" << endl;
    cout << "2. Отрезки фиксированной длины" << endl;
    cout << "3. Ручной ввод" << endl;
    cout << "4. Отрезки с концами в квадрате единичной длины" << endl;
    cout << "5. Автоматические тесты (по заданию)" << endl;
    cout << "Ваш выбор: ";
    cin >> choice;

    if (choice == 5) {
        // Автоматические тесты
        int test_choice;
        cout << "\nВыберите тест:" << endl;
        cout << "1. Тест 3.1: n = 1...10000, шаг 100" << endl;
        cout << "2. Тест 3.2: n=1000, k=1...1000, шаг 10" << endl;
        cout << "3. Тест 3.3: r=0.001, n=1...10000, шаг 100" << endl;
        cout << "4. Тест 3.4: n=10000, r=0.0001...0.01, шаг 0.0001" << endl;
        cout << "Ваш выбор: ";
        cin >> test_choice;

        switch (test_choice) {
        case 1: test3_1(); break;
        case 2: test3_2(); break;
        case 3: test3_3(); break;
        case 4: test3_4(); break;
        default: cout << "Неверный выбор теста" << endl;
        }
        return 0;
    }

    // Оригинальный режим работы (выборы 1-4)
    SetSection set;
    int n, k;
    double segment_length;

    cout << "Введите общее количество отрезков n: ";
    cin >> n;

    if (choice != 5) {
        cout << "Введите число k (k < n-1): ";
        cin >> k;

        if (k >= n - 1) {
            cout << "Ошибка: k должно быть меньше n-1" << endl;
            return 1;
        }
    }

    switch (choice) {
    case 1: {
        // Генерация случайных отрезков с контролируемыми пересечениями
        set.generate_controlled_sections(n, k);
        break;
    }
    case 2: {
        cout << "Введите длину отрезков: ";
        cin >> segment_length;
        // Генерация отрезков фиксированной длины с контролируемыми пересечениями
        set.generate_controlled_fixed_length_sections(n, k, segment_length);
        break;
    }
    case 3: {
        set.input_sections_count(n);
        break;
    }
    case 4: {
        // Отрезки с концами в квадрате единичной длины
        set.generate_random_sections(n, 0.0, 1.0);
        break;
    }
    default: {
        cout << "Неверный выбор" << endl;
        return 1;
    }
    }

    cout << "Сгенерировано " << set.size() << " отрезков" << endl;

    // Для случаев 1, 2, 4 - проверяем контролируемые пересечения
    if (choice == 1 || choice == 2 || choice == 4) {
        // Проверка пересечения отрезков k+1 и k+2
        if (n > k + 1) {
            section s1 = set.getSection(k);
            section s2 = set.getSection(k + 1);
            cout << "Проверка пересечения отрезков " << k + 1 << " и " << k + 2 << ": "
                << (set.intersection(s1, s2) ? "ПЕРЕСЕКАЮТСЯ" : "НЕ ПЕРЕСЕКАЮТСЯ") << endl;
        }
    }

    // Измерение времени работы тривиального алгоритма
    section naive_s1, naive_s2;
    double time_naive = measureTime([&]() {
        set.intersectionNaive(naive_s1, naive_s2);
        });

    // Подготовка событий для эффективного алгоритма (не входит в измерение времени)
    vector<Event> events;
    prepareEvents(set, events);

    // Измерение времени только обработки в AVL-дереве (без копирования данных)
    section effective_s1, effective_s2;
    double time_effective = measureTime([&]() {
        processEvents(set, events, effective_s1, effective_s2);
        });

    cout << "\nРЕЗУЛЬТАТЫ ИЗМЕРЕНИЙ ВРЕМЕНИ:" << endl;
    cout << "Т1 (тривиальный алгоритм): " << time_naive << " секунд" << endl;
    cout << "Т2 (нетривиальный алгоритм, только AVL-обработка): " << time_effective << " секунд" << endl;
    cout << "Отношение Т1/Т2: " << (time_effective > 0 ? time_naive / time_effective : 0) << endl;

    // Проверка результатов
    bool naive_found = false, effective_found = false;

    // Запускаем алгоритмы для проверки результатов
    section check_s1, check_s2;
    naive_found = set.intersectionNaive(check_s1, check_s2);
    effective_found = set.intersectionEffective(check_s1, check_s2);

    cout << "\nПРОВЕРКА РЕЗУЛЬТАТОВ:" << endl;
    cout << "Тривиальный алгоритм: " << (naive_found ? "Найдено пересечение" : "Пересечений нет") << endl;
    cout << "Эффективный алгоритм: " << (effective_found ? "Найдено пересечение" : "Пересечений нет") << endl;

    if (naive_found != effective_found) {
        cout << "ВНИМАНИЕ: Алгоритмы дали разные результаты!" << endl;
    }

    return 0;
}