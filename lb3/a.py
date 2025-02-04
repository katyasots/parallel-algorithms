import matplotlib.pyplot as plt
from datetime import datetime

data = {}

with open('./data/data.txt', 'r') as file:
    for line in file:
        parts = line.split()
        size = int(parts[2])
        time = float(parts[4])
        procs = int(parts[6])

        if size not in data:
            data[size] = {}
        if procs not in data[size]:
            data[size][procs] = []
        data[size][procs].append(time)

plt.figure(figsize=(10, 6))
for size in data:
    processes = sorted(data[size].keys())
    slowdown = [(sum(data[size][3]) / sum(data[size][p])) for p in processes]
    plt.plot(processes, slowdown, label=f'Объем данных: {size}')

plt.xlabel('Число процессов')
plt.ylabel('Замедление (T_3 / T_p)')
plt.legend()
plt.grid(True)

plt.show()
