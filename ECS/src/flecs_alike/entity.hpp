#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

namespace ecs
{
/**
 * @brief ����� ��������
 */
class entity
{
public:
	entity(size_t ID)
		: m_ID(ID)
		, valid(true)
	{
	}

	/**
	 * @brief ��������� ��������� � ��������.
	 * @brief � ���������� ����������� ��������� ��� ������������ ����������.
	 */
	template <typename _TComponent, typename... _TArgs>
	entity& add(_TArgs&&... args)
	{
		m_components[typeid(_TComponent)] = std::make_shared<_TComponent>(std::forward<_TArgs>(args)...);
		return *this;
	}

	/**
	 * @brief ���������� ��������� �� ��������� ��������� � ��������.
	 * @brief ���� �� ������� ����� ���������, �� ������� ������� ���������.
	 */
	template <typename _TComponent>
	_TComponent* get()
	{
		auto it = m_components.find(typeid(_TComponent));
		return (it != m_components.end())
			? static_cast<_TComponent*>(it->second.get())
			: nullptr;
	}

	/**
	 * @brief ���������� ��������� �� ��������� ��������� � ��������.
	 * @brief ���� �� ������� ����� ���������, �� ������� ������� ���������.
	 */
	void* get(std::type_index componentType)
	{
		auto it = m_components.find(componentType);
		return (it != m_components.end())
			? it->second.get()
			: nullptr;
	}

	template <typename _TComponent>
	void remove()
	{
		auto it = m_components.find(typeid(_TComponent));
		if (it != m_components.end())
		{
			m_components.erase(it);
		}
	}

	/**
	 * @brief ���������� ��������� �� ��������� ��������� � ��������.
	 * @brief ���� �� ������� ����� ���������, �� ������� ������� ���������.
	 */
	template <typename _TComponent>
	bool has() const
	{
		return m_components.find(typeid(_TComponent)) != m_components.end();
	}

	/**
	 * @brief ���������� ��������� �� ��������� ��������� � ��������.
	 * @brief ���� �� ������� ����� ���������, �� ������� ������� ���������.
	 */
	bool has(std::type_index component) const
	{
		return m_components.find(component) != m_components.end();
	}

	/**
	 * @brief �������� �������� ��� ����������
	 * @see ecs::entity_manager::invalidate()
	 */
	void destruct()
	{
		valid = false;
	}

	/**
	 * @brief ���������� ������������� ��������
	 */
	size_t ID() const { return m_ID; }

	/**
	 * @brief ��������� �������� �� �������� ��������
	 * @see ecs::entity_manager::invalidate()
	 */
	bool is_valid() const { return valid; }

private:
	size_t m_ID;
	bool valid;

	std::unordered_map<std::type_index, std::shared_ptr<void>> m_components;
};
}; // namespace ecs
