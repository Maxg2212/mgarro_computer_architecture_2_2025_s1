// Compile
// $> g++ --std=c++20  -pthread -Wall -Wextra tls_mod.cpp  -o tlsm
// Run
// $> ./tlsm

#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>

enum class state {READ, EXECUTE, VALIDATE, COMMIT, RETRY};

struct speculative_data {
    int value {0};
    std::atomic<int> version {0};
};

struct thread_context {
    state current_state {state::READ};
    int start_version {0};
    int local_value {0};
    int retries_left {3};
    bool committed {false};
};

void speculative_worker(std::vector<speculative_data>& shared_vars, std::atomic<int>& successes, int thread_id) {
    std::mt19937 rng(thread_id); // Semilla determinista por thread
    std::uniform_int_distribution<int> delay_dist(10, 29); // Retardo en ms
    std::uniform_int_distribution<int> var_selector(0, shared_vars.size() - 1); // Selección de variable compartida

    for (int i = 0; i < 5; ++i) {
        thread_context ctx;

        int selected_var_index = var_selector(rng);
        speculative_data& data = shared_vars[selected_var_index];
        
        while (!ctx.committed && ctx.retries_left >= 0) {
            switch(ctx.current_state) {
                case state::READ:
                    // Capture initial state
                    ctx.start_version = data.version.load(std::memory_order_acquire);
                    ctx.local_value = data.value;
                    ctx.current_state = state::EXECUTE;
                break;

                case state::EXECUTE:
                    // Speculative computation
                    ctx.local_value += 1;
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay_dist(rng)));
                    ctx.current_state = state::VALIDATE;
                break;

                case state::VALIDATE:
                    // Atomic state validation
                    if (data.version.compare_exchange_weak(ctx.start_version, ctx.start_version + 1, std::memory_order_release, std::memory_order_relaxed)){
                        ctx.current_state = state::COMMIT;
                    }
                    else {
                        ctx.current_state = state::RETRY;
                    }
                break;

                case state::COMMIT:
                    // Finalise successful transaction
                    data.value = ctx.local_value;
                    successes.fetch_add(1, std::memory_order_relaxed);
                    ctx.committed = true;
                break;

                case state::RETRY:
                    // Handle failed transaction
                    ctx.retries_left--; 
                    if (ctx.retries_left >= 0) {
                        ctx.current_state = state::READ;
                    } else {
                        // Logging when retries are exceeded
                        std::cout << "Thread ID: " << thread_id << " retry limit exceeded.\n";
                    }
                break;
            }
        }
    }
}

int main() {
    unsigned int user_thread_count;
    std::cout << "Ingrese número de hilos a usar (max " << std::thread::hardware_concurrency() << "): ";
    std::cin >> user_thread_count;
    user_thread_count = std::min(user_thread_count, std::thread::hardware_concurrency());

    int variable_count = 2; // Se puede modificar si querés más
    std::vector<speculative_data> shared_data(variable_count);
    std::atomic<int> total_successes{0};

    std::vector<std::thread> workers;
    for (unsigned int i = 0; i < user_thread_count; ++i) {
        workers.emplace_back(speculative_worker, std::ref(shared_data), std::ref(total_successes), i);
    }

    for (auto& t : workers) {
        t.join();
    }

    for (int i = 0; i < variable_count; ++i) {
        std::cout << "Final value for shared_data[" << i << "]: " << shared_data[i].value << "\n";
    }
    std::cout << "Successful Commits: " << total_successes << "\n";
    std::cout << "Theoretical maximum: " << user_thread_count * 5 << "\n";

    return 0;
}
