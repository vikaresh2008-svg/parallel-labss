import sys
import numpy as np

def load_matrix(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
    # Первая строка — размер N
    n = int(lines[0].strip())
    # Остальные строки — элементы матрицы
    data = []
    for line in lines[1:]:
        if line.strip():
            data.extend(map(float, line.split()))
    return np.array(data).reshape((n, n))

def verify(size):
    file_a = f"matrix_A_{size}.txt"
    file_b = f"matrix_B_{size}.txt"
    file_c = f"matrix_C_{size}.txt"

    print(f"--- Проверка для N = {size} ---")
    
    # 1. Загрузка матриц
    A = load_matrix(file_a)
    B = load_matrix(file_b)
    C_cpp = load_matrix(file_c)

    # 2. Эталонное умножение через NumPy
    C_expected = np.matmul(A, B)

    # 3. Сравнение с учетом погрешности вещественных чисел (double)
    is_correct = np.allclose(C_cpp, C_expected, rtol=1e-5, atol=1e-5)

    # Максимальное абсолютное отклонение
    max_diff = np.max(np.abs(C_cpp - C_expected))

    if is_correct:
        print(f"[ОК] Результаты совпадают! Макс. расхождение: {max_diff:.2e}\n")
        return True
    else:
        print(f"[ОШИБКА] Результаты НЕ совпадают! Макс. расхождение: {max_diff:.2e}\n")
        return False

if __name__ == "__main__":
    sizes = [200, 400, 800, 1200, 1600, 2000]
    
    # Если передан конкретный размер аргументом
    if len(sys.argv) > 1:
        sizes = [int(sys.argv[1])]

    all_ok = True
    for N in sizes:
        try:
            if not verify(N):
                all_ok = False
        except FileNotFoundError as e:
            print(f"Файл не найден для N={N}: {e}")
            all_ok = False

    if all_ok:
        print("ВЕРИФИКАЦИЯ УСПЕШНО ПРОЙДЕНА ДЛЯ ВСЕХ МАТРИЦ!")
    else:
        print("ОШИБКА ВЕРИФИКАЦИИ!")