#include "switchHistoryStats.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

namespace
{
using SwitchHistoryStats::SwitchHistory;

void expect(bool condition, const char *message)
{
	if (!condition)
	{
		std::cerr << "FAIL: " << message << "\n";
		std::exit(1);
	}
}

void testEmptyHistory()
{
	SwitchHistory history;
	expect(!SwitchHistoryStats::mostSwitchedFrom(history).has_value(), "empty: mostSwitchedFrom");
	expect(!SwitchHistoryStats::mostSwitchedTo(history).has_value(), "empty: mostSwitchedTo");
}

// Regression for the startup crash: a single chrome -> HPR row leaves the
// "most switched to" map empty after HPR is filtered out. The old code
// dereferenced std::max_element()'s end() iterator in that case.
void testSingleRowToHpr()
{
	SwitchHistory history;
	history[{"chrome", "HPR"}].push_back(1794239990383ULL);

	auto from = SwitchHistoryStats::mostSwitchedFrom(history);
	expect(from.has_value(), "single chrome->HPR: mostSwitchedFrom should exist");
	expect(from->first == "chrome" && from->second == 1, "single chrome->HPR: mostSwitchedFrom value");
	expect(!SwitchHistoryStats::mostSwitchedTo(history).has_value(),
		   "single chrome->HPR: mostSwitchedTo should be empty");
}

// Symmetric regression: a single HPR -> chrome row leaves the
// "most switched away from" map empty after HPR is filtered out.
void testSingleRowFromHpr()
{
	SwitchHistory history;
	history[{"HPR", "chrome"}].push_back(1794239990383ULL);

	expect(!SwitchHistoryStats::mostSwitchedFrom(history).has_value(),
		   "single HPR->chrome: mostSwitchedFrom should be empty");
	auto to = SwitchHistoryStats::mostSwitchedTo(history);
	expect(to.has_value(), "single HPR->chrome: mostSwitchedTo should exist");
	expect(to->first == "chrome" && to->second == 1, "single HPR->chrome: mostSwitchedTo value");
}

void testMultipleRows()
{
	SwitchHistory history;
	history[{"Unknown", "chrome"}].push_back(1794239990382ULL);
	history[{"Unknown", "chrome"}].push_back(1794239990383ULL);
	history[{"chrome", "HPR"}].push_back(1794239990384ULL);

	auto from = SwitchHistoryStats::mostSwitchedFrom(history);
	expect(from.has_value(), "multiple rows: mostSwitchedFrom should exist");
	expect(from->first == "Unknown" && from->second == 2, "multiple rows: mostSwitchedFrom value");

	auto to = SwitchHistoryStats::mostSwitchedTo(history);
	expect(to.has_value(), "multiple rows: mostSwitchedTo should exist");
	expect(to->first == "chrome" && to->second == 2, "multiple rows: mostSwitchedTo value");
}
} // namespace

int main()
{
	testEmptyHistory();
	testSingleRowToHpr();
	testSingleRowFromHpr();
	testMultipleRows();

	std::cout << "switch_history_stats_test: PASS\n";
	return 0;
}
