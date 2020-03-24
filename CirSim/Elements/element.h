#ifndef _ELEMENT_H
#define _ELEMENT_H

#define SMALL_RES 0.0001
#define big_or_zero(x)  ((x) >= DBL_EPSILON ? 0.0 : 0.0001)
#define check_not_zero(x) ((x) >= DBL_EPSILON ? (x) : 0.0001)

#include <map>
#include <list>
#include <string>
#include "../Math/complex.h"

enum class ElementType { Resistor, Capacitor, Inductor };

class Element
{
protected: //Elements Map
	static std::map<std::string, Element*> elementsMap;

private: //Members
	std::string m_name;
	ElementType m_type;
	
protected: //Constructors
	Element(const std::string& name, ElementType type);
	~Element();

public: //Static Elements Map Methods
	static inline bool elementExists(std::string elementName) { return (elementsMap.find(elementName) != elementsMap.end()); }
	static inline size_t getElementsCount() { return elementsMap.size(); }

public: //Logic
	inline bool operator==(const Element& rhs) const { return rhs.m_name == m_name; }

public: //Setters
	inline void setName(const std::string& name) { elementsMap.erase(m_name); m_name = name; elementsMap.emplace(m_name, this); }

public: //Getters
	inline const std::string& geName() const { return m_name; }
	inline ElementType getType() const { return m_type; }

	Element(const Element&) = delete;
	void operator=(const Element&) = delete;
};

#endif //_ELEMENT_H