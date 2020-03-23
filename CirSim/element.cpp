#include "element.h"

//Constructors
Element::Element(const std::string& name, ElementType type)
	: m_name("")
	, m_type(type)
{
}

Element::~Element()
{
	delete element;
}

Element* Element::element;
std::map<std::string, Element*> Element::elementsMap;
//Map Methods
bool Element::isFound(std::string elementName)
{
	if (elementsMap.find(elementName) == elementsMap.end())
		return false;
	return true;
}

unsigned int Element::getMapSize()
{
	return elementsMap.size();
}

Element* Element::createElement(std::string elementName)
{
	if (isFound(elementName))
		return elementsMap[elementName];

	else
	{	
		element = new Element(elementName, ElementType::Resistor);
		elementsMap[elementName] = element;

		switch (elementName[0])
		{
		case 'R':
		case 'r':
			elementsMap[elementName]->m_type = ElementType::Resistor;
			break;
		case 'C':
		case 'c':
			elementsMap[elementName]->m_type = ElementType::Capacitor;
			break;
		case 'L':
		case 'l':
			elementsMap[elementName]->m_type = ElementType::Inductor;
			break;
		}

		return element;
	}
}
void Element::eraseElement(std::string elementName)
{
	if (isFound(elementName)) //Avoid deleting uncreated elements
		elementsMap.erase(elementName);
}