#pragma once

#include <tuple>

namespace ecs
{

template <typename _TSystem, typename _TWrite, typename... _TReads>
struct SystemInfo
{
	using SystemType = _TSystem;
	using WriteComponent = _TWrite;
	using ReadComponents = std::tuple<_TReads...>;
};

template <typename... _TSystemInfos>
struct SystemGraph
{
	using Systems = std::tuple<_TSystemInfos...>;
};

} // namespace ecs

namespace ecs::details
{

template <typename T, size_t Capacity>
struct ConstexprVector
{
	std::array<T, Capacity> data{};
	size_t size = 0;
	constexpr void push_back(const T& value) { data[size++] = value; }
};

template <typename T, size_t Capacity>
struct ConstexprQueue
{
	std::array<T, Capacity> data{};
	size_t head = 0, tail = 0;
	constexpr void push(const T& value) { data[tail++] = value; }
	constexpr T front() { return data[head]; }
	constexpr void pop() { head++; }
	constexpr bool empty() const { return head == tail; }
	constexpr size_t get_size() const { return tail - head; }
};

template <typename TGraph>
constexpr auto BuildExecutionStages()
{
	constexpr auto system_infos = typename TGraph::Systems{};
	constexpr size_t num_systems = std::tuple_size_v<decltype(system_infos)>;

	std::array<TypeId, num_systems> write_deps{};
	std::array<std::array<TypeId, num_systems>, num_systems> read_deps{};
	std::array<size_t, num_systems> read_counts{};

	std::apply([&](auto... info) {
		size_t i = 0;
		([&] {
			using Info = decltype(info);
			write_deps[i] = TypeIdOf<typename Info::WriteComponent>();

			std::apply([&](auto... read_comp_types) {
				size_t j = 0;
				([&] {
					read_deps[i][j++] = TypeIdOf<decltype(read_comp_types)>();
				}(),
					...);
				read_counts[i] = j;
			},
				typename Info::ReadComponents{});

			i++;
		}(),
			...);
	},
		system_infos);

	std::array<int, num_systems> in_degree{};
	ConstexprVector<size_t, num_systems * num_systems> adj_list[num_systems];

	for (size_t writer_idx = 0; writer_idx < num_systems; ++writer_idx)
	{
		if (write_deps[writer_idx] == TypeIdOf<void>())
			continue;

		for (size_t reader_idx = 0; reader_idx < num_systems; ++reader_idx)
		{
			for (size_t dep_idx = 0; dep_idx < read_counts[reader_idx]; ++dep_idx)
			{
				if (writer_idx != reader_idx && write_deps[writer_idx] == read_deps[reader_idx][dep_idx])
				{
					adj_list[writer_idx].push_back(reader_idx);
					in_degree[reader_idx]++;
				}
			}
		}
	}

	ConstexprQueue<size_t, num_systems> q;
	for (size_t i = 0; i < num_systems; ++i)
	{
		if (in_degree[i] == 0)
		{
			q.push(i);
		}
	}

	ConstexprVector<ConstexprVector<size_t, num_systems>, num_systems> stages;
	while (!q.empty())
	{
		size_t stage_size = q.get_size();
		ConstexprVector<size_t, num_systems> current_stage;
		for (size_t i = 0; i < stage_size; ++i)
		{
			size_t u = q.front();
			q.pop();
			current_stage.push_back(u);

			for (size_t j = 0; j < adj_list[u].size; ++j)
			{
				size_t v = adj_list[u].data[j];
				in_degree[v]--;
				if (in_degree[v] == 0)
				{
					q.push(v);
				}
			}
		}
		stages.push_back(current_stage);
	}

	return stages;
}

} // namespace ecs::details
