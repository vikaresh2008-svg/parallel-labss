import std;


void generate_matrix_file(const std::string& filename, size_t n) {
    std::ofstream out(filename);
    if (!out) {
        std::println(std::cerr, "Ошибка создания файла: {}", filename);
        return;
    }

    std::mt19937 gen(42); 
    std::uniform_real_distribution<double> dist(-100.0, 100.0);

    out << n << "\n";
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            out << dist(gen) << (j + 1 == n ? "" : " ");
        }
        out << "\n";
    }
}


std::vector<double> load_matrix(const std::string& filename, size_t& n) {
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("Не удалось открыть файл: " + filename);
    }

    in >> n;
    std::vector<double> matrix(n * n);
    for (size_t i = 0; i < n * n; ++i) {
        in >> matrix[i];
    }
    return matrix;
}

int main() {
    
    std::vector<size_t> sizes = { 200, 400, 800, 1200, 1600, 2000 };

    std::ofstream report("experiment_results.txt");
    report << "Size,Time_sec,GFLOPs,Memory_MB\n";

    std::println("{:<8} | {:<12} | {:<10} | {:<10}", "Размер N", "Время (сек)", "GFLOPs", "Память (МБ)");
    std::println("{:-<50}", "");

    for (size_t N : sizes) {
        std::string fileA = std::format("matrix_A_{}.txt", N);
        std::string fileB = std::format("matrix_B_{}.txt", N);
        std::string fileC = std::format("matrix_C_{}.txt", N);

        
        if (!std::filesystem::exists(fileA)) generate_matrix_file(fileA, N);
        if (!std::filesystem::exists(fileB)) generate_matrix_file(fileB, N);

        size_t nA = 0, nB = 0;
        auto A = load_matrix(fileA, nA);
        auto B = load_matrix(fileB, nB);

        if (nA != N || nB != N) {
            std::println(std::cerr, "Ошибка размера матриц в файле для N = {}", N);
            continue;
        }

        std::vector<double> C(N * N, 0.0);

        
        auto start = std::chrono::high_resolution_clock::now();

        
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                double sum = 0.0;
                for (size_t k = 0; k < N; ++k) {
                    sum += A[i * N + k] * B[k * N + j];
                }
                C[i * N + j] = sum;
            }
        }

        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        
        double operations = 2.0 * N * N * N;
        double gflops = (operations / elapsed.count()) / 1e9;
        double memory_mb = (3.0 * N * N * sizeof(double)) / (1024.0 * 1024.0);

        
        std::println("{:<8} | {:<12.4f} | {:<10.4f} | {:<10.2f}", N, elapsed.count(), gflops, memory_mb);

        
        std::ofstream out_c(fileC);
        out_c << N << "\n";
        out_c << std::fixed << std::setprecision(6);
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                out_c << C[i * N + j] << (j + 1 == N ? "" : " ");
            }
            out_c << "\n";
        }

        
        report << std::format("{},{:.6f},{:.4f},{:.2f}\n", N, elapsed.count(), gflops, memory_mb);
    }

    std::println("\nЭксперименты завершены. Сводный отчет сохранен в 'experiment_results.txt'.");

    
    std::println("\nЗапуск автоматической верификации через Python...");
    int status = std::system("python verify.py");

    if (status == 0) {
        std::println("Верификация завершена успешно!");
    }
    else {
        std::println("Ошибка при выполнении верификации.");
    }

    return 0;
}