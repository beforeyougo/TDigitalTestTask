#include "OrderProcessingTools.h"
#include <chrono>
#include <filesystem>

int main(int argc, char** argv)
{
	if (argc < 3) {
		std::cout << "Wrong number of arguments. Need to specify path to syncshots and updates files";
		return -1;
	}

	std::ifstream syncShots, updates;

	// First argument is path to the sync shots file
	syncShots.open(argv[1]);
	if (!syncShots.is_open()) {
		std::cout << "Could not open sync shots file: " << argv[1];
		return -1;
	}

	// Second - is path to the updates file
	updates.open(argv[2]);
	if (!updates.is_open()) {
		std::cout << "Could not open updates file: " << argv[2];
		return -1;
	}

	// If third argument is specified, we treat it as path to resulting derictory
	std::filesystem::path resultPath;
	if (argc > 3) {
		resultPath = argv[3];
		if (!std::filesystem::exists(resultPath)) {
			std::cout << "Passed path to the resulting directory does not exit: " << argv[3];
			return -1;
		}
	}
	else {
		resultPath = "results/";
		if (!std::filesystem::exists(resultPath)) {
			std::filesystem::create_directory(resultPath);
		}
	}

	resultPath /= "results.csv";
	std::ofstream results;
	results.open(resultPath);

	std::cout << "Started files processing" << std::endl;
	try {
		auto begin = std::chrono::steady_clock::now();
		ordtools::ProcessSyncShotsAndUpdates(syncShots, updates, results, /* logFeatures = */ true);
		auto end = std::chrono::steady_clock::now();
		auto elapsed_s = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
		std::cout << "Processing is finished, elapsed time is "
                  << elapsed_s.count() << " microseconds" << std::endl;
	}
	catch (const std::exception& e) {
		std::cout << "Error while processing files: " << e.what() << std::endl;
	}

	syncShots.close();
	updates.close();
	results.close();
	return 0;
}