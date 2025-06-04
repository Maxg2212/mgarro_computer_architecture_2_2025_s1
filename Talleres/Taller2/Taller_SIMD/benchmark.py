# Requiere: pip install matplotlib pandas

import pandas as pd
import matplotlib.pyplot as plt

serial_df = pd.read_csv("results_serial.csv")
simd_df = pd.read_csv("results_simd.csv")

serial_df['size_KB'] = serial_df['size_bytes'] / 1024.0
simd_df['size_KB'] = simd_df['size_bytes'] / 1024.0

for alignment in serial_df['alignment'].unique():
    s_data = serial_df[serial_df['alignment'] == alignment]
    v_data = simd_df[simd_df['alignment'] == alignment]

    plt.figure(figsize=(10, 6))
    plt.plot(s_data['size_KB'], s_data['exec_time_us'], label=f"Serial {alignment}B", marker='o')
    plt.plot(v_data['size_KB'], v_data['exec_time_us'], label=f"SIMD {alignment}B", marker='x')
    plt.xlabel("Input Size (KB)")
    plt.ylabel("Execution Time (us)")
    plt.title(f"Execution Time vs Input Size (Alignment {alignment} bytes)")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(f"exec_time_align_{alignment}.png")

plt.show()
print("Gráficas generadas.")
