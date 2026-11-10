#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace SwitchHistoryStats
{
using SwitchHistory = std::map<std::pair<std::string, std::string>, std::vector<uint64_t>>;

// Returns the non-HPR app most often switched away from, with its switch
// count. Returns std::nullopt when there is nothing to report (empty history
// or every recorded source is HPR).
inline std::optional<std::pair<std::string, std::size_t>> mostSwitchedFrom(const SwitchHistory &history)
{
	std::map<std::string, std::size_t> switchCounts;
	const std::string selfApp = "HPR";
	for (const auto &[apps, vec] : history)
	{
		const std::string &fromApp = apps.first;
		if (fromApp == selfApp)
			continue;
		switchCounts[fromApp] += vec.size();
	}

	if (switchCounts.empty())
		return std::nullopt;

	auto maxIt = std::max_element(switchCounts.begin(), switchCounts.end(),
								  [](const auto &a, const auto &b) { return a.second < b.second; });
	return *maxIt;
}

// Returns the non-HPR app most often switched to, with its switch count.
// Returns std::nullopt when there is nothing to report (empty history or
// every recorded target is HPR).
inline std::optional<std::pair<std::string, std::size_t>> mostSwitchedTo(const SwitchHistory &history)
{
	std::map<std::string, std::size_t> switchCounts;
	const std::string selfApp = "HPR";
	for (const auto &[apps, vec] : history)
	{
		const std::string &toApp = apps.second;
		if (toApp == selfApp)
			continue;
		switchCounts[toApp] += vec.size();
	}

	if (switchCounts.empty())
		return std::nullopt;

	auto maxIt = std::max_element(switchCounts.begin(), switchCounts.end(),
								  [](const auto &a, const auto &b) { return a.second < b.second; });
	return *maxIt;
}
} // namespace SwitchHistoryStats
