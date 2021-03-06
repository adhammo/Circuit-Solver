#include "voltagesource.h"

#include "cccs.h"
#include "ccvs.h"

//Static Member declaration
std::list <VoltageSource*> VoltageSource::voltageSources;

//Constructors
VoltageSource::VoltageSource(const std::string& voltageSrcName, ElementType type, Node& posNode, Node& negNode, Complex supplyVoltage, Complex internalImpedance)
	: Element(voltageSrcName, type)
	, m_posNode(&posNode)
	, m_negNode(&negNode)
	, m_supplyVoltage(supplyVoltage)
	, m_current(0.0, 0.0)
{
	m_posNode->linkElement(this);
	m_negNode->linkElement(this);
	voltageSources.push_back(this);
}

VoltageSource::VoltageSource(const std::string& voltageSrcName, Node& posNode, Node& negNode, Complex supplyVoltage, Complex internalImpedance)
	: Element(voltageSrcName, ElementType::VS)
	, m_posNode(&posNode)
	, m_negNode(&negNode)
	, m_supplyVoltage(supplyVoltage)
	, m_current(0.0, 0.0)
{
	m_posNode->linkElement(this);
	m_negNode->linkElement(this);
	voltageSources.push_back(this);
}

VoltageSource::~VoltageSource()
{
	m_posNode->unLinkElement(this);
	m_negNode->unLinkElement(this);
	m_posNode = m_negNode = nullptr;

	std::string name = getName();
	ElementType type = getType();
	for (std::list<VoltageSource*>::iterator it = voltageSources.begin(); it != voltageSources.end(); it++)
	{
		if (name == (*it)->getName() && type == (*it)->getType())
		{
			voltageSources.erase(it);
			break;
		}
	}
}

//Static Voltage Source Creation 
VoltageSource* VoltageSource::createVoltageSource(const std::string& voltageSrcName, Node& posNode, Node& negNode, Complex supplyVoltage, Complex internalImpedance)
{
	std::string name = elementNameWithType(voltageSrcName, ElementType::VS);
	VoltageSource* voltagesource = (VoltageSource*)getElement(name);
	if (voltagesource == nullptr)
		voltagesource = new VoltageSource(voltageSrcName, posNode, negNode, supplyVoltage, internalImpedance);
	return voltagesource;
}

//Matrix Operations
void VoltageSource::injectIntoMatrix(Complex* matrix, size_t matrixWidth, const std::map<std::string, size_t>& nodeIndexMap, const std::map<std::string, size_t>& voltageIndexMap, double angularFrequency)
{
	if (nodeIndexMap.find(m_posNode->getName()) == nodeIndexMap.end()
		|| nodeIndexMap.find(m_negNode->getName()) == nodeIndexMap.end())
		throw std::runtime_error("VoltageSource: Couldn't find a Node.");

	if (voltageIndexMap.find(getName()) == voltageIndexMap.end())
		throw std::runtime_error("VoltageSource: Couldn't find a Voltage Source.");

	size_t posIdx = nodeIndexMap.at(m_posNode->getName());
	size_t negIdx = nodeIndexMap.at(m_negNode->getName());
	size_t constRow = matrixWidth - 1;
	size_t voltageIdx = voltageIndexMap.at(getName());

	Complex supplyVoltage = getSupplyVoltage();
	Complex internalImpedance = getInternalImpedance();
	matrix[voltageIdx * matrixWidth + posIdx] = 1;
	matrix[voltageIdx * matrixWidth + negIdx] = -1;
	matrix[voltageIdx * matrixWidth + voltageIdx] = internalImpedance;
	matrix[voltageIdx * matrixWidth + constRow] = supplyVoltage;
	matrix[posIdx * matrixWidth + voltageIdx] = -1;
	matrix[negIdx * matrixWidth + voltageIdx] = 1;
}

void VoltageSource::injectVSCurrentControlIntoMatrix(Complex* matrix, size_t matrixWidth, CCVS* ccvs, Complex totalCurrentFactor, const std::map<std::string, size_t>& nodeIndexMap, const std::map<std::string, size_t>& voltageIndexMap, double angularFrequency)
{
	if (voltageIndexMap.find(getName()) == voltageIndexMap.end()
		|| voltageIndexMap.find(ccvs->getName()) == voltageIndexMap.end())
		throw std::runtime_error("VoltageSource: Couldn't find a Voltage Source.");

	size_t voltageSourceIdx = voltageIndexMap.at(getName());
	size_t voltageIdx = voltageIndexMap.at(ccvs->getName());

	matrix[voltageIdx * matrixWidth + voltageSourceIdx] = -totalCurrentFactor;
}

void VoltageSource::injectCSCurrentControlIntoMatrix(Complex* matrix, size_t matrixWidth, CCCS* cccs, Complex totalCurrentFactor, const std::map<std::string, size_t>& nodeIndexMap, const std::map<std::string, size_t>& voltageIndexMap, double angularFrequency)
{
	if (nodeIndexMap.find(cccs->getPosNode()->getName()) == nodeIndexMap.end()
		|| nodeIndexMap.find(cccs->getNegNode()->getName()) == nodeIndexMap.end())
		throw std::runtime_error("VoltageSource: Couldn't find a Node.");

	if (voltageIndexMap.find(getName()) == voltageIndexMap.end())
		throw std::runtime_error("VoltageSource: Couldn't find a Voltage Source.");

	size_t posIdx = nodeIndexMap.at(cccs->getPosNode()->getName());
	size_t negIdx = nodeIndexMap.at(cccs->getNegNode()->getName());
	size_t voltageSourceIdx = voltageIndexMap.at(getName());

	matrix[posIdx * matrixWidth + voltageSourceIdx] -= totalCurrentFactor;
	matrix[negIdx * matrixWidth + voltageSourceIdx] += totalCurrentFactor;
}

void VoltageSource::fillVoltageSourcesFromVector(Complex* vector, std::map<std::string, size_t>& voltageIndexMap)
{
	for (std::list<VoltageSource*>::iterator it = voltageSources.begin(); it != voltageSources.end(); it++)
		(*it)->fillFromVector(vector, voltageIndexMap);
}