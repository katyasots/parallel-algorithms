import re
import matplotlib.pyplot as plt

def get_key(my_dict, val):
    for key, value in my_dict.items():
        if val in value:
            return int(key)

    return False


def parse_line(line):
    match = re.match(r'Matrix size: (\d+) Time: ([0-9.e-]+) Processes: (\d+)', line)
    if match:
        matrix_size = int(match.group(1))
        execution_time = float(match.group(2))
        num_processes = int(match.group(3))
        return {'matrix_size': matrix_size, 'execution_time': execution_time, 'num_processes': num_processes}
    return None


seq_data = {}
with open('data/seq.txt', 'r') as file:
    for line in file:
        item = parse_line(line)
        if item and item['num_processes'] == 1:
            seq_data[item['matrix_size']] = item['execution_time']

parallel_data = []
with open('data/data.txt', 'r') as file:
    for line in file:
        item = parse_line(line)
        if item:
            parallel_data.append(item)

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
    speedup = []
    for item in parallel_data:
        if item['matrix_size'] in sizes:
            key = get_key(matrix_groups, item['matrix_size'])
            seq_time = seq_data.get(key)
            if seq_time is not None:
                speedup_value = seq_time / item['execution_time']
                num_processes.append(item['num_processes'])
                speedup.append(speedup_value)

    sorted_data = sorted(zip(num_processes, speedup))
    sorted_num_processes, sorted_speedup = zip(*sorted_data)

    plt.plot(sorted_num_processes, sorted_speedup, marker='o', label=f'{group_name} Matrix Sizes')

plt.xlabel('Number of Processes')
plt.ylabel('Speedup')
plt.title('Speedup vs Number of Processes for Different Matrix Size Groups')
plt.legend()
plt.grid(True)

plt.savefig('graphics/speedup_vs_processes_grouped.png')