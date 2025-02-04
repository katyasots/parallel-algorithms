import matplotlib.pyplot as plt
from datetime import datetime

data = {}

with open('./data.txt', 'r') as file:
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

current_time = datetime.now().strftime('%d_%H:%M:%S')

plt.figure(figsize=(10, 6))
for size in data:
    processes = sorted(data[size].keys())
    times = [sum(data[size][p]) for p in processes]

    plt.plot(processes, times, label=f'Объем данных: {size}')

plt.xlabel('Число процессов')
plt.ylabel('Время')
plt.legend()
plt.grid(True)

filename = f'./Time_{current_time}.png'
plt.savefig(filename)
# plt.show()


plt.figure(figsize=(10, 6))
for size in data:
    processes = sorted(data[size].keys())
    slowdown = [(sum(data[size][2]) / sum(data[size][p])) for p in processes]
    plt.plot(processes, slowdown, label=f'Объем данных: {size}')

plt.xlabel('Число прцоессов')
plt.ylabel('Замедление (T_1 / T_p)')
plt.legend()
plt.grid(True)

filename = f'./Slowdown_{current_time}.png'
plt.savefig(filename)
# plt.show()