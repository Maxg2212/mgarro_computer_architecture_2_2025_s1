#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>
#include <cstdlib>

// Estados posibles
enum class state {READ, EXECUTE, VALIDATE, COMMIT, RETRY};

// Estructura de datos compartida alineada para evitar false sharing
struct alignas(64) speculative_data {
    int value {0};
    std::atomic<int> version {0};
};

// Estado de cada hilo
struct thread_context {
    state current_state {state::READ};
    int start_version {0};
    int local_value {0};
    int retries_left {3};
    bool committed {false};
};

// Hilo de trabajo especulativo
void speculative_worker(speculative_data& data, std::atomic<int>& successes, int thread_id, int work_iterations) {
    for (int i = 0; i < work_iterations; ++i) {
        thread_context ctx;

        while (!ctx.committed && ctx.retries_left >= 0) {
            switch(ctx.current_state) {
                case state::READ:
                    ctx.start_version = data.version.load(std::memory_order_acquire);
                    ctx.local_value = data.value;
                    ctx.current_state = state::EXECUTE;
                break;

                case state::EXECUTE:
                    ctx.local_value += 1;
                    std::this_thread::sleep_for(std::chrono::milliseconds(5 + rand() % 10));
                    ctx.current_state = state::VALIDATE;
                break;

                case state::VALIDATE:
                    if (data.version.compare_exchange_weak(ctx.start_version, ctx.start_version + 1,
                                                            std::memory_order_release, std::memory_order_relaxed)) {
                        ctx.current_state = state::COMMIT;
                    } else {
                        ctx.current_state = state::RETRY;
                    }
                break;

                case state::COMMIT:
                    data.value = ctx.local_value;
                    successes.fetch_add(1, std::memory_order_relaxed);
                    ctx.committed = true;
                break;

                case state::RETRY:
                    ctx.retries_left--; 
                    if (ctx.retries_left >= 0) {
                        ctx.current_state = state::READ;
                    } else {
                        std::cout << "Thread ID: " << thread_id << " retry limit exceeded.\n";
                    }
                break;
            }
        }
    }
}

int main() {
    speculative_data shared_data;
    std::atomic<int> total_successes{0};

    // Detecta la cantidad real de hardware threads disponibles
    unsigned int hw_threads = std::thread::hardware_concurrency();
    if (hw_threads == 0) hw_threads = 2; // fallback por si devuelve 0

    const int thread_count = hw_threads;
    const int work_per_thread = 3;  // reducimos carga por hilo

    std::vector<std::thread> workers;

    std::cout << "Detected hardware threads: " << thread_count << "\n";

    for (int i = 0; i < thread_count; ++i) {
        workers.emplace_back(speculative_worker, std::ref(shared_data),
                             std::ref(total_successes), i, work_per_thread);
    }

    for (auto& t : workers) {
        t.join();
    }

    std::cout << "Final value: " << shared_data.value << "\n";
    std::cout << "Successful Commits: " << total_successes << "\n";
    std::cout << "Theoretical maximum: " << thread_count * work_per_thread << "\n";

    return 0;
}

