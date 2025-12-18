import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Функция для анализа одного файла
def analyze_file(filename, file_label):
    # Чтение данных из CSV файла
    df = pd.read_csv(filename)
    
    print(f"\n{'='*50}")
    print(f"Анализ файла: {filename} ({file_label})")
    print(f"{'='*50}")
    
    # Определение названий столбцов (могут отличаться в разных файлах)
    if 'n' in df.columns:
        x_col = 'n'
        x_label = 'Количество отрезков (n)'
    elif 'k' in df.columns:
        x_col = 'k'
        x_label = 'Параметр k'
    elif 'r' in df.columns:
        x_col = 'r'
        x_label = 'Параметр r'
    else:
        x_col = df.columns[0]  # первый столбец
        x_label = df.columns[0]
    
    # Настройка стиля графика
    plt.style.use('default')
    
    # График 1: логарифмическая шкала по Y
    plt.figure(figsize=(12, 8))
    plt.plot(df[x_col], df['T1'], 'b-', linewidth=2, label='T1 (тривиальный алгоритм)', alpha=0.7, marker='o', markersize=3)
    plt.plot(df[x_col], df['T2'], 'r-', linewidth=2, label='T2 (эффективный алгоритм)', alpha=0.7, marker='s', markersize=3)
    
    plt.xlabel(x_label, fontsize=12)
    plt.ylabel('Время выполнения (секунды)', fontsize=12)
    plt.title(f'{file_label}: Зависимость времени выполнения алгоритмов ({filename})', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend(fontsize=11)
    plt.yscale('log')
    plt.grid(True, which='both', alpha=0.2)
    plt.tight_layout()
    plt.show()
    
    # График 2: линейная шкала
    plt.figure(figsize=(12, 8))
    plt.plot(df[x_col], df['T1'], 'b-', linewidth=2, label='T1 (тривиальный алгоритм)', alpha=0.7, marker='o', markersize=3)
    plt.plot(df[x_col], df['T2'], 'r-', linewidth=2, label='T2 (эффективный алгоритм)', alpha=0.7, marker='s', markersize=3)
    
    plt.xlabel(x_label, fontsize=12)
    plt.ylabel('Время выполнения (секунды)', fontsize=12)
    plt.title(f'{file_label}: Зависимость времени выполнения (линейная шкала)', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend(fontsize=11)
    plt.tight_layout()
    plt.show()
    
    # График 3: отношение T1/T2
    plt.figure(figsize=(12, 6))
    ratio = df['T1'] / df['T2']
    plt.plot(df[x_col], ratio, 'g-', linewidth=2, label='Отношение T1/T2', marker='^', markersize=4)
    
    plt.xlabel(x_label, fontsize=12)
    plt.ylabel('Отношение T1/T2', fontsize=12)
    plt.title(f'{file_label}: Отношение времени работы алгоритмов', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend(fontsize=11)
    
    # Добавляем горизонтальную линию для среднего значения
    mean_ratio = ratio.mean()
    plt.axhline(y=mean_ratio, color='red', linestyle='--', alpha=0.7, 
                label=f'Среднее отношение: {mean_ratio:.1f}')
    plt.legend(fontsize=11)
    
    plt.tight_layout()
    plt.show()
    
    # Вывод статистики
    print("Статистика по данным:")
    print(f"Количество измерений: {len(df)}")
    print(f"Диапазон {x_label}: от {df[x_col].min()} до {df[x_col].max()}")
    print(f"Максимальное T1: {df['T1'].max():.6e} сек")
    print(f"Максимальное T2: {df['T2'].max():.6e} сек")
    print(f"Минимальное T1: {df['T1'].min():.6e} сек")
    print(f"Минимальное T2: {df['T2'].min():.6e} сек")
    print(f"Среднее T1: {df['T1'].mean():.6e} сек")
    print(f"Среднее T2: {df['T2'].mean():.6e} сек")
    print(f"Среднее отношение T1/T2: {ratio.mean():.2f}")
    print(f"Максимальное отношение T1/T2: {ratio.max():.2f}")
    print(f"Минимальное отношение T1/T2: {ratio.min():.2f}")
    print(f"Медианное отношение T1/T2: {ratio.median():.2f}")
    
    # Дополнительный анализ для файла с параметром r
    if x_col == 'r':
        print("\nДополнительный анализ для параметра r:")
        # Находим точку с максимальным выигрышем
        max_ratio_idx = ratio.idxmax()
        print(f"Максимальный выигрыш при r = {df[x_col].iloc[max_ratio_idx]}: {ratio.iloc[max_ratio_idx]:.1f} раз")
        
        # Анализ корреляции между r и временем выполнения
        corr_T1_r = df[x_col].corr(df['T1'])
        corr_T2_r = df[x_col].corr(df['T2'])
        print(f"Корреляция r-T1: {corr_T1_r:.3f}")
        print(f"Корреляция r-T2: {corr_T2_r:.3f}")
    
    return df, ratio, x_col

# Функция для сравнительного анализа всех файлов
def compare_all_files(dataframes, x_columns, filenames, labels):
    plt.figure(figsize=(16, 12))
    
    colors = ['blue', 'red', 'green', 'purple']
    linestyles = ['-', '--', '-.', ':']
    markers = ['o', 's', '^', 'D']
    
    # График T1 для всех файлов
    plt.subplot(2, 2, 1)
    for i, (df, x_col, label) in enumerate(zip(dataframes, x_columns, labels)):
        plt.plot(df[x_col], df['T1'], 
                color=colors[i], linestyle=linestyles[i], marker=markers[i], markersize=3,
                linewidth=2, label=f'T1 - {label}', alpha=0.8)
    
    plt.xlabel('Параметр', fontsize=12)
    plt.ylabel('Время T1 (секунды)', fontsize=12)
    plt.title('Сравнение времени T1 (тривиальный алгоритм) для всех файлов', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend(fontsize=10)
    plt.yscale('log')
    
    # График T2 для всех файлов
    plt.subplot(2, 2, 2)
    for i, (df, x_col, label) in enumerate(zip(dataframes, x_columns, labels)):
        plt.plot(df[x_col], df['T2'], 
                color=colors[i], linestyle=linestyles[i], marker=markers[i], markersize=3,
                linewidth=2, label=f'T2 - {label}', alpha=0.8)
    
    plt.xlabel('Параметр', fontsize=12)
    plt.ylabel('Время T2 (секунды)', fontsize=12)
    plt.title('Сравнение времени T2 (эффективный алгоритм) для всех файлов', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend(fontsize=10)
    plt.yscale('log')
    
    # График отношений T1/T2 для всех файлов
    plt.subplot(2, 2, 3)
    for i, (df, x_col, label) in enumerate(zip(dataframes, x_columns, labels)):
        ratio = df['T1'] / df['T2']
        plt.plot(df[x_col], ratio, 
                color=colors[i], linestyle=linestyles[i], marker=markers[i], markersize=4,
                linewidth=2, label=f'T1/T2 - {label}', alpha=0.8)
    
    plt.xlabel('Параметр', fontsize=12)
    plt.ylabel('Отношение T1/T2', fontsize=12)
    plt.title('Сравнение отношений T1/T2 для всех файлов', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    plt.legend(fontsize=10)
    
    # Сводный график средних отношений
    plt.subplot(2, 2, 4)
    avg_ratios = []
    for i, (df, label) in enumerate(zip(dataframes, labels)):
        ratio = df['T1'] / df['T2']
        avg_ratios.append(ratio.mean())
    
    bars = plt.bar(labels, avg_ratios, color=colors[:len(labels)], alpha=0.7)
    plt.ylabel('Среднее отношение T1/T2', fontsize=12)
    plt.title('Средняя эффективность алгоритмов по файлам', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3, axis='y')
    
    # Добавляем значения на столбцы
    for bar, value in zip(bars, avg_ratios):
        plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.1,
                f'{value:.1f}x', ha='center', va='bottom', fontweight='bold')
    
    plt.tight_layout()
    plt.show()

# Анализ всех файлов
files_to_analyze = [
    ('test3_1.csv', 'Эксперимент 1 (n)'),
    ('test3_2.csv', 'Эксперимент 2 (k)'), 
    ('test3_3.csv', 'Эксперимент 3'),
    ('test3_4.csv', 'Эксперимент 4 (r)')
]

all_dataframes = []
all_ratios = []
x_columns = []
filenames = []
labels = []

for filename, label in files_to_analyze:
    try:
        df, ratio, x_col = analyze_file(filename, label)
        all_dataframes.append(df)
        all_ratios.append(ratio)
        x_columns.append(x_col)
        filenames.append(filename)
        labels.append(label)
    except FileNotFoundError:
        print(f"\nПредупреждение: Файл {filename} не найден. Пропускаем.")
    except Exception as e:
        print(f"\nОшибка при обработке файла {filename}: {e}")

# Сравнительный анализ всех файлов
if len(all_dataframes) > 1:
    print(f"\n{'='*60}")
    print("СРАВНИТЕЛЬНЫЙ АНАЛИЗ ВСЕХ ФАЙЛОВ")
    print(f"{'='*60}")
    compare_all_files(all_dataframes, x_columns, filenames, labels)

# Сводная статистика
if all_dataframes:
    print(f"\n{'='*60}")
    print("СВОДНАЯ СТАТИСТИКА")
    print(f"{'='*60}")
    
    for i, (df, label, ratio, x_col) in enumerate(zip(all_dataframes, labels, all_ratios, x_columns)):
        print(f"\n{label}:")
        print(f"  Параметр: {x_col}")
        print(f"  Размер данных: {len(df)} записей")
        print(f"  Диапазон параметра: {df[x_col].min()} - {df[x_col].max()}")
        print(f"  Макс. T1/T2: {ratio.max():.1f}x")
        print(f"  Мин. T1/T2: {ratio.min():.1f}x")
        print(f"  Сред. T1/T2: {ratio.mean():.1f}x")
        print(f"  Мед. T1/T2: {ratio.median():.1f}x")
        print(f"  Выигрыш эффективного алгоритма: в {ratio.mean():.1f} раз в среднем")

# Специальный анализ для test3_4.csv (параметр r)
if 'test3_4.csv' in filenames:
    idx = filenames.index('test3_4.csv')
    df_r = all_dataframes[idx]
    ratio_r = all_ratios[idx]
    
    print(f"\n{'='*60}")
    print("ДЕТАЛЬНЫЙ АНАЛИЗ test3_4.csv (параметр r)")
    print(f"{'='*60}")
    
    # Анализ распределения времени по диапазонам r
    r_ranges = [(0.0001, 0.001), (0.001, 0.005), (0.005, 0.01)]
    
    for r_min, r_max in r_ranges:
        mask = (df_r['r'] >= r_min) & (df_r['r'] <= r_max)
        subset = df_r[mask]
        if len(subset) > 0:
            ratio_subset = subset['T1'] / subset['T2']
            print(f"\nДиапазон r: {r_min} - {r_max}")
            print(f"  Количество точек: {len(subset)}")
            print(f"  Среднее T1: {subset['T1'].mean():.6e} сек")
            print(f"  Среднее T2: {subset['T2'].mean():.6e} сек")
            print(f"  Среднее отношение T1/T2: {ratio_subset.mean():.1f}x")
    
    # Топ-5 наибольших выигрышей
    print(f"\nТоп-5 наибольших выигрышей эффективного алгоритма:")
    top5_idx = ratio_r.nlargest(5).index
    for idx in top5_idx:
        print(f"  r = {df_r['r'].iloc[idx]}: {ratio_r.iloc[idx]:.1f}x выигрыш")