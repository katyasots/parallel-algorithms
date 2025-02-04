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
    times = [sum(data[size][p]) for p in processes]

    plt.plot(processes, times, label=f'Объем данных: {size}')

plt.xlabel('Число процессов')
plt.ylabel('Время')
plt.legend()
plt.grid(True)

plt.show()
