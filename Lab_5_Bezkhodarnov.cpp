#include <chrono>
#include <thread>
#include <random>
#include <vector>
#include <climits>
#include <iostream>
#include <algorithm>

void MergeSortImpl(std::vector<int> &values, std::vector<int> &buffer, int l, int r) {
	if (l < r) {
		int m = (l + r) / 2;
		MergeSortImpl(values, buffer, l, m);
		MergeSortImpl(values, buffer, m + 1, r);

		int k = l;
		for (int i = l, j = m + 1; i <= m || j <= r; ) {
		if (j > r || (i <= m && values[i] < values[j])) {
			buffer[k] = values[i];
			++i;
		} else {
			buffer[k] = values[j];
			++j;
		}
		++k;
		}
		for (int i = l; i <= r; ++i) {
		values[i] = buffer[i];
		}
	}
}

void MergeSort(std::vector<int> &values) {
	if (!values.empty()) {
		std::vector<int> buffer(values.size());
		MergeSortImpl(values, buffer, 0, values.size() - 1);
	}
}

void MultiThreadMergeSort(std::vector<int> &values, unsigned int threads_count) {
	if (!values.empty()) {
		std::vector<int> buffer(values.size());
		if (threads_count > values.size() / 2) {
			std::cout << "The count of threads has been automatically changed to the count of elements divided by 2.\n";
			threads_count = values.size() / 2;
		}
		std::vector<std::thread> threads(threads_count);
		unsigned int thread_elements_count = values.size() / threads_count;
		if (values.size() % threads_count) {
			thread_elements_count++;
		}
		

		std::vector<unsigned int> thread_arrays_size(threads_count);
		for (unsigned int i = 0; i < threads.size() - 1; i++) {
			threads[i] = std::thread(MergeSortImpl, ref(values), ref(buffer), i * thread_elements_count, (i + 1) * thread_elements_count - 1);
			thread_arrays_size[i] = thread_elements_count;
		}
		threads[threads.size() - 1] = std::thread(MergeSortImpl, ref(values), ref(buffer), (threads.size() - 1) * thread_elements_count, values.size() - 1);
		thread_arrays_size[threads.size() - 1] = values.size() - (threads.size() - 1) * thread_elements_count;

		std::vector<int> min_values(threads_count);
		std::vector<unsigned int> part_indexes(threads_count, 0);
		for (unsigned int i = 0; i < threads.size(); i++) {
			if (threads[i].joinable()) {
				threads[i].join();
			}
			min_values[i] = values[i * thread_elements_count];
		}

		std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
		for (unsigned int i = 0; i < values.size(); i++) {
			std::vector<int>::iterator min = std::min_element(min_values.begin(), min_values.end());
			values[i] = *min;
			unsigned int index = std::distance(min_values.begin(), min);
			if (++part_indexes[index] < thread_arrays_size[index]) {
				*min = buffer[index * thread_elements_count + part_indexes[index]];
			} else {
				*min = INT_MAX;
			}
		}
		std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
		std::cout << "Merge:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << '\n';
	}
}

int main() {
	unsigned int n, k;
	std::cout << "Input count of numbers: ";
	std::cin >> n;
	std::cout << "Input count of threads: ";
	std::cin >> k;
	
	std::default_random_engine dre(0);
    std::uniform_int_distribution<> uid(-1000, 1000);
	std::vector<int> array(n), temp1, temp2;
	for (unsigned int i = 0; i < array.size(); i++) {
		array[i] = uid(dre);
	}
	temp1 = temp2 = array;
	
	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
	MergeSort(temp1);
	std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();

	std::cout << "Sort:\t\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << '\n';
	std::cout << "Is array sorted: " << std::is_sorted(temp1.begin(), temp1.end()) << '\n';

	start = std::chrono::high_resolution_clock::now();
	MultiThreadMergeSort(temp2, k);
	end = std::chrono::high_resolution_clock::now();

	std::cout << "MultiSort:\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << '\n';
	std::cout << "Is array sorted: " << std::is_sorted(temp2.begin(), temp2.end()) << '\n';
	
	return 0;
}
