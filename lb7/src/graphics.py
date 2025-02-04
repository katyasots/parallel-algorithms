import re
import matplotlib.pyplot as plt

def parse_line(line):
    match = re.match(r'Matrix size: (\d+) Time: ([0-9.e-]+) Processes: (\d+)', line)
    if match:
        matrix_size = int(match.group(1))
        execution_time = float(match.group(2))
        num_processes = int(match.group(3))
        return {'matrix_size': matrix_size, 'execution_time': execution_time, 'num_processes': num_processes}
    return None

data = []
with open('data/data.txt', 'r') as file:
    for line in file:
        item = parse_line(line)
        if item:
            data.append(item)

matrix_groups = {
    '10': [8, 10, 12, 14],
    '50': [48, 49, 50, 52],
    '100': [100, 102, 104, 105],
    '200': [200, 201, 203, 198],
    '500': [498, 500, 501, 504],
    '1000': [1000, 1001, 1002]
}

plt.figure(figsize=(12, 8))

for group_name, sizes in matrix_groups.items():
    num_processes = []
    execution_times = []
    for item in data:
        if item['matrix_size'] in sizes:
            num_processes.append(item['num_processes'])
            execution_times.append(item['execution_time'])

    sorted_data = sorted(zip(num_processes, execution_times))
    sorted_num_processes, sorted_execution_times = zip(*sorted_data)

    plt.plot(sorted_num_processes, sorted_execution_times, marker='o', label=f'{group_name} Matrix Sizes')

plt.xlabel('Number of Processes')
plt.ylabel('Execution Time (seconds)')
plt.title('Execution Time vs Number of Processes for Different Matrix Size Groups')
plt.legend()
plt.grid(True)

plt.yscale('log')

plt.savefig('graphics/execution_time_vs_processes_grouped.png')