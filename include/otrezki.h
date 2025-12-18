#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <limits>
#include <chrono>
#include "avl_tree.h"

#define M_PI 3.14159265358979323846

// Точка на плоскости в декартовой системе координат
struct point {
    double x;
    double y;

    // Лексикографическое сравнение, т е по x потом по y
    bool operator<(const point& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }

    bool operator==(const point& other) const {
        return x == other.x && y == other.y;
    }
};

// Отрезок на плоскости
struct section {
    point begin;
    point end;

    bool operator==(const section& other) const {
        return begin == other.begin && end == other.end;
    }
};

// Структура для события - точки конца отрезка(для заметающей прямой)
struct Event {
    point p;            // Точка события
    int segment_index;  // Индекс отрезка
    bool is_left;       // true - левый конец, false - правый конец
    
    // Лексикографическое сравнение, т е по x потом по y
    bool operator<(const Event& other) const {
        return (p.x < other.p.x) || (p.x == other.p.x && p.y < other.p.y);
    }
};

class SetSection {
    std::vector<section> S;

public:
    SetSection() = default;

    SetSection(point A, point B) {
        section AB;
        AB.begin = A;
        AB.end = B;
        if (AB.begin == AB.end) {
            throw std::runtime_error("Section is point");
        }
        S.push_back(AB);
    }

    void add_section(section sec) {
        if (sec.begin == sec.end) {
            throw std::runtime_error("It's point");
        }
        S.push_back(sec);
    }

    void add_section(point A, point B) {
        section AB{ A, B };
        if (AB.begin == AB.end) {
            throw std::runtime_error("Section is point");
        }
        S.push_back(AB);
    }

    size_t size() const {
        return S.size();
    }

    // Случайные координаты начала и конца
    void generate_random_sections(int n, double min_coord = 0.0, double max_coord = 1.0) {
        S.clear();

        for (int i = 0; i < n; ++i) {
            section sec;

            // Генерируем случайные координаты для начала отрезка
            // Делим случайное число rand на RAND_MAX(2^15-1) и получаем дробное число от 0 до 1
            sec.begin.x = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
            sec.begin.y = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);

            // Генерируем случайные координаты для конца отрезка
            sec.end.x = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
            sec.end.y = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);

            S.push_back(sec);
        }
    }

    //генерация отрезков заданной длины со случайными центрами и углами
    void generate_sections_fixed_length(int n, double segment_length, double min_coord = 0.0, double max_coord = 1.0) {
        S.clear();

        for (int i = 0; i < n; ++i) {
            section sec;

            // Генерируем случайные координаты центра отрезка
            double center_x = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
            double center_y = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);

            // Генерируем случайный угол с осью абсцисс (в радианах)
            double angle = (double)rand() / RAND_MAX * 2 * M_PI; // от 0 до 2pi

            // Вычисляем координаты концов отрезка(тригонометрия)
            double half_length = segment_length / 2.0;
            double dx = half_length * cos(angle);       // Проекция на Ox
            double dy = half_length * sin(angle);       // Проекция на Oy

            sec.begin.x = center_x - dx;
            sec.begin.y = center_y - dy;
            sec.end.x = center_x + dx;
            sec.end.y = center_y + dy;

            S.push_back(sec);
        }
    }

    // Непосредственный ввод координат концов отрезкА
    void input_single_section() {
        section sec;
        std::cout << "Введите координаты отрезка:\n";
        std::cout << "Начальная точка (x y): ";
        std::cin >> sec.begin.x >> sec.begin.y;
        std::cout << "Конечная точка (x y): ";
        std::cin >> sec.end.x >> sec.end.y;

        S.push_back(sec);
    }

    //непосредственный ввод координат концов отрезкОВ
    void input_sections_count(int n) {
        S.clear();

        for (int i = 0; i < n; ++i) {
            section sec;
            std::cout << "Отрезок " << i + 1 << ":\n";

            std::cout << "  Начальная точка (x y): ";
            std::cin >> sec.begin.x >> sec.begin.y;

            std::cout << "  Конечная точка (x y): ";
            std::cin >> sec.end.x >> sec.end.y;

            if (sec.begin == sec.end) {
                throw std::runtime_error("Section is point");
            }
            S.push_back(sec);
        }
    }

    // Проверка пересечений
    bool intersection(section AB, section CD) const {
        point A = AB.begin;
        point B = AB.end;
        point C = CD.begin;
        point D = CD.end;

        // Как точка лежит относительно отрезка
        int ABC = side(A, B, C);
        int ABD = side(A, B, D);
        int CDA = side(C, D, A);
        int CDB = side(C, D, B);

        // Лежит ли точка на отрезке
        if (ABC == 0 || ABD == 0 || CDA == 0 || CDB == 0) return true;

        if (ABC != ABD && CDA != CDB) return true;
        return false;
    }

    int side(point A, point B, point C) const {
        // Векторное произведенме (B-A)*(C-A) or AB*AC
        double det = (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);

        if (det > 0) return 1;		//left
        else if (det < 0) return 2; //right
        return 0;					//on AB
    }

    // Наивный алгоритм поиска пересечения за O(n^2)
    bool intersectionNaive(section& s1, section& s2) const {
        int n = S.size();

        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (intersection(S[i], S[j])) {
                    s1 = S[i];
                    s2 = S[j];
                    return true;
                }
            }
        }
        return false;
    }

    // Доступ к отрезкам по индексам
    const section& getSection(int index) const {
        if (index < 0 || index >= S.size()) {
            throw std::out_of_range("Invalid section index");
        }
        return S[index];
    }

    section& getSection(int index) {
        if (index < 0 || index >= S.size()) {
            throw std::out_of_range("Invalid section index");
        }
        return S[index];
    }

