#include "vccs.h"

//Constructors
VCCS::VCCS(const std::string& vcvsName, Node& posNode, Node& negNode, Complex factor, const std::string& controlPosNode, const std::string& controlNegNode, Complex internalAdmittance)
	: CurrentSource(vcvsName, posNode, negNode, 0.0, internalAdmittance)
	, m_controlPosNode(controlPosNode)
	, m_controlNegNode(controlNegNode)
	, m_voltageFactor(factor)
{
	setType(ElementType::VCCS);
}

VCCS::~VCCS()
{
	m_controlPosNode = m_controlNegNode = nullptr;
}

//Static Voltage Source Creation 
VCCS* VCCS::createVCCS(const std::string& ccvsName, Node& posNode, Node& negNode, Complex factor, const std::string& controlPosNode, const std::string& controlNegNode, Complex internalAdmittance)
{
	std::string name = "vccs" + ccvsName;
	if (elementExists(name))
		return (VCCS*)elementsMap[name];

	VCCS* vccs = new VCCS(ccvsName, posNode, negNode, factor, controlPosNode, controlNegNode, internalAdmittance);
	elementsMap.emplace(name, vccs);
	return vccs;
}

//Matrix Operations
void VCCS::injectIntoMatrix(Complex* matrix, size_t matrixWidth, std::map<std::string, size_t>& nodeIndexMap, std::map<std::string, size_t>& voltageIndexMap, double angularFrequency)
{
	if (nodeIndexMap.find(m_posNode->getName()) == nodeIndexMap.end()
		|| nodeIndexMap.find(m_negNode->getName()) == nodeIndexMap.end()
		|| nodeIndexMap.find(m_controlPosNode) == nodeIndexMap.end()
		|| nodeIndexMap.find(m_controlNegNode) == nodeIndexMap.end())
		throw std::logic_error("VCCS: Couldn't find a Node.");

	size_t posIdx = nodeIndexMap[m_posNode->getName()];
	size_t negIdx = nodeIndexMap[m_negNode->getName()];
	size_t controlPosIdx = nodeIndexMap[m_controlPosNode];
	size_t controlNegIdx = nodeIndexMap[m_controlNegNode];
	size_t constRow = matrixWidth - 1;

	Complex voltageFactor = getVoltageFactor();
	Complex internalAdmittance = getInternalAdmittance();
	matrix[posIdx * matrixWidth + controlPosIdx] -= voltageFactor;
	matrix[posIdx * matrixWidth + controlNegIdx] += voltageFactor;
	matrix[negIdx * matrixWidth + controlPosIdx] += voltageFactor;
	matrix[negIdx * matrixWidth + controlNegIdx] -= voltageFactor;
	matrix[posIdx * matrixWidth + posIdx] += internalAdmittance;
	matrix[posIdx * matrixWidth + negIdx] -= internalAdmittance;
	matrix[negIdx * matrixWidth + posIdx] -= internalAdmittance;
	matrix[negIdx * matrixWidth + negIdx] += internalAdmittance;
}