public:
    // Вычисляет y-координату пересечения отрезка с вертикальной линией x
    double get_y_at_x(const section& seg, double x) const {
        //Проверка на вертикальную прямую(т е верхняя и нижняя точка лежат на одном x)
        if (std::abs(seg.end.x - seg.begin.x) < 1e-9) {
            // Вертикальный отрезок - возвращаем среднее y
            return (seg.begin.y + seg.end.y) / 2.0;
        }
        // Линейная интерполяция: y = y1 + (y2-y1)*(x-x1)/(x2-x1)
        return seg.begin.y + (seg.end.y - seg.begin.y) *
            (x - seg.begin.x) / (seg.end.x - seg.begin.x);
    }
    // Эффективный алгоритм поиска пересечения за O(n log n) с использованием AVL-дерева
    bool intersectionEffective(section& s1, section& s2) {
        if (S.empty()) return false;
    
        // 1. Создаем массив событий
        std::vector<Event> events;
        for (int i = 0; i < S.size(); ++i) {
            // Убеждаемся, что левый конец имеет меньшую x-координату
            point left = S[i].begin;
            point right = S[i].end;
            
            // Иначе меняем местами лево и право
            if (right < left) {
                std::swap(left, right);
            }
    
            events.push_back({ left, i, true });  // левый конец
            events.push_back({ right, i, false }); // правый конец
        }
    
        // 2. Лексикографическая сортировка событий
        std::sort(events.begin(), events.end());
    
        // 3. АВЛ-дерево для хранения активных отрезков (ключ - y-координата при текущем x)
        AVLTree<double, int> active_segments;
        double current_x = 0;
    
        // 4. Обработка событий слева направо
        for (const auto& event : events) {
            current_x = event.p.x;
            int seg_id = event.segment_index;
            const section& current_seg = S[seg_id];
    
            if (event.is_left) {
                // Вставка отрезка в дерево активных
                double y_key = get_y_at_x(current_seg, current_x);
                active_segments.insert(y_key, seg_id);
    
                // Находим соседей в дереве с помощью готовых методов
                try {
                    // Поиск предшественника (ПОД) - O(log n)
                    double pred_key;
                    int pred_id = -1;
                    if (active_segments.predecessor(y_key, pred_key, pred_id)) {
                        // Проверка пересечения с предшественником
                        if (intersection(S[pred_id], current_seg)) {
                            s1 = S[pred_id];
                            s2 = current_seg;
                            return true;
                        }
                    }
    
                    // Поиск преемника (НАД) - O(log n)
                    double succ_key;
                    int succ_id = -1;
                    if (active_segments.successor(y_key, succ_key, succ_id)) {
                        // Проверка пересечения с преемником
                        if (intersection(S[succ_id], current_seg)) {
                            s1 = S[succ_id];
                            s2 = current_seg;
                            return true;
                        }
                    }
                }
                catch (...) {
                    // Игнорируем ошибки при поиске соседей
                }
    
            }
            else {
                // Удаление отрезка из дерева активных
                double y_key = get_y_at_x(current_seg, current_x);
    
                // Находим соседей перед удалением с помощью готовых методов
                try {
                    // Поиск предшественника и преемника - O(log n)
                    double pred_key, succ_key;
                    int pred_id = -1, succ_id = -1;
    
                    bool has_pred = active_segments.predecessor(y_key, pred_key, pred_id);
                    bool has_succ = active_segments.successor(y_key, succ_key, succ_id);
    
                    // Проверка пересечения между соседями
                    if (has_pred && has_succ && intersection(S[pred_id], S[succ_id])) {
                        s1 = S[pred_id];
                        s2 = S[succ_id];
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

    // Версия эффективного алгоритма с предварительно подготовленными событиями(для правильного счёта времени T2: отсортированные события)
    bool intersectionEffectiveWithPreparedEvents(section& s1, section& s2, const std::vector<Event>& events) {
        if (S.empty()) return false;

        // АВЛ-дерево для хранения активных отрезков
        AVLTree<double, int> active_segments;
        double current_x = 0;

        // Обработка уже отсортированных событий (только работа с AVL-деревом)
        for (const auto& event : events) {
            current_x = event.p.x;
            int seg_id = event.segment_index;
            const section& current_seg = S[seg_id];

            if (event.is_left) {
                // Вставка отрезка
                double y_key = get_y_at_x(current_seg, current_x);
                active_segments.insert(y_key, seg_id);

                // Находим соседей в дереве с помощью successor и predecessor
                try {
                    // Поиск предшественника (ПОД) - O(log n)
                    double pred_key;
                    int pred_id = -1;
                    if (active_segments.predecessor(y_key, pred_key, pred_id)) {
                        // Проверка пересечения с предшественником
                        if (intersection(S[pred_id], current_seg)) {
                            s1 = S[pred_id];
                            s2 = current_seg;
                            return true;
                        }
                    }

                    // Поиск преемника (НАД) - O(log n)
                    double succ_key;
                    int succ_id = -1;
                    if (active_segments.successor(y_key, succ_key, succ_id)) {
                        // Проверка пересечения с преемником
                        if (intersection(S[succ_id], current_seg)) {
                            s1 = S[succ_id];
                            s2 = current_seg;
                            return true;
                        }
                    }
                }
                catch (...) {
                    // Игнорируем ошибки при поиске соседей
                }

            }
            else {
                // Удаление отрезка
                double y_key = get_y_at_x(current_seg, current_x);

                // Находим соседей перед удалением с помощью successor и predecessor
                try {
                    // Поиск предшественника и преемника - O(log n)
                    double pred_key, succ_key;
                    int pred_id = -1, succ_id = -1;

                    bool has_pred = active_segments.predecessor(y_key, pred_key, pred_id);
                    bool has_succ = active_segments.successor(y_key, succ_key, succ_id);

                    // Проверка пересечения между соседями
                    if (has_pred && has_succ && intersection(S[pred_id], S[succ_id])) {
                        s1 = S[pred_id];
                        s2 = S[succ_id];
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

    // Упрощенная версия эффективного алгоритма (только проверка факта пересечения)
    bool hasIntersectionEffective() {
        section s1, s2;
        return intersectionEffective(s1, s2);
    }

// Генерация отрезков с контролируемыми пересечениями (случайные координаты)
    void generate_controlled_sections(int n, int k, double min_coord = 0.0, double max_coord = 1.0) {
        S.clear();

        // 1. Генерируем k непересекающихся отрезков
        for (int i = 0; i < k; ++i) {
            section sec;
            bool valid = false;
            int attempts = 0;

            while (!valid && attempts < 100) {
                // Генерируем случайные координаты
                sec.begin.x = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
                sec.begin.y = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
                sec.end.x = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
                sec.end.y = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);

                // Проверяем, что отрезок не вырожден
                if (sec.begin == sec.end) continue;

                // Проверяем пересечение с уже существующими отрезками
                valid = true;
                for (int j = 0; j < i; ++j) {
                    if (intersection(S[j], sec)) {
                        valid = false;
                        break;
                    }
                }
                attempts++;
            }

            if (valid) {
                S.push_back(sec);
            }
            else {
                // Если не удалось сгенерировать непересекающийся отрезок, создаем очень короткий
                sec.begin.x = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
                sec.begin.y = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
                sec.end.x = sec.begin.x + 0.001;
                sec.end.y = sec.begin.y + 0.001;
                S.push_back(sec);
            }
        }

        // 2. Добавляем два пересекающихся отрезка (k+1 и k+2)
        if (n > k) {
            // Первый пересекающийся отрезок
            section sec1;
            sec1.begin.x = 0.3;
            sec1.begin.y = 0.3;
            sec1.end.x = 0.7;
            sec1.end.y = 0.7;
            S.push_back(sec1);

            // Второй пересекающийся отрезок (пересекается с первым)
            section sec2;
            sec2.begin.x = 0.3;
            sec2.begin.y = 0.7;
            sec2.end.x = 0.7;
            sec2.end.y = 0.3;
            S.push_back(sec2);
        }

        // 3. Добавляем оставшиеся случайные отрезки
        for (int i = k + 2; i < n; ++i) {
            section sec;
            sec.begin.x = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
            sec.begin.y = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
            sec.end.x = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
            sec.end.y = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
            S.push_back(sec);
        }
    }

    // Генерация отрезков фиксированной длины с контролируемыми пересечениями
    void generate_controlled_fixed_length_sections(int n, int k, double segment_length,
        double min_coord = 0.0, double max_coord = 1.0) {
        S.clear();

        // 1. Генерируем k непересекающихся отрезков фиксированной длины
        for (int i = 0; i < k; ++i) {
            section sec;
            bool valid = false;
            int attempts = 0;

            while (!valid && attempts < 100) {
                // Генерируем случайный центр и угол
                double center_x = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
                double center_y = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
                double angle = (double)rand() / RAND_MAX * 2 * M_PI;

                // Вычисляем координаты концов
                double half_length = segment_length / 2.0;
                double dx = half_length * cos(angle);
                double dy = half_length * sin(angle);

                sec.begin.x = center_x - dx;
                sec.begin.y = center_y - dy;
                sec.end.x = center_x + dx;
                sec.end.y = center_y + dy;

                // Проверяем, что отрезок находится в пределах области
                if (sec.begin.x < min_coord || sec.begin.x > max_coord ||
                    sec.begin.y < min_coord || sec.begin.y > max_coord ||
                    sec.end.x < min_coord || sec.end.x > max_coord ||
                    sec.end.y < min_coord || sec.end.y > max_coord) {
                    continue;
                }

                // Проверяем пересечение с уже существующими отрезками
                valid = true;
                for (int j = 0; j < i; ++j) {
                    if (intersection(S[j], sec)) {
                        valid = false;
                        break;
                    }
                }
                attempts++;
            }

            if (valid) {
                S.push_back(sec);
            }
        }

        // 2. Добавляем два пересекающихся отрезка (k+1 и k+2)
        if (n > k) {
            // Первый пересекающийся отрезок
            section sec1;
            sec1.begin.x = 0.4;
            sec1.begin.y = 0.4;
            sec1.end.x = 0.6;
            sec1.end.y = 0.6;
            S.push_back(sec1);

            // Второй пересекающийся отрезок (пересекается с первым)
            section sec2;
            sec2.begin.x = 0.4;
            sec2.begin.y = 0.6;
            sec2.end.x = 0.6;
            sec2.end.y = 0.4;
            S.push_back(sec2);
        }

        // 3. Добавляем оставшиеся отрезки фиксированной длины
        for (int i = k + 2; i < n; ++i) {
            section sec;

            double center_x = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
            double center_y = min_coord + (double)rand() / RAND_MAX * (max_coord - min_coord);
            double angle = (double)rand() / RAND_MAX * 2 * M_PI;

            double half_length = segment_length / 2.0;
            double dx = half_length * cos(angle);
            double dy = half_length * sin(angle);

            sec.begin.x = center_x - dx;
            sec.begin.y = center_y - dy;
            sec.end.x = center_x + dx;
            sec.end.y = center_y + dy;

            S.push_back(sec);
        }
    }
